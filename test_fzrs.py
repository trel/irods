#!/usr/bin/env python3
"""Tests for fzrs.py, the iRODS Federation Zone Registrar Service.

How to run
----------

python-irodsclient must be importable. If it is not installed, point
PYTHONPATH at a checkout of it:

    export PYTHONPATH=/src/python-irodsclient

Run the suite with either unittest or pytest:

    python3 -m unittest -v test_fzrs
    python3 -m pytest -v test_fzrs.py

With coverage (python3-coverage):

    python3 -m coverage run --branch --include=fzrs.py -m pytest -q test_fzrs.py
    python3 -m coverage report -m

The HTTP tests start the real fzrs server in-process on an ephemeral port and
drive it with http.client. iRODS is replaced by a fake, so no live zone is
needed. Reload is exercised against a real child process that records the
SIGHUP it receives.

Live tests against a running zone are skipped unless both of these are set:

    FZRS_LIVE_IRODS_ENV_FILE   path to the service account irods_environment.json
    FZRS_LIVE_ADMIN_PASSWORD   password of the irods_user_name in that file

and one more live test runs when a rodsuser is also provided:

    FZRS_LIVE_RODSUSER / FZRS_LIVE_RODSUSER_PASSWORD

The live tests must run as the iRODS service account, because
python-irodsclient looks for the .irodsA password file in the running user's
home directory. For example:

    su - irods -c 'cd /src/irods && PYTHONPATH=/src/python-irodsclient \
        FZRS_LIVE_IRODS_ENV_FILE=/var/lib/irods/.irods/irods_environment.json \
        FZRS_LIVE_ADMIN_PASSWORD=... python3 -m pytest -q test_fzrs.py -k LiveIRODS'
"""

import base64
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

from cryptography.hazmat.primitives import hashes, serialization
from cryptography.hazmat.primitives.asymmetric import ec, padding, rsa
from cryptography.hazmat.primitives.ciphers.aead import AESGCM

import fzrs

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
# Applicant keys (generated once per run)
# ---------------------------------------------------------------------------

def _pem_public(key):
    return key.public_key().public_bytes(
        serialization.Encoding.PEM, serialization.PublicFormat.SubjectPublicKeyInfo).decode()


RSA_KEY = rsa.generate_private_key(public_exponent=65537, key_size=2048)
RSA_PUB_PEM = _pem_public(RSA_KEY)
RSA_PUB_SSH = RSA_KEY.public_key().public_bytes(
    serialization.Encoding.OpenSSH, serialization.PublicFormat.OpenSSH).decode() + " applicant@example.org"
RSA_SMALL_PUB_PEM = _pem_public(rsa.generate_private_key(public_exponent=65537, key_size=1024))
EC_PUB_PEM = _pem_public(ec.generate_private_key(ec.SECP256R1()))


def b64d(s):
    return base64.urlsafe_b64decode(s + "=" * (-len(s) % 4))


def decrypt_payload(jwe, private_key=RSA_KEY):
    """The applicant-side decrypt from fzrs-spec.md section 5.5."""
    header, enc_key, iv, ciphertext, tag = jwe.split(".")
    cek = private_key.decrypt(b64d(enc_key), padding.OAEP(padding.MGF1(hashes.SHA256()), hashes.SHA256(), None))
    plaintext = AESGCM(cek).decrypt(b64d(iv), b64d(ciphertext) + b64d(tag), header.encode())
    return json.loads(plaintext)


def openssl_fingerprint(pem):
    """The applicant-side fingerprint recipe from fzrs-spec.md section 5.5."""
    der = subprocess.run(["openssl", "pkey", "-pubin", "-outform", "DER"], input=pem.encode(),
                         capture_output=True, check=True).stdout
    digest = subprocess.run(["openssl", "dgst", "-sha256", "-binary"], input=der,
                            capture_output=True, check=True).stdout
    return "SHA256:" + base64.b64encode(digest).decode().rstrip("=")


# ---------------------------------------------------------------------------
# Fakes
# ---------------------------------------------------------------------------

class FakeSession:
    def __init__(self):
        self.cleaned_up = False

    def cleanup(self):
        self.cleaned_up = True


class FakeIRODS:
    """Stands in for fzrs.IRODS. Users map name -> (password, type)."""

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

    host = property(lambda self: self.env["irods_host"])
    port = property(lambda self: self.env["irods_port"])
    zone = property(lambda self: self._zone)

    def admin_session(self, username, password):
        if username == "unreachable":
            raise fzrs.HTTPError(502, "cannot reach iRODS: fake outage")
        entry = self.users.get(username)
        if entry is None or entry[0] != password:
            raise fzrs.HTTPError(401, "invalid iRODS credentials",
                                 {"WWW-Authenticate": 'Basic realm="fzrs"'})
        if entry[1] != "rodsadmin":
            raise fzrs.HTTPError(403, "user {} is not a rodsadmin".format(username))
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


class Fixture:
    """Temp directory with config, env, server_config, HUP child, and running server."""

    def __init__(self, config_overrides=None, start_server=True):
        self.tmp = tempfile.TemporaryDirectory(prefix="fzrs-test-")
        d = self.tmp.name
        self.server_config_file = os.path.join(d, "server_config.json")
        self.env_file = os.path.join(d, "irods_environment.json")
        self.config_file = os.path.join(d, "fzrs.json")
        self.pid_file = os.path.join(d, "server.pid")
        self.hup_log = os.path.join(d, "hup.log")

        write_json(self.server_config_file, {
            "zone_name": HOME_ZONE, "zone_key": HOME_ZONE_KEY, "zone_port": 1247,
            "catalog_provider_hosts": [HOME_HOST], "federation": [],
            "negotiation_key": "home_negotiation_key_never_shown",
        })
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

        self.config = fzrs.load_config(self.config_file)
        self.app = fzrs.App(self.config)
        self.irods = FakeIRODS()
        self.app.irods = self.irods
        self.app.start()

        self.server = None
        if start_server:
            self.server = fzrs.Server(("127.0.0.1", 0), self.app)
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
        return resp.status, decoded, dict(resp.getheaders())

    def apply(self, zone_name="otherZone", hosts=("localhost",), port=1247, zone_key=None, **extra):
        body = {"zone_name": zone_name, "zone_port": port, "catalog_provider_hosts": list(hosts)}
        if zone_key is not None:
            body["zone_key"] = zone_key
        body.update(extra)
        return self.request("POST", "/apply", body)

    def judge(self, application_id, decision, note=None, auth=("rods", "rods")):
        body = {"application_id": application_id, "decision": decision}
        if note is not None:
            body["decision_note"] = note
        return self.request("POST", "/judge", body, auth=auth)


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
            key = fzrs.generate_key()
            self.assertRegex(key, r"^[A-Za-z0-9_]{32}$")
            self.assertTrue(fzrs.NEGOTIATION_KEY_RE.match(key))
            self.assertTrue(fzrs.ZONE_KEY_RE.match(key))
        self.assertNotEqual(fzrs.generate_key(), fzrs.generate_key())

    def test_is_valid_host(self):
        for good in ("localhost", "icat.example.org", "a-b.c", "10.0.0.5", "::1", "x" * 63,
                     "icat.example.org."):
            self.assertTrue(fzrs.is_valid_host(good), good)
        for bad in ("", "-bad", "bad-", "a..b", "x" * 64, "a" * 254, "under_score", None, 5,
                    "host name", "host:1247"):
            self.assertFalse(fzrs.is_valid_host(bad), repr(bad))

    def test_is_int_rejects_bool(self):
        self.assertTrue(fzrs.is_int(0))
        self.assertFalse(fzrs.is_int(True))
        self.assertFalse(fzrs.is_int("1"))

    def test_iso_roundtrip(self):
        now = fzrs.utc_now()
        self.assertEqual(fzrs.from_iso(fzrs.to_iso(now)), now)
        self.assertEqual(now.microsecond, 0)

    def test_atomic_write_json_cleans_up_on_failure(self):
        with tempfile.TemporaryDirectory() as d:
            path = os.path.join(d, "out.json")
            with self.assertRaises(TypeError):
                fzrs.atomic_write_json(path, {"x": object()})
            self.assertEqual(os.listdir(d), [])
            fzrs.atomic_write_json(path, {"x": 1}, mode=0o600)
            self.assertEqual(oct(os.stat(path).st_mode & 0o777), "0o600")
            self.assertEqual(json.load(open(path)), {"x": 1})


class ConfigTests(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.addCleanup(self.tmp.cleanup)
        self.path = os.path.join(self.tmp.name, "fzrs.json")

    def load(self, obj):
        if isinstance(obj, str):
            with open(self.path, "w") as f:
                f.write(obj)
        else:
            write_json(self.path, obj)
        return fzrs.load_config(self.path)

    def test_defaults_and_relative_paths(self):
        os.environ.pop("IRODS_ENVIRONMENT_FILE", None)
        cfg = self.load({"port": 9100, "queue_file": "q.json"})
        self.assertEqual(cfg["bind_address"], "0.0.0.0")
        self.assertEqual(cfg["application_expiration_in_seconds"], 7 * 24 * 60 * 60)
        self.assertEqual(cfg["max_pending_applications"], 100)
        self.assertEqual(cfg["max_request_body_bytes"], 65536)
        self.assertIs(cfg["require_public_key"], False)
        self.assertEqual(cfg["queue_file"], os.path.join(self.tmp.name, "q.json"))
        self.assertEqual(cfg["server_config_file"], "/etc/irods/server_config.json")
        self.assertEqual(cfg["server_pid_file"], "/var/run/irods/irods-server.pid")
        self.assertTrue(cfg["irods_environment_file"].endswith("/.irods/irods_environment.json"))
        self.assertTrue(os.path.isabs(cfg["irods_environment_file"]))

    def test_default_queue_file_sits_next_to_config(self):
        cfg = self.load({"port": 9100})
        self.assertEqual(cfg["queue_file"], os.path.join(self.tmp.name, "fzrs-queue.json"))

    def test_env_var_selects_environment_file(self):
        os.environ["IRODS_ENVIRONMENT_FILE"] = "/some/where/env.json"
        try:
            self.assertEqual(self.load({"port": 1})["irods_environment_file"], "/some/where/env.json")
        finally:
            del os.environ["IRODS_ENVIRONMENT_FILE"]

    def test_errors(self):
        cases = [
            ({}, "missing required"),
            ({"port": 1, "bogus": 2}, "unknown configuration key"),
            ({"port": 0}, "port must be"),
            ({"port": 70000}, "port must be"),
            ({"port": "9100"}, "port must be"),
            ({"port": 1, "application_expiration_in_seconds": -1}, "application_expiration"),
            ({"port": 1, "max_pending_applications": True}, "max_pending"),
            ({"port": 1, "bind_address": ""}, "bind_address"),
            ({"port": 1, "queue_file": 7}, "queue_file"),
            ({"port": 1, "require_public_key": "yes"}, "require_public_key"),
            ([1, 2], "JSON object"),
            ("not json", "not valid JSON"),
        ]
        for obj, fragment in cases:
            with self.assertRaises(fzrs.ConfigError, msg=repr(obj)) as cm:
                self.load(obj)
            self.assertIn(fragment, str(cm.exception))

    def test_missing_file(self):
        with self.assertRaises(fzrs.ConfigError):
            fzrs.load_config(os.path.join(self.tmp.name, "nope.json"))

    def test_irods_environment_errors(self):
        p = os.path.join(self.tmp.name, "env.json")
        with self.assertRaises(fzrs.ConfigError):
            fzrs.load_irods_environment(p)
        with open(p, "w") as f:
            f.write("{")
        with self.assertRaises(fzrs.ConfigError):
            fzrs.load_irods_environment(p)
        write_json(p, {"irods_host": "h"})
        with self.assertRaises(fzrs.ConfigError) as cm:
            fzrs.load_irods_environment(p)
        self.assertIn("irods_port", str(cm.exception))


class QueueTests(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.addCleanup(self.tmp.cleanup)
        self.path = os.path.join(self.tmp.name, "queue.json")

    def record(self, zone, status="pending", expires=None):
        now = fzrs.utc_now()
        return {"application_id": zone + "-id", "status": status, "zone_name": zone,
                "submitted_at": fzrs.to_iso(now), "expires_at": fzrs.to_iso(expires or now)}

    def test_creates_file_and_roundtrips(self):
        q = fzrs.Queue(self.path)
        q.load()
        self.assertTrue(os.path.exists(self.path))
        self.assertEqual(oct(os.stat(self.path).st_mode & 0o777), "0o600")
        q.applications["a-id"] = self.record("a")
        q.save()
        q2 = fzrs.Queue(self.path)
        q2.load()
        self.assertEqual(list(q2.applications), ["a-id"])

    def test_corrupt_or_wrong_schema_is_an_error(self):
        with open(self.path, "w") as f:
            f.write("{nope")
        with self.assertRaises(ValueError):
            fzrs.Queue(self.path).load()
        write_json(self.path, {"schema_version": 99, "applications": []})
        with self.assertRaises(fzrs.ConfigError):
            fzrs.Queue(self.path).load()

    def test_expire_and_counts(self):
        q = fzrs.Queue(self.path)
        q.load()
        later = fzrs.utc_now() + fzrs.datetime.timedelta(seconds=3600)
        q.applications["a-id"] = self.record("a")
        q.applications["b-id"] = self.record("b", expires=later)
        q.applications["c-id"] = self.record("c", status="approved")
        self.assertTrue(q.expire(fzrs.utc_now()))
        self.assertEqual(q.applications["a-id"]["status"], "expired")
        self.assertEqual(q.applications["b-id"]["status"], "pending")
        self.assertEqual(q.applications["c-id"]["status"], "approved")
        self.assertFalse(q.expire(fzrs.utc_now()))
        self.assertEqual(q.count_pending(), 1)
        self.assertIs(q.find_pending_by_zone_name("b"), q.applications["b-id"])
        self.assertIsNone(q.find_pending_by_zone_name("a"))


class ValidateApplyRequestTests(unittest.TestCase):
    def check(self, body, fragment):
        with self.assertRaises(fzrs.HTTPError) as cm:
            fzrs.validate_apply_request(body)
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
        out = fzrs.validate_apply_request(
            {"zone_name": "a.b_c", "zone_port": 0, "catalog_provider_hosts": ["h", "10.0.0.1"]})
        self.assertEqual(out, {"zone_name": "a.b_c", "zone_port": 0,
                               "catalog_provider_hosts": ["h", "10.0.0.1"], "zone_key": None,
                               "name": None, "email": None, "public_key": None,
                               "public_key_fingerprint": None})
        out = fzrs.validate_apply_request(
            {"zone_name": "a", "zone_port": 65535, "catalog_provider_hosts": ["h"], "zone_key": "K_1",
             "name": "Ada", "email": "ada@example.org", "public_key": RSA_PUB_PEM})
        self.assertEqual(out["zone_key"], "K_1")
        self.assertEqual((out["name"], out["email"], out["public_key"]), ("Ada", "ada@example.org", RSA_PUB_PEM))
        self.assertEqual(out["public_key_fingerprint"], openssl_fingerprint(RSA_PUB_PEM))

    def test_name_and_email_rules(self):
        base = {"zone_name": "z", "zone_port": 1247, "catalog_provider_hosts": ["h"]}
        self.check(dict(base, name=""), "name must be")
        self.check(dict(base, name=5), "name must be")
        self.check(dict(base, name="x" * 257), "name must be")
        for bad in ("", "a", "@", "a@", "@b", "no-at-sign", "a@@b", "a@b@c", 7, "x" * 250 + "@abcd"):
            self.check(dict(base, email=bad), "email must be")
        out = fzrs.validate_apply_request(dict(base, name="x" * 256, email="a@b"))
        self.assertEqual((len(out["name"]), out["email"]), (256, "a@b"))

    def test_public_key_rules(self):
        base = {"zone_name": "z", "zone_port": 1247, "catalog_provider_hosts": ["h"]}
        self.check(dict(base, public_key=""), "non-empty string")
        self.check(dict(base, public_key=5), "non-empty string")
        self.check(dict(base, public_key="x" * 16385), "at most 16384")
        self.check(dict(base, public_key="not a key"), "PEM (BEGIN PUBLIC KEY) or OpenSSH")
        self.check(dict(base, public_key="-----BEGIN PUBLIC KEY-----\ngarbage\n-----END PUBLIC KEY-----"),
                   "PEM (BEGIN PUBLIC KEY) or OpenSSH")
        self.check(dict(base, public_key=EC_PUB_PEM), "must be an RSA key")
        self.check(dict(base, public_key=RSA_SMALL_PUB_PEM), "at least 2048 bits")
        pem = fzrs.validate_apply_request(dict(base, public_key=RSA_PUB_PEM))
        ssh = fzrs.validate_apply_request(dict(base, public_key=RSA_PUB_SSH))
        self.assertEqual(pem["public_key_fingerprint"], ssh["public_key_fingerprint"])
        self.assertEqual(ssh["public_key"], RSA_PUB_SSH)


class EncryptedPayloadTests(unittest.TestCase):
    def test_jwe_structure_and_roundtrip(self):
        jwe = fzrs.encrypt_payload(RSA_PUB_PEM, {"zone_key": "z", "negotiation_key": "n"})
        parts = jwe.split(".")
        self.assertEqual(len(parts), 5)
        self.assertNotIn("=", jwe)
        self.assertEqual(json.loads(b64d(parts[0])), {"alg": "RSA-OAEP-256", "enc": "A256GCM"})
        self.assertEqual(len(b64d(parts[1])), 256)  # 2048-bit RSA
        self.assertEqual(len(b64d(parts[2])), 12)
        self.assertEqual(len(b64d(parts[4])), 16)
        self.assertEqual(decrypt_payload(jwe), {"zone_key": "z", "negotiation_key": "n"})
        self.assertEqual(decrypt_payload(fzrs.encrypt_payload(RSA_PUB_SSH, {"a": 1})), {"a": 1})

    def test_fresh_key_and_iv_each_time(self):
        a = fzrs.encrypt_payload(RSA_PUB_PEM, {"x": 1})
        b = fzrs.encrypt_payload(RSA_PUB_PEM, {"x": 1})
        self.assertNotEqual(a, b)
        self.assertNotEqual(a.split(".")[2], b.split(".")[2])

    def test_wrong_private_key_cannot_decrypt(self):
        other = rsa.generate_private_key(public_exponent=65537, key_size=2048)
        jwe = fzrs.encrypt_payload(RSA_PUB_PEM, {"x": 1})
        with self.assertRaises(ValueError):
            decrypt_payload(jwe, other)

    def test_tampered_ciphertext_is_rejected(self):
        parts = fzrs.encrypt_payload(RSA_PUB_PEM, {"x": 1}).split(".")
        ct = bytearray(b64d(parts[3]))
        ct[0] ^= 0xFF
        parts[3] = fzrs.b64url(bytes(ct))
        with self.assertRaises(Exception):
            decrypt_payload(".".join(parts))

    def test_fingerprint_matches_openssl(self):
        key = fzrs.parse_public_key(RSA_PUB_PEM)
        fp = fzrs.public_key_fingerprint(key)
        self.assertRegex(fp, r"^SHA256:[A-Za-z0-9+/]{43}$")
        self.assertEqual(fp, openssl_fingerprint(RSA_PUB_PEM))


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
        self.assertEqual(body["status_url"], "/status?application_id=" + body["application_id"])
        expires = fzrs.from_iso(body["expires_at"])
        self.assertAlmostEqual((expires - fzrs.utc_now()).total_seconds(), 7 * 24 * 3600, delta=5)
        queue = json.load(open(self.fx.config["queue_file"]))
        self.assertEqual(queue["applications"][0]["application_id"], body["application_id"])
        self.assertEqual(queue["applications"][0]["remote_address"], "127.0.0.1")
        self.assertTrue(queue["applications"][0]["zone_key_generated"])

    def test_apply_keeps_supplied_zone_key(self):
        status, body, _ = self.fx.apply(zone_key="MY_KEY")
        self.assertEqual(status, 201)
        self.assertEqual(body["zone_key"], "MY_KEY")
        _, listing, _ = self.fx.request("GET", "/list", auth=("rods", "rods"))
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
        status, body, _ = self.fx.request("POST", "/apply", {"zone_name": "x"})
        self.assertEqual(status, 400)
        self.assertIn("missing required field", body["error"])

    def test_malformed_json_is_400(self):
        status, body, _ = self.fx.request("POST", "/apply", raw_body=b"not json")
        self.assertEqual(status, 400)
        self.assertIn("not valid JSON", body["error"])

    def test_missing_content_length_is_400(self):
        data = raw_http(self.fx.port, b"POST /apply HTTP/1.0\r\nHost: x\r\n\r\n")
        self.assertIn("HTTP/1.1 400", data)
        self.assertIn("Content-Length is required", data)

    def test_bad_content_length_is_400(self):
        for value in ("abc", "-5"):
            data = raw_http(self.fx.port,
                            "POST /apply HTTP/1.0\r\nHost: x\r\nContent-Length: {}\r\n\r\n".format(value).encode())
            self.assertIn("HTTP/1.1 400", data, value)
            self.assertIn("Content-Length is", data)

    def test_oversized_body_is_413(self):
        status, body, _ = self.fx.request("POST", "/apply", raw_body=b"a" * 70000)
        self.assertEqual(status, 413)

    def test_x_forwarded_for_is_recorded(self):
        self.fx.request("POST", "/apply", {"zone_name": "z", "zone_port": 1, "catalog_provider_hosts": ["h"]},
                        headers={"X-Forwarded-For": "203.0.113.9, 10.0.0.1"})
        _, listing, _ = self.fx.request("GET", "/list", auth=("rods", "rods"))
        self.assertEqual(listing["applications"][0]["remote_address"], "203.0.113.9")

    def test_unknown_path_and_wrong_method(self):
        self.assertEqual(self.fx.request("GET", "/nothing")[0], 404)
        self.assertEqual(self.fx.request("GET", "/apply")[0], 405)
        status, _, headers = self.fx.request("PUT", "/list", raw_body=b"")
        self.assertEqual(status, 405)
        self.assertEqual(headers["Allow"], "GET")
        self.assertEqual(self.fx.request("DELETE", "/judge", raw_body=b"")[0], 405)
        self.assertEqual(self.fx.request("GET", "/list/", auth=("rods", "rods"))[0], 200)


class PublicKeyApplyTests(ServerTestCase):
    def test_apply_with_key_encrypts_secrets(self):
        status, body, _ = self.fx.apply(name="Ada Lovelace", email="ada@example.org", public_key=RSA_PUB_PEM)
        self.assertEqual(status, 201)
        self.assertNotIn("negotiation_key", body)
        self.assertNotIn("zone_key", body)
        self.assertNotIn("name", body)
        self.assertNotIn("email", body)
        self.assertEqual(body["public_key_fingerprint"], openssl_fingerprint(RSA_PUB_PEM))
        secrets_ = decrypt_payload(body["encrypted_payload"])
        self.assertEqual(set(secrets_), {"negotiation_key", "zone_key"})
        self.assertRegex(secrets_["negotiation_key"], r"^[A-Za-z0-9_]{32}$")

        # the admin sees everything in plaintext
        _, listing, _ = self.fx.request("GET", "/list", auth=("rods", "rods"))
        rec = listing["applications"][0]
        self.assertEqual(rec["negotiation_key"], secrets_["negotiation_key"])
        self.assertEqual(rec["zone_key"], secrets_["zone_key"])
        self.assertEqual(rec["name"], "Ada Lovelace")
        self.assertEqual(rec["email"], "ada@example.org")
        self.assertEqual(rec["public_key"], RSA_PUB_PEM)
        self.assertEqual(rec["public_key_fingerprint"], body["public_key_fingerprint"])

        # the raw response never carried the secrets
        self.assertNotIn(secrets_["negotiation_key"], json.dumps(body))
        self.assertNotIn(secrets_["zone_key"], json.dumps(body))

    def test_apply_with_openssh_key_and_supplied_zone_key(self):
        status, body, _ = self.fx.apply(public_key=RSA_PUB_SSH, zone_key="MY_KEY")
        self.assertEqual(status, 201)
        self.assertEqual(decrypt_payload(body["encrypted_payload"])["zone_key"], "MY_KEY")
        self.assertNotIn("MY_KEY", json.dumps(body))

    def test_apply_without_key_is_plaintext_and_records_nulls(self):
        status, body, _ = self.fx.apply()
        self.assertEqual(status, 201)
        self.assertIn("zone_key", body)
        self.assertNotIn("encrypted_payload", body)
        _, listing, _ = self.fx.request("GET", "/list", auth=("rods", "rods"))
        rec = listing["applications"][0]
        self.assertEqual((rec["name"], rec["email"], rec["public_key"], rec["public_key_fingerprint"]),
                         (None, None, None, None))

    def test_bad_keys_are_400(self):
        for key, fragment in ((RSA_SMALL_PUB_PEM, "2048"), (EC_PUB_PEM, "RSA"), ("garbage", "PEM")):
            status, body, _ = self.fx.apply(public_key=key)
            self.assertEqual(status, 400, key[:20])
            self.assertIn(fragment, body["error"])
        self.assertEqual(self.fx.apply(email="nope")[0], 400)
        self.assertEqual(self.fx.apply(name="")[0], 400)
        _, listing, _ = self.fx.request("GET", "/list", auth=("rods", "rods"))
        self.assertEqual(listing["applications"], [])

    def test_status_after_approval_is_encrypted(self):
        _, a, _ = self.fx.apply(name="Ada", email="ada@example.org", public_key=RSA_PUB_PEM,
                                hosts=["localhost"], port=1248)
        aid = a["application_id"]
        _, pending, _ = self.fx.request("GET", "/status?application_id=" + aid)
        self.assertNotIn("encrypted_payload", pending)
        self.assertNotIn("public_key_fingerprint", pending)

        status, judged, _ = self.fx.judge(aid, "approve")
        self.assertEqual(status, 200)
        # judge response stays plaintext and carries the identity fields
        self.assertEqual(judged["applicant_federation_entry"]["zone_key"], HOME_ZONE_KEY)
        self.assertEqual(judged["application"]["name"], "Ada")
        self.assertEqual(judged["application"]["email"], "ada@example.org")
        self.assertNotIn("encrypted_payload", judged)

        status, body, _ = self.fx.request("GET", "/status?application_id=" + aid)
        self.assertEqual(status, 200)
        self.assertEqual(body["status"], "approved")
        self.assertNotIn("applicant_federation_entry", body)
        self.assertNotIn("applicant_mkzone_command", body)
        self.assertNotIn("name", body)
        self.assertNotIn("email", body)
        self.assertNotIn(HOME_ZONE_KEY, json.dumps(body))
        self.assertEqual(body["public_key_fingerprint"], a["public_key_fingerprint"])
        payload = decrypt_payload(body["encrypted_payload"])
        self.assertEqual(set(payload), {"applicant_federation_entry", "applicant_mkzone_command"})
        self.assertEqual(payload["applicant_federation_entry"], judged["applicant_federation_entry"])
        self.assertEqual(payload["applicant_mkzone_command"], judged["applicant_mkzone_command"])

        # a second poll is encrypted afresh but decrypts to the same thing
        _, again, _ = self.fx.request("GET", "/status?application_id=" + aid)
        self.assertNotEqual(again["encrypted_payload"], body["encrypted_payload"])
        self.assertEqual(decrypt_payload(again["encrypted_payload"]), payload)

    def test_rejected_keyed_application_has_no_payload(self):
        _, a, _ = self.fx.apply(public_key=RSA_PUB_PEM)
        self.fx.judge(a["application_id"], "reject")
        _, body, _ = self.fx.request("GET", "/status?application_id=" + a["application_id"])
        self.assertEqual(body["status"], "rejected")
        self.assertNotIn("encrypted_payload", body)


class RequirePublicKeyTests(ServerTestCase):
    config_overrides = {"require_public_key": True}

    def test_key_required(self):
        status, body, _ = self.fx.apply()
        self.assertEqual(status, 400)
        self.assertIn("requires a public_key", body["error"])
        _, listing, _ = self.fx.request("GET", "/list", auth=("rods", "rods"))
        self.assertEqual(listing["applications"], [])
        status, body, _ = self.fx.apply(public_key=RSA_PUB_PEM)
        self.assertEqual(status, 201)
        self.assertIn("encrypted_payload", body)


class QueueFullTests(ServerTestCase):
    config_overrides = {"max_pending_applications": 2}

    def test_queue_full_is_503(self):
        self.assertEqual(self.fx.apply("a")[0], 201)
        self.assertEqual(self.fx.apply("b")[0], 201)
        status, body, _ = self.fx.apply("c")
        self.assertEqual(status, 503)
        self.assertIn("full", body["error"])


class StatusTests(ServerTestCase):
    def test_status_pending_hides_keys(self):
        _, applied, _ = self.fx.apply()
        status, body, _ = self.fx.request("GET", "/status?application_id=" + applied["application_id"])
        self.assertEqual(status, 200)
        self.assertEqual(body["status"], "pending")
        self.assertNotIn("negotiation_key", body)
        self.assertNotIn("zone_key", body)
        self.assertNotIn("applicant_federation_entry", body)
        self.assertIsNone(body["judged_at"])

    def test_status_errors(self):
        self.assertEqual(self.fx.request("GET", "/status?application_id=nope")[0], 404)
        self.assertEqual(self.fx.request("GET", "/status")[0], 400)
        self.assertEqual(self.fx.request("GET", "/status?application_id=")[0], 400)
        self.assertEqual(self.fx.request("GET", "/status?application_id=a&application_id=b")[0], 400)


# ---------------------------------------------------------------------------
# HTTP: authentication and list
# ---------------------------------------------------------------------------

class AuthTests(ServerTestCase):
    def test_missing_credentials(self):
        status, body, headers = self.fx.request("GET", "/list")
        self.assertEqual(status, 401)
        self.assertEqual(headers["WWW-Authenticate"], 'Basic realm="fzrs"')
        self.assertIn("authentication required", body["error"])

    def test_malformed_headers(self):
        for header in ("Bearer abc", "Basic", "Basic !!!notbase64", "Basic " + base64.b64encode(b"nocolon").decode(),
                       "Basic " + base64.b64encode(b":pw").decode(),
                       "Basic " + base64.b64encode(b"rods#tempZone:rods").decode(),
                       "Basic " + base64.b64encode(b"bad user:rods").decode(),
                       "Basic " + base64.b64encode(b"\xff\xfe:x").decode()):
            status, _, headers = self.fx.request("GET", "/list", headers={"Authorization": header})
            self.assertEqual(status, 401, header)
            self.assertIn("WWW-Authenticate", headers)

    def test_bad_password_and_unknown_user(self):
        self.assertEqual(self.fx.request("GET", "/list", auth=("rods", "wrong"))[0], 401)
        self.assertEqual(self.fx.request("GET", "/list", auth=("nobody", "x"))[0], 401)

    def test_rodsuser_is_403(self):
        status, body, _ = self.fx.request("GET", "/list", auth=("alice", "alicepass"))
        self.assertEqual(status, 403)
        self.assertIn("not a rodsadmin", body["error"])

    def test_unreachable_irods_is_502(self):
        self.assertEqual(self.fx.request("GET", "/list", auth=("unreachable", "x"))[0], 502)

    def test_sessions_are_cleaned_up(self):
        self.fx.request("GET", "/list", auth=("rods", "rods"))
        self.fx.apply()
        _, listing, _ = self.fx.request("GET", "/list", auth=("rods", "rods"))
        self.fx.judge(listing["applications"][0]["application_id"], "reject")
        self.assertEqual(len(self.fx.irods.sessions), 3)
        self.assertTrue(all(s.cleaned_up for s in self.fx.irods.sessions))

    def test_judge_requires_auth_after_reading_body(self):
        status, _, headers = self.fx.request("POST", "/judge", {"application_id": "x", "decision": "approve"})
        self.assertEqual(status, 401)
        self.assertIn("WWW-Authenticate", headers)


class ListTests(ServerTestCase):
    def test_list_default_filters_and_order(self):
        self.fx.apply("a")
        time.sleep(1.1)  # distinct submitted_at seconds
        _, b, _ = self.fx.apply("b")
        self.fx.judge(b["application_id"], "reject", note="no")
        status, body, _ = self.fx.request("GET", "/list", auth=("rods", "rods"))
        self.assertEqual(status, 200)
        self.assertEqual([r["zone_name"] for r in body["applications"]], ["a"])
        self.assertIn("negotiation_key", body["applications"][0])
        _, body, _ = self.fx.request("GET", "/list?status=rejected", auth=("rods", "rods"))
        self.assertEqual([r["zone_name"] for r in body["applications"]], ["b"])
        self.assertEqual(body["applications"][0]["decision_note"], "no")
        self.assertEqual(body["applications"][0]["judged_by"], "rods")
        _, body, _ = self.fx.request("GET", "/list?status=all", auth=("rods", "rods"))
        self.assertEqual([r["zone_name"] for r in body["applications"]], ["a", "b"])
        for empty in ("approved", "expired"):
            _, body, _ = self.fx.request("GET", "/list?status=" + empty, auth=("rods", "rods"))
            self.assertEqual(body["applications"], [])

    def test_bad_filter_is_400(self):
        status, body, _ = self.fx.request("GET", "/list?status=bogus", auth=("rods", "rods"))
        self.assertEqual(status, 400)
        self.assertIn("status must be one of", body["error"])
        self.assertEqual(self.fx.request("GET", "/list?status=all&status=pending", auth=("rods", "rods"))[0], 400)


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
            status, resp, _ = self.fx.request("POST", "/judge", body, auth=("rods", "rods"))
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
        _, s, _ = self.fx.request("GET", "/status?application_id=" + a["application_id"])
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
        backups = [f for f in os.listdir(self.fx.tmp.name) if ".fzrs-backup-" in f]
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
        _, s, _ = self.fx.request("GET", "/status?application_id=" + a["application_id"])
        self.assertEqual(s["status"], "approved")
        self.assertEqual(s["applicant_federation_entry"], applicant)
        self.assertEqual(s["applicant_mkzone_command"], body["applicant_mkzone_command"])
        self.assertNotIn("home_negotiation_key_never_shown", json.dumps(s))

        # second judge is a conflict
        status, body, _ = self.fx.judge(a["application_id"], "reject")
        self.assertEqual(status, 409)
        self.assertIn("already been approved", body["error"])

        # persisted
        queue = json.load(open(self.fx.config["queue_file"]))
        self.assertEqual(queue["applications"][0]["status"], "approved")

    def test_unresolvable_host_is_refused(self):
        _, a, _ = self.fx.apply(hosts=["icat.nowhere.invalid"])
        status, body, _ = self.fx.judge(a["application_id"], "approve")
        self.assertEqual(status, 400)
        self.assertIn("does not resolve", body["error"])
        self.assertEqual(self.fx.irods.zones, {})
        self.assertEqual(self.fx.server_config()["federation"], [])
        _, s, _ = self.fx.request("GET", "/status?application_id=" + a["application_id"])
        self.assertEqual(s["status"], "pending")

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
        _, s, _ = self.fx.request("GET", "/status?application_id=" + a["application_id"])
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
        _, s, _ = self.fx.request("GET", "/status?application_id=" + a["application_id"])
        self.assertEqual(s["status"], "pending")

        # fix the pid file and retry: no second mkzone, no second backup, one HUP
        with open(self.fx.pid_file, "w") as f:
            f.write(str(self.fx.child.pid))
        self.fx.irods.mkzone_error = "must not be called again"
        status, body, _ = self.fx.judge(a["application_id"], "approve")
        self.assertEqual(status, 200)
        self.assertEqual(body["application"]["status"], "approved")
        self.assertEqual(len(self.fx.server_config()["federation"]), 1)
        self.assertEqual(len([f for f in os.listdir(self.fx.tmp.name) if ".fzrs-backup-" in f]), 1)
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
    config_overrides = {"application_expiration_in_seconds": 1}

    def test_expired_application(self):
        _, a, _ = self.fx.apply()
        time.sleep(2.1)
        _, body, _ = self.fx.request("GET", "/list?status=expired", auth=("rods", "rods"))
        self.assertEqual([r["application_id"] for r in body["applications"]], [a["application_id"]])
        _, body, _ = self.fx.request("GET", "/list", auth=("rods", "rods"))
        self.assertEqual(body["applications"], [])
        status, body, _ = self.fx.judge(a["application_id"], "approve")
        self.assertEqual(status, 410)
        self.assertIn("expired", body["error"])
        _, s, _ = self.fx.request("GET", "/status?application_id=" + a["application_id"])
        self.assertEqual(s["status"], "expired")
        # an expired zone name no longer blocks a new application
        self.assertEqual(self.fx.apply()[0], 201)


class ErrorPathTests(ServerTestCase):
    def test_unhandled_exception_is_500(self):
        def boom(query):
            raise RuntimeError("kaboom")
        self.fx.app.status = boom
        status, body, _ = self.fx.request("GET", "/status?application_id=x")
        self.assertEqual(status, 500)
        self.assertIn("kaboom", body["error"])

    def test_unread_body_is_discarded_on_error(self):
        # 404 before the body is read; the server must drain it and keep serving
        status, _, _ = self.fx.request("POST", "/nothing", {"some": "body"})
        self.assertEqual(status, 404)
        self.assertEqual(self.fx.request("GET", "/status?application_id=x")[0], 404)

    def test_real_irods_unreachable_is_502(self):
        probe = socket.socket()
        probe.bind(("127.0.0.1", 0))
        port = probe.getsockname()[1]
        probe.close()
        env = {"irods_host": "127.0.0.1", "irods_port": port, "irods_zone_name": HOME_ZONE,
               "irods_user_name": "rods", "irods_client_server_policy": "CS_NEG_REFUSE"}
        real = fzrs.IRODS("/nonexistent/env.json", env)
        with self.assertRaises(fzrs.HTTPError) as cm:
            real.admin_session("rods", "rods")
        self.assertEqual(cm.exception.status, 502)
        self.assertIn("cannot reach iRODS", cm.exception.message)


class ClientDisconnectTests(ServerTestCase):
    def test_client_closing_early_does_not_crash_server(self):
        s = socket.create_connection(("127.0.0.1", self.fx.port), timeout=10)
        s.sendall(b"POST /apply HTTP/1.1\r\nHost: x\r\nContent-Length: 70000\r\n\r\n")
        s.close()  # server answers 413 into a closed socket
        time.sleep(0.2)
        self.assertEqual(self.fx.request("GET", "/status?application_id=x")[0], 404)


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
            app = fzrs.App(fx.config)
            app.irods = FakeIRODS()
            with self.assertRaises(fzrs.ConfigError) as cm:
                app.start()
            self.assertIn("does not match", str(cm.exception))
        finally:
            fx.close()

    def test_real_irods_wrapper_reads_environment(self):
        fx = Fixture(start_server=False)
        try:
            app = fzrs.App(fx.config)
            self.assertIsInstance(app.irods, fzrs.IRODS)
            self.assertEqual((app.irods.host, app.irods.port, app.irods.zone), (HOME_HOST, 1247, HOME_ZONE))
        finally:
            fx.close()


class MainTests(unittest.TestCase):
    def run_main(self, argv):
        import io
        from contextlib import redirect_stdout
        out = io.StringIO()
        with redirect_stdout(out):
            rc = fzrs.main(argv)
        return rc, out.getvalue()

    def test_usage(self):
        for argv in (["fzrs"], ["fzrs", "--help"], ["fzrs", "a", "b"]):
            rc, out = self.run_main(argv)
            self.assertEqual(rc, 2)
            self.assertIn("usage:", out)

    def test_bad_config(self):
        with tempfile.TemporaryDirectory() as d:
            rc, out = self.run_main(["fzrs", os.path.join(d, "missing.json")])
            self.assertEqual(rc, 1)
            self.assertIn("configuration file not found", out)
            p = os.path.join(d, "fzrs.json")
            write_json(p, {"port": 1, "irods_environment_file": os.path.join(d, "none.json")})
            rc, out = self.run_main(["fzrs", p])
            self.assertEqual(rc, 1)
            self.assertIn("iRODS environment file not found", out)

    def test_startup_failure_outside_config_errors(self):
        fx = Fixture(start_server=False)
        try:
            os.unlink(fx.server_config_file)
            rc, out = self.run_main(["fzrs", fx.config_file])
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
            rc, out = self.run_main(["fzrs", fx.config_file])
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
            proc = subprocess.Popen([sys.executable, fzrs.__file__, fx.config_file],
                                    stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True)
            try:
                for _ in range(200):
                    try:
                        conn = http.client.HTTPConnection("127.0.0.1", port, timeout=1)
                        conn.request("GET", "/status?application_id=x")
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

LIVE_ENV = os.environ.get("FZRS_LIVE_IRODS_ENV_FILE")
LIVE_PASSWORD = os.environ.get("FZRS_LIVE_ADMIN_PASSWORD")
LIVE_RODSUSER = os.environ.get("FZRS_LIVE_RODSUSER")
LIVE_RODSUSER_PASSWORD = os.environ.get("FZRS_LIVE_RODSUSER_PASSWORD")


@unittest.skipUnless(LIVE_ENV and LIVE_PASSWORD, "set FZRS_LIVE_IRODS_ENV_FILE and FZRS_LIVE_ADMIN_PASSWORD")
class LiveIRODSTests(unittest.TestCase):
    def setUp(self):
        self.env = fzrs.load_irods_environment(LIVE_ENV)
        self.irods = fzrs.IRODS(LIVE_ENV, self.env)
        self.admin = self.env["irods_user_name"]

    def test_admin_session_success(self):
        session = self.irods.admin_session(self.admin, LIVE_PASSWORD)
        try:
            self.assertEqual(session.users.get(self.admin).type, "rodsadmin")
            self.assertFalse(self.irods.zone_exists(session, "fzrs_no_such_zone_" + fzrs.generate_key()[:8]))
            self.assertTrue(self.irods.zone_exists(session, self.irods.zone))
        finally:
            session.cleanup()

    def test_admin_session_bad_password(self):
        with self.assertRaises(fzrs.HTTPError) as cm:
            self.irods.admin_session(self.admin, "definitely-wrong-" + fzrs.generate_key())
        self.assertEqual(cm.exception.status, 401)

    @unittest.skipUnless(LIVE_RODSUSER and LIVE_RODSUSER_PASSWORD,
                         "set FZRS_LIVE_RODSUSER and FZRS_LIVE_RODSUSER_PASSWORD")
    def test_rodsuser_is_403(self):
        with self.assertRaises(fzrs.HTTPError) as cm:
            self.irods.admin_session(LIVE_RODSUSER, LIVE_RODSUSER_PASSWORD)
        self.assertEqual(cm.exception.status, 403)

    def test_service_session(self):
        session = self.irods.service_session()
        try:
            self.assertEqual(session.zone, self.irods.zone)
        finally:
            session.cleanup()

    def test_mkzone_and_rmzone(self):
        session = self.irods.admin_session(self.admin, LIVE_PASSWORD)
        name = "fzrsTestZone" + fzrs.generate_key()[:6].replace("_", "x")
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
