#!/usr/bin/env python3
"""iRODS Federation Zone Registrar Service (FZRS).

A single-file HTTP service that fronts one iRODS zone (the "home zone") and
accepts applications from remote zones that wish to federate with it. A
rodsadmin of the home zone reviews the queue and approves or rejects each
application. On approval FZRS creates the remote zone in the home catalog,
appends a federation entry to the home zone's server_config.json, and signals
the iRODS server to reload its configuration.

Usage:

    python3 fzrs.py /path/to/fzrs.json

Run this as the iRODS service account on a catalog provider of the home zone.
The service speaks plain HTTP only and MUST be deployed behind a
TLS-terminating reverse proxy: admin credentials travel in HTTP Basic headers.

Endpoints:

    POST /apply                       unauthenticated  submit an application
    GET  /status?application_id=<id>  unauthenticated  poll an application
    GET  /list?status=<filter>        rodsadmin        list applications
    POST /judge                       rodsadmin        approve or reject

See fzrs-spec.md for the full specification.
"""

import base64
import datetime
import ipaddress
import json
import logging
import os
import re
import secrets
import shutil
import signal
import socket
import sys
import tempfile
import threading
import uuid
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer
from urllib.parse import parse_qs, urlsplit

import irods.exception
import irods.session
from irods.api_number import api_number
from irods.message import GeneralAdminRequest, iRODSMessage

# ---------------------------------------------------------------------------
# Constants and validation rules (derived from server_config.json schema)
# ---------------------------------------------------------------------------

VERSION = "0.1.0"

ZONE_NAME_RE = re.compile(r"^[A-Za-z0-9_.]{1,63}$")
ZONE_KEY_RE = re.compile(r"^[A-Za-z0-9_]{1,49}$")
NEGOTIATION_KEY_RE = re.compile(r"^[A-Za-z0-9_]{32}$")
HOSTNAME_LABEL_RE = re.compile(r"^(?!-)[A-Za-z0-9-]{1,63}(?<!-)$")
USERNAME_RE = re.compile(r"^[A-Za-z0-9_.@-]{1,63}$")

KEY_ALPHABET = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_"
KEY_LENGTH = 32

ZONE_KEY_SIGNING_HASH_SCHEME = "sha256"

STATUS_PENDING = "pending"
STATUS_APPROVED = "approved"
STATUS_REJECTED = "rejected"
STATUS_EXPIRED = "expired"
ALL_STATUSES = (STATUS_PENDING, STATUS_APPROVED, STATUS_REJECTED, STATUS_EXPIRED)

DECISION_NOTE_MAX_LENGTH = 1024

QUEUE_SCHEMA_VERSION = 1

CONFIG_DEFAULTS = {
    "bind_address": "0.0.0.0",
    "application_expiration_in_seconds": 7 * 24 * 60 * 60,
    "queue_file": None,  # resolved to <config dir>/fzrs-queue.json
    "max_pending_applications": 100,
    "max_request_body_bytes": 65536,
    "irods_environment_file": None,  # resolved from env var / ~/.irods
    "server_config_file": "/etc/irods/server_config.json",
    "server_pid_file": "/var/run/irods/irods-server.pid",
}
CONFIG_REQUIRED = ("port",)
CONFIG_KNOWN_KEYS = set(CONFIG_DEFAULTS) | set(CONFIG_REQUIRED)

log = logging.getLogger("fzrs")


# ---------------------------------------------------------------------------
# Small helpers
# ---------------------------------------------------------------------------

class ConfigError(Exception):
    pass


class HTTPError(Exception):
    def __init__(self, status, message, headers=None):
        super().__init__(message)
        self.status = status
        self.message = message
        self.headers = headers or {}


def utc_now():
    return datetime.datetime.now(datetime.timezone.utc).replace(microsecond=0)


def to_iso(dt):
    return dt.strftime("%Y-%m-%dT%H:%M:%SZ")


def from_iso(s):
    return datetime.datetime.strptime(s, "%Y-%m-%dT%H:%M:%SZ").replace(tzinfo=datetime.timezone.utc)


def generate_key():
    return "".join(secrets.choice(KEY_ALPHABET) for _ in range(KEY_LENGTH))


def is_valid_host(value):
    if not isinstance(value, str) or not value or len(value) > 253:
        return False
    try:
        ipaddress.ip_address(value)
        return True
    except ValueError:
        pass
    return all(HOSTNAME_LABEL_RE.match(label) for label in value.rstrip(".").split("."))


def is_int(value):
    return isinstance(value, int) and not isinstance(value, bool)


def atomic_write_json(path, obj, mode=None):
    """Write obj as JSON to path via a temp file in the same directory."""
    directory = os.path.dirname(os.path.abspath(path))
    fd, tmp = tempfile.mkstemp(prefix=".fzrs-", suffix=".tmp", dir=directory)
    try:
        with os.fdopen(fd, "w") as f:
            json.dump(obj, f, indent=4, sort_keys=True)
            f.write("\n")
            f.flush()
            os.fsync(f.fileno())
        if mode is not None:
            os.chmod(tmp, mode)
        os.replace(tmp, path)
    except BaseException:
        try:
            os.unlink(tmp)
        except OSError:
            pass
        raise


# ---------------------------------------------------------------------------
# Configuration
# ---------------------------------------------------------------------------

def load_config(path):
    try:
        with open(path, "r") as f:
            raw = json.load(f)
    except FileNotFoundError:
        raise ConfigError("configuration file not found: {}".format(path))
    except json.JSONDecodeError as e:
        raise ConfigError("configuration file is not valid JSON: {}".format(e))
    if not isinstance(raw, dict):
        raise ConfigError("configuration file must contain a JSON object")

    unknown = sorted(set(raw) - CONFIG_KNOWN_KEYS)
    if unknown:
        raise ConfigError("unknown configuration key(s): {}".format(", ".join(unknown)))
    for key in CONFIG_REQUIRED:
        if key not in raw:
            raise ConfigError("missing required configuration key: {}".format(key))

    cfg = dict(CONFIG_DEFAULTS)
    cfg.update(raw)
    config_dir = os.path.dirname(os.path.abspath(path))

    def positive_int(key, maximum=None):
        v = cfg[key]
        if not is_int(v) or v <= 0 or (maximum is not None and v > maximum):
            raise ConfigError("{} must be an integer in [1, {}]".format(key, maximum or "inf"))

    positive_int("port", 65535)
    positive_int("application_expiration_in_seconds")
    positive_int("max_pending_applications")
    positive_int("max_request_body_bytes")

    if not isinstance(cfg["bind_address"], str) or not cfg["bind_address"]:
        raise ConfigError("bind_address must be a non-empty string")

    def resolve_path(key, default):
        v = cfg[key]
        if v is None:
            v = default
        if not isinstance(v, str) or not v:
            raise ConfigError("{} must be a non-empty string".format(key))
        v = os.path.expanduser(v)
        if not os.path.isabs(v):
            v = os.path.join(config_dir, v)
        cfg[key] = os.path.normpath(v)

    resolve_path("queue_file", "fzrs-queue.json")
    resolve_path("irods_environment_file",
                 os.environ.get("IRODS_ENVIRONMENT_FILE") or "~/.irods/irods_environment.json")
    resolve_path("server_config_file", CONFIG_DEFAULTS["server_config_file"])
    resolve_path("server_pid_file", CONFIG_DEFAULTS["server_pid_file"])
    return cfg


def load_irods_environment(path):
    try:
        with open(path, "r") as f:
            env = json.load(f)
    except FileNotFoundError:
        raise ConfigError("iRODS environment file not found: {}".format(path))
    except json.JSONDecodeError as e:
        raise ConfigError("iRODS environment file is not valid JSON: {}".format(e))
    for key in ("irods_host", "irods_port", "irods_zone_name", "irods_user_name"):
        if key not in env:
            raise ConfigError("iRODS environment file is missing {}".format(key))
    return env


# ---------------------------------------------------------------------------
# Queue
# ---------------------------------------------------------------------------

class Queue:
    """In-memory application queue persisted to a JSON file.

    Callers must hold self.lock around any read-modify-write sequence.
    """

    def __init__(self, path):
        self.path = path
        self.lock = threading.Lock()
        self.applications = {}  # application_id -> record, insertion ordered

    def load(self):
        if not os.path.exists(self.path):
            self.applications = {}
            self.save()
            return
        with open(self.path, "r") as f:
            data = json.load(f)  # a corrupt file is a startup error
        if not isinstance(data, dict) or data.get("schema_version") != QUEUE_SCHEMA_VERSION:
            raise ConfigError("queue file {} has an unexpected format".format(self.path))
        self.applications = {}
        for record in data.get("applications", []):
            self.applications[record["application_id"]] = record

    def save(self):
        atomic_write_json(self.path, {
            "schema_version": QUEUE_SCHEMA_VERSION,
            "applications": list(self.applications.values()),
        }, mode=0o600)

    def expire(self, now):
        """Mark overdue pending applications expired. Returns True if changed."""
        changed = False
        for record in self.applications.values():
            if record["status"] == STATUS_PENDING and from_iso(record["expires_at"]) <= now:
                record["status"] = STATUS_EXPIRED
                changed = True
        if changed:
            self.save()
        return changed

    def count_pending(self):
        return sum(1 for r in self.applications.values() if r["status"] == STATUS_PENDING)

    def find_pending_by_zone_name(self, zone_name):
        for r in self.applications.values():
            if r["status"] == STATUS_PENDING and r["zone_name"] == zone_name:
                return r
        return None


# ---------------------------------------------------------------------------
# iRODS access
# ---------------------------------------------------------------------------

# Keys of the service account environment that describe the connection rather
# than the identity; these are copied into admin sessions so SSL/negotiation
# settings are honored.
ENV_KEYS_NOT_COPIED = {
    "irods_user_name", "irods_cwd", "irods_home", "irods_default_resource",
    "irods_authentication_file", "irods_authentication_scheme",
    "schema_name", "schema_version",
}


class IRODS:
    def __init__(self, env_file, env):
        self.env_file = env_file
        self.env = env

    @property
    def host(self):
        return self.env["irods_host"]

    @property
    def port(self):
        return int(self.env["irods_port"])

    @property
    def zone(self):
        return self.env["irods_zone_name"]

    def service_session(self):
        return irods.session.iRODSSession(irods_env_file=self.env_file)

    def admin_session(self, username, password):
        """Authenticate username/password against the home zone and require rodsadmin.

        Raises HTTPError(401) on bad credentials, HTTPError(403) if not a
        rodsadmin, HTTPError(502) if iRODS is unreachable. Returns an open
        session the caller must clean up.
        """
        kwargs = {k: v for k, v in self.env.items() if k not in ENV_KEYS_NOT_COPIED}
        kwargs.update({
            "irods_host": self.host,
            "irods_port": self.port,
            "irods_zone_name": self.zone,
            "irods_user_name": username,
            "password": password,
            "irods_authentication_scheme": self.env.get("irods_authentication_scheme", "native"),
        })
        session = irods.session.iRODSSession(**kwargs)
        try:
            session.server_version  # forces a connection and authentication
            user = session.users.get(username)
        except (irods.exception.CAT_INVALID_AUTHENTICATION,
                irods.exception.CAT_INVALID_USER,
                irods.exception.PAMException,
                irods.exception.UserDoesNotExist):
            session.cleanup()
            raise HTTPError(401, "invalid iRODS credentials", {"WWW-Authenticate": 'Basic realm="fzrs"'})
        except irods.exception.NetworkException as e:
            session.cleanup()
            raise HTTPError(502, "cannot reach iRODS: {}".format(e))
        except Exception:
            session.cleanup()
            raise
        if user.type != "rodsadmin":
            session.cleanup()
            raise HTTPError(403, "user {} is not a rodsadmin".format(username))
        return session

    @staticmethod
    def zone_exists(session, zone_name):
        try:
            session.zones.get(zone_name)
            return True
        except irods.exception.ZoneDoesNotExist:
            return False

    @staticmethod
    def mkzone(session, zone_name, connection_string):
        """Equivalent of: iadmin mkzone <zone_name> remote <host:port>."""
        body = GeneralAdminRequest("add", "zone", zone_name, "remote", connection_string, "")
        request = iRODSMessage("RODS_API_REQ", msg=body, int_info=api_number["GENERAL_ADMIN_AN"])
        with session.pool.get_connection() as conn:
            conn.send(request)
            conn.recv()

    @staticmethod
    def rmzone(session, zone_name):
        session.zones.remove(zone_name)


# ---------------------------------------------------------------------------
# Home zone server_config.json handling
# ---------------------------------------------------------------------------

class HomeZoneConfig:
    def __init__(self, server_config_file, pid_file):
        self.path = server_config_file
        self.pid_file = pid_file

    def read(self):
        with open(self.path, "r") as f:
            return json.load(f)

    def facts(self):
        cfg = self.read()
        return {
            "zone_name": cfg["zone_name"],
            "zone_key": cfg["zone_key"],
            "zone_port": cfg["zone_port"],
            "catalog_provider_hosts": list(cfg["catalog_provider_hosts"]),
        }

    @staticmethod
    def federation_entry_for(zone_name, cfg):
        for entry in cfg.get("federation", []):
            if entry.get("zone_name") == zone_name:
                return entry
        return None

    def add_federation_entry(self, entry):
        """Append entry to the federation array. Raises if zone_name already present."""
        cfg = self.read()
        if self.federation_entry_for(entry["zone_name"], cfg) is not None:
            raise RuntimeError("federation entry for zone {} already exists in {}".format(
                entry["zone_name"], self.path))
        cfg.setdefault("federation", []).append(entry)
        st = os.stat(self.path)
        backup = "{}.fzrs-backup-{}".format(self.path, utc_now().strftime("%Y%m%dT%H%M%SZ"))
        shutil.copy2(self.path, backup)
        atomic_write_json(self.path, cfg, mode=st.st_mode & 0o777)
        log.info("appended federation entry for %s to %s (backup at %s)", entry["zone_name"], self.path, backup)

    def reload_server(self):
        try:
            with open(self.pid_file, "r") as f:
                pid = int(f.read().strip())
        except (OSError, ValueError) as e:
            raise RuntimeError("cannot read server pid file {}: {}".format(self.pid_file, e))
        try:
            os.kill(pid, signal.SIGHUP)
        except OSError as e:
            raise RuntimeError("cannot signal iRODS server pid {}: {}".format(pid, e))
        log.info("sent SIGHUP to iRODS server pid %d", pid)


# ---------------------------------------------------------------------------
# Application logic
# ---------------------------------------------------------------------------

def validate_apply_request(body):
    if not isinstance(body, dict):
        raise HTTPError(400, "request body must be a JSON object")
    allowed = {"zone_name", "zone_port", "catalog_provider_hosts", "zone_key"}
    unknown = sorted(set(body) - allowed)
    if unknown:
        raise HTTPError(400, "unknown field(s): {}".format(", ".join(unknown)))
    for key in ("zone_name", "zone_port", "catalog_provider_hosts"):
        if key not in body:
            raise HTTPError(400, "missing required field: {}".format(key))

    zone_name = body["zone_name"]
    if not isinstance(zone_name, str) or not ZONE_NAME_RE.match(zone_name):
        raise HTTPError(400, "zone_name must match ^[A-Za-z0-9_.]{1,63}$")

    zone_port = body["zone_port"]
    if not is_int(zone_port) or not 0 <= zone_port <= 65535:
        raise HTTPError(400, "zone_port must be an integer in [0, 65535]")

    hosts = body["catalog_provider_hosts"]
    if not isinstance(hosts, list) or not hosts:
        raise HTTPError(400, "catalog_provider_hosts must be a non-empty array of hostnames")
    if len(set(hosts)) != len(hosts):
        raise HTTPError(400, "catalog_provider_hosts must not contain duplicates")
    for h in hosts:
        if not is_valid_host(h):
            raise HTTPError(400, "catalog_provider_hosts contains an invalid hostname: {!r}".format(h))

    zone_key = body.get("zone_key")
    if zone_key is not None and (not isinstance(zone_key, str) or not ZONE_KEY_RE.match(zone_key)):
        raise HTTPError(400, "zone_key must match ^[A-Za-z0-9_]{1,49}$")

    return {
        "zone_name": zone_name,
        "zone_port": zone_port,
        "catalog_provider_hosts": list(hosts),
        "zone_key": zone_key,
    }


class App:
    def __init__(self, config):
        self.config = config
        self.queue = Queue(config["queue_file"])
        env = load_irods_environment(config["irods_environment_file"])
        self.irods = IRODS(config["irods_environment_file"], env)
        self.home = HomeZoneConfig(config["server_config_file"], config["server_pid_file"])

    def start(self):
        self.queue.load()
        # Fail fast if the home zone facts are unreadable.
        facts = self.home.facts()
        if facts["zone_name"] != self.irods.zone:
            raise ConfigError("zone_name in {} ({}) does not match irods_zone_name in {} ({})".format(
                self.home.path, facts["zone_name"], self.irods.env_file, self.irods.zone))
        log.info("home zone %s at %s:%d, service account %s", self.irods.zone, self.irods.host,
                 self.irods.port, self.irods.env["irods_user_name"])
        log.info("queue file %s holds %d application(s)", self.queue.path, len(self.queue.applications))

    # -- helpers ------------------------------------------------------------

    def sweep(self):
        with self.queue.lock:
            if self.queue.expire(utc_now()):
                log.info("expiration sweep marked applications expired")

    def applicant_federation_entry(self, record):
        facts = self.home.facts()
        return {
            "catalog_provider_hosts": facts["catalog_provider_hosts"],
            "negotiation_key": record["negotiation_key"],
            "zone_key": facts["zone_key"],
            "zone_key_signing_hash_scheme": ZONE_KEY_SIGNING_HASH_SCHEME,
            "zone_name": facts["zone_name"],
            "zone_port": facts["zone_port"],
        }

    def applicant_mkzone_command(self):
        facts = self.home.facts()
        return "iadmin mkzone {} remote {}:{}".format(
            facts["zone_name"], facts["catalog_provider_hosts"][0], facts["zone_port"])

    @staticmethod
    def home_federation_entry(record):
        return {
            "catalog_provider_hosts": list(record["catalog_provider_hosts"]),
            "negotiation_key": record["negotiation_key"],
            "zone_key": record["zone_key"],
            "zone_key_signing_hash_scheme": ZONE_KEY_SIGNING_HASH_SCHEME,
            "zone_name": record["zone_name"],
            "zone_port": record["zone_port"],
        }

    @staticmethod
    def entry_matches_record(entry, record):
        expected = App.home_federation_entry(record)
        return all(entry.get(k) == v for k, v in expected.items())

    # -- endpoints ----------------------------------------------------------

    def apply(self, body, remote_address):
        req = validate_apply_request(body)
        if req["zone_name"] == self.irods.zone:
            raise HTTPError(400, "zone_name must not be the home zone's name")
        now = utc_now()
        with self.queue.lock:
            if self.queue.count_pending() >= self.config["max_pending_applications"]:
                raise HTTPError(503, "application queue is full")
            if self.queue.find_pending_by_zone_name(req["zone_name"]) is not None:
                raise HTTPError(409, "an application for zone {} is already pending".format(req["zone_name"]))
            record = {
                "application_id": str(uuid.uuid4()),
                "status": STATUS_PENDING,
                "submitted_at": to_iso(now),
                "expires_at": to_iso(now + datetime.timedelta(
                    seconds=self.config["application_expiration_in_seconds"])),
                "remote_address": remote_address,
                "zone_name": req["zone_name"],
                "zone_port": req["zone_port"],
                "catalog_provider_hosts": req["catalog_provider_hosts"],
                "zone_key": req["zone_key"] if req["zone_key"] is not None else generate_key(),
                "zone_key_generated": req["zone_key"] is None,
                "negotiation_key": generate_key(),
                "judged_at": None,
                "judged_by": None,
                "decision_note": None,
            }
            self.queue.applications[record["application_id"]] = record
            self.queue.save()
        log.info("application %s submitted for zone %s from %s", record["application_id"],
                 record["zone_name"], remote_address)
        return 201, {
            "application_id": record["application_id"],
            "status": record["status"],
            "expires_at": record["expires_at"],
            "negotiation_key": record["negotiation_key"],
            "zone_key": record["zone_key"],
            "status_url": "/status?application_id={}".format(record["application_id"]),
        }

    def status(self, query):
        ids = query.get("application_id", [])
        if len(ids) != 1 or not ids[0]:
            raise HTTPError(400, "application_id query parameter is required")
        with self.queue.lock:
            record = self.queue.applications.get(ids[0])
            if record is None:
                raise HTTPError(404, "no such application")
            record = dict(record)
        response = {k: record[k] for k in (
            "application_id", "status", "submitted_at", "expires_at", "judged_at", "decision_note")}
        if record["status"] == STATUS_APPROVED:
            response["applicant_federation_entry"] = self.applicant_federation_entry(record)
            response["applicant_mkzone_command"] = self.applicant_mkzone_command()
        return 200, response

    def list_applications(self, query, admin):
        status = query.get("status", [STATUS_PENDING])
        if len(status) != 1 or status[0] not in ALL_STATUSES + ("all",):
            raise HTTPError(400, "status must be one of {}".format(", ".join(ALL_STATUSES + ("all",))))
        wanted = status[0]
        with self.queue.lock:
            records = [dict(r) for r in self.queue.applications.values()
                       if wanted == "all" or r["status"] == wanted]
        records.sort(key=lambda r: r["submitted_at"])
        log.info("%s listed %d application(s) with status=%s", admin, len(records), wanted)
        return 200, {"applications": records}

    def judge(self, body, admin, session):
        if not isinstance(body, dict):
            raise HTTPError(400, "request body must be a JSON object")
        unknown = sorted(set(body) - {"application_id", "decision", "decision_note"})
        if unknown:
            raise HTTPError(400, "unknown field(s): {}".format(", ".join(unknown)))
        application_id = body.get("application_id")
        if not isinstance(application_id, str) or not application_id:
            raise HTTPError(400, "application_id is required")
        decision = body.get("decision")
        if decision not in ("approve", "reject"):
            raise HTTPError(400, "decision must be \"approve\" or \"reject\"")
        note = body.get("decision_note")
        if note is not None and (not isinstance(note, str) or len(note) > DECISION_NOTE_MAX_LENGTH):
            raise HTTPError(400, "decision_note must be a string of at most {} characters".format(
                DECISION_NOTE_MAX_LENGTH))

        with self.queue.lock:
            record = self.queue.applications.get(application_id)
            if record is None:
                raise HTTPError(404, "no such application")
            if record["status"] in (STATUS_APPROVED, STATUS_REJECTED):
                raise HTTPError(409, "application has already been {}".format(record["status"]))
            if record["status"] == STATUS_EXPIRED:
                raise HTTPError(410, "application has expired")

            if decision == "reject":
                self._finalize(record, STATUS_REJECTED, admin, note)
                log.info("%s rejected application %s (zone %s)", admin, application_id, record["zone_name"])
                return 200, {"application": dict(record)}

            self._approve(record, session)
            self._finalize(record, STATUS_APPROVED, admin, note)
            log.info("%s approved application %s (zone %s)", admin, application_id, record["zone_name"])
            return 200, {
                "application": dict(record),
                "home_federation_entry": self.home_federation_entry(record),
                "applicant_federation_entry": self.applicant_federation_entry(record),
                "applicant_mkzone_command": self.applicant_mkzone_command(),
            }

    def _finalize(self, record, status, admin, note):
        record["status"] = status
        record["judged_at"] = to_iso(utc_now())
        record["judged_by"] = admin
        record["decision_note"] = note
        self.queue.save()

    def _approve(self, record, session):
        """Run the home-zone side of federation. Caller holds the queue lock."""
        zone_name = record["zone_name"]
        connection_string = "{}:{}".format(record["catalog_provider_hosts"][0], record["zone_port"])
        entry = self.home_federation_entry(record)

        # Step 0: every applicant host must resolve. iRODS agents retry DNS
        # for ~30 seconds per unresolvable remote zone host at startup
        # (getaddrinfo_with_retry), which would stall every connection to
        # the home zone.
        for host in record["catalog_provider_hosts"]:
            try:
                socket.getaddrinfo(host, None)
            except socket.gaierror as e:
                raise HTTPError(400, "catalog_provider_hosts entry {!r} does not resolve: {}; "
                                     "refusing to federate with an unresolvable host".format(host, e))

        # Step 1: inspect current state (also detects a retry after a failed reload).
        zone_present = self.irods.zone_exists(session, zone_name)
        existing_entry = self.home.federation_entry_for(zone_name, self.home.read())
        retry_of_reload = (zone_present and existing_entry is not None
                           and self.entry_matches_record(existing_entry, record))
        if zone_present and not retry_of_reload:
            raise HTTPError(409, "zone {} already exists in the home catalog".format(zone_name))

        if not retry_of_reload:
            # Step 2: create the remote zone in the catalog.
            try:
                self.irods.mkzone(session, zone_name, connection_string)
                log.info("created remote zone %s (%s) in home catalog", zone_name, connection_string)
            except Exception as e:
                log.error("mkzone %s failed: %s", zone_name, e)
                raise HTTPError(500, "step mkzone failed: {}".format(e))

            # Step 3: append the federation entry to server_config.json.
            try:
                self.home.add_federation_entry(entry)
            except Exception as e:
                log.error("editing %s failed: %s; rolling back mkzone", self.home.path, e)
                try:
                    self.irods.rmzone(session, zone_name)
                except Exception as e2:
                    log.error("rollback rmzone %s failed: %s", zone_name, e2)
                    raise HTTPError(500, "step server_config failed: {}; rollback rmzone also failed: {}; "
                                         "manual cleanup required".format(e, e2))
                raise HTTPError(500, "step server_config failed: {}; mkzone rolled back".format(e))
        else:
            log.info("zone %s and its federation entry already present; retrying reload only", zone_name)

        # Step 4: reload the iRODS server configuration.
        try:
            self.home.reload_server()
        except Exception as e:
            log.error("reload failed: %s", e)
            raise HTTPError(500, "step reload failed: {}. The catalog zone and the federation entry "
                                 "are in place; fix the cause and judge again to retry the reload.".format(e))


# ---------------------------------------------------------------------------
# HTTP layer
# ---------------------------------------------------------------------------

class Handler(BaseHTTPRequestHandler):
    server_version = "fzrs/" + VERSION
    sys_version = ""
    protocol_version = "HTTP/1.1"

    @property
    def app(self):
        return self.server.app

    # -- plumbing -------------------------------------------------------------

    def log_message(self, fmt, *args):  # route http.server's own logging to ours
        log.debug("%s " + fmt, self.client_address[0], *args)

    def send_json(self, status, obj, headers=None):
        data = (json.dumps(obj, indent=2, sort_keys=True) + "\n").encode("utf-8")
        self.send_response(status)
        self.send_header("Content-Type", "application/json")
        self.send_header("Content-Length", str(len(data)))
        for k, v in (headers or {}).items():
            self.send_header(k, v)
        self.end_headers()
        self.wfile.write(data)

    def remote_address(self):
        forwarded = self.headers.get("X-Forwarded-For")
        if forwarded:
            return forwarded.split(",")[0].strip()
        return self.client_address[0]

    def read_json_body(self):
        length = self.headers.get("Content-Length")
        if length is None:
            raise HTTPError(400, "Content-Length is required")
        try:
            length = int(length)
        except ValueError:
            raise HTTPError(400, "Content-Length is not an integer")
        if length < 0:
            raise HTTPError(400, "Content-Length is negative")
        if length > self.app.config["max_request_body_bytes"]:
            raise HTTPError(413, "request body too large")
        raw = self.rfile.read(length)
        self._body_consumed = True
        try:
            return json.loads(raw.decode("utf-8"))
        except (UnicodeDecodeError, json.JSONDecodeError) as e:
            raise HTTPError(400, "request body is not valid JSON: {}".format(e))

    def discard_body(self):
        if getattr(self, "_body_consumed", False):
            return
        try:
            length = int(self.headers.get("Content-Length") or 0)
        except ValueError:
            length = 0
        if 0 < length <= self.app.config["max_request_body_bytes"]:
            self.rfile.read(length)
        self._body_consumed = True

    def basic_auth(self):
        """Returns (username, session). Raises HTTPError on failure."""
        challenge = {"WWW-Authenticate": 'Basic realm="fzrs"'}
        header = self.headers.get("Authorization")
        if not header:
            raise HTTPError(401, "authentication required", challenge)
        parts = header.split(None, 1)
        if len(parts) != 2 or parts[0].lower() != "basic":
            raise HTTPError(401, "only HTTP Basic authentication is supported", challenge)
        try:
            decoded = base64.b64decode(parts[1], validate=True).decode("utf-8")
        except (ValueError, UnicodeDecodeError):
            raise HTTPError(401, "malformed Authorization header", challenge)
        username, sep, password = decoded.partition(":")
        if not sep or not username or not password:
            raise HTTPError(401, "malformed Basic credentials", challenge)
        if "#" in username or not USERNAME_RE.match(username):
            raise HTTPError(401, "username must be a plain home-zone username", challenge)
        session = self.app.irods.admin_session(username, password)
        return username, session

    # -- dispatch -------------------------------------------------------------

    def dispatch(self, method):
        self._body_consumed = False
        parsed = urlsplit(self.path)
        path = parsed.path.rstrip("/") or "/"
        query = parse_qs(parsed.query, keep_blank_values=True)
        status = 500
        try:
            self.app.sweep()
            status, body, headers = self.route(method, path, query)
            self.send_json(status, body, headers)
        except HTTPError as e:
            status = e.status
            self.discard_body()
            self.send_json(e.status, {"error": e.message}, e.headers)
        except (BrokenPipeError, ConnectionResetError):
            log.info("client went away while serving %s %s", method, path)
        except Exception as e:
            log.exception("unhandled error serving %s %s", method, path)
            try:
                self.send_json(500, {"error": "internal error: {}".format(e)})
            except Exception:
                pass
        finally:
            log.info("%s %s %s -> %d", self.remote_address(), method, self.path, status)

    def route(self, method, path, query):
        routes = {
            ("POST", "/apply"): self.handle_apply,
            ("GET", "/status"): self.handle_status,
            ("GET", "/list"): self.handle_list,
            ("POST", "/judge"): self.handle_judge,
        }
        known_paths = {p for _, p in routes}
        if path not in known_paths:
            raise HTTPError(404, "no such endpoint")
        handler = routes.get((method, path))
        if handler is None:
            raise HTTPError(405, "method not allowed", {"Allow": ", ".join(
                m for m, p in routes if p == path)})
        return handler(query)

    def handle_apply(self, query):
        body = self.read_json_body()
        status, response = self.app.apply(body, self.remote_address())
        return status, response, None

    def handle_status(self, query):
        status, response = self.app.status(query)
        return status, response, None

    def handle_list(self, query):
        username, session = self.basic_auth()
        try:
            status, response = self.app.list_applications(query, username)
        finally:
            session.cleanup()
        return status, response, None

    def handle_judge(self, query):
        body = self.read_json_body()
        username, session = self.basic_auth()
        try:
            status, response = self.app.judge(body, username, session)
        finally:
            session.cleanup()
        return status, response, None

    def do_GET(self):
        self.dispatch("GET")

    def do_POST(self):
        self.dispatch("POST")

    def do_PUT(self):
        self.dispatch("PUT")

    def do_DELETE(self):
        self.dispatch("DELETE")


class Server(ThreadingHTTPServer):
    daemon_threads = True
    allow_reuse_address = True

    def __init__(self, address, app):
        super().__init__(address, Handler)
        self.app = app


# ---------------------------------------------------------------------------
# Entry point
# ---------------------------------------------------------------------------

def main(argv):
    logging.basicConfig(stream=sys.stdout, level=logging.INFO,
                        format="%(asctime)s %(levelname)s %(name)s: %(message)s")
    logging.getLogger("irods").setLevel(logging.WARNING)

    if len(argv) != 2 or argv[1] in ("-h", "--help"):
        print("usage: {} <config.json>".format(os.path.basename(argv[0])))
        return 2

    try:
        config = load_config(argv[1])
        app = App(config)
        app.start()
    except ConfigError as e:
        print("fzrs: {}".format(e))
        return 1
    except Exception as e:
        print("fzrs: startup failed: {}".format(e))
        return 1

    try:
        server = Server((config["bind_address"], config["port"]), app)
    except OSError as e:
        print("fzrs: cannot listen on {}:{}: {}".format(config["bind_address"], config["port"], e))
        return 1

    def stop(signum, frame):
        log.info("received signal %d, shutting down", signum)
        threading.Thread(target=server.shutdown, daemon=True).start()

    signal.signal(signal.SIGTERM, stop)
    signal.signal(signal.SIGINT, stop)

    log.warning("serving plain HTTP on %s:%d; deploy behind a TLS-terminating proxy",
                config["bind_address"], config["port"])
    try:
        server.serve_forever()
    finally:
        server.server_close()
    log.info("stopped")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
