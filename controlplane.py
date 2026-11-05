#!/usr/bin/env python3
"""iRODS Control Plane Service.

A single-file HTTP service that fronts one iRODS zone (the "home zone") and
runs as that zone's service account on a catalog provider. It offers three
features:

  federation  a remote zone applies to federate with the home zone, a
              rodsadmin approves or rejects, and on approval the service
              performs the home-zone side (mkzone, server_config.json entry,
              reload) and hands the applicant its own federation stanza.
  keyserver   a rodsadmin reads the lifecycle of the home zone's zone_key
              (current key, its expiry, the already-minted next key).
  bootstrap   a rodsadmin creates a single-use bootstrap key bound to a
              catalog service role; whoever redeems it within its lifetime
              receives the zone facts needed to run setup_irods.py and join
              the zone as a catalog consumer or an additional provider.
              Creating the key is the approval.

Usage:

    python3 controlplane.py /path/to/controlplane.json

The service speaks plain HTTP only and must be deployed behind a
TLS-terminating reverse proxy. It assumes the channel to every client is
already encrypted: admin credentials travel in HTTP Basic headers and every
applicant-facing secret travels as a plain JSON field. The service does no
cryptography of its own beyond generating random keys and ids.

Dependencies: python-irodsclient.

Endpoints:

    GET  /info                                   unauthenticated
    POST /federation/apply                       unauthenticated
    GET  /federation/status?application_id=<id>  unauthenticated
    GET  /federation/list?status=<filter>        rodsadmin
    POST /federation/judge                       rodsadmin
    GET  /keyserver/local_zone                   rodsadmin
    POST /bootstrap/create                       rodsadmin
    POST /bootstrap/redeem                       unauthenticated
    POST /bootstrap/revoke                       rodsadmin
    GET  /bootstrap/list?status=<filter>         rodsadmin

See controlplane-spec.md for the full specification.
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

SERVICE_NAME = "controlplane"
VERSION = "0.1.0"
AUTH_CHALLENGE = {"WWW-Authenticate": 'Basic realm="controlplane"'}

ZONE_NAME_RE = re.compile(r"^[A-Za-z0-9_.]{1,63}$")
ZONE_KEY_RE = re.compile(r"^[A-Za-z0-9_]{1,49}$")
NEGOTIATION_KEY_RE = re.compile(r"^[A-Za-z0-9_]{32}$")
HOSTNAME_LABEL_RE = re.compile(r"^(?!-)[A-Za-z0-9-]{1,63}(?<!-)$")
USERNAME_RE = re.compile(r"^[A-Za-z0-9_.@-]{1,63}$")
UUID_RE = re.compile(r"^[0-9a-f]{8}-[0-9a-f]{4}-4[0-9a-f]{3}-[89ab][0-9a-f]{3}-[0-9a-f]{12}$")

KEY_ALPHABET = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_"
KEY_LENGTH = 32

ZONE_KEY_SIGNING_HASH_SCHEME = "sha256"

STATUS_PENDING = "pending"
STATUS_APPROVED = "approved"
STATUS_REJECTED = "rejected"
STATUS_EXPIRED = "expired"
ALL_STATUSES = (STATUS_PENDING, STATUS_APPROVED, STATUS_REJECTED, STATUS_EXPIRED)

KEY_AVAILABLE = "available"
KEY_REDEEMED = "redeemed"
KEY_EXPIRED = "expired"
KEY_REVOKED = "revoked"
ALL_KEY_STATUSES = (KEY_AVAILABLE, KEY_REDEEMED, KEY_EXPIRED, KEY_REVOKED)

ROLES = ("consumer", "provider")

DECISION_NOTE_MAX_LENGTH = 1024
NOTE_MAX_LENGTH = 1024
NAME_MAX_LENGTH = 256
EMAIL_MAX_LENGTH = 254

STATE_SCHEMA_VERSION = 1

# Zone-wide keys of server_config.json copied verbatim into bootstrap zone facts.
ZONE_FACT_KEYS = (
    "schema_name", "schema_version", "zone_name", "zone_port", "zone_user", "zone_auth_scheme",
    "zone_key", "negotiation_key", "catalog_provider_hosts", "client_server_policy",
    "default_hash_scheme", "match_hash_policy", "encryption",
)

DEFAULT_PORT = 1248  # one above the conventional iRODS zone port

CONFIG_DEFAULTS = {
    "port": DEFAULT_PORT,
    "bind_address": "0.0.0.0",
    "state_file": None,  # resolved to <config dir>/controlplane-state.json
    "max_request_body_bytes": 65536,
    "irods_environment_file": None,  # resolved from env var / ~/.irods
    "server_config_file": "/etc/irods/server_config.json",
    "server_pid_file": "/var/run/irods/irods-server.pid",
}
FEATURE_DEFAULTS = {
    "federation": {
        "application_expiration_in_seconds": 7 * 24 * 60 * 60,
        "max_pending_applications": 100,
    },
    "keyserver": {
        "lifetime_of_zone_keys_in_seconds": 90 * 24 * 60 * 60,
        "lifetime_of_bootstrap_keys_in_seconds": 60 * 60,
    },
}
CONFIG_KNOWN_KEYS = set(CONFIG_DEFAULTS) | set(FEATURE_DEFAULTS)

log = logging.getLogger("controlplane")


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


def plus_seconds(dt, seconds):
    return dt + datetime.timedelta(seconds=seconds)


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
    fd, tmp = tempfile.mkstemp(prefix=".controlplane-", suffix=".tmp", dir=directory)
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

    cfg = dict(CONFIG_DEFAULTS)
    cfg.update({k: v for k, v in raw.items() if k not in FEATURE_DEFAULTS})
    for feature, defaults in FEATURE_DEFAULTS.items():
        given = raw.get(feature, {})
        if not isinstance(given, dict):
            raise ConfigError("{} must be a JSON object".format(feature))
        unknown = sorted(set(given) - set(defaults))
        if unknown:
            raise ConfigError("unknown {} configuration key(s): {}".format(feature, ", ".join(unknown)))
        section = dict(defaults)
        section.update(given)
        cfg[feature] = section
    config_dir = os.path.dirname(os.path.abspath(path))

    def positive_int(section, key, maximum=None):
        v = section[key]
        if not is_int(v) or v <= 0 or (maximum is not None and v > maximum):
            raise ConfigError("{} must be an integer in [1, {}]".format(key, maximum or "inf"))

    positive_int(cfg, "port", 65535)
    positive_int(cfg, "max_request_body_bytes")
    if not isinstance(cfg["bind_address"], str) or not cfg["bind_address"]:
        raise ConfigError("bind_address must be a non-empty string")
    positive_int(cfg["federation"], "application_expiration_in_seconds")
    positive_int(cfg["federation"], "max_pending_applications")
    positive_int(cfg["keyserver"], "lifetime_of_zone_keys_in_seconds")
    positive_int(cfg["keyserver"], "lifetime_of_bootstrap_keys_in_seconds")

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

    resolve_path("state_file", "controlplane-state.json")
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
# State (spec section 5.1)
# ---------------------------------------------------------------------------

class State:
    """All persistent state, kept in memory and written atomically as one JSON file.

    Callers must hold self.lock around any read-modify-write sequence.
    """

    def __init__(self, path):
        self.path = path
        self.lock = threading.Lock()
        self.federation = {}      # application_id -> record, insertion ordered
        self.bootstrap_keys = {}  # bootstrap_key -> record
        self.keyserver = {}       # spec 5.4, empty until adopted

    def load(self):
        if not os.path.exists(self.path):
            self.save()
            return
        with open(self.path, "r") as f:
            data = json.load(f)  # a corrupt file is a startup error
        try:
            if data["schema_version"] != STATE_SCHEMA_VERSION:
                raise ValueError("schema_version")
            self.federation = {r["application_id"]: r for r in data["federation"]["applications"]}
            self.bootstrap_keys = {r["bootstrap_key"]: r for r in data["bootstrap"]["keys"]}
            self.keyserver = data["keyserver"]
            if not isinstance(self.keyserver, dict):
                raise ValueError("keyserver")
        except (KeyError, TypeError, ValueError):
            raise ConfigError("state file {} has an unexpected format".format(self.path))

    def save(self):
        atomic_write_json(self.path, {
            "schema_version": STATE_SCHEMA_VERSION,
            "federation": {"applications": list(self.federation.values())},
            "bootstrap": {"keys": list(self.bootstrap_keys.values())},
            "keyserver": self.keyserver,
        }, mode=0o600)

    def expire(self, now):
        """Mark overdue pending applications and available keys expired. Returns True if changed."""
        changed = False
        for record in self.federation.values():
            if record["status"] == STATUS_PENDING and from_iso(record["expires_at"]) <= now:
                record["status"] = STATUS_EXPIRED
                changed = True
        for key in self.bootstrap_keys.values():
            if key["status"] == KEY_AVAILABLE and from_iso(key["expires_at"]) <= now:
                key["status"] = KEY_EXPIRED
                changed = True
        if changed:
            self.save()
        return changed

    @staticmethod
    def count_pending(applications):
        return sum(1 for r in applications.values() if r["status"] == STATUS_PENDING)

    def find_pending_federation_by_zone_name(self, zone_name):
        for r in self.federation.values():
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
            raise HTTPError(401, "invalid iRODS credentials", AUTH_CHALLENGE)
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

    def zone_facts(self, role):
        """Zone-wide facts for a new server of the given role (spec 7.3.2), copied verbatim."""
        cfg = self.read()
        facts = {"catalog_service_role": role}
        for key in ZONE_FACT_KEYS:
            facts[key] = cfg[key]
        if role == "provider":
            facts["database"] = cfg["plugin_configuration"]["database"]
        return facts

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
        backup = "{}.controlplane-backup-{}".format(self.path, utc_now().strftime("%Y%m%dT%H%M%SZ"))
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
# Request validation
# ---------------------------------------------------------------------------

def validate_federation_apply(body):
    if not isinstance(body, dict):
        raise HTTPError(400, "request body must be a JSON object")
    allowed = {"zone_name", "zone_port", "catalog_provider_hosts", "zone_key", "name", "email"}
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

    name = body.get("name")
    if name is not None and (not isinstance(name, str) or not 1 <= len(name) <= NAME_MAX_LENGTH):
        raise HTTPError(400, "name must be a string of 1 to {} characters".format(NAME_MAX_LENGTH))

    email = body.get("email")
    if email is not None:
        local, sep, domain = email.partition("@") if isinstance(email, str) else ("", "", "")
        if (not isinstance(email, str) or not 3 <= len(email) <= EMAIL_MAX_LENGTH
                or not sep or not local or not domain or "@" in domain):
            raise HTTPError(400, "email must be a string of at most {} characters with exactly one @".format(
                EMAIL_MAX_LENGTH))

    return {
        "zone_name": zone_name,
        "zone_port": zone_port,
        "catalog_provider_hosts": list(hosts),
        "zone_key": zone_key,
        "name": name,
        "email": email,
    }


def validate_bootstrap_create(body):
    """Return (catalog_service_role, lifetime_in_seconds or None, note or None)."""
    if not isinstance(body, dict):
        raise HTTPError(400, "request body must be a JSON object")
    unknown = sorted(set(body) - {"catalog_service_role", "lifetime_in_seconds", "note"})
    if unknown:
        raise HTTPError(400, "unknown field(s): {}".format(", ".join(unknown)))
    role = body.get("catalog_service_role")
    if role not in ROLES:
        raise HTTPError(400, "catalog_service_role must be one of {}".format(", ".join(ROLES)))
    lifetime = body.get("lifetime_in_seconds")
    if lifetime is not None and (not is_int(lifetime) or lifetime <= 0):
        raise HTTPError(400, "lifetime_in_seconds must be a positive integer")
    note = body.get("note")
    if note is not None and (not isinstance(note, str) or len(note) > NOTE_MAX_LENGTH):
        raise HTTPError(400, "note must be a string of at most {} characters".format(NOTE_MAX_LENGTH))
    return role, lifetime, note


def validate_bootstrap_key_body(body):
    """Body of redeem and revoke: exactly one field, bootstrap_key."""
    if not isinstance(body, dict):
        raise HTTPError(400, "request body must be a JSON object")
    unknown = sorted(set(body) - {"bootstrap_key"})
    if unknown:
        raise HTTPError(400, "unknown field(s): {}".format(", ".join(unknown)))
    bootstrap_key = body.get("bootstrap_key")
    if bootstrap_key is None:
        raise HTTPError(400, "missing required field: bootstrap_key")
    if not isinstance(bootstrap_key, str) or not UUID_RE.match(bootstrap_key):
        raise HTTPError(400, "bootstrap_key must be a lower-case uuid4 string")
    return bootstrap_key


def validate_judge(body):
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
    return application_id, decision, note


def single_query_value(query, name, required):
    values = query.get(name, [])
    if required and (len(values) != 1 or not values[0]):
        raise HTTPError(400, "{} query parameter is required".format(name))
    if not required and len(values) > 1:
        raise HTTPError(400, "{} query parameter must be given at most once".format(name))
    return values[0] if values else None


def status_filter(query):
    wanted = single_query_value(query, "status", required=False) or STATUS_PENDING
    if wanted not in ALL_STATUSES + ("all",):
        raise HTTPError(400, "status must be one of {}".format(", ".join(ALL_STATUSES + ("all",))))
    return wanted


def key_status_filter(query):
    wanted = single_query_value(query, "status", required=False) or "all"
    if wanted not in ALL_KEY_STATUSES + ("all",):
        raise HTTPError(400, "status must be one of {}".format(", ".join(ALL_KEY_STATUSES + ("all",))))
    return wanted


# ---------------------------------------------------------------------------
# Application logic
# ---------------------------------------------------------------------------

class App:
    def __init__(self, config):
        self.config = config
        self.state = State(config["state_file"])
        env = load_irods_environment(config["irods_environment_file"])
        self.irods = IRODS(config["irods_environment_file"], env)
        self.home = HomeZoneConfig(config["server_config_file"], config["server_pid_file"])

    def start(self):
        self.state.load()
        # Fail fast if the home zone facts are unreadable.
        facts = self.home.facts()
        if facts["zone_name"] != self.irods.zone:
            raise ConfigError("zone_name in {} ({}) does not match irods_zone_name in {} ({})".format(
                self.home.path, facts["zone_name"], self.irods.env_file, self.irods.zone))
        log.info("home zone %s at %s:%d, service account %s", self.irods.zone, self.irods.host,
                 self.irods.port, self.irods.env["irods_user_name"])
        log.info("state file %s holds %d federation application(s) and %d bootstrap key(s)",
                 self.state.path, len(self.state.federation), len(self.state.bootstrap_keys))

    # -- shared helpers -------------------------------------------------------

    def sweep(self):
        with self.state.lock:
            if self.state.expire(utc_now()):
                log.info("expiration sweep marked applications or bootstrap keys expired")

    def feature_config(self, feature):
        return self.config[feature]

    def check_queue_room(self, applications, feature):
        """Caller holds the state lock."""
        if State.count_pending(applications) >= self.feature_config(feature)["max_pending_applications"]:
            raise HTTPError(503, "{} application queue is full".format(feature))

    def new_record(self, now, feature, remote_address):
        return {
            "application_id": str(uuid.uuid4()),
            "status": STATUS_PENDING,
            "submitted_at": to_iso(now),
            "expires_at": to_iso(plus_seconds(now, self.feature_config(feature)["application_expiration_in_seconds"])),
            "remote_address": remote_address,
        }

    @staticmethod
    def bookkeeping(record, *extra):
        keys = ("application_id", "status") + extra + ("submitted_at", "expires_at", "judged_at", "decision_note")
        return {k: record[k] for k in keys}

    def lookup_for_judge(self, applications, application_id):
        """Caller holds the state lock."""
        record = applications.get(application_id)
        if record is None:
            raise HTTPError(404, "no such application")
        if record["status"] in (STATUS_APPROVED, STATUS_REJECTED):
            raise HTTPError(409, "application has already been {}".format(record["status"]))
        if record["status"] == STATUS_EXPIRED:
            raise HTTPError(410, "application has expired")
        return record

    def finalize(self, record, status, admin, note):
        record["status"] = status
        record["judged_at"] = to_iso(utc_now())
        record["judged_by"] = admin
        record["decision_note"] = note
        self.state.save()

    def info(self):
        cfg = self.feature_config("federation")
        return 200, {
            "service": SERVICE_NAME,
            "version": VERSION,
            "federation": {"application_expiration_in_seconds": cfg["application_expiration_in_seconds"]},
        }

    # -- federation (spec 7.1) --------------------------------------------------

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

    def federation_apply(self, body, remote_address):
        req = validate_federation_apply(body)
        if req["zone_name"] == self.irods.zone:
            raise HTTPError(400, "zone_name must not be the home zone's name")
        now = utc_now()
        with self.state.lock:
            self.check_queue_room(self.state.federation, "federation")
            if self.state.find_pending_federation_by_zone_name(req["zone_name"]) is not None:
                raise HTTPError(409, "an application for zone {} is already pending".format(req["zone_name"]))
            record = self.new_record(now, "federation", remote_address)
            record.update({
                "zone_name": req["zone_name"],
                "zone_port": req["zone_port"],
                "catalog_provider_hosts": req["catalog_provider_hosts"],
                "zone_key": req["zone_key"] if req["zone_key"] is not None else generate_key(),
                "zone_key_generated": req["zone_key"] is None,
                "negotiation_key": generate_key(),
                "name": req["name"],
                "email": req["email"],
                "judged_at": None,
                "judged_by": None,
                "decision_note": None,
            })
            self.state.federation[record["application_id"]] = record
            self.state.save()
        log.info("federation application %s submitted for zone %s from %s",
                 record["application_id"], record["zone_name"], remote_address)
        return 201, {
            "application_id": record["application_id"],
            "status": record["status"],
            "expires_at": record["expires_at"],
            "negotiation_key": record["negotiation_key"],
            "zone_key": record["zone_key"],
            "status_url": "/federation/status?application_id={}".format(record["application_id"]),
        }

    def federation_status(self, query):
        application_id = single_query_value(query, "application_id", required=True)
        with self.state.lock:
            record = self.state.federation.get(application_id)
            if record is None:
                raise HTTPError(404, "no such application")
            record = dict(record)
        response = self.bookkeeping(record)
        if record["status"] == STATUS_APPROVED:
            response["applicant_federation_entry"] = self.applicant_federation_entry(record)
            response["applicant_mkzone_command"] = self.applicant_mkzone_command()
        return 200, response

    def federation_list(self, query, admin):
        wanted = status_filter(query)
        with self.state.lock:
            records = [dict(r) for r in self.state.federation.values()
                       if wanted == "all" or r["status"] == wanted]
        records.sort(key=lambda r: r["submitted_at"])
        log.info("%s listed %d federation application(s) with status=%s", admin, len(records), wanted)
        return 200, {"applications": records}

    def federation_judge(self, body, admin, session):
        application_id, decision, note = validate_judge(body)
        with self.state.lock:
            record = self.lookup_for_judge(self.state.federation, application_id)
            if decision == "reject":
                self.finalize(record, STATUS_REJECTED, admin, note)
                log.info("%s rejected federation application %s (zone %s)", admin, application_id,
                         record["zone_name"])
                return 200, {"application": dict(record)}

            self._federation_approve(record, session)
            self.finalize(record, STATUS_APPROVED, admin, note)
            log.info("%s approved federation application %s (zone %s)", admin, application_id,
                     record["zone_name"])
            return 200, {
                "application": dict(record),
                "home_federation_entry": self.home_federation_entry(record),
                "applicant_federation_entry": self.applicant_federation_entry(record),
                "applicant_mkzone_command": self.applicant_mkzone_command(),
            }

    def _federation_approve(self, record, session):
        """Run the home-zone side of federation. Caller holds the state lock."""
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

    # -- keyserver (spec 5.4, 7.2) ----------------------------------------------

    def reconcile_zone_key(self, now):
        """Bring the zone key state in line with server_config.json (spec 5.4). Caller holds the state lock."""
        lifetime = self.config["keyserver"]["lifetime_of_zone_keys_in_seconds"]
        current = self.home.read()["zone_key"]
        ks = self.state.keyserver

        def mint_next(expires_at):
            return {"value": generate_key(), "minted_at": to_iso(now),
                    "expires_at": to_iso(plus_seconds(expires_at, lifetime))}

        def adopt(level, why):
            issued_at = now
            expires_at = plus_seconds(issued_at, lifetime)
            self.state.keyserver = {
                "zone_key": {"value": current, "issued_at": to_iso(issued_at), "expires_at": to_iso(expires_at)},
                "next_zone_key": mint_next(expires_at),
            }
            self.state.save()
            log.log(level, "%s; adopted the zone_key in %s with expiry %s and minted a next key",
                    why, self.home.path, to_iso(expires_at))

        if not ks:
            adopt(logging.INFO, "no zone key state yet")
        elif current == ks["zone_key"]["value"]:
            pass
        elif current == ks["next_zone_key"]["value"]:
            adopt(logging.INFO, "zone key rotation detected: {} now holds the previously minted next key".format(
                self.home.path))
        else:
            adopt(logging.WARNING, "zone_key in {} matches neither the current nor the next key; it was "
                                   "changed by hand".format(self.home.path))
        return self.state.keyserver

    def keyserver_local_zone(self, admin):
        with self.state.lock:
            ks = json.loads(json.dumps(self.reconcile_zone_key(utc_now())))
        log.info("%s read the local zone key lifecycle", admin)
        return 200, {
            "zone_name": self.irods.zone,
            "lifetime_of_zone_keys_in_seconds": self.config["keyserver"]["lifetime_of_zone_keys_in_seconds"],
            "zone_key": ks["zone_key"],
            "next_zone_key": ks["next_zone_key"],
        }

    # -- bootstrap (spec 7.3) ---------------------------------------------------

    def bootstrap_create(self, body, admin):
        role, lifetime, note = validate_bootstrap_create(body)
        if lifetime is None:
            lifetime = self.config["keyserver"]["lifetime_of_bootstrap_keys_in_seconds"]
        now = utc_now()
        with self.state.lock:
            self.reconcile_zone_key(now)
            key = {
                "bootstrap_key": str(uuid.uuid4()),
                "status": KEY_AVAILABLE,
                "catalog_service_role": role,
                "note": note,
                "created_at": to_iso(now),
                "created_by": admin,
                "expires_at": to_iso(plus_seconds(now, lifetime)),
                "redeemed_at": None,
                "redeemed_from": None,
                "revoked_at": None,
                "revoked_by": None,
            }
            self.state.bootstrap_keys[key["bootstrap_key"]] = key
            self.state.save()
            key = dict(key)
        log.info("%s created a %s bootstrap key expiring %s", admin, role, key["expires_at"])
        return 201, key

    def bootstrap_redeem(self, body, remote_address):
        bootstrap_key = validate_bootstrap_key_body(body)
        now = utc_now()
        with self.state.lock:
            self.reconcile_zone_key(now)
            key = self.state.bootstrap_keys.get(bootstrap_key)
            if key is None or key["status"] in (KEY_REDEEMED, KEY_REVOKED):
                log.warning("refused a bootstrap redemption from %s: key %s", remote_address,
                            "unknown" if key is None else key["status"])
                raise HTTPError(403, "bootstrap key is not valid")
            if key["status"] == KEY_EXPIRED:
                log.warning("refused a bootstrap redemption from %s: key expired", remote_address)
                raise HTTPError(403, "bootstrap key has expired")
            # Read the facts before consuming the key, so an unreadable or
            # incomplete server_config.json leaves the key available.
            facts = self.home.zone_facts(key["catalog_service_role"])
            key["status"] = KEY_REDEEMED
            key["redeemed_at"] = to_iso(now)
            key["redeemed_from"] = remote_address
            self.state.save()
            role = key["catalog_service_role"]
        log.info("bootstrap key for a %s redeemed from %s; zone facts released", role, remote_address)
        return 200, {"zone_facts": facts}

    def bootstrap_revoke(self, body, admin):
        bootstrap_key = validate_bootstrap_key_body(body)
        now = utc_now()
        with self.state.lock:
            self.reconcile_zone_key(now)
            key = self.state.bootstrap_keys.get(bootstrap_key)
            if key is None:
                raise HTTPError(404, "no such bootstrap key")
            if key["status"] != KEY_AVAILABLE:
                raise HTTPError(409, "bootstrap key is already {}".format(key["status"]))
            key["status"] = KEY_REVOKED
            key["revoked_at"] = to_iso(now)
            key["revoked_by"] = admin
            self.state.save()
            key = dict(key)
        log.info("%s revoked a %s bootstrap key", admin, key["catalog_service_role"])
        return 200, {"bootstrap_key": key}

    def bootstrap_list(self, query, admin):
        wanted = key_status_filter(query)
        with self.state.lock:
            self.reconcile_zone_key(utc_now())
            keys = [dict(k) for k in self.state.bootstrap_keys.values()
                    if wanted == "all" or k["status"] == wanted]
        keys.sort(key=lambda k: k["created_at"])
        log.info("%s listed %d bootstrap key(s) with status=%s", admin, len(keys), wanted)
        return 200, {"bootstrap_keys": keys}


# ---------------------------------------------------------------------------
# HTTP layer
# ---------------------------------------------------------------------------

class Handler(BaseHTTPRequestHandler):
    server_version = SERVICE_NAME + "/" + VERSION
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
        header = self.headers.get("Authorization")
        if not header:
            raise HTTPError(401, "authentication required", AUTH_CHALLENGE)
        parts = header.split(None, 1)
        if len(parts) != 2 or parts[0].lower() != "basic":
            raise HTTPError(401, "only HTTP Basic authentication is supported", AUTH_CHALLENGE)
        try:
            decoded = base64.b64decode(parts[1], validate=True).decode("utf-8")
        except (ValueError, UnicodeDecodeError):
            raise HTTPError(401, "malformed Authorization header", AUTH_CHALLENGE)
        username, sep, password = decoded.partition(":")
        if not sep or not username or not password:
            raise HTTPError(401, "malformed Basic credentials", AUTH_CHALLENGE)
        if "#" in username or not USERNAME_RE.match(username):
            raise HTTPError(401, "username must be a plain home-zone username", AUTH_CHALLENGE)
        session = self.app.irods.admin_session(username, password)
        return username, session

    def as_admin(self, func, *args):
        """Authenticate, run func(username, session, *args), clean up the session."""
        username, session = self.basic_auth()
        try:
            return func(username, session, *args)
        finally:
            session.cleanup()

    # -- dispatch -------------------------------------------------------------

    ROUTES = {
        ("GET", "/info"): "handle_info",
        ("POST", "/federation/apply"): "handle_federation_apply",
        ("GET", "/federation/status"): "handle_federation_status",
        ("GET", "/federation/list"): "handle_federation_list",
        ("POST", "/federation/judge"): "handle_federation_judge",
        ("GET", "/keyserver/local_zone"): "handle_keyserver_local_zone",
        ("POST", "/bootstrap/create"): "handle_bootstrap_create",
        ("POST", "/bootstrap/redeem"): "handle_bootstrap_redeem",
        ("POST", "/bootstrap/revoke"): "handle_bootstrap_revoke",
        ("GET", "/bootstrap/list"): "handle_bootstrap_list",
    }

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
        known_paths = {p for _, p in self.ROUTES}
        if path not in known_paths:
            raise HTTPError(404, "no such endpoint")
        name = self.ROUTES.get((method, path))
        if name is None:
            raise HTTPError(405, "method not allowed", {"Allow": ", ".join(
                m for m, p in self.ROUTES if p == path)})
        return getattr(self, name)(query)

    def handle_info(self, query):
        status, response = self.app.info()
        return status, response, {"Cache-Control": "no-store"}

    def handle_federation_apply(self, query):
        body = self.read_json_body()
        status, response = self.app.federation_apply(body, self.remote_address())
        return status, response, None

    def handle_federation_status(self, query):
        return self.app.federation_status(query) + (None,)

    def handle_federation_list(self, query):
        return self.as_admin(lambda user, session: self.app.federation_list(query, user)) + (None,)

    def handle_federation_judge(self, query):
        body = self.read_json_body()
        return self.as_admin(lambda user, session: self.app.federation_judge(body, user, session)) + (None,)

    def handle_keyserver_local_zone(self, query):
        status, response = self.as_admin(lambda user, session: self.app.keyserver_local_zone(user))
        return status, response, {"Cache-Control": "no-store"}

    def handle_bootstrap_create(self, query):
        body = self.read_json_body()
        return self.as_admin(lambda user, session: self.app.bootstrap_create(body, user)) + (None,)

    def handle_bootstrap_redeem(self, query):
        body = self.read_json_body()
        status, response = self.app.bootstrap_redeem(body, self.remote_address())
        return status, response, None

    def handle_bootstrap_revoke(self, query):
        body = self.read_json_body()
        return self.as_admin(lambda user, session: self.app.bootstrap_revoke(body, user)) + (None,)

    def handle_bootstrap_list(self, query):
        return self.as_admin(lambda user, session: self.app.bootstrap_list(query, user)) + (None,)

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
    request_queue_size = 128  # listen backlog; socketserver's default of 5 drops connections under a burst

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
        print("{}: {}".format(SERVICE_NAME, e))
        return 1
    except Exception as e:
        print("{}: startup failed: {}".format(SERVICE_NAME, e))
        return 1

    try:
        server = Server((config["bind_address"], config["port"]), app)
    except OSError as e:
        print("{}: cannot listen on {}:{}: {}".format(SERVICE_NAME, config["bind_address"], config["port"], e))
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
