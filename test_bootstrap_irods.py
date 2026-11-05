#!/usr/bin/env python3
"""Tests for scripts/bootstrap_irods.py.

The script is loaded from its path, since scripts/ is not a package. A fake
control plane answers the redeem endpoint on an ephemeral port, a temp copy of
the server_config template stands in for the installed one, and a fake
setup_irods.py records the arguments it was called with. Nothing here touches
a real zone or runs the real setup.

    python3 -m pytest -q test_bootstrap_irods.py
"""

import http.server
import importlib.util
import io
import json
import os
import shutil
import subprocess
import sys
import tempfile
import threading
import unittest
from unittest import mock

HERE = os.path.dirname(os.path.abspath(__file__))
SCRIPT = os.path.join(HERE, "scripts", "bootstrap_irods.py")
spec = importlib.util.spec_from_file_location("bootstrap_irods", SCRIPT)
bootstrap_irods = importlib.util.module_from_spec(spec)
spec.loader.exec_module(bootstrap_irods)

KEY = "0b2f6c1e-7d3a-4f8b-9e21-5c4d3b2a1f00"
FQDN = "new.example.org"
ACCOUNT = {"name": "adm." + FQDN, "password": "Qm7xL2pW9sD4fG6hJ8kN0bV3cX5zA1eR"}
TEMPLATE = {
    "schema_name": "server_config", "schema_version": "v5",
    "advanced_settings": {
        "maximum_size_for_single_buffer_in_megabytes": 32,
        "default_number_of_transfer_threads": 4,
        "transfer_buffer_size_for_parallel_transfer_in_megabytes": 4,
        "other_setting": 1,
    },
    "connection_pool_refresh_time_in_seconds": 300,
    "federation": [],
    "plugin_configuration": {"authentication": {}, "network": {}, "resource": {}, "rule_engines": []},
    "log_level": {"agent": "info"},
}
DATABASE = {"technology": "postgres", "host": "localhost", "port": 5432, "name": "ICAT",
            "odbc_driver": "PostgreSQL ANSI", "username": "irods", "password": "db-secret"}


def facts(role):
    f = {
        "catalog_service_role": role, "schema_name": "server_config", "schema_version": "v5",
        "zone_name": "homeZone", "zone_port": 1247, "zone_user": ACCOUNT["name"], "zone_auth_scheme": "native",
        "zone_key": "HOME_ZONE_KEY", "negotiation_key": "n" * 32,
        "catalog_provider_hosts": ["icat.home.example.org"], "client_server_policy": "CS_NEG_REFUSE",
        "default_hash_scheme": "SHA256", "match_hash_policy": "compatible",
        "encryption": {"algorithm": "AES-256-CBC", "key_size": 32, "num_hash_rounds": 16, "salt_size": 8},
    }
    if role == "provider":
        f["database"] = dict(DATABASE)
    return f


def answer(role, account=None):
    return {"service_account": dict(ACCOUNT if account is None else account), "zone_facts": facts(role)}


class FakeControlPlane:
    """Answers POST /bootstrap/redeem with whatever `reply` holds: (status, body bytes)."""

    def __init__(self):
        self.reply = (200, json.dumps(answer("consumer")).encode())
        self.requests = []
        outer = self

        class Handler(http.server.BaseHTTPRequestHandler):
            def log_message(self, *a):
                pass

            def do_POST(self):
                length = int(self.headers.get("Content-Length", 0))
                outer.requests.append((self.path, self.headers.get("Content-Type"), self.rfile.read(length)))
                status, body = outer.reply
                self.send_response(status)
                self.send_header("Content-Type", "application/json")
                self.send_header("Content-Length", str(len(body)))
                self.end_headers()
                self.wfile.write(body)

        self.server = http.server.HTTPServer(("127.0.0.1", 0), Handler)
        self.port = self.server.server_address[1]
        threading.Thread(target=self.server.serve_forever, daemon=True).start()

    def address(self):
        return "http://127.0.0.1:{}".format(self.port)

    def close(self):
        self.server.shutdown()
        self.server.server_close()


class ScriptTestCase(unittest.TestCase):
    """Temp cwd, temp template and setup script, fake control plane, patched FQDN and root check."""

    def setUp(self):
        self.tmp = tempfile.mkdtemp(prefix="bootstrap-irods-test-")
        self.addCleanup(shutil.rmtree, self.tmp)
        self.old_cwd = os.getcwd()
        os.chdir(self.tmp)
        self.addCleanup(os.chdir, self.old_cwd)

        self.template = os.path.join(self.tmp, "server_config.json.template")
        with open(self.template, "w") as f:
            json.dump(TEMPLATE, f)
        self.setup_log = os.path.join(self.tmp, "setup.log")
        self.setup = os.path.join(self.tmp, "setup_irods.py")
        with open(self.setup, "w") as f:
            f.write("import json, sys\n"
                    "json.dump(sys.argv[1:], open({!r}, 'w'))\n"
                    "sys.exit(int(__import__('os').environ.get('FAKE_SETUP_EXIT', '0')))\n".format(self.setup_log))
        self.configuration_path = os.path.join(self.tmp, "bootstrap_irods.homeZone.json")

        self.cp = FakeControlPlane()
        self.addCleanup(self.cp.close)

        patches = [
            mock.patch.object(bootstrap_irods, "SETUP_IRODS", self.setup),
            mock.patch.object(bootstrap_irods, "SERVER_CONFIG_TEMPLATE", self.template),
            mock.patch.object(bootstrap_irods.socket, "getfqdn", return_value=FQDN),
            mock.patch.object(bootstrap_irods.os, "geteuid", return_value=0),
        ]
        for p in patches:
            p.start()
            self.addCleanup(p.stop)

    def run_main(self, *args, dry_run=True):
        """Run main with the fake control plane and KEY unless other arguments are given."""
        args = list(args) if args else [self.cp.address(), KEY]
        argv = ["bootstrap_irods.py"] + (["--dry-run"] if dry_run else []) + args
        out, err = io.StringIO(), io.StringIO()
        rc = bootstrap_irods.main(argv, stdout=out, stderr=err)
        return rc, out.getvalue(), err.getvalue()

    def redeem(self):
        return bootstrap_irods.redeem(bootstrap_irods.parse_control_plane(self.cp.address()), KEY, FQDN)

    def written(self):
        self.assertEqual(oct(os.stat(self.configuration_path).st_mode & 0o777), "0o600")
        with open(self.configuration_path) as f:
            return json.load(f)


class ParseArgumentsTests(unittest.TestCase):
    def test_control_plane_forms(self):
        for text, expected in (("https://cp.example.org:1248", "https://cp.example.org:1248/bootstrap/redeem"),
                               ("https://cp.example.org", "https://cp.example.org:1248/bootstrap/redeem"),
                               ("cp.example.org", "https://cp.example.org:1248/bootstrap/redeem"),
                               ("cp.example.org:9200", "https://cp.example.org:9200/bootstrap/redeem"),
                               ("https://cp.example.org:1248/", "https://cp.example.org:1248/bootstrap/redeem"),
                               ("http://127.0.0.1:9200", "http://127.0.0.1:9200/bootstrap/redeem"),
                               ("http://[::1]:9200", "http://[::1]:9200/bootstrap/redeem")):
            self.assertEqual(bootstrap_irods.parse_control_plane(text), expected, text)

    def test_control_plane_rejects(self):
        for text, fragment in (("ftp://cp.example.org", "https://host:port"),
                               ("https://", "host name"),
                               ("https://cp.example.org/bootstrap/redeem", "just scheme, host, and port"),
                               ("https://cp.example.org/bootstrap/redeem?bootstrap_key=" + KEY, "just scheme, host, and port"),
                               ("https://cp.example.org#x", "just scheme, host, and port"),
                               ("https://rods:rods@cp.example.org", "just scheme, host, and port"),
                               ("cp.example.org:notaport", "invalid port")):
            with self.assertRaises(bootstrap_irods.BootstrapError, msg=text) as cm:
                bootstrap_irods.parse_control_plane(text)
            self.assertEqual(cm.exception.code, 2)
            self.assertIn(fragment, cm.exception.message)

    def test_bootstrap_key(self):
        self.assertEqual(bootstrap_irods.parse_bootstrap_key(KEY), KEY)
        for bad in (KEY.upper(), "not-a-uuid", "", KEY + "x"):
            with self.assertRaises(bootstrap_irods.BootstrapError, msg=bad) as cm:
                bootstrap_irods.parse_bootstrap_key(bad)
            self.assertEqual(cm.exception.code, 2)
            self.assertIn("uuid4", cm.exception.message)


class BuildConfigurationTests(unittest.TestCase):
    def test_consumer(self):
        cfg = bootstrap_irods.build_configuration(facts("consumer"), TEMPLATE, FQDN, ACCOUNT)
        self.assertEqual(set(cfg), {"host_system_information", "server_config", "default_resource_name",
                                    "default_resource_directory", "service_account_environment", "admin_password"})
        self.assertEqual(cfg["host_system_information"],
                         {"service_account_user_name": "irods", "service_account_group_name": "irods"})
        self.assertEqual(cfg["admin_password"], ACCOUNT["password"])
        self.assertEqual(cfg["default_resource_name"], "newResource")
        self.assertEqual(cfg["default_resource_directory"], "/var/lib/irods/Vault")
        sc = cfg["server_config"]
        for k in bootstrap_irods.FACT_KEYS:
            self.assertEqual(sc[k], facts("consumer")[k], k)
        self.assertEqual(sc["zone_user"], ACCOUNT["name"])
        self.assertEqual((sc["host"], sc["server_port_range_start"], sc["server_port_range_end"]),
                         (FQDN, 20000, 20199))
        self.assertEqual(sc["default_resource_name"], "newResource")
        self.assertEqual(sc["federation"], [])
        self.assertEqual(sc["catalog_provider_hosts"], ["icat.home.example.org"])
        self.assertNotIn("database", sc["plugin_configuration"])
        self.assertEqual(sc["log_level"], TEMPLATE["log_level"])  # template carried through
        env = cfg["service_account_environment"]
        self.assertEqual(env, {
            "schema_name": "service_account_environment", "schema_version": "v5",
            "irods_host": FQDN, "irods_port": 1247, "irods_default_resource": "newResource",
            "irods_home": "/homeZone/home/adm.new.example.org", "irods_cwd": "/homeZone/home/adm.new.example.org",
            "irods_user_name": "adm.new.example.org", "irods_zone_name": "homeZone",
            "irods_client_server_policy": "CS_NEG_REFUSE",
            "irods_encryption_algorithm": "AES-256-CBC", "irods_encryption_key_size": 32,
            "irods_encryption_num_hash_rounds": 16, "irods_encryption_salt_size": 8,
            "irods_default_hash_scheme": "SHA256", "irods_match_hash_policy": "compatible",
            "irods_maximum_size_for_single_buffer_in_megabytes": 32,
            "irods_default_number_of_transfer_threads": 4,
            "irods_transfer_buffer_size_for_parallel_transfer_in_megabytes": 4,
            "irods_connection_pool_refresh_time_in_seconds": 300,
        })
        self.assertEqual(TEMPLATE["plugin_configuration"].get("database"), None)  # template untouched
        self.assertNotIn("rods", json.dumps(cfg).replace("irods", ""))  # the zone admin appears nowhere

    def test_provider(self):
        cfg = bootstrap_irods.build_configuration(facts("provider"), TEMPLATE, FQDN, ACCOUNT)
        sc = cfg["server_config"]
        self.assertEqual(cfg["default_resource_name"], "demoResc")
        self.assertEqual(sc["catalog_provider_hosts"], ["icat.home.example.org", FQDN])
        self.assertEqual(sc["plugin_configuration"]["database"], DATABASE)  # verbatim, host untouched
        self.assertEqual(sc["plugin_configuration"]["database"]["host"], "localhost")
        self.assertNotIn("database", TEMPLATE["plugin_configuration"])

    def test_provider_already_listed_is_not_duplicated(self):
        f = facts("provider")
        f["catalog_provider_hosts"] = [FQDN, "icat.home.example.org"]
        cfg = bootstrap_irods.build_configuration(f, TEMPLATE, FQDN, ACCOUNT)
        self.assertEqual(cfg["server_config"]["catalog_provider_hosts"], [FQDN, "icat.home.example.org"])

    def test_what_the_file_holds(self):
        self.assertEqual(bootstrap_irods.what_the_file_holds("consumer"),
                         "the zone key, the negotiation key, and this server's account password")
        self.assertEqual(bootstrap_irods.what_the_file_holds("provider"),
                         "the zone key, the negotiation key, this server's account password, and the database password")


class ParseAnswerTests(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.mkdtemp()
        self.addCleanup(shutil.rmtree, self.tmp)
        self.old = os.getcwd()
        os.chdir(self.tmp)
        self.addCleanup(os.chdir, self.old)

    def test_problems_are_named_and_answer_saved(self):
        def without(role, key):
            a = answer(role)
            del a["zone_facts"][key]
            return a

        def with_facts(role, **changes):
            a = answer(role)
            a["zone_facts"].update(changes)
            return a

        cases = [
            ({}, "no zone_facts"),
            ([], "no zone_facts"),
            ({}, "no service_account"),
            (without("consumer", "zone_key"), "missing zone_key"),
            (with_facts("consumer", catalog_service_role="primary"), "not consumer or provider"),
            (without("provider", "database"), "needs database"),
            (with_facts("consumer", catalog_provider_hosts=[]), "non-empty list"),
            ({"zone_facts": facts("consumer")}, "no service_account object"),
            ({"zone_facts": facts("consumer"), "service_account": "adm.x"}, "no service_account object"),
            (answer("consumer", {"name": 5, "password": "x"}), "service_account.name is not an iRODS user name"),
            (answer("consumer", {"name": "bad name", "password": "x"}), "service_account.name is not an iRODS user name"),
            (answer("consumer", {"name": "adm.other", "password": "x"}), "zone_user is not the service account"),
            (answer("consumer", {"name": ACCOUNT["name"], "password": ""}), "password is not 1 to 42"),
            (answer("consumer", {"name": ACCOUNT["name"], "password": "x" * 43}), "password is not 1 to 42"),
            (answer("consumer", {"name": ACCOUNT["name"], "password": 7}), "password is not 1 to 42"),
            (answer("consumer", {"name": ACCOUNT["name"]}), "password is not 1 to 42"),
        ]
        for bad, fragment in cases:
            with self.assertRaises(bootstrap_irods.BootstrapError, msg=fragment) as cm:
                bootstrap_irods.parse_answer(bad)
            self.assertEqual(cm.exception.code, 6)
            self.assertIn(fragment, cm.exception.message)
            saved = os.path.join(self.tmp, "bootstrap_irods.redeem.json")
            self.assertEqual(oct(os.stat(saved).st_mode & 0o777), "0o600")
            self.assertEqual(json.load(open(saved)), bad)
        self.assertEqual(bootstrap_irods.parse_answer(answer("provider")), (facts("provider"), ACCOUNT))
        self.assertFalse(os.path.exists(os.path.join(self.tmp, "x")))


class PreflightTests(ScriptTestCase):
    def test_missing_setup_script(self):
        os.unlink(self.setup)
        with self.assertRaises(bootstrap_irods.BootstrapError) as cm:
            bootstrap_irods.preflight(run_setup=False)
        self.assertEqual(cm.exception.code, 3)
        self.assertIn("setup_irods.py not found", cm.exception.message)

    def test_missing_template(self):
        os.unlink(self.template)
        with self.assertRaises(bootstrap_irods.BootstrapError) as cm:
            bootstrap_irods.preflight(run_setup=False)
        self.assertIn("server_config template", cm.exception.message)

    def test_root_required_only_when_running_setup(self):
        with mock.patch.object(bootstrap_irods.os, "geteuid", return_value=1000):
            self.assertEqual(bootstrap_irods.preflight(run_setup=False), FQDN)
            with self.assertRaises(bootstrap_irods.BootstrapError) as cm:
                bootstrap_irods.preflight(run_setup=True)
            self.assertIn("root", cm.exception.message)

    def test_fqdn_required(self):
        with mock.patch.object(bootstrap_irods.socket, "getfqdn", return_value="localhost"):
            with self.assertRaises(bootstrap_irods.BootstrapError) as cm:
                bootstrap_irods.preflight(run_setup=False)
            self.assertIn("fully qualified", cm.exception.message)

    def test_preflight_failure_sends_nothing(self):
        os.unlink(self.setup)
        rc, out, err = self.run_main()
        self.assertEqual(rc, 3)
        self.assertEqual(self.cp.requests, [])
        self.assertEqual(os.listdir(self.tmp), ["server_config.json.template"])


class RedeemTests(ScriptTestCase):
    def test_posts_key_and_host_in_the_body_only(self):
        got = self.redeem()
        self.assertEqual(got["zone_facts"]["zone_name"], "homeZone")
        self.assertEqual(got["service_account"], ACCOUNT)
        self.assertEqual(self.cp.requests, [("/bootstrap/redeem", "application/json",
                                             json.dumps({"bootstrap_key": KEY, "host": FQDN}).encode())])
        self.assertNotIn(KEY.encode(), self.cp.requests[0][0].encode())  # never in the path

    def test_refused_key_is_4(self):
        self.cp.reply = (403, b'{"error": "bootstrap key has expired"}')
        with self.assertRaises(bootstrap_irods.BootstrapError) as cm:
            self.redeem()
        self.assertEqual(cm.exception.code, 4)
        self.assertIn("refused the key: bootstrap key has expired", cm.exception.message)

    def test_taken_account_is_4(self):
        self.cp.reply = (409, b'{"error": "user adm.new.example.org already exists"}')
        with self.assertRaises(bootstrap_irods.BootstrapError) as cm:
            self.redeem()
        self.assertEqual(cm.exception.code, 4)
        self.assertIn("refused this host: user adm.new.example.org already exists", cm.exception.message)
        self.assertIn("remove that account", cm.exception.message)

    def test_other_statuses_are_5(self):
        for reply in ((500, b'{"error": "internal error: boom"}'), (502, b'{"error": "cannot reach iRODS"}'),
                      (404, b"<html>nope</html>")):
            self.cp.reply = reply
            with self.assertRaises(bootstrap_irods.BootstrapError, msg=reply) as cm:
                self.redeem()
            self.assertEqual(cm.exception.code, 5)
            self.assertIn(str(reply[0]), cm.exception.message)

    def test_unreachable_is_5(self):
        self.cp.close()
        with self.assertRaises(bootstrap_irods.BootstrapError) as cm:
            self.redeem()
        self.assertEqual(cm.exception.code, 5)
        self.assertIn("cannot reach", cm.exception.message)

    def test_non_json_success_is_6_and_saved(self):
        self.cp.reply = (200, b"not json")
        with self.assertRaises(bootstrap_irods.BootstrapError) as cm:
            self.redeem()
        self.assertEqual(cm.exception.code, 6)
        self.assertEqual(open(os.path.join(self.tmp, "bootstrap_irods.redeem.json"), "rb").read(), b"not json")


class MainTests(ScriptTestCase):
    def test_dry_run_writes_file_and_prints_command(self):
        rc, out, err = self.run_main()
        self.assertEqual(rc, 0, err)
        self.assertIn("redeemed a consumer bootstrap key for zone homeZone; this server's account is adm.new.example.org",
                      out)
        self.assertIn("wrote " + self.configuration_path, out)
        self.assertIn("dry run; would now run:", out)
        self.assertIn(self.setup + " --json_configuration_file " + self.configuration_path, out)
        self.assertIn("plain http", err)  # warned, because the fake is http
        self.assertFalse(os.path.exists(self.setup_log))  # setup was not run
        self.assertNotIn(ACCOUNT["password"], out + err)
        cfg = self.written()  # kept: a dry run is for looking at it
        self.assertEqual(cfg["admin_password"], ACCOUNT["password"])
        self.assertEqual(cfg["service_account_environment"]["irods_user_name"], ACCOUNT["name"])
        self.assertEqual(cfg["server_config"]["host"], FQDN)
        self.assertEqual(cfg["server_config"]["zone_key"], "HOME_ZONE_KEY")
        self.assertEqual(cfg, bootstrap_irods.build_configuration(facts("consumer"), TEMPLATE, FQDN, ACCOUNT))
        self.assertEqual(json.loads(self.cp.requests[0][2]), {"bootstrap_key": KEY, "host": FQDN})

    def test_successful_setup_removes_the_file(self):
        self.cp.reply = (200, json.dumps(answer("provider")).encode())
        rc, out, err = self.run_main(dry_run=False)
        self.assertEqual(rc, 0, err)
        self.assertIn("running: ", out)
        self.assertIn("setup succeeded; removed " + self.configuration_path, out)
        self.assertEqual(json.load(open(self.setup_log)), ["--json_configuration_file", self.configuration_path])
        self.assertFalse(os.path.exists(self.configuration_path))
        self.assertEqual(sorted(os.listdir(self.tmp)), ["server_config.json.template", "setup.log", "setup_irods.py"])
        self.assertNotIn(ACCOUNT["password"], out + err)

    def test_failed_setup_keeps_the_file_and_says_what_it_holds(self):
        self.cp.reply = (200, json.dumps(answer("provider")).encode())
        with mock.patch.dict(os.environ, {"FAKE_SETUP_EXIT": "3"}):
            rc, out, err = self.run_main(dry_run=False)
        self.assertEqual(rc, 3)
        self.assertIn("setup_irods.py exited 3; kept " + self.configuration_path, err)
        self.assertIn("the zone key, the negotiation key, this server's account password, and the database password",
                      err)
        self.assertIn("sudo " + sys.executable + " " + self.setup + " --json_configuration_file " + self.configuration_path,
                      err)
        self.assertEqual(self.written()["server_config"]["plugin_configuration"]["database"], DATABASE)
        self.assertNotIn(ACCOUNT["password"], out + err)

    def test_not_root_refuses_before_redeeming(self):
        with mock.patch.object(bootstrap_irods.os, "geteuid", return_value=1000):
            rc, out, err = self.run_main(dry_run=False)
        self.assertEqual(rc, 3)
        self.assertIn("sudo", err)
        self.assertEqual(self.cp.requests, [])

    def test_refused_key_writes_nothing(self):
        self.cp.reply = (403, b'{"error": "bootstrap key is not valid"}')
        rc, out, err = self.run_main()
        self.assertEqual(rc, 4)
        self.assertIn("bootstrap key is not valid", err)
        self.assertEqual(sorted(os.listdir(self.tmp)), ["server_config.json.template", "setup_irods.py"])

    def test_taken_account_writes_nothing(self):
        self.cp.reply = (409, b'{"error": "user adm.new.example.org already exists"}')
        rc, out, err = self.run_main()
        self.assertEqual(rc, 4)
        self.assertIn("user adm.new.example.org already exists", err)
        self.assertEqual(sorted(os.listdir(self.tmp)), ["server_config.json.template", "setup_irods.py"])

    def test_unusable_answer_is_6(self):
        self.cp.reply = (200, b'{"zone_facts": {"zone_name": "x"}}')
        rc, out, err = self.run_main()
        self.assertEqual(rc, 6)
        self.assertIn("missing zone_key", err)
        self.assertIn("no service_account object", err)
        self.assertTrue(os.path.exists(os.path.join(self.tmp, "bootstrap_irods.redeem.json")))
        self.assertFalse(os.path.exists(self.configuration_path))

    def test_usage(self):
        for argv in ([], ["--help"], ["--dry-run"], [self.cp.address()], [KEY],
                     [self.cp.address(), KEY, "extra"], ["--bogus", self.cp.address(), KEY],
                     [self.cp.address(), "--dry-run", KEY]):
            rc = bootstrap_irods.main(["x"] + argv, stdout=io.StringIO(), stderr=io.StringIO())
            self.assertEqual(rc, 2, argv)
        rc, out, err = self.run_main(self.cp.address() + "/bootstrap/redeem?bootstrap_key=" + KEY, KEY)
        self.assertEqual(rc, 2)
        self.assertIn("just scheme, host, and port", err)
        rc, out, err = self.run_main(self.cp.address(), KEY.upper())
        self.assertEqual(rc, 2)
        self.assertIn("uuid4", err)
        self.assertEqual(self.cp.requests, [])

    def test_script_runs_as_a_program(self):
        proc = subprocess.run([sys.executable, SCRIPT], capture_output=True, text=True)
        self.assertEqual(proc.returncode, 2)
        self.assertIn("usage:", proc.stderr)


if __name__ == "__main__":
    unittest.main()
