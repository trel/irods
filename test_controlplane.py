#!/usr/bin/env python3
"""Tests for controlplane.py, the iRODS Control Plane Service.

How to run
----------

python-irodsclient must be importable. If it is not installed, point
PYTHONPATH at a checkout of it:

    export PYTHONPATH=/src/python-irodsclient

The suite also needs PyYAML and jsonschema (python3-yaml, python3-jsonschema)
to check every request and response against controlplane-openapi.yaml.

Run the suite with either unittest or pytest:

    python3 -m unittest -v test_controlplane
    python3 -m pytest -v test_controlplane.py

With coverage (python3-coverage):

    python3 -m coverage run --branch --include=controlplane.py -m pytest -q test_controlplane.py
    python3 -m coverage report -m

The HTTP tests start the real control plane server in-process on an ephemeral
port and drive it with http.client. iRODS is replaced by a fake, so no live zone is
needed. Reload is exercised against a real child process that records the
SIGHUP it receives.

Every request the suite sends through the fixture and every response it gets
back is checked against controlplane-openapi.yaml: the method and path must be
declared, the status code must be declared for that operation, declared
headers must be present, and the JSON body must validate against the declared
schema. A response the contract does not describe fails the test that
produced it, so controlplane.py and controlplane-openapi.yaml cannot drift
apart silently.

Live tests against a running zone are skipped unless both of these are set:

    CONTROLPLANE_LIVE_IRODS_ENV_FILE   path to the service account irods_environment.json
    CONTROLPLANE_LIVE_ADMIN_PASSWORD   password of the irods_user_name in that file

and one more live test runs when a rodsuser is also provided:

    CONTROLPLANE_LIVE_RODSUSER / CONTROLPLANE_LIVE_RODSUSER_PASSWORD

The live tests must run as the iRODS service account, because
python-irodsclient looks for the .irodsA password file in the running user's
home directory. For example:

    su - irods -c 'cd /src/irods && PYTHONPATH=/src/python-irodsclient \
        CONTROLPLANE_LIVE_IRODS_ENV_FILE=/var/lib/irods/.irods/irods_environment.json \
        CONTROLPLANE_LIVE_ADMIN_PASSWORD=... python3 -m pytest -q test_controlplane.py -k LiveIRODS'
"""

import base64
import copy
import http.client
import json
import os
import signal
import socket
import subprocess
import sys
import tempfile
import threading
import time
import unittest
import unittest.mock
import uuid
from urllib.parse import parse_qs, urlsplit

import irods.exception
import jsonschema
import yaml

import controlplane

HOME_ZONE = "tempZone"
HOME_ZONE_KEY = "HOME_ZONE_KEY"
HOME_HOST = "icat.home.example.org"

HUP_CHILD = (
    "import signal, sys, time\n"
    "path = sys.argv[1]\n"
    "def h(*a):\n"
    "    open(path, 'a').write('HUP\\n')\n"
    "signal.signal(signal.SIGHUP, h)\n"
    "open(path, 'a').write('READY\\n')\n"
    "while True:\n"
    "    time.sleep(0.05)\n"
)


# ---------------------------------------------------------------------------
# Fakes
# ---------------------------------------------------------------------------

HOST = "rs01.example.org"        # what a redeeming server calls itself
ACCOUNT = "adm." + HOST          # the account the default prefix makes of it


class FakeSession:
    def __init__(self):
        self.cleaned_up = False

    def cleanup(self):
        self.cleaned_up = True


class FakeIRODS:
    """Stands in for controlplane.IRODS. Users map name -> (password, type)."""

    def __init__(self, zone=HOME_ZONE):
        self._zone = zone
        self.env = {"irods_host": HOME_HOST, "irods_port": 1247,
                    "irods_zone_name": zone, "irods_user_name": "rods"}
        self.env_file = "/nonexistent/irods_environment.json"
        self.users = {"rods": ("rods", "rodsadmin"), "alice": ("alicepass", "rodsuser")}
        self.zones = {}  # name -> connection string
        self.sessions = []
        self.mkzone_error = None
        self.rmzone_error = None
        self.created = []           # service accounts created, in order
        self.create_error = None    # HTTPError to raise from create_service_account
        self.remove_error = None    # message to raise from remove_service_account

    host = property(lambda self: self.env["irods_host"])
    port = property(lambda self: self.env["irods_port"])
    zone = property(lambda self: self._zone)

    def admin_session(self, username, password):
        if username == "unreachable":
            raise controlplane.HTTPError(502, "cannot reach iRODS: fake outage")
        entry = self.users.get(username)
        if entry is None or entry[0] != password:
            raise controlplane.HTTPError(401, "invalid iRODS credentials",
                                 {"WWW-Authenticate": 'Basic realm="controlplane"'})
        if entry[1] != "rodsadmin":
            raise controlplane.HTTPError(403, "user {} is not a rodsadmin".format(username))
        session = FakeSession()
        self.sessions.append(session)
        return session

    def zone_exists(self, session, zone_name):
        return zone_name in self.zones

    def mkzone(self, session, zone_name, connection_string):
        if self.mkzone_error:
            raise RuntimeError(self.mkzone_error)
        self.zones[zone_name] = connection_string

    def rmzone(self, session, zone_name):
        if self.rmzone_error:
            raise RuntimeError(self.rmzone_error)
        del self.zones[zone_name]

    def create_service_account(self, name, password):
        if self.create_error is not None:
            raise self.create_error
        if name in self.users:
            raise controlplane.HTTPError(409, "user {} already exists".format(name))
        self.users[name] = (password, "rodsadmin")
        self.created.append(name)

    def remove_service_account(self, name):
        if self.remove_error:
            raise RuntimeError(self.remove_error)
        del self.users[name]

    def service_accounts(self):
        return sorted(name for name, (_, t) in self.users.items() if name not in ("rods", "alice"))


# ---------------------------------------------------------------------------
# Shared fixture
# ---------------------------------------------------------------------------

def write_json(path, obj):
    with open(path, "w") as f:
        json.dump(obj, f)


def raw_http(port, request_bytes):
    """Send raw bytes and return the whole response (HTTP/1.0, server closes)."""
    s = socket.create_connection(("127.0.0.1", port), timeout=10)
    s.sendall(request_bytes)
    chunks = []
    while True:
        chunk = s.recv(4096)
        if not chunk:
            break
        chunks.append(chunk)
    s.close()
    return b"".join(chunks).decode()


OPENAPI_FILE = os.path.join(os.path.dirname(os.path.abspath(__file__)), "controlplane-openapi.yaml")
HTTP_METHODS = ("get", "put", "post", "delete", "options", "head", "patch", "trace")


class OpenAPIContract:
    """Checks live traffic against controlplane-openapi.yaml (spec section 15).

    OpenAPI 3.0 schemas are JSON Schema draft 4 plus `nullable`, so the
    document is translated once: `nullable: true` becomes a `null` member
    of `type` (and of `enum` when present). Everything else validates
    unchanged with jsonschema's Draft4Validator.
    """

    def __init__(self, path=OPENAPI_FILE):
        with open(path) as f:
            self.doc = yaml.safe_load(f)
        self.root = self._to_draft4(copy.deepcopy(self.doc))
        self.resolver = jsonschema.RefResolver.from_schema(self.root)

    @classmethod
    def _to_draft4(cls, node):
        if isinstance(node, dict):
            if node.pop("nullable", False) and "type" in node:
                node["type"] = [node["type"], "null"]
                if "enum" in node:
                    node["enum"] = list(node["enum"]) + [None]
            for value in node.values():
                cls._to_draft4(value)
        elif isinstance(node, list):
            for value in node:
                cls._to_draft4(value)
        return node

    def deref(self, obj):
        while isinstance(obj, dict) and "$ref" in obj:
            obj = self.resolver.resolve(obj["$ref"])[1]
        return obj

    def validate(self, instance, schema, what):
        validator = jsonschema.Draft4Validator(schema, resolver=self.resolver)
        errors = sorted(validator.iter_errors(instance), key=lambda e: list(e.path))
        if errors:
            raise AssertionError("{} does not match controlplane-openapi.yaml:\n  {}\n  instance: {}".format(
                what, "\n  ".join(e.message for e in errors[:3]), json.dumps(instance)[:400]))

    def operation(self, method, path):
        """Return (path item, operation) for a request, either of which may be None."""
        path = urlsplit(path).path.rstrip("/") or "/"
        item = self.root["paths"].get(path)
        return item, (item or {}).get(method.lower())

    def check(self, method, path, status, body, headers, request_body=None):
        """Assert that one exchange is described by the contract."""
        what = "{} {} -> {}".format(method, path, status)
        headers = {k.lower(): v for k, v in headers.items()}
        error = self.root["components"]["schemas"]["Error"]
        item, op = self.operation(method, path)
        if item is None:
            assert status == 404, "{}: undeclared paths must answer 404".format(what)
            return self.validate(body, error, what)
        if op is None:
            assert status == 405, "{}: undeclared methods must answer 405".format(what)
            allowed = ", ".join(m.upper() for m in item if m in HTTP_METHODS)
            assert headers.get("allow") == allowed, "{}: Allow header is {!r}, expected {!r}".format(
                what, headers.get("allow"), allowed)
            return self.validate(body, error, what)

        response = op["responses"].get(str(status))
        assert response is not None, "{}: controlplane-openapi.yaml does not declare {} for this operation".format(
            what, status)
        response = self.deref(response)
        for name, header in response.get("headers", {}).items():
            assert name.lower() in headers, "{}: declared header {} is missing".format(what, name)
            self.validate(headers[name.lower()], header["schema"], "{} header {}".format(what, name))
        assert headers.get("content-type") == "application/json", what
        self.validate(body, response["content"]["application/json"]["schema"], what + " body")

        if 200 <= status < 300:
            # a request the service accepted must itself be what the contract describes
            if request_body is not None and "requestBody" in op:
                schema = op["requestBody"]["content"]["application/json"]["schema"]
                self.validate(request_body, schema, what + " request body")
            query = parse_qs(urlsplit(path).query, keep_blank_values=True)
            for param in op.get("parameters", []):
                values = query.get(param["name"], [])
                if param.get("required"):
                    assert len(values) == 1, "{}: query parameter {} is required".format(what, param["name"])
                for value in values:
                    self.validate(value, param["schema"], "{} query parameter {}".format(what, param["name"]))

    def examples(self):
        """Yield (description, example value, schema) for every example in the document."""
        for name, schema in self.root["components"]["schemas"].items():
            if "example" in schema:
                yield "components.schemas.{}".format(name), schema["example"], schema
        containers = [("components.responses.{}".format(n), r)
                      for n, r in self.root["components"]["responses"].items()]
        for path, item in self.root["paths"].items():
            for method, op in item.items():
                if method not in HTTP_METHODS:
                    continue
                where = "{} {}".format(method.upper(), path)
                if "requestBody" in op:
                    containers.append((where + " request", op["requestBody"]))
                for code, response in op["responses"].items():
                    if "$ref" not in response:
                        containers.append(("{} -> {}".format(where, code), response))
        for where, container in containers:
            media = container["content"]["application/json"]
            if "example" in media:
                yield where + " example", media["example"], media["schema"]
            for name, example in media.get("examples", {}).items():
                yield "{} example {}".format(where, name), example["value"], media["schema"]


CONTRACT = OpenAPIContract()


HOME_NEGOTIATION_KEY = "home_negotiation_key_never_shown"
HOME_DATABASE = {"technology": "postgres", "host": "localhost", "port": 5432, "name": "ICAT",
                 "odbc_driver": "PostgreSQL ANSI", "username": "irods", "password": "db-secret"}


def home_server_config():
    """A server_config.json with everything the zone facts of spec 7.3.2 copy."""
    return {
        "schema_name": "server_config", "schema_version": "v5",
        "zone_name": HOME_ZONE, "zone_key": HOME_ZONE_KEY, "zone_port": 1247, "zone_user": "rods",
        "zone_auth_scheme": "native", "negotiation_key": HOME_NEGOTIATION_KEY,
        "catalog_provider_hosts": [HOME_HOST], "catalog_service_role": "provider", "host": HOME_HOST,
        "client_server_policy": "CS_NEG_REFUSE", "default_hash_scheme": "SHA256",
        "match_hash_policy": "compatible",
        "encryption": {"algorithm": "AES-256-CBC", "key_size": 32, "num_hash_rounds": 16, "salt_size": 8},
        "federation": [],
        "plugin_configuration": {"database": dict(HOME_DATABASE), "rule_engines": []},
        "server_port_range_start": 20000, "server_port_range_end": 20199,
    }


class Fixture:
    """Temp directory with config, env, server_config, HUP child, and running server."""

    def __init__(self, config_overrides=None, start_server=True):
        self.tmp = tempfile.TemporaryDirectory(prefix="controlplane-test-")
        d = self.tmp.name
        self.server_config_file = os.path.join(d, "server_config.json")
        self.env_file = os.path.join(d, "irods_environment.json")
        self.config_file = os.path.join(d, "controlplane.json")
        self.pid_file = os.path.join(d, "server.pid")
        self.hup_log = os.path.join(d, "hup.log")

        write_json(self.server_config_file, home_server_config())
        os.chmod(self.server_config_file, 0o600)
        write_json(self.env_file, {"irods_host": HOME_HOST, "irods_port": 1247,
                                   "irods_zone_name": HOME_ZONE, "irods_user_name": "rods"})

        self.child = subprocess.Popen([sys.executable, "-c", HUP_CHILD, self.hup_log])
        for _ in range(200):
            if os.path.exists(self.hup_log) and "READY" in open(self.hup_log).read():
                break
            time.sleep(0.01)
        with open(self.pid_file, "w") as f:
            f.write("{}\n".format(self.child.pid))

        config = {"port": 1, "irods_environment_file": self.env_file,
                  "server_config_file": self.server_config_file,
                  "server_pid_file": self.pid_file}
        config.update(config_overrides or {})
        write_json(self.config_file, config)

        self.config = controlplane.load_config(self.config_file)
        self.app = controlplane.App(self.config)
        self.irods = FakeIRODS()
        self.app.irods = self.irods
        self.app.start()

        self.server = None
        if start_server:
            self.server = controlplane.Server(("127.0.0.1", 0), self.app)
            self.port = self.server.server_address[1]
            self.thread = threading.Thread(target=self.server.serve_forever, daemon=True)
            self.thread.start()

    def close(self):
        if self.server is not None:
            self.server.shutdown()
            self.server.server_close()
        self.child.kill()
        self.child.wait()
        self.tmp.cleanup()

    # -- helpers --------------------------------------------------------------

    def hups(self):
        return open(self.hup_log).read().count("HUP")

    def server_config(self):
        with open(self.server_config_file) as f:
            return json.load(f)

    def request(self, method, path, body=None, auth=None, headers=None, raw_body=None):
        conn = http.client.HTTPConnection("127.0.0.1", self.port, timeout=10)
        hdrs = dict(headers or {})
        if auth is not None:
            token = base64.b64encode("{}:{}".format(*auth).encode()).decode()
            hdrs["Authorization"] = "Basic " + token
        data = raw_body
        if body is not None:
            data = json.dumps(body).encode()
            hdrs["Content-Type"] = "application/json"
        conn.request(method, path, body=data, headers=hdrs)
        resp = conn.getresponse()
        payload = resp.read()
        conn.close()
        try:
            decoded = json.loads(payload.decode()) if payload else None
        except ValueError:
            decoded = payload
        hdrs_out = dict(resp.getheaders())
        CONTRACT.check(method, path, resp.status, decoded, hdrs_out, request_body=body)
        return resp.status, decoded, hdrs_out

    def apply(self, zone_name="otherZone", hosts=("localhost",), port=1247, zone_key=None, **extra):
        body = {"zone_name": zone_name, "zone_port": port, "catalog_provider_hosts": list(hosts)}
        if zone_key is not None:
            body["zone_key"] = zone_key
        body.update(extra)
        return self.request("POST", "/federation/apply", body)

    def judge(self, application_id, decision, note=None, auth=("rods", "rods"), feature="federation"):
        body = {"application_id": application_id, "decision": decision}
        if note is not None:
            body["decision_note"] = note
        return self.request("POST", "/{}/judge".format(feature), body, auth=auth)

    def state(self):
        with open(self.config["state_file"]) as f:
            return json.load(f)

    # -- keyserver and bootstrap helpers --------------------------------------

    def local_zone(self, auth=("rods", "rods")):
        return self.request("GET", "/keyserver/local_zone", auth=auth)

    def create(self, role="consumer", auth=("rods", "rods"), **extra):
        body = {"catalog_service_role": role}
        body.update(extra)
        return self.request("POST", "/bootstrap/create", body, auth=auth)

    def new_key(self, role="consumer", **extra):
        status, body, _ = self.create(role, **extra)
        assert status == 201, (status, body)
        return body["bootstrap_key"]

    def redeem(self, bootstrap_key, host=HOST, **kwargs):
        return self.request("POST", "/bootstrap/redeem", {"bootstrap_key": bootstrap_key, "host": host}, **kwargs)

    def revoke(self, bootstrap_key, auth=("rods", "rods")):
        return self.request("POST", "/bootstrap/revoke", {"bootstrap_key": bootstrap_key}, auth=auth)

    def bootstrap_list(self, query="", auth=("rods", "rods")):
        return self.request("GET", "/bootstrap/list" + query, auth=auth)


def expected_facts(role, account=None):
    home = home_server_config()
    facts = {"catalog_service_role": role}
    facts.update({k: home[k] for k in controlplane.ZONE_FACT_KEYS})
    facts["zone_user"] = ACCOUNT if account is None else account
    if role == "provider":
        facts["database"] = dict(HOME_DATABASE)
    return facts


class ServerTestCase(unittest.TestCase):
    config_overrides = None

    def setUp(self):
        self.fx = Fixture(self.config_overrides)
        self.addCleanup(self.fx.close)


# ---------------------------------------------------------------------------
# Unit tests: helpers, config, queue
# ---------------------------------------------------------------------------

class HelperTests(unittest.TestCase):
    def test_generate_key_format(self):
        for _ in range(50):
            key = controlplane.generate_key()
            self.assertRegex(key, r"^[A-Za-z0-9_]{32}$")
            self.assertTrue(controlplane.NEGOTIATION_KEY_RE.match(key))
            self.assertTrue(controlplane.ZONE_KEY_RE.match(key))
        self.assertNotEqual(controlplane.generate_key(), controlplane.generate_key())

    def test_is_valid_host(self):
        for good in ("localhost", "icat.example.org", "a-b.c", "10.0.0.5", "::1", "x" * 63,
                     "icat.example.org."):
            self.assertTrue(controlplane.is_valid_host(good), good)
        for bad in ("", "-bad", "bad-", "a..b", "x" * 64, "a" * 254, "under_score", None, 5,
                    "host name", "host:1247"):
            self.assertFalse(controlplane.is_valid_host(bad), repr(bad))

    def test_is_int_rejects_bool(self):
        self.assertTrue(controlplane.is_int(0))
        self.assertFalse(controlplane.is_int(True))
        self.assertFalse(controlplane.is_int("1"))

    def test_iso_roundtrip(self):
        now = controlplane.utc_now()
        self.assertEqual(controlplane.from_iso(controlplane.to_iso(now)), now)
        self.assertEqual(now.microsecond, 0)

    def test_atomic_write_json_cleans_up_on_failure(self):
        with tempfile.TemporaryDirectory() as d:
            path = os.path.join(d, "out.json")
            with self.assertRaises(TypeError):
                controlplane.atomic_write_json(path, {"x": object()})
            self.assertEqual(os.listdir(d), [])
            controlplane.atomic_write_json(path, {"x": 1}, mode=0o600)
            self.assertEqual(oct(os.stat(path).st_mode & 0o777), "0o600")
            self.assertEqual(json.load(open(path)), {"x": 1})


class ConfigTests(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.addCleanup(self.tmp.cleanup)
        self.path = os.path.join(self.tmp.name, "controlplane.json")

    def load(self, obj):
        if isinstance(obj, str):
            with open(self.path, "w") as f:
                f.write(obj)
        else:
            write_json(self.path, obj)
        return controlplane.load_config(self.path)

    def test_defaults_and_relative_paths(self):
        os.environ.pop("IRODS_ENVIRONMENT_FILE", None)
        cfg = self.load({"state_file": "s.json"})
        self.assertEqual(cfg["port"], 1248)
        self.assertEqual(cfg["bind_address"], "0.0.0.0")
        self.assertEqual(cfg["max_request_body_bytes"], 65536)
        self.assertEqual(cfg["state_file"], os.path.join(self.tmp.name, "s.json"))
        self.assertEqual(cfg["server_config_file"], "/etc/irods/server_config.json")
        self.assertEqual(cfg["server_pid_file"], "/var/run/irods/irods-server.pid")
        self.assertTrue(cfg["irods_environment_file"].endswith("/.irods/irods_environment.json"))
        self.assertTrue(os.path.isabs(cfg["irods_environment_file"]))
        self.assertEqual(cfg["federation"], {"application_expiration_in_seconds": 7 * 24 * 3600,
                                             "max_pending_applications": 100})
        self.assertEqual(cfg["bootstrap"], {"default_lifetime_of_bootstrap_keys_in_seconds": 3600,
                                            "new_admin_account_prefix": "adm."})
        self.assertEqual(cfg["keyserver"], {"lifetime_of_zone_keys_in_seconds": 90 * 24 * 3600})

    def test_nested_overrides_merge_with_defaults(self):
        cfg = self.load({"port": 1, "federation": {"application_expiration_in_seconds": 60},
                         "bootstrap": {"default_lifetime_of_bootstrap_keys_in_seconds": 5}})
        self.assertEqual(cfg["federation"], {"application_expiration_in_seconds": 60,
                                             "max_pending_applications": 100})
        self.assertEqual(cfg["bootstrap"], {"default_lifetime_of_bootstrap_keys_in_seconds": 5,
                                            "new_admin_account_prefix": "adm."})
        cfg = self.load({"port": 1, "bootstrap": {"new_admin_account_prefix": ""}})
        self.assertEqual(cfg["bootstrap"], {"default_lifetime_of_bootstrap_keys_in_seconds": 3600,
                                            "new_admin_account_prefix": ""})
        self.assertEqual(cfg["keyserver"], {"lifetime_of_zone_keys_in_seconds": 90 * 24 * 3600})

    def test_default_state_file_sits_next_to_config(self):
        cfg = self.load({})
        self.assertEqual(cfg["state_file"], os.path.join(self.tmp.name, "controlplane-state.json"))

    def test_env_var_selects_environment_file(self):
        os.environ["IRODS_ENVIRONMENT_FILE"] = "/some/where/env.json"
        try:
            self.assertEqual(self.load({"port": 1})["irods_environment_file"], "/some/where/env.json")
        finally:
            del os.environ["IRODS_ENVIRONMENT_FILE"]

    def test_errors(self):
        cases = [
            ({"bogus": 2}, "unknown configuration key"),
            ({"port": None}, "port must be"),
            ({"port": 1, "application_expiration_in_seconds": 1}, "unknown configuration key"),
            ({"port": 0}, "port must be"),
            ({"port": 70000}, "port must be"),
            ({"port": "9100"}, "port must be"),
            ({"port": 1, "federation": {"application_expiration_in_seconds": -1}}, "application_expiration"),
            ({"port": 1, "federation": {"max_pending_applications": True}}, "max_pending"),
            ({"port": 1, "federation": {"nope": 1}}, "unknown federation configuration key"),
            ({"port": 1, "bootstrap": {"max_pending_applications": 1}}, "unknown bootstrap configuration key"),
            ({"port": 1, "keyserver": {"application_expiration_in_seconds": 1}}, "unknown keyserver configuration key"),
            ({"port": 1, "keyserver": {"lifetime_of_zone_keys_in_seconds": 0}}, "lifetime_of_zone_keys"),
            ({"port": 1, "bootstrap": {"default_lifetime_of_bootstrap_keys_in_seconds": "1"}}, "default_lifetime_of_bootstrap_keys"),
            ({"port": 1, "bootstrap": {"new_admin_account_prefix": 5}}, "new_admin_account_prefix"),
            ({"port": 1, "bootstrap": {"new_admin_account_prefix": "a" * 17}}, "new_admin_account_prefix"),
            ({"port": 1, "bootstrap": {"new_admin_account_prefix": "adm "}}, "new_admin_account_prefix"),
            ({"port": 1, "bootstrap": {"new_admin_account_prefix": "adm@"}}, "new_admin_account_prefix"),
            ({"port": 1, "keyserver": {"default_lifetime_of_bootstrap_keys_in_seconds": 1}}, "unknown keyserver configuration key"),
            ({"port": 1, "keyserver": []}, "keyserver must be a JSON object"),
            ({"port": 1, "bind_address": ""}, "bind_address"),
            ({"port": 1, "state_file": 7}, "state_file"),
            ([1, 2], "JSON object"),
            ("not json", "not valid JSON"),
        ]
        for obj, fragment in cases:
            with self.assertRaises(controlplane.ConfigError, msg=repr(obj)) as cm:
                self.load(obj)
            self.assertIn(fragment, str(cm.exception))

    def test_missing_file(self):
        with self.assertRaises(controlplane.ConfigError):
            controlplane.load_config(os.path.join(self.tmp.name, "nope.json"))

    def test_irods_environment_errors(self):
        p = os.path.join(self.tmp.name, "env.json")
        with self.assertRaises(controlplane.ConfigError):
            controlplane.load_irods_environment(p)
        with open(p, "w") as f:
            f.write("{")
        with self.assertRaises(controlplane.ConfigError):
            controlplane.load_irods_environment(p)
        write_json(p, {"irods_host": "h"})
        with self.assertRaises(controlplane.ConfigError) as cm:
            controlplane.load_irods_environment(p)
        self.assertIn("irods_port", str(cm.exception))


class StateTests(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.addCleanup(self.tmp.cleanup)
        self.path = os.path.join(self.tmp.name, "state.json")

    def record(self, name, status="pending", expires=None):
        now = controlplane.utc_now()
        return {"application_id": name + "-id", "status": status, "zone_name": name,
                "submitted_at": controlplane.to_iso(now), "expires_at": controlplane.to_iso(expires or now)}

    def key(self, name, status="available", expires=None):
        now = controlplane.utc_now()
        return {"bootstrap_key": name, "status": status, "created_at": controlplane.to_iso(now),
                "expires_at": controlplane.to_iso(expires or now)}

    def test_creates_file_and_roundtrips(self):
        st = controlplane.State(self.path)
        st.load()
        self.assertTrue(os.path.exists(self.path))
        self.assertEqual(oct(os.stat(self.path).st_mode & 0o777), "0o600")
        self.assertEqual(json.load(open(self.path)), {
            "schema_version": 1, "federation": {"applications": []},
            "bootstrap": {"keys": []}, "keyserver": {}})
        st.federation["a-id"] = self.record("a")
        st.bootstrap_keys["k"] = self.key("k")
        st.keyserver = {"zone_key": {"value": "v"}}
        st.save()
        st2 = controlplane.State(self.path)
        st2.load()
        self.assertEqual(list(st2.federation), ["a-id"])
        self.assertEqual(list(st2.bootstrap_keys), ["k"])
        self.assertEqual(st2.keyserver, {"zone_key": {"value": "v"}})

    def test_corrupt_or_wrong_schema_is_an_error(self):
        with open(self.path, "w") as f:
            f.write("{nope")
        with self.assertRaises(ValueError):
            controlplane.State(self.path).load()
        for bad in ({"schema_version": 99}, {"schema_version": 1, "federation": {}},
                    {"schema_version": 1, "federation": {"applications": []},
                     "bootstrap": {"keys": []}, "keyserver": []}, [1]):
            write_json(self.path, bad)
            with self.assertRaises(controlplane.ConfigError, msg=repr(bad)):
                controlplane.State(self.path).load()

    def test_expire_and_counts(self):
        st = controlplane.State(self.path)
        st.load()
        later = controlplane.utc_now() + controlplane.datetime.timedelta(seconds=3600)
        st.federation["a-id"] = self.record("a")
        st.federation["b-id"] = self.record("b", expires=later)
        st.federation["c-id"] = self.record("c", status="approved")
        st.bootstrap_keys["old"] = self.key("old")
        st.bootstrap_keys["fresh"] = self.key("fresh", expires=later)
        st.bootstrap_keys["gone"] = self.key("gone", status="redeemed")
        st.bootstrap_keys["pulled"] = self.key("pulled", status="revoked")
        self.assertTrue(st.expire(controlplane.utc_now()))
        self.assertEqual([r["status"] for r in st.federation.values()], ["expired", "pending", "approved"])
        self.assertEqual([k["status"] for k in st.bootstrap_keys.values()],
                         ["expired", "available", "redeemed", "revoked"])
        self.assertFalse(st.expire(controlplane.utc_now()))
        self.assertEqual(controlplane.State.count_pending(st.federation), 1)
        self.assertIs(st.find_pending_federation_by_zone_name("b"), st.federation["b-id"])
        self.assertIsNone(st.find_pending_federation_by_zone_name("a"))


class ValidateFederationApplyTests(unittest.TestCase):
    def check(self, body, fragment):
        with self.assertRaises(controlplane.HTTPError) as cm:
            controlplane.validate_federation_apply(body)
        self.assertEqual(cm.exception.status, 400)
        self.assertIn(fragment, cm.exception.message)

    def test_rejections(self):
        base = {"zone_name": "z", "zone_port": 1247, "catalog_provider_hosts": ["h"]}
        self.check([], "JSON object")
        self.check(dict(base, extra=1), "unknown field")
        self.check({"zone_port": 1}, "missing required field: zone_name")
        self.check(dict(base, zone_name="bad zone"), "zone_name must match")
        self.check(dict(base, zone_name="x" * 64), "zone_name must match")
        self.check(dict(base, zone_port=True), "zone_port must be")
        self.check(dict(base, zone_port=-1), "zone_port must be")
        self.check(dict(base, catalog_provider_hosts=[]), "non-empty array")
        self.check(dict(base, catalog_provider_hosts="h"), "non-empty array")
        self.check(dict(base, catalog_provider_hosts=["h", "h"]), "duplicates")
        self.check(dict(base, catalog_provider_hosts=["-h"]), "invalid hostname")
        self.check(dict(base, zone_key="has space"), "zone_key must match")
        self.check(dict(base, zone_key="k" * 50), "zone_key must match")
        self.check(dict(base, zone_key=5), "zone_key must match")

    def test_accepts(self):
        out = controlplane.validate_federation_apply(
            {"zone_name": "a.b_c", "zone_port": 0, "catalog_provider_hosts": ["h", "10.0.0.1"]})
        self.assertEqual(out, {"zone_name": "a.b_c", "zone_port": 0,
                               "catalog_provider_hosts": ["h", "10.0.0.1"], "zone_key": None,
                               "name": None, "email": None})
        out = controlplane.validate_federation_apply(
            {"zone_name": "a", "zone_port": 65535, "catalog_provider_hosts": ["h"], "zone_key": "K_1",
             "name": "Ada", "email": "ada@example.org"})
        self.assertEqual(out["zone_key"], "K_1")
        self.assertEqual((out["name"], out["email"]), ("Ada", "ada@example.org"))

    def test_name_and_email_rules(self):
        base = {"zone_name": "z", "zone_port": 1247, "catalog_provider_hosts": ["h"]}
        self.check(dict(base, name=""), "name must be")
        self.check(dict(base, name=5), "name must be")
        self.check(dict(base, name="x" * 257), "name must be")
        for bad in ("", "a", "@", "a@", "@b", "no-at-sign", "a@@b", "a@b@c", 7, "x" * 250 + "@abcd"):
            self.check(dict(base, email=bad), "email must be")
        out = controlplane.validate_federation_apply(dict(base, name="x" * 256, email="a@b"))
        self.assertEqual((len(out["name"]), out["email"]), (256, "a@b"))

# ---------------------------------------------------------------------------
# HTTP: apply and status
# ---------------------------------------------------------------------------

class ApplyTests(ServerTestCase):
    def test_apply_generates_keys(self):
        status, body, _ = self.fx.apply()
        self.assertEqual(status, 201)
        self.assertEqual(body["status"], "pending")
        self.assertRegex(body["negotiation_key"], r"^[A-Za-z0-9_]{32}$")
        self.assertRegex(body["zone_key"], r"^[A-Za-z0-9_]{32}$")
        self.assertEqual(body["status_url"], "/federation/status?application_id=" + body["application_id"])
        expires = controlplane.from_iso(body["expires_at"])
        self.assertAlmostEqual((expires - controlplane.utc_now()).total_seconds(), 7 * 24 * 3600, delta=5)
        queue = json.load(open(self.fx.config["state_file"]))["federation"]
        self.assertEqual(queue["applications"][0]["application_id"], body["application_id"])
        self.assertEqual(queue["applications"][0]["remote_address"], "127.0.0.1")
        self.assertTrue(queue["applications"][0]["zone_key_generated"])

    def test_apply_keeps_supplied_zone_key(self):
        status, body, _ = self.fx.apply(zone_key="MY_KEY")
        self.assertEqual(status, 201)
        self.assertEqual(body["zone_key"], "MY_KEY")
        _, listing, _ = self.fx.request("GET", "/federation/list", auth=("rods", "rods"))
        self.assertFalse(listing["applications"][0]["zone_key_generated"])

    def test_duplicate_pending_zone_is_409(self):
        self.assertEqual(self.fx.apply()[0], 201)
        status, body, _ = self.fx.apply(hosts=["other.example.org"])
        self.assertEqual(status, 409)
        self.assertIn("already pending", body["error"])

    def test_rejected_zone_may_reapply(self):
        _, first, _ = self.fx.apply()
        self.assertEqual(self.fx.judge(first["application_id"], "reject")[0], 200)
        self.assertEqual(self.fx.apply()[0], 201)

    def test_home_zone_name_is_rejected(self):
        status, body, _ = self.fx.apply(zone_name=HOME_ZONE)
        self.assertEqual(status, 400)
        self.assertIn("home zone", body["error"])

    def test_validation_error_is_400(self):
        status, body, _ = self.fx.request("POST", "/federation/apply", {"zone_name": "x"})
        self.assertEqual(status, 400)
        self.assertIn("missing required field", body["error"])

    def test_malformed_json_is_400(self):
        status, body, _ = self.fx.request("POST", "/federation/apply", raw_body=b"not json")
        self.assertEqual(status, 400)
        self.assertIn("not valid JSON", body["error"])

    def test_missing_content_length_is_400(self):
        data = raw_http(self.fx.port, b"POST /federation/apply HTTP/1.0\r\nHost: x\r\n\r\n")
        self.assertIn("HTTP/1.1 400", data)
        self.assertIn("Content-Length is required", data)

    def test_bad_content_length_is_400(self):
        for value in ("abc", "-5"):
            data = raw_http(self.fx.port,
                            "POST /federation/apply HTTP/1.0\r\nHost: x\r\nContent-Length: {}\r\n\r\n".format(value).encode())
            self.assertIn("HTTP/1.1 400", data, value)
            self.assertIn("Content-Length is", data)

    def test_oversized_body_is_413(self):
        status, body, _ = self.fx.request("POST", "/federation/apply", raw_body=b"a" * 70000)
        self.assertEqual(status, 413)

    def test_x_forwarded_for_is_recorded(self):
        self.fx.request("POST", "/federation/apply", {"zone_name": "z", "zone_port": 1, "catalog_provider_hosts": ["h"]},
                        headers={"X-Forwarded-For": "203.0.113.9, 10.0.0.1"})
        _, listing, _ = self.fx.request("GET", "/federation/list", auth=("rods", "rods"))
        self.assertEqual(listing["applications"][0]["remote_address"], "203.0.113.9")

    def test_unknown_path_and_wrong_method(self):
        self.assertEqual(self.fx.request("GET", "/nothing")[0], 404)
        self.assertEqual(self.fx.request("GET", "/federation/apply")[0], 405)
        status, _, headers = self.fx.request("PUT", "/federation/list", raw_body=b"")
        self.assertEqual(status, 405)
        self.assertEqual(headers["Allow"], "GET")
        self.assertEqual(self.fx.request("DELETE", "/federation/judge", raw_body=b"")[0], 405)
        self.assertEqual(self.fx.request("GET", "/federation/list/", auth=("rods", "rods"))[0], 200)


class NameAndEmailTests(ServerTestCase):
    def test_shown_to_admins_only(self):
        status, body, _ = self.fx.apply(name="Ada Lovelace", email="ada@example.org")
        self.assertEqual(status, 201)
        self.assertNotIn("name", body)
        self.assertNotIn("email", body)
        aid = body["application_id"]

        _, listing, _ = self.fx.request("GET", "/federation/list", auth=("rods", "rods"))
        rec = listing["applications"][0]
        self.assertEqual((rec["name"], rec["email"]), ("Ada Lovelace", "ada@example.org"))
        self.assertEqual((rec["negotiation_key"], rec["zone_key"]), (body["negotiation_key"], body["zone_key"]))

        _, pending, _ = self.fx.request("GET", "/federation/status?application_id=" + aid)
        self.assertNotIn("name", pending)
        self.assertNotIn("email", pending)

        status, judged, _ = self.fx.judge(aid, "approve")
        self.assertEqual(status, 200)
        self.assertEqual((judged["application"]["name"], judged["application"]["email"]),
                         ("Ada Lovelace", "ada@example.org"))

        _, approved, _ = self.fx.request("GET", "/federation/status?application_id=" + aid)
        self.assertEqual(approved["status"], "approved")
        self.assertNotIn("name", approved)
        self.assertNotIn("email", approved)
        self.assertNotIn("Ada", json.dumps(approved))
        self.assertEqual(approved["applicant_federation_entry"]["zone_key"], HOME_ZONE_KEY)

    def test_absent_fields_are_recorded_as_null(self):
        self.assertEqual(self.fx.apply()[0], 201)
        _, listing, _ = self.fx.request("GET", "/federation/list", auth=("rods", "rods"))
        rec = listing["applications"][0]
        self.assertEqual((rec["name"], rec["email"]), (None, None))

    def test_bad_name_or_email_is_400(self):
        self.assertEqual(self.fx.apply(email="nope")[0], 400)
        self.assertEqual(self.fx.apply(name="")[0], 400)
        _, listing, _ = self.fx.request("GET", "/federation/list", auth=("rods", "rods"))
        self.assertEqual(listing["applications"], [])


class InfoTests(ServerTestCase):
    def test_info_contents(self):
        status, body, headers = self.fx.request("GET", "/info")
        self.assertEqual(status, 200)
        self.assertEqual(headers["Cache-Control"], "no-store")
        self.assertEqual(set(body), {"service", "version", "federation"})
        self.assertEqual((body["service"], body["version"]), ("controlplane", controlplane.VERSION))
        self.assertEqual(body["federation"], {"application_expiration_in_seconds": 7 * 24 * 3600})
        self.assertNotIn("zone_name", json.dumps(body))
        self.assertNotIn(HOME_ZONE, json.dumps(body))
        self.assertEqual(self.fx.request("POST", "/info", raw_body=b"")[0], 405)


class InfoReflectsConfigTests(ServerTestCase):
    config_overrides = {"federation": {"application_expiration_in_seconds": 60}}

    def test_info_reports_configured_expiration(self):
        _, body, _ = self.fx.request("GET", "/info")
        self.assertEqual(body["federation"]["application_expiration_in_seconds"], 60)


class QueueFullTests(ServerTestCase):
    config_overrides = {"federation": {"max_pending_applications": 2}}

    def test_queue_full_is_503(self):
        self.assertEqual(self.fx.apply("a")[0], 201)
        self.assertEqual(self.fx.apply("b")[0], 201)
        status, body, _ = self.fx.apply("c")
        self.assertEqual(status, 503)
        self.assertIn("full", body["error"])


class StatusTests(ServerTestCase):
    def test_status_pending_hides_keys(self):
        _, applied, _ = self.fx.apply()
        status, body, _ = self.fx.request("GET", "/federation/status?application_id=" + applied["application_id"])
        self.assertEqual(status, 200)
        self.assertEqual(body["status"], "pending")
        self.assertNotIn("negotiation_key", body)
        self.assertNotIn("zone_key", body)
        self.assertNotIn("applicant_federation_entry", body)
        self.assertIsNone(body["judged_at"])

    def test_status_errors(self):
        self.assertEqual(self.fx.request("GET", "/federation/status?application_id=nope")[0], 404)
        self.assertEqual(self.fx.request("GET", "/federation/status")[0], 400)
        self.assertEqual(self.fx.request("GET", "/federation/status?application_id=")[0], 400)
        self.assertEqual(self.fx.request("GET", "/federation/status?application_id=a&application_id=b")[0], 400)


# ---------------------------------------------------------------------------
# HTTP: authentication and list
# ---------------------------------------------------------------------------

class AuthTests(ServerTestCase):
    def test_missing_credentials(self):
        status, body, headers = self.fx.request("GET", "/federation/list")
        self.assertEqual(status, 401)
        self.assertEqual(headers["WWW-Authenticate"], 'Basic realm="controlplane"')
        self.assertIn("authentication required", body["error"])

    def test_malformed_headers(self):
        for header in ("Bearer abc", "Basic", "Basic !!!notbase64", "Basic " + base64.b64encode(b"nocolon").decode(),
                       "Basic " + base64.b64encode(b":pw").decode(),
                       "Basic " + base64.b64encode(b"rods#tempZone:rods").decode(),
                       "Basic " + base64.b64encode(b"bad user:rods").decode(),
                       "Basic " + base64.b64encode(b"\xff\xfe:x").decode()):
            status, _, headers = self.fx.request("GET", "/federation/list", headers={"Authorization": header})
            self.assertEqual(status, 401, header)
            self.assertIn("WWW-Authenticate", headers)

    def test_bad_password_and_unknown_user(self):
        self.assertEqual(self.fx.request("GET", "/federation/list", auth=("rods", "wrong"))[0], 401)
        self.assertEqual(self.fx.request("GET", "/federation/list", auth=("nobody", "x"))[0], 401)

    def test_rodsuser_is_403(self):
        status, body, _ = self.fx.request("GET", "/federation/list", auth=("alice", "alicepass"))
        self.assertEqual(status, 403)
        self.assertIn("not a rodsadmin", body["error"])

    def test_unreachable_irods_is_502(self):
        self.assertEqual(self.fx.request("GET", "/federation/list", auth=("unreachable", "x"))[0], 502)

    def test_sessions_are_cleaned_up(self):
        self.fx.request("GET", "/federation/list", auth=("rods", "rods"))
        self.fx.apply()
        _, listing, _ = self.fx.request("GET", "/federation/list", auth=("rods", "rods"))
        self.fx.judge(listing["applications"][0]["application_id"], "reject")
        self.assertEqual(len(self.fx.irods.sessions), 3)
        self.assertTrue(all(s.cleaned_up for s in self.fx.irods.sessions))

    def test_judge_requires_auth_after_reading_body(self):
        status, _, headers = self.fx.request("POST", "/federation/judge", {"application_id": "x", "decision": "approve"})
        self.assertEqual(status, 401)
        self.assertIn("WWW-Authenticate", headers)


class ListTests(ServerTestCase):
    def test_list_default_filters_and_order(self):
        self.fx.apply("a")
        time.sleep(1.1)  # distinct submitted_at seconds
        _, b, _ = self.fx.apply("b")
        self.fx.judge(b["application_id"], "reject", note="no")
        status, body, _ = self.fx.request("GET", "/federation/list", auth=("rods", "rods"))
        self.assertEqual(status, 200)
        self.assertEqual([r["zone_name"] for r in body["applications"]], ["a"])
        self.assertIn("negotiation_key", body["applications"][0])
        _, body, _ = self.fx.request("GET", "/federation/list?status=rejected", auth=("rods", "rods"))
        self.assertEqual([r["zone_name"] for r in body["applications"]], ["b"])
        self.assertEqual(body["applications"][0]["decision_note"], "no")
        self.assertEqual(body["applications"][0]["judged_by"], "rods")
        _, body, _ = self.fx.request("GET", "/federation/list?status=all", auth=("rods", "rods"))
        self.assertEqual([r["zone_name"] for r in body["applications"]], ["a", "b"])
        for empty in ("approved", "expired"):
            _, body, _ = self.fx.request("GET", "/federation/list?status=" + empty, auth=("rods", "rods"))
            self.assertEqual(body["applications"], [])

    def test_bad_filter_is_400(self):
        status, body, _ = self.fx.request("GET", "/federation/list?status=bogus", auth=("rods", "rods"))
        self.assertEqual(status, 400)
        self.assertIn("status must be one of", body["error"])
        self.assertEqual(self.fx.request("GET", "/federation/list?status=all&status=pending", auth=("rods", "rods"))[0], 400)


# ---------------------------------------------------------------------------
# HTTP: judge
# ---------------------------------------------------------------------------

class JudgeValidationTests(ServerTestCase):
    def test_body_validation(self):
        _, a, _ = self.fx.apply()
        aid = a["application_id"]
        cases = [
            ([], "JSON object"),
            ({"application_id": aid, "decision": "approve", "x": 1}, "unknown field"),
            ({"decision": "approve"}, "application_id is required"),
            ({"application_id": "", "decision": "approve"}, "application_id is required"),
            ({"application_id": aid, "decision": "maybe"}, "decision must be"),
            ({"application_id": aid, "decision": "approve", "decision_note": 5}, "decision_note"),
            ({"application_id": aid, "decision": "approve", "decision_note": "x" * 1025}, "decision_note"),
        ]
        for body, fragment in cases:
            status, resp, _ = self.fx.request("POST", "/federation/judge", body, auth=("rods", "rods"))
            self.assertEqual(status, 400, body)
            self.assertIn(fragment, resp["error"])

    def test_unknown_application_is_404(self):
        self.assertEqual(self.fx.judge("nope", "approve")[0], 404)

    def test_rodsuser_cannot_judge(self):
        _, a, _ = self.fx.apply()
        self.assertEqual(self.fx.judge(a["application_id"], "approve", auth=("alice", "alicepass"))[0], 403)
        self.assertEqual(self.fx.irods.zones, {})


class RejectTests(ServerTestCase):
    def test_reject(self):
        _, a, _ = self.fx.apply()
        status, body, _ = self.fx.judge(a["application_id"], "reject", note="not now")
        self.assertEqual(status, 200)
        rec = body["application"]
        self.assertEqual(rec["status"], "rejected")
        self.assertEqual(rec["judged_by"], "rods")
        self.assertEqual(rec["decision_note"], "not now")
        self.assertIsNotNone(rec["judged_at"])
        self.assertEqual(self.fx.irods.zones, {})
        self.assertEqual(self.fx.server_config()["federation"], [])
        self.assertEqual(self.fx.hups(), 0)
        status, body, _ = self.fx.judge(a["application_id"], "approve")
        self.assertEqual(status, 409)
        self.assertIn("already been rejected", body["error"])
        _, s, _ = self.fx.request("GET", "/federation/status?application_id=" + a["application_id"])
        self.assertEqual(s["status"], "rejected")
        self.assertNotIn("applicant_federation_entry", s)


class ApproveTests(ServerTestCase):
    def wait_for_hup(self, count):
        for _ in range(200):
            if self.fx.hups() >= count:
                return
            time.sleep(0.01)
        self.fail("expected {} SIGHUP(s), saw {}".format(count, self.fx.hups()))

    def test_approve_happy_path(self):
        _, a, _ = self.fx.apply("otherZone", hosts=["localhost", "127.0.0.1"], port=1248)
        status, body, _ = self.fx.judge(a["application_id"], "approve", note="ok")
        self.assertEqual(status, 200)

        # catalog
        self.assertEqual(self.fx.irods.zones, {"otherZone": "localhost:1248"})

        # record
        rec = body["application"]
        self.assertEqual(rec["status"], "approved")
        self.assertEqual(rec["judged_by"], "rods")
        self.assertEqual(rec["decision_note"], "ok")

        # home side stanza
        home = body["home_federation_entry"]
        self.assertEqual(home, {
            "catalog_provider_hosts": ["localhost", "127.0.0.1"],
            "negotiation_key": a["negotiation_key"],
            "zone_key": a["zone_key"],
            "zone_key_signing_hash_scheme": "sha256",
            "zone_name": "otherZone",
            "zone_port": 1248,
        })
        cfg = self.fx.server_config()
        self.assertEqual(cfg["federation"], [home])
        self.assertEqual(cfg["zone_key"], HOME_ZONE_KEY)  # rest of file intact
        self.assertEqual(oct(os.stat(self.fx.server_config_file).st_mode & 0o777), "0o600")
        backups = [f for f in os.listdir(self.fx.tmp.name) if ".controlplane-backup-" in f]
        self.assertEqual(len(backups), 1)
        self.assertEqual(json.load(open(os.path.join(self.fx.tmp.name, backups[0])))["federation"], [])

        # applicant side stanza
        applicant = body["applicant_federation_entry"]
        self.assertEqual(applicant, {
            "catalog_provider_hosts": [HOME_HOST],
            "negotiation_key": a["negotiation_key"],
            "zone_key": HOME_ZONE_KEY,
            "zone_key_signing_hash_scheme": "sha256",
            "zone_name": HOME_ZONE,
            "zone_port": 1247,
        })
        self.assertEqual(body["applicant_mkzone_command"],
                         "iadmin mkzone tempZone remote icat.home.example.org:1247")
        self.assertNotIn("home_negotiation_key_never_shown", json.dumps(body))

        # reload
        self.wait_for_hup(1)

        # status now reveals the applicant stanza
        _, s, _ = self.fx.request("GET", "/federation/status?application_id=" + a["application_id"])
        self.assertEqual(s["status"], "approved")
        self.assertEqual(s["applicant_federation_entry"], applicant)
        self.assertEqual(s["applicant_mkzone_command"], body["applicant_mkzone_command"])
        self.assertNotIn("home_negotiation_key_never_shown", json.dumps(s))

        # second judge is a conflict
        status, body, _ = self.fx.judge(a["application_id"], "reject")
        self.assertEqual(status, 409)
        self.assertIn("already been approved", body["error"])

        # persisted
        queue = json.load(open(self.fx.config["state_file"]))["federation"]
        self.assertEqual(queue["applications"][0]["status"], "approved")

    def test_unresolvable_host_is_refused(self):
        _, a, _ = self.fx.apply(hosts=["icat.nowhere.invalid"])
        status, body, _ = self.fx.judge(a["application_id"], "approve")
        self.assertEqual(status, 400)
        self.assertIn("does not resolve", body["error"])
        self.assertEqual(self.fx.irods.zones, {})
        self.assertEqual(self.fx.server_config()["federation"], [])
        _, s, _ = self.fx.request("GET", "/federation/status?application_id=" + a["application_id"])
        self.assertEqual(s["status"], "pending")

    def test_other_federation_entries_are_left_alone(self):
        cfg = self.fx.server_config()
        other = {"zone_name": "unrelatedZone", "zone_key": "k", "negotiation_key": "n" * 32,
                 "catalog_provider_hosts": ["elsewhere"], "zone_port": 1}
        cfg["federation"].append(other)
        write_json(self.fx.server_config_file, cfg)
        _, a, _ = self.fx.apply()
        status, body, _ = self.fx.judge(a["application_id"], "approve")
        self.assertEqual(status, 200)
        self.assertEqual(self.fx.server_config()["federation"], [other, body["home_federation_entry"]])
        self.wait_for_hup(1)

    def test_zone_already_in_catalog_is_409(self):
        self.fx.irods.zones["otherZone"] = "elsewhere:1247"
        _, a, _ = self.fx.apply()
        status, body, _ = self.fx.judge(a["application_id"], "approve")
        self.assertEqual(status, 409)
        self.assertIn("already exists in the home catalog", body["error"])
        self.assertEqual(self.fx.server_config()["federation"], [])
        self.assertEqual(self.fx.hups(), 0)
        self.assertEqual(self.fx.judge(a["application_id"], "reject")[0], 200)

    def test_mkzone_failure_is_500_and_changes_nothing(self):
        self.fx.irods.mkzone_error = "CAT_SQL_ERR"
        _, a, _ = self.fx.apply()
        status, body, _ = self.fx.judge(a["application_id"], "approve")
        self.assertEqual(status, 500)
        self.assertIn("step mkzone failed", body["error"])
        self.assertEqual(self.fx.irods.zones, {})
        self.assertEqual(self.fx.server_config()["federation"], [])
        self.assertEqual(self.fx.hups(), 0)
        _, s, _ = self.fx.request("GET", "/federation/status?application_id=" + a["application_id"])
        self.assertEqual(s["status"], "pending")

    def test_config_edit_failure_rolls_back_mkzone(self):
        cfg = self.fx.server_config()
        cfg["federation"].append({"zone_name": "otherZone", "zone_key": "stale", "negotiation_key": "x" * 32,
                                  "catalog_provider_hosts": ["old"], "zone_port": 1})
        write_json(self.fx.server_config_file, cfg)
        _, a, _ = self.fx.apply()
        status, body, _ = self.fx.judge(a["application_id"], "approve")
        self.assertEqual(status, 500)
        self.assertIn("step server_config failed", body["error"])
        self.assertIn("mkzone rolled back", body["error"])
        self.assertEqual(self.fx.irods.zones, {})
        self.assertEqual(self.fx.hups(), 0)
        self.assertEqual(len(self.fx.server_config()["federation"]), 1)

    def test_config_edit_failure_with_failed_rollback(self):
        cfg = self.fx.server_config()
        cfg["federation"].append({"zone_name": "otherZone", "zone_key": "stale", "negotiation_key": "x" * 32,
                                  "catalog_provider_hosts": ["old"], "zone_port": 1})
        write_json(self.fx.server_config_file, cfg)
        self.fx.irods.rmzone_error = "boom"
        _, a, _ = self.fx.apply()
        status, body, _ = self.fx.judge(a["application_id"], "approve")
        self.assertEqual(status, 500)
        self.assertIn("manual cleanup required", body["error"])
        self.assertEqual(self.fx.irods.zones, {"otherZone": "localhost:1247"})

    def test_reload_failure_then_retry_only_reloads(self):
        os.unlink(self.fx.pid_file)
        _, a, _ = self.fx.apply()
        status, body, _ = self.fx.judge(a["application_id"], "approve")
        self.assertEqual(status, 500)
        self.assertIn("step reload failed", body["error"])
        self.assertIn("judge again", body["error"])
        self.assertEqual(self.fx.irods.zones, {"otherZone": "localhost:1247"})
        self.assertEqual(len(self.fx.server_config()["federation"]), 1)
        _, s, _ = self.fx.request("GET", "/federation/status?application_id=" + a["application_id"])
        self.assertEqual(s["status"], "pending")

        # fix the pid file and retry: no second mkzone, no second backup, one HUP
        with open(self.fx.pid_file, "w") as f:
            f.write(str(self.fx.child.pid))
        self.fx.irods.mkzone_error = "must not be called again"
        status, body, _ = self.fx.judge(a["application_id"], "approve")
        self.assertEqual(status, 200)
        self.assertEqual(body["application"]["status"], "approved")
        self.assertEqual(len(self.fx.server_config()["federation"]), 1)
        self.assertEqual(len([f for f in os.listdir(self.fx.tmp.name) if ".controlplane-backup-" in f]), 1)
        self.wait_for_hup(1)

    def test_reload_failure_on_bad_pid(self):
        with open(self.fx.pid_file, "w") as f:
            f.write("not-a-pid")
        _, a, _ = self.fx.apply()
        status, body, _ = self.fx.judge(a["application_id"], "approve")
        self.assertEqual(status, 500)
        self.assertIn("cannot read server pid file", body["error"])
        with open(self.fx.pid_file, "w") as f:
            f.write("999999999")
        status, body, _ = self.fx.judge(a["application_id"], "approve")
        self.assertEqual(status, 500)
        self.assertIn("cannot signal", body["error"])


class ExpirationTests(ServerTestCase):
    config_overrides = {"federation": {"application_expiration_in_seconds": 1}}

    def test_expired_application(self):
        _, a, _ = self.fx.apply()
        time.sleep(2.1)
        _, body, _ = self.fx.request("GET", "/federation/list?status=expired", auth=("rods", "rods"))
        self.assertEqual([r["application_id"] for r in body["applications"]], [a["application_id"]])
        _, body, _ = self.fx.request("GET", "/federation/list", auth=("rods", "rods"))
        self.assertEqual(body["applications"], [])
        status, body, _ = self.fx.judge(a["application_id"], "approve")
        self.assertEqual(status, 410)
        self.assertIn("expired", body["error"])
        _, s, _ = self.fx.request("GET", "/federation/status?application_id=" + a["application_id"])
        self.assertEqual(s["status"], "expired")
        # an expired zone name no longer blocks a new application
        self.assertEqual(self.fx.apply()[0], 201)


class ErrorPathTests(ServerTestCase):
    def test_unhandled_exception_is_500(self):
        def boom(query):
            raise RuntimeError("kaboom")
        self.fx.app.federation_status = boom
        status, body, _ = self.fx.request("GET", "/federation/status?application_id=x")
        self.assertEqual(status, 500)
        self.assertIn("kaboom", body["error"])

    def test_unread_body_is_discarded_on_error(self):
        # 404 before the body is read; the server must drain it and keep serving
        status, _, _ = self.fx.request("POST", "/nothing", {"some": "body"})
        self.assertEqual(status, 404)
        self.assertEqual(self.fx.request("GET", "/federation/status?application_id=x")[0], 404)

    def test_real_irods_unreachable_is_502(self):
        probe = socket.socket()
        probe.bind(("127.0.0.1", 0))
        port = probe.getsockname()[1]
        probe.close()
        env = {"irods_host": "127.0.0.1", "irods_port": port, "irods_zone_name": HOME_ZONE,
               "irods_user_name": "rods", "irods_client_server_policy": "CS_NEG_REFUSE"}
        real = controlplane.IRODS("/nonexistent/env.json", env)
        with self.assertRaises(controlplane.HTTPError) as cm:
            real.admin_session("rods", "rods")
        self.assertEqual(cm.exception.status, 502)
        self.assertIn("cannot reach iRODS", cm.exception.message)


class ClientDisconnectTests(ServerTestCase):
    def test_client_closing_early_does_not_crash_server(self):
        s = socket.create_connection(("127.0.0.1", self.fx.port), timeout=10)
        s.sendall(b"POST /federation/apply HTTP/1.1\r\nHost: x\r\nContent-Length: 70000\r\n\r\n")
        s.close()  # server answers 413 into a closed socket
        time.sleep(0.2)
        self.assertEqual(self.fx.request("GET", "/federation/status?application_id=x")[0], 404)


# ---------------------------------------------------------------------------
# OpenAPI contract (controlplane-openapi.yaml)
# ---------------------------------------------------------------------------

class OpenAPIContractTests(ServerTestCase):
    """The checks below complement the per-request validation every ServerTestCase performs."""

    def test_document_basics(self):
        doc = CONTRACT.doc
        self.assertTrue(doc["openapi"].startswith("3.0."), doc["openapi"])
        self.assertEqual(doc["info"]["version"], controlplane.VERSION)
        declared = {(m.upper(), p) for p, item in doc["paths"].items() for m in item if m in HTTP_METHODS}
        self.assertEqual(declared, set(controlplane.Handler.ROUTES))
        admin_paths = {"/federation/list", "/federation/judge", "/keyserver/local_zone",
                       "/bootstrap/create", "/bootstrap/revoke", "/bootstrap/list"}
        for path, item in doc["paths"].items():
            for method in (m for m in item if m in HTTP_METHODS):
                if path in admin_paths:
                    self.assertEqual(item[method]["security"], [{"rodsadminBasic": []}], path)
                else:
                    self.assertNotIn("security", item[method], path)
        self.assertEqual(doc["components"]["securitySchemes"]["rodsadminBasic"],
                         {"type": "http", "scheme": "basic",
                          "description": doc["components"]["securitySchemes"]["rodsadminBasic"]["description"]})
        self.assertEqual({t["name"] for t in doc["tags"]}, {"federation", "keyserver", "bootstrap", "service"})

    def test_every_ref_resolves(self):
        refs = []

        def walk(node):
            if isinstance(node, dict):
                if "$ref" in node:
                    refs.append(node["$ref"])
                for v in node.values():
                    walk(v)
            elif isinstance(node, list):
                for v in node:
                    walk(v)
        walk(CONTRACT.doc)
        self.assertGreater(len(refs), 50)
        for ref in refs:
            self.assertTrue(ref.startswith("#/"), ref)
            self.assertIsInstance(CONTRACT.resolver.resolve(ref)[1], dict, ref)

    def test_every_example_matches_its_schema(self):
        seen = 0
        for where, example, schema in CONTRACT.examples():
            CONTRACT.validate(example, schema, where)
            seen += 1
        self.assertGreater(seen, 50)

    def test_every_operation_is_served(self):
        # a declared operation must reach a handler: never 404 (unknown path) or 405 (wrong method)
        for path, item in CONTRACT.doc["paths"].items():
            for method in (m for m in item if m in HTTP_METHODS):
                body = {} if method == "post" else None
                status, _, _ = self.fx.request(method.upper(), path, body, auth=("rods", "rods"))
                self.assertNotIn(status, (404, 405), "{} {}".format(method.upper(), path))
                for other in ("GET", "POST", "PUT", "DELETE"):
                    if other.lower() in item:
                        continue
                    raw = b"" if other in ("PUT", "DELETE", "POST") else None
                    self.assertEqual(self.fx.request(other, path, raw_body=raw)[0], 405,
                                     "{} {}".format(other, path))

    def test_undeclared_path_and_status_code_fail_the_check(self):
        with self.assertRaises(AssertionError):
            CONTRACT.check("GET", "/info", 418, {"error": "x"}, {"Content-Type": "application/json"})
        with self.assertRaises(AssertionError):
            CONTRACT.check("GET", "/nowhere", 200, {}, {"Content-Type": "application/json"})
        with self.assertRaises(AssertionError):
            CONTRACT.check("GET", "/info", 200, {"service": "controlplane"}, {"Content-Type": "application/json"})
        with self.assertRaises(AssertionError):
            CONTRACT.check("POST", "/info", 405, {"error": "x"}, {"Content-Type": "application/json"})

# ---------------------------------------------------------------------------
# HTTP: keyserver
# ---------------------------------------------------------------------------

RODS = ("rods", "rods")
UUID_RE = r"^[0-9a-f]{8}-[0-9a-f]{4}-4[0-9a-f]{3}-[89ab][0-9a-f]{3}-[0-9a-f]{12}$"


class KeyserverTests(ServerTestCase):
    def test_local_zone_requires_rodsadmin(self):
        status, _, headers = self.fx.local_zone(auth=None)
        self.assertEqual(status, 401)
        self.assertEqual(headers["WWW-Authenticate"], 'Basic realm="controlplane"')
        self.assertEqual(self.fx.local_zone(auth=("alice", "alicepass"))[0], 403)
        self.assertEqual(self.fx.local_zone(auth=("unreachable", "x"))[0], 502)
        self.assertEqual(self.fx.state()["keyserver"], {})  # nothing adopted for a refused caller

    def test_adoption_on_first_read(self):
        before = controlplane.utc_now()
        status, body, headers = self.fx.local_zone()
        self.assertEqual(status, 200)
        self.assertEqual(headers["Cache-Control"], "no-store")
        self.assertEqual(set(body), {"zone_name", "lifetime_of_zone_keys_in_seconds", "zone_key", "next_zone_key"})
        self.assertEqual(body["zone_name"], HOME_ZONE)
        self.assertEqual(body["lifetime_of_zone_keys_in_seconds"], 90 * 24 * 3600)
        zk, nk = body["zone_key"], body["next_zone_key"]
        self.assertEqual(zk["value"], HOME_ZONE_KEY)
        issued = controlplane.from_iso(zk["issued_at"])
        self.assertLessEqual(abs((issued - before).total_seconds()), 5)
        self.assertEqual(controlplane.from_iso(zk["expires_at"]), issued + controlplane.datetime.timedelta(days=90))
        self.assertRegex(nk["value"], r"^[A-Za-z0-9_]{32}$")
        self.assertNotEqual(nk["value"], HOME_ZONE_KEY)
        self.assertEqual(nk["minted_at"], zk["issued_at"])
        self.assertEqual(controlplane.from_iso(nk["expires_at"]),
                         controlplane.from_iso(zk["expires_at"]) + controlplane.datetime.timedelta(days=90))
        # persisted, mode 0600, nothing touched in iRODS or server_config.json
        state = self.fx.state()
        self.assertEqual(state["keyserver"], {"zone_key": zk, "next_zone_key": nk})
        self.assertEqual(oct(os.stat(self.fx.config["state_file"]).st_mode & 0o777), "0o600")
        self.assertEqual(self.fx.server_config()["zone_key"], HOME_ZONE_KEY)
        self.assertEqual(self.fx.hups(), 0)
        # steady state: a second read mints nothing
        self.assertEqual(self.fx.local_zone()[1], body)

    def test_rotation_is_detected(self):
        _, first, _ = self.fx.local_zone()
        cfg = self.fx.server_config()
        cfg["zone_key"] = first["next_zone_key"]["value"]
        write_json(self.fx.server_config_file, cfg)
        with self.assertLogs("controlplane", level="INFO") as logs:
            _, second, _ = self.fx.local_zone()
        self.assertTrue(any("rotation detected" in line for line in logs.output), logs.output)
        self.assertEqual(second["zone_key"]["value"], first["next_zone_key"]["value"])
        self.assertGreaterEqual(second["zone_key"]["issued_at"], first["zone_key"]["issued_at"])
        self.assertNotIn(second["next_zone_key"]["value"], (first["zone_key"]["value"], first["next_zone_key"]["value"]))
        self.assertEqual(self.fx.state()["keyserver"]["next_zone_key"], second["next_zone_key"])
        self.assertEqual(self.fx.local_zone()[1], second)

    def test_hand_changed_key_is_adopted_with_warning(self):
        _, first, _ = self.fx.local_zone()
        cfg = self.fx.server_config()
        cfg["zone_key"] = "MANUALLY_SET"
        write_json(self.fx.server_config_file, cfg)
        with self.assertLogs("controlplane", level="WARNING") as logs:
            _, second, _ = self.fx.local_zone()
        self.assertTrue(any("changed by hand" in line for line in logs.output), logs.output)
        self.assertEqual(second["zone_key"]["value"], "MANUALLY_SET")
        self.assertNotEqual(second["next_zone_key"]["value"], first["next_zone_key"]["value"])


class ValidateBootstrapBodiesTests(unittest.TestCase):
    def check(self, func, body, fragment):
        with self.assertRaises(controlplane.HTTPError) as cm:
            func(body)
        self.assertEqual(cm.exception.status, 400)
        self.assertIn(fragment, cm.exception.message)

    def test_create(self):
        v = controlplane.validate_bootstrap_create
        self.check(v, [], "JSON object")
        self.check(v, {}, "catalog_service_role must be one of")
        self.check(v, {"catalog_service_role": "primary"}, "catalog_service_role must be one of")
        self.check(v, {"catalog_service_role": "consumer", "x": 1}, "unknown field(s): x")
        for bad in (0, -1, True, "60", 1.5):
            self.check(v, {"catalog_service_role": "consumer", "lifetime_in_seconds": bad}, "lifetime_in_seconds")
        for bad in (5, "x" * 1025):
            self.check(v, {"catalog_service_role": "consumer", "note": bad}, "note must be")
        self.assertEqual(v({"catalog_service_role": "provider"}), ("provider", None, None))
        self.assertEqual(v({"catalog_service_role": "consumer", "lifetime_in_seconds": 1, "note": "x" * 1024}),
                         ("consumer", 1, "x" * 1024))

    def test_key_body(self):
        v = controlplane.validate_bootstrap_key_body
        key = str(uuid.uuid4())
        self.check(v, [], "JSON object")
        self.check(v, {}, "missing required field: bootstrap_key")
        self.check(v, {"bootstrap_key": key, "catalog_service_role": "consumer"},
                   "unknown field(s): catalog_service_role")
        for bad in ("not-a-uuid", key.upper(), 5):
            self.check(v, {"bootstrap_key": bad}, "bootstrap_key must be")
        self.assertEqual(v({"bootstrap_key": key}), key)

    def test_redeem_body(self):
        def v(body, prefix="adm."):
            return controlplane.validate_bootstrap_redeem(body, prefix)
        key = str(uuid.uuid4())
        self.check(v, [], "JSON object")
        self.check(v, {}, "missing required field: bootstrap_key")
        self.check(v, {"bootstrap_key": "nope", "host": HOST}, "bootstrap_key must be")
        self.check(v, {"bootstrap_key": key}, "missing required field: host")
        self.check(v, {"bootstrap_key": key, "host": HOST, "note": "x"}, "unknown field(s): note")
        for bad in ("", 5, "10.0.0.1", "::1", "bad_host", "-x.example.org", "a" * 64 + ".org"):
            self.check(v, {"bootstrap_key": key, "host": bad}, "host must be a hostname")
        just_fits = ("ab." * 20)[:-1]      # 59 chars + "adm." = 63
        too_long = ("ab." * 21)[:-1]       # 62 chars + "adm." = 66
        self.assertEqual(v({"bootstrap_key": key, "host": just_fits}), (key, just_fits, "adm." + just_fits))
        self.check(v, {"bootstrap_key": key, "host": too_long}, "host is too long")
        self.assertEqual(v({"bootstrap_key": key, "host": too_long}, ""), (key, too_long, too_long))
        self.assertEqual(v({"bootstrap_key": key, "host": HOST}), (key, HOST, ACCOUNT))
        self.assertEqual(v({"bootstrap_key": key, "host": HOST}, "svc-"), (key, HOST, "svc-" + HOST))


# ---------------------------------------------------------------------------
# HTTP: bootstrap
# ---------------------------------------------------------------------------

KEY_FIELDS = {"bootstrap_key", "status", "catalog_service_role", "note", "created_at", "created_by",
              "expires_at", "redeemed_at", "redeemed_from", "host", "service_account", "revoked_at",
              "revoked_by"}
PASSWORD_RE = r"^[A-Za-z0-9_]{32}$"


class BootstrapCreateTests(ServerTestCase):
    def test_create_defaults(self):
        before = controlplane.utc_now()
        status, body, _ = self.fx.create()
        self.assertEqual(status, 201)
        self.assertEqual(set(body), KEY_FIELDS)
        self.assertRegex(body["bootstrap_key"], UUID_RE)
        self.assertEqual((body["status"], body["catalog_service_role"], body["note"], body["created_by"]),
                         ("available", "consumer", None, "rods"))
        self.assertEqual((body["redeemed_at"], body["redeemed_from"], body["host"], body["service_account"],
                          body["revoked_at"], body["revoked_by"]), (None,) * 6)
        created = controlplane.from_iso(body["created_at"])
        self.assertLessEqual(abs((created - before).total_seconds()), 5)
        self.assertEqual(controlplane.from_iso(body["expires_at"]), created + controlplane.datetime.timedelta(hours=1))
        _, listing, _ = self.fx.bootstrap_list()
        self.assertEqual(listing["bootstrap_keys"], [body])
        self.assertEqual(self.fx.state()["bootstrap"], {"keys": [body]})
        # creating reconciles the zone key state too
        self.assertEqual(self.fx.state()["keyserver"]["zone_key"]["value"], HOME_ZONE_KEY)
        self.assertNotEqual(self.fx.new_key(), self.fx.new_key())
        self.assertEqual((self.fx.irods.zones, self.fx.hups()), ({}, 0))

    def test_create_with_lifetime_and_note(self):
        status, body, _ = self.fx.create("provider", lifetime_in_seconds=600, note="server room")
        self.assertEqual(status, 201)
        self.assertEqual((body["catalog_service_role"], body["note"]), ("provider", "server room"))
        self.assertEqual(controlplane.from_iso(body["expires_at"]) - controlplane.from_iso(body["created_at"]),
                         controlplane.datetime.timedelta(seconds=600))

    def test_validation(self):
        cases = [
            ([], "JSON object"),
            ({}, "catalog_service_role must be one of"),
            ({"catalog_service_role": "primary"}, "catalog_service_role must be one of"),
            ({"catalog_service_role": "consumer", "bootstrap_key": "x"}, "unknown field(s): bootstrap_key"),
            ({"catalog_service_role": "consumer", "lifetime_in_seconds": 0}, "lifetime_in_seconds"),
            ({"catalog_service_role": "consumer", "note": "x" * 1025}, "note must be"),
        ]
        for body, fragment in cases:
            status, resp, _ = self.fx.request("POST", "/bootstrap/create", body, auth=RODS)
            self.assertEqual(status, 400, body)
            self.assertIn(fragment, resp["error"])
        self.assertEqual(self.fx.request("POST", "/bootstrap/create", raw_body=b"nope", auth=RODS)[0], 400)
        self.assertEqual(self.fx.request("POST", "/bootstrap/create", raw_body=b"a" * 70000, auth=RODS)[0], 413)
        self.assertEqual(self.fx.bootstrap_list()[1]["bootstrap_keys"], [])

    def test_auth_and_method(self):
        self.assertEqual(self.fx.create(auth=None)[0], 401)
        self.assertEqual(self.fx.create(auth=("alice", "alicepass"))[0], 403)
        self.assertEqual(self.fx.create(auth=("unreachable", "x"))[0], 502)
        # the body is parsed before the credentials are checked
        self.assertEqual(self.fx.request("POST", "/bootstrap/create", raw_body=b"nope")[0], 400)
        status, _, headers = self.fx.request("GET", "/bootstrap/create", auth=RODS)
        self.assertEqual((status, headers["Allow"]), (405, "POST"))
        self.assertEqual(self.fx.bootstrap_list()[1]["bootstrap_keys"], [])

    def test_missing_content_length_is_400(self):
        token = base64.b64encode(b"rods:rods").decode()
        data = raw_http(self.fx.port, "POST /bootstrap/create HTTP/1.0\r\nHost: x\r\nAuthorization: Basic {}\r\n\r\n"
                        .format(token).encode())
        self.assertIn("HTTP/1.1 400", data)
        self.assertIn("Content-Length is required", data)


class BootstrapRedeemTests(ServerTestCase):
    def test_redeem_consumer(self):
        key = self.fx.new_key()
        before = controlplane.utc_now()
        with self.assertLogs("controlplane", level="INFO") as logs:
            status, body, _ = self.fx.redeem(key)
        self.assertEqual(status, 200)
        self.assertEqual(set(body), {"service_account", "zone_facts"})
        self.assertEqual(body["zone_facts"], expected_facts("consumer"))
        self.assertEqual(body["zone_facts"]["zone_user"], ACCOUNT)
        self.assertEqual(body["zone_facts"]["zone_key"], HOME_ZONE_KEY)
        self.assertEqual(body["zone_facts"]["negotiation_key"], HOME_NEGOTIATION_KEY)
        self.assertNotIn("database", body["zone_facts"])
        self.assertNotIn("db-secret", json.dumps(body))

        # the account exists in iRODS with exactly the password returned, and nowhere else
        account = body["service_account"]
        self.assertEqual(set(account), {"name", "password"})
        self.assertEqual(account["name"], ACCOUNT)
        self.assertRegex(account["password"], PASSWORD_RE)
        self.assertEqual(self.fx.irods.users[ACCOUNT], (account["password"], "rodsadmin"))
        self.assertEqual(self.fx.irods.created, [ACCOUNT])
        self.assertNotIn(account["password"], open(self.fx.config["state_file"]).read())
        self.assertFalse(any(account["password"] in line for line in logs.output), logs.output)
        self.assertTrue(any("by host {}; created rodsadmin {}".format(HOST, ACCOUNT) in line
                            for line in logs.output), logs.output)

        _, listing, _ = self.fx.bootstrap_list()
        rec = listing["bootstrap_keys"][0]
        self.assertEqual((rec["status"], rec["redeemed_from"], rec["host"], rec["service_account"]),
                         ("redeemed", "127.0.0.1", HOST, ACCOUNT))
        self.assertLessEqual(abs((controlplane.from_iso(rec["redeemed_at"]) - before).total_seconds()), 5)
        saved = self.fx.state()["bootstrap"]["keys"][0]
        self.assertEqual((saved["status"], saved["host"], saved["service_account"]), ("redeemed", HOST, ACCOUNT))

        # single use, and a second host name does not help
        for host in (HOST, "rs02.example.org"):
            status, again, _ = self.fx.redeem(key, host=host)
            self.assertEqual(status, 403)
            self.assertIn("not valid", again["error"])
            self.assertNotIn("zone_facts", again)
        self.assertEqual(self.fx.irods.created, [ACCOUNT])

        # nothing else touched in iRODS or on disk
        self.assertEqual((self.fx.irods.zones, self.fx.hups()), ({}, 0))
        self.assertEqual(self.fx.server_config(), home_server_config())

    def test_each_redemption_gets_its_own_account(self):
        first = self.fx.redeem(self.fx.new_key(), host="rs01.example.org")[1]
        second = self.fx.redeem(self.fx.new_key("provider"), host="rs02.example.org")[1]
        self.assertEqual(self.fx.irods.created, ["adm.rs01.example.org", "adm.rs02.example.org"])
        self.assertNotEqual(first["service_account"]["password"], second["service_account"]["password"])
        self.assertEqual(second["zone_facts"]["zone_user"], "adm.rs02.example.org")
        self.assertEqual([k["service_account"] for k in self.fx.bootstrap_list()[1]["bootstrap_keys"]],
                         ["adm.rs01.example.org", "adm.rs02.example.org"])

    def test_existing_account_is_409_and_consumes_nothing(self):
        self.assertEqual(self.fx.redeem(self.fx.new_key())[0], 200)
        key = self.fx.new_key()
        status, body, _ = self.fx.redeem(key)
        self.assertEqual(status, 409)
        self.assertEqual(body["error"], "user {} already exists".format(ACCOUNT))
        self.assertEqual(self.fx.bootstrap_list()[1]["bootstrap_keys"][1]["status"], "available")
        self.assertEqual(self.fx.irods.created, [ACCOUNT])
        # the same key works for another host name
        self.assertEqual(self.fx.redeem(key, host="rs02.example.org")[0], 200)
        self.assertEqual(self.fx.irods.created, [ACCOUNT, "adm.rs02.example.org"])

    def test_irods_failure_leaves_key_available(self):
        key = self.fx.new_key()
        for error in (controlplane.HTTPError(502, "cannot reach iRODS: fake outage"),
                      controlplane.HTTPError(500, "creating user failed: CAT_SQL_ERR")):
            self.fx.irods.create_error = error
            status, body, _ = self.fx.redeem(key)
            self.assertEqual((status, body["error"]), (error.status, error.message))
            self.assertEqual(self.fx.bootstrap_list()[1]["bootstrap_keys"][0]["status"], "available")
            self.assertEqual(self.fx.irods.created, [])
        self.fx.irods.create_error = None
        self.assertEqual(self.fx.redeem(key)[0], 200)

    def test_failed_state_write_removes_the_account(self):
        key = self.fx.new_key()
        state_before = open(self.fx.config["state_file"]).read()
        with unittest.mock.patch.object(self.fx.app.state, "save", side_effect=OSError("disk full")):
            with self.assertLogs("controlplane", level="ERROR") as logs:
                status, body, _ = self.fx.redeem(key)
        self.assertEqual(status, 500)
        self.assertIn("persisting the redemption failed: disk full; account removed", body["error"])
        self.assertTrue(any("removing account " + ACCOUNT in line for line in logs.output), logs.output)
        self.assertEqual(self.fx.irods.created, [ACCOUNT])
        self.assertNotIn(ACCOUNT, self.fx.irods.users)
        self.assertEqual(open(self.fx.config["state_file"]).read(), state_before)
        rec = self.fx.bootstrap_list()[1]["bootstrap_keys"][0]
        self.assertEqual((rec["status"], rec["host"], rec["service_account"]), ("available", None, None))
        # with the write working again the same key and host succeed
        self.assertEqual(self.fx.redeem(key)[0], 200)
        self.assertIn(ACCOUNT, self.fx.irods.users)

    def test_failed_state_write_and_failed_rollback_say_so(self):
        key = self.fx.new_key()
        self.fx.irods.remove_error = "CAT_SQL_ERR"
        with unittest.mock.patch.object(self.fx.app.state, "save", side_effect=OSError("disk full")):
            with self.assertLogs("controlplane", level="ERROR"):
                status, body, _ = self.fx.redeem(key)
        self.assertEqual(status, 500)
        self.assertIn("rollback rmuser also failed: CAT_SQL_ERR; manual cleanup required", body["error"])
        self.assertIn(ACCOUNT, self.fx.irods.users)
        self.assertEqual(self.fx.bootstrap_list()[1]["bootstrap_keys"][0]["status"], "available")

    def test_redeem_provider_includes_database(self):
        status, body, _ = self.fx.redeem(self.fx.new_key("provider"))
        self.assertEqual(status, 200)
        self.assertEqual(body["zone_facts"], expected_facts("provider"))
        self.assertEqual(body["zone_facts"]["database"], HOME_DATABASE)
        self.assertIn("db-secret", json.dumps(body))

    def test_role_is_fixed_by_the_key(self):
        key = self.fx.new_key("consumer")
        status, body, _ = self.fx.request("POST", "/bootstrap/redeem",
                                          {"bootstrap_key": key, "catalog_service_role": "provider"})
        self.assertEqual(status, 400)
        self.assertIn("unknown field(s): catalog_service_role", body["error"])
        self.assertEqual(self.fx.bootstrap_list()[1]["bootstrap_keys"][0]["status"], "available")

    def test_unknown_key_is_403_and_consumes_nothing(self):
        self.fx.new_key()
        with self.assertLogs("controlplane", level="WARNING") as logs:
            status, body, _ = self.fx.redeem(str(uuid.uuid4()))
        self.assertEqual(status, 403)
        self.assertIn("not valid", body["error"])
        self.assertTrue(any("refused a bootstrap redemption from 127.0.0.1" in line for line in logs.output),
                        logs.output)
        self.assertEqual(self.fx.bootstrap_list()[1]["bootstrap_keys"][0]["status"], "available")

    def test_validation(self):
        key = self.fx.new_key()
        cases = [
            ([], "JSON object"),
            ({}, "missing required field: bootstrap_key"),
            ({"bootstrap_key": "not-a-uuid", "host": HOST}, "bootstrap_key must be"),
            ({"bootstrap_key": key.upper(), "host": HOST}, "bootstrap_key must be"),
            ({"bootstrap_key": 5, "host": HOST}, "bootstrap_key must be"),
            ({"bootstrap_key": key}, "missing required field: host"),
            ({"bootstrap_key": key, "host": "10.0.0.1"}, "host must be a hostname"),
            ({"bootstrap_key": key, "host": "rs01_example"}, "host must be a hostname"),
            ({"bootstrap_key": key, "host": ("ab." * 21)[:-1]}, "host is too long"),
            ({"bootstrap_key": key, "host": HOST, "note": "x"}, "unknown field(s): note"),
        ]
        for body, fragment in cases:
            status, resp, _ = self.fx.request("POST", "/bootstrap/redeem", body)
            self.assertEqual(status, 400, body)
            self.assertIn(fragment, resp["error"])
        self.assertEqual(self.fx.request("POST", "/bootstrap/redeem", raw_body=b"not json")[0], 400)
        self.assertEqual(self.fx.request("POST", "/bootstrap/redeem", raw_body=b"a" * 70000)[0], 413)
        self.assertEqual(self.fx.request("GET", "/bootstrap/redeem")[0], 405)
        self.assertEqual(self.fx.bootstrap_list()[1]["bootstrap_keys"][0]["status"], "available")
        self.assertEqual(self.fx.irods.created, [])

    def test_x_forwarded_for_is_recorded(self):
        key = self.fx.new_key()
        self.assertEqual(self.fx.redeem(key, headers={"X-Forwarded-For": "203.0.113.9, 10.0.0.1"})[0], 200)
        self.assertEqual(self.fx.bootstrap_list()[1]["bootstrap_keys"][0]["redeemed_from"], "203.0.113.9")

    def test_facts_are_read_at_redeem_time(self):
        key = self.fx.new_key()
        self.assertEqual(self.fx.state()["keyserver"]["zone_key"]["value"], HOME_ZONE_KEY)
        cfg = self.fx.server_config()
        cfg["zone_key"] = "ROTATED_BY_HAND"
        write_json(self.fx.server_config_file, cfg)
        with self.assertLogs("controlplane", level="WARNING") as logs:
            status, body, _ = self.fx.redeem(key)
        self.assertEqual(status, 200)
        self.assertEqual(body["zone_facts"]["zone_key"], "ROTATED_BY_HAND")
        self.assertTrue(any("changed by hand" in line for line in logs.output), logs.output)
        self.assertEqual(self.fx.state()["keyserver"]["zone_key"]["value"], "ROTATED_BY_HAND")

    def test_missing_fact_leaves_key_available(self):
        key = self.fx.new_key()
        cfg = self.fx.server_config()
        del cfg["encryption"]
        write_json(self.fx.server_config_file, cfg)
        status, body, _ = self.fx.redeem(key)
        self.assertEqual(status, 500)
        self.assertIn("encryption", body["error"])
        self.assertEqual(self.fx.bootstrap_list()[1]["bootstrap_keys"][0]["status"], "available")
        self.assertEqual(self.fx.irods.created, [])
        write_json(self.fx.server_config_file, home_server_config())
        self.assertEqual(self.fx.redeem(key)[0], 200)


class BootstrapAccountPrefixTests(ServerTestCase):
    config_overrides = {"bootstrap": {"new_admin_account_prefix": "svc-"}}

    def test_prefix_from_configuration(self):
        status, body, _ = self.fx.redeem(self.fx.new_key())
        self.assertEqual(status, 200)
        self.assertEqual(body["service_account"]["name"], "svc-" + HOST)
        self.assertEqual(body["zone_facts"], expected_facts("consumer", "svc-" + HOST))
        self.assertEqual(self.fx.irods.created, ["svc-" + HOST])
        self.assertEqual(self.fx.bootstrap_list()[1]["bootstrap_keys"][0]["service_account"], "svc-" + HOST)


class BootstrapEmptyPrefixTests(ServerTestCase):
    config_overrides = {"bootstrap": {"new_admin_account_prefix": ""}}

    def test_account_is_the_host_name(self):
        status, body, _ = self.fx.redeem(self.fx.new_key())
        self.assertEqual(status, 200)
        self.assertEqual(body["service_account"]["name"], HOST)
        self.assertEqual(body["zone_facts"]["zone_user"], HOST)


class BootstrapRevokeTests(ServerTestCase):
    def test_revoke(self):
        key = self.fx.new_key("provider", note="oops")
        before = controlplane.utc_now()
        status, body, _ = self.fx.revoke(key)
        self.assertEqual(status, 200)
        self.assertEqual(set(body), {"bootstrap_key"})
        rec = body["bootstrap_key"]
        self.assertEqual(set(rec), KEY_FIELDS)
        self.assertEqual((rec["bootstrap_key"], rec["status"], rec["revoked_by"], rec["note"]),
                         (key, "revoked", "rods", "oops"))
        self.assertLessEqual(abs((controlplane.from_iso(rec["revoked_at"]) - before).total_seconds()), 5)
        self.assertEqual((rec["redeemed_at"], rec["redeemed_from"]), (None, None))
        self.assertEqual(self.fx.bootstrap_list()[1]["bootstrap_keys"], [rec])
        self.assertEqual(self.fx.state()["bootstrap"]["keys"], [rec])
        # a revoked key cannot be redeemed, and is not distinguished from an unknown one
        status, body, _ = self.fx.redeem(key)
        self.assertEqual(status, 403)
        self.assertIn("not valid", body["error"])
        # nor revoked again
        status, body, _ = self.fx.revoke(key)
        self.assertEqual(status, 409)
        self.assertIn("already revoked", body["error"])
        self.assertEqual((self.fx.irods.zones, self.fx.hups()), ({}, 0))

    def test_redeemed_key_cannot_be_revoked(self):
        key = self.fx.new_key()
        self.assertEqual(self.fx.redeem(key)[0], 200)
        status, body, _ = self.fx.revoke(key)
        self.assertEqual(status, 409)
        self.assertIn("already redeemed", body["error"])
        self.assertEqual(self.fx.bootstrap_list()[1]["bootstrap_keys"][0]["status"], "redeemed")

    def test_unknown_key_is_404(self):
        status, body, _ = self.fx.revoke(str(uuid.uuid4()))
        self.assertEqual(status, 404)
        self.assertIn("no such bootstrap key", body["error"])

    def test_validation_and_auth(self):
        key = self.fx.new_key()
        for body, fragment in (([], "JSON object"), ({}, "missing required field"),
                               ({"bootstrap_key": "nope"}, "bootstrap_key must be"),
                               ({"bootstrap_key": key, "x": 1}, "unknown field(s): x")):
            status, resp, _ = self.fx.request("POST", "/bootstrap/revoke", body, auth=RODS)
            self.assertEqual(status, 400, body)
            self.assertIn(fragment, resp["error"])
        self.assertEqual(self.fx.revoke(key, auth=None)[0], 401)
        self.assertEqual(self.fx.revoke(key, auth=("alice", "alicepass"))[0], 403)
        self.assertEqual(self.fx.revoke(key, auth=("unreachable", "x"))[0], 502)
        # the body is parsed before the credentials are checked
        self.assertEqual(self.fx.request("POST", "/bootstrap/revoke", raw_body=b"nope")[0], 400)
        self.assertEqual(self.fx.request("POST", "/bootstrap/revoke", raw_body=b"a" * 70000, auth=RODS)[0], 413)
        self.assertEqual(self.fx.request("GET", "/bootstrap/revoke", auth=RODS)[0], 405)
        self.assertEqual(self.fx.bootstrap_list()[1]["bootstrap_keys"][0]["status"], "available")


class BootstrapListTests(ServerTestCase):
    def test_filters_order_and_auth(self):
        a = self.fx.new_key("consumer")
        time.sleep(1.1)  # distinct created_at seconds
        b = self.fx.new_key("provider")
        c = self.fx.new_key("consumer", note="third")
        self.assertEqual(self.fx.redeem(b)[0], 200)
        self.assertEqual(self.fx.revoke(c)[0], 200)
        status, body, _ = self.fx.bootstrap_list()
        self.assertEqual(status, 200)
        self.assertEqual(set(body), {"bootstrap_keys"})
        self.assertEqual([k["bootstrap_key"] for k in body["bootstrap_keys"]], [a, b, c])
        self.assertEqual([k["status"] for k in body["bootstrap_keys"]], ["available", "redeemed", "revoked"])
        self.assertEqual([(k["host"], k["service_account"]) for k in body["bootstrap_keys"]],
                         [(None, None), (HOST, ACCOUNT), (None, None)])
        for wanted, keys in (("available", [a]), ("redeemed", [b]), ("revoked", [c]), ("expired", []),
                             ("all", [a, b, c])):
            _, body, _ = self.fx.bootstrap_list("?status=" + wanted)
            self.assertEqual([k["bootstrap_key"] for k in body["bootstrap_keys"]], keys, wanted)
        status, body, _ = self.fx.bootstrap_list("?status=pending")
        self.assertEqual(status, 400)
        self.assertIn("status must be one of", body["error"])
        self.assertEqual(self.fx.bootstrap_list("?status=all&status=available")[0], 400)
        self.assertEqual(self.fx.bootstrap_list(auth=None)[0], 401)
        self.assertEqual(self.fx.bootstrap_list(auth=("alice", "alicepass"))[0], 403)
        # the federation queue is separate
        self.assertEqual(self.fx.request("GET", "/federation/list?status=all", auth=RODS)[1]["applications"], [])


class BootstrapKeyExpiryTests(ServerTestCase):
    config_overrides = {"bootstrap": {"default_lifetime_of_bootstrap_keys_in_seconds": 1}}

    def test_expired_key_is_refused_listed_and_unrevokable(self):
        key = self.fx.new_key()
        long_lived = self.fx.new_key(lifetime_in_seconds=3600)
        time.sleep(2.1)
        _, listing, _ = self.fx.bootstrap_list()
        self.assertEqual([k["status"] for k in listing["bootstrap_keys"]], ["expired", "available"])
        status, body, _ = self.fx.redeem(key)
        self.assertEqual(status, 403)
        self.assertIn("expired", body["error"])
        status, body, _ = self.fx.revoke(key)
        self.assertEqual(status, 409)
        self.assertIn("already expired", body["error"])
        self.assertEqual(self.fx.bootstrap_list("?status=expired")[1]["bootstrap_keys"][0]["bootstrap_key"], key)
        self.assertEqual(self.fx.redeem(long_lived)[0], 200)


class BootstrapConcurrencyTests(ServerTestCase):
    def test_single_use_under_concurrency(self):
        key = self.fx.new_key()
        results = []
        barrier = threading.Barrier(16)

        def go():
            barrier.wait()
            results.append(self.fx.redeem(key)[0])
        threads = [threading.Thread(target=go) for _ in range(16)]
        for t in threads:
            t.start()
        for t in threads:
            t.join()
        self.assertEqual(sorted(results), [200] + [403] * 15)
        _, listing, _ = self.fx.bootstrap_list()
        self.assertEqual([k["status"] for k in listing["bootstrap_keys"]], ["redeemed"])
        self.assertEqual(self.fx.irods.created, [ACCOUNT])


# ---------------------------------------------------------------------------
# App start-up checks and main()
# ---------------------------------------------------------------------------

class StartupTests(unittest.TestCase):
    def test_zone_name_mismatch_is_a_config_error(self):
        fx = Fixture(start_server=False)
        try:
            cfg = fx.server_config()
            cfg["zone_name"] = "someOtherZone"
            write_json(fx.server_config_file, cfg)
            app = controlplane.App(fx.config)
            app.irods = FakeIRODS()
            with self.assertRaises(controlplane.ConfigError) as cm:
                app.start()
            self.assertIn("does not match", str(cm.exception))
        finally:
            fx.close()

    def test_real_irods_wrapper_reads_environment(self):
        fx = Fixture(start_server=False)
        try:
            app = controlplane.App(fx.config)
            self.assertIsInstance(app.irods, controlplane.IRODS)
            self.assertEqual((app.irods.host, app.irods.port, app.irods.zone), (HOME_HOST, 1247, HOME_ZONE))
        finally:
            fx.close()


class MainTests(unittest.TestCase):
    def run_main(self, argv):
        import io
        from contextlib import redirect_stdout
        out = io.StringIO()
        with redirect_stdout(out):
            rc = controlplane.main(argv)
        return rc, out.getvalue()

    def test_usage(self):
        for argv in (["controlplane"], ["controlplane", "--help"], ["controlplane", "a", "b"]):
            rc, out = self.run_main(argv)
            self.assertEqual(rc, 2)
            self.assertIn("usage:", out)

    def test_bad_config(self):
        with tempfile.TemporaryDirectory() as d:
            rc, out = self.run_main(["controlplane", os.path.join(d, "missing.json")])
            self.assertEqual(rc, 1)
            self.assertIn("configuration file not found", out)
            p = os.path.join(d, "controlplane.json")
            write_json(p, {"port": 1, "irods_environment_file": os.path.join(d, "none.json")})
            rc, out = self.run_main(["controlplane", p])
            self.assertEqual(rc, 1)
            self.assertIn("iRODS environment file not found", out)

    def test_startup_failure_outside_config_errors(self):
        fx = Fixture(start_server=False)
        try:
            os.unlink(fx.server_config_file)
            rc, out = self.run_main(["controlplane", fx.config_file])
            self.assertEqual(rc, 1)
            self.assertIn("startup failed", out)
        finally:
            fx.close()

    def test_bind_failure(self):
        fx = Fixture(start_server=False)
        try:
            taken = socket.socket()
            taken.bind(("127.0.0.1", 0))
            taken.listen(1)
            port = taken.getsockname()[1]
            cfg = json.load(open(fx.config_file))
            cfg.update({"port": port, "bind_address": "127.0.0.1"})
            write_json(fx.config_file, cfg)
            rc, out = self.run_main(["controlplane", fx.config_file])
            taken.close()
            self.assertEqual(rc, 1)
            self.assertIn("cannot listen on", out)
        finally:
            fx.close()

    def test_serve_and_terminate(self):
        fx = Fixture(start_server=False)
        try:
            probe = socket.socket()
            probe.bind(("127.0.0.1", 0))
            port = probe.getsockname()[1]
            probe.close()
            cfg = json.load(open(fx.config_file))
            cfg.update({"port": port, "bind_address": "127.0.0.1"})
            write_json(fx.config_file, cfg)
            proc = subprocess.Popen([sys.executable, controlplane.__file__, fx.config_file],
                                    stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True)
            try:
                for _ in range(200):
                    try:
                        conn = http.client.HTTPConnection("127.0.0.1", port, timeout=1)
                        conn.request("GET", "/federation/status?application_id=x")
                        status = conn.getresponse().status
                        conn.close()
                        break
                    except OSError:
                        time.sleep(0.05)
                else:
                    self.fail("server did not come up")
                self.assertEqual(status, 404)
            finally:
                proc.send_signal(signal.SIGTERM)
                out, _ = proc.communicate(timeout=20)
            self.assertEqual(proc.returncode, 0, out)
            self.assertIn("serving plain HTTP", out)
            self.assertIn("received signal", out)
            self.assertIn("stopped", out)
        finally:
            fx.close()


# ---------------------------------------------------------------------------
# Live iRODS tests (opt-in)
# ---------------------------------------------------------------------------

LIVE_ENV = os.environ.get("CONTROLPLANE_LIVE_IRODS_ENV_FILE")
LIVE_PASSWORD = os.environ.get("CONTROLPLANE_LIVE_ADMIN_PASSWORD")
LIVE_RODSUSER = os.environ.get("CONTROLPLANE_LIVE_RODSUSER")
LIVE_RODSUSER_PASSWORD = os.environ.get("CONTROLPLANE_LIVE_RODSUSER_PASSWORD")


class ServiceAccountCreationTests(unittest.TestCase):
    """IRODS.create_service_account against a mocked python-irodsclient session."""

    def setUp(self):
        self.irods = controlplane.IRODS("/nonexistent/env.json", {"irods_host": HOME_HOST, "irods_port": 1247,
                                                                 "irods_zone_name": HOME_ZONE})
        self.session = unittest.mock.Mock()
        self.irods.service_session = unittest.mock.Mock(return_value=self.session)

    def expect(self, status, fragment):
        with self.assertRaises(controlplane.HTTPError) as cm:
            self.irods.create_service_account(ACCOUNT, "p" * 32)
        self.assertEqual(cm.exception.status, status, cm.exception.message)
        self.assertIn(fragment, cm.exception.message)
        if self.irods.service_session.return_value is self.session:
            self.assertTrue(self.session.cleanup.called)

    def test_success_creates_a_rodsadmin_and_sets_the_password(self):
        self.irods.create_service_account(ACCOUNT, "p" * 32)
        self.session.users.create.assert_called_once_with(ACCOUNT, "rodsadmin")
        self.session.users.modify.assert_called_once_with(ACCOUNT, "password", "p" * 32)
        self.session.users.remove.assert_not_called()
        self.session.cleanup.assert_called_once_with()

    def test_existing_user_is_409(self):
        self.session.users.create.side_effect = irods.exception.CATALOG_ALREADY_HAS_ITEM_BY_THAT_NAME()
        self.expect(409, "user {} already exists".format(ACCOUNT))
        self.session.users.modify.assert_not_called()

    def test_unreachable_is_502(self):
        self.session.users.create.side_effect = irods.exception.NetworkException("refused")
        self.expect(502, "cannot reach iRODS")

    def test_other_create_failure_is_500(self):
        self.session.users.create.side_effect = RuntimeError("CAT_SQL_ERR")
        self.expect(500, "creating user {} failed: CAT_SQL_ERR".format(ACCOUNT))

    def test_session_failure_is_500(self):
        self.irods.service_session = unittest.mock.Mock(side_effect=FileNotFoundError("no .irodsA"))
        self.expect(500, "cannot open the service account session: no .irodsA")

    def test_password_failure_removes_the_account(self):
        self.session.users.modify.side_effect = RuntimeError("PAM_AUTH_PASSWORD_INVALID")
        with self.assertLogs("controlplane", level="ERROR"):
            self.expect(500, "setting the password of {} failed: PAM_AUTH_PASSWORD_INVALID; account removed"
                        .format(ACCOUNT))
        self.session.users.remove.assert_called_once_with(ACCOUNT)

    def test_password_failure_and_failed_removal_say_so(self):
        self.session.users.modify.side_effect = RuntimeError("boom")
        self.session.users.remove.side_effect = RuntimeError("CAT_SQL_ERR")
        with self.assertLogs("controlplane", level="ERROR"):
            self.expect(500, "rollback rmuser also failed: CAT_SQL_ERR; manual cleanup required")

    def test_remove_service_account(self):
        self.irods.remove_service_account(ACCOUNT)
        self.session.users.remove.assert_called_once_with(ACCOUNT)
        self.session.cleanup.assert_called_once_with()
        self.session.users.remove.side_effect = irods.exception.UserDoesNotExist()
        with self.assertRaises(irods.exception.UserDoesNotExist):
            self.irods.remove_service_account(ACCOUNT)
        self.assertEqual(self.session.cleanup.call_count, 2)


@unittest.skipUnless(LIVE_ENV and LIVE_PASSWORD, "set CONTROLPLANE_LIVE_IRODS_ENV_FILE and CONTROLPLANE_LIVE_ADMIN_PASSWORD")
class LiveIRODSTests(unittest.TestCase):
    def setUp(self):
        self.env = controlplane.load_irods_environment(LIVE_ENV)
        self.irods = controlplane.IRODS(LIVE_ENV, self.env)
        self.admin = self.env["irods_user_name"]

    def test_admin_session_success(self):
        session = self.irods.admin_session(self.admin, LIVE_PASSWORD)
        try:
            self.assertEqual(session.users.get(self.admin).type, "rodsadmin")
            self.assertFalse(self.irods.zone_exists(session, "controlplane_no_such_zone_" + controlplane.generate_key()[:8]))
            self.assertTrue(self.irods.zone_exists(session, self.irods.zone))
        finally:
            session.cleanup()

    def test_admin_session_bad_password(self):
        with self.assertRaises(controlplane.HTTPError) as cm:
            self.irods.admin_session(self.admin, "definitely-wrong-" + controlplane.generate_key())
        self.assertEqual(cm.exception.status, 401)

    @unittest.skipUnless(LIVE_RODSUSER and LIVE_RODSUSER_PASSWORD,
                         "set CONTROLPLANE_LIVE_RODSUSER and CONTROLPLANE_LIVE_RODSUSER_PASSWORD")
    def test_rodsuser_is_403(self):
        with self.assertRaises(controlplane.HTTPError) as cm:
            self.irods.admin_session(LIVE_RODSUSER, LIVE_RODSUSER_PASSWORD)
        self.assertEqual(cm.exception.status, 403)

    def test_service_session(self):
        session = self.irods.service_session()
        try:
            self.assertEqual(session.zone, self.irods.zone)
        finally:
            session.cleanup()

    def test_create_and_remove_service_account(self):
        name = "controlplane.test." + controlplane.generate_key()[:8].replace("_", "x")
        password = controlplane.generate_key()
        self.irods.create_service_account(name, password)
        try:
            session = self.irods.admin_session(name, password)  # logs in and is a rodsadmin
            session.cleanup()
            with self.assertRaises(controlplane.HTTPError) as cm:
                self.irods.create_service_account(name, controlplane.generate_key())
            self.assertEqual((cm.exception.status, cm.exception.message), (409, "user {} already exists".format(name)))
        finally:
            self.irods.remove_service_account(name)
        with self.assertRaises(controlplane.HTTPError) as cm:
            self.irods.admin_session(name, password)
        self.assertEqual(cm.exception.status, 401)
        with self.assertRaises(Exception):
            self.irods.remove_service_account(name)

    def test_create_service_account_unreachable_is_502(self):
        env = dict(self.env)
        env["irods_port"] = 1
        with tempfile.TemporaryDirectory() as d:
            env_file = os.path.join(d, "irods_environment.json")
            write_json(env_file, env)
            with self.assertRaises(controlplane.HTTPError) as cm:
                controlplane.IRODS(env_file, env).create_service_account("controlplane.unreachable", "x" * 32)
        self.assertEqual(cm.exception.status, 502)
        self.assertIn("cannot reach iRODS", cm.exception.message)

    def test_mkzone_and_rmzone(self):
        session = self.irods.admin_session(self.admin, LIVE_PASSWORD)
        name = "controlplaneTestZone" + controlplane.generate_key()[:6].replace("_", "x")
        try:
            self.irods.mkzone(session, name, "localhost:1299")
            self.assertTrue(self.irods.zone_exists(session, name))
        finally:
            try:
                self.irods.rmzone(session, name)
            finally:
                session.cleanup()
        self.assertFalse(self.irods.zone_exists(session, name))


if __name__ == "__main__":
    unittest.main()
