#!/usr/bin/env python3
"""Join this machine to an iRODS zone with a bootstrap key.

Usage
-----

    sudo python3 bootstrap_irods.py [--dry-run] <control-plane> <bootstrap-key>

<control-plane> is where the zone's control plane answers, as scheme, host, and
port; the scheme defaults to https and the port to 1248:

    https://cp.example.org:1248
    cp.example.org

<bootstrap-key> is the key the zone administrator created there and handed to
you, a lower-case uuid4 such as 0b2f6c1e-7d3a-4f8b-9e21-5c4d3b2a1f00. The
script POSTs it in a JSON body to <control-plane>/bootstrap/redeem; the key
never appears in a URL.

The key is the only credential you need. The administrator bound it to a
catalog service role when creating it, so the control plane tells this script
whether the new server is a catalog consumer or an additional catalog
provider, and sends the database credentials only in the provider case. The
control plane also creates a rodsadmin account for this server, named after
its host name, and sends that account's password in the same answer. The
server runs as that account from then on; the zone's own administrator
password is never needed here and never seen by this script. The key is single
use and expires, typically an hour after creation.

What the script does, in order
------------------------------

1. Checks that it can do the job before spending the key: Python 3.9 or newer,
   the iRODS server package installed (setup_irods.py and the server_config
   template present), the machine's fully qualified domain name determinable,
   and, unless --dry-run is given, that it is running as root because
   setup_irods.py must.
2. POSTs the key and this machine's fully qualified domain name to the redeem
   endpoint. On success the answer holds the service account (name and
   password) and the zone facts: zone name, port, authentication scheme, zone
   key, negotiation key, catalog provider hosts, the zone-wide hashing,
   policy, and encryption settings, and for a provider the database
   credentials. A refused key (unknown, already redeemed, revoked, or expired)
   or an account name already taken in the zone ends the run here and nothing
   is written.
3. Builds the json_configuration_file that setup_irods.py accepts with
   --json_configuration_file. Everything the answer does not supply is derived
   the same way setup_irods.py's interactive prompts derive their defaults:

   - host: this machine's fully qualified domain name, the same name that was
     sent to the control plane.
   - service account: operating system user irods, group irods; iRODS user
     the account the control plane created, which the facts also carry as
     zone_user.
   - server port range: 20000 to 20199.
   - default resource: demoResc for a provider, <shortname>Resource for a
     consumer, with its vault at /var/lib/irods/Vault.
   - every other server_config key: the installed server_config template.
   - the service account's irods_environment.json: assembled from the facts
     and the values above, exactly as setup_irods.py assembles it.
   - a provider appends its own fully qualified domain name to
     catalog_provider_hosts; the database settings are used exactly as the
     control plane sent them, including the host.

4. Writes the file as bootstrap_irods.<zone>.json in the current directory
   with mode 0600, because it holds the zone key, the negotiation key, this
   server's account password, and for a provider the database password.
5. Runs setup_irods.py --json_configuration_file with that file. When setup
   succeeds the file is deleted, since everything in it now lives in
   /etc/irods and the service account's home, and the script exits 0. When
   setup fails the file is kept so you can fix the cause and rerun setup
   without asking for another key, the script says so with a reminder of
   what the file holds, and it exits with setup's exit code:

       sudo python3 /var/lib/irods/scripts/setup_irods.py --json_configuration_file bootstrap_irods.<zone>.json

   With --dry-run the command is printed instead of run, the file is kept,
   and the script exits 0.

Exit codes
----------

    0  success (or, with --dry-run, the command was printed)
    2  usage error: bad arguments, a malformed control plane address, or a
       key that is not a lower-case uuid4
    3  preflight failed; nothing was sent to the control plane
    4  the control plane refused the key, or an account for this host name
       already exists in the zone
    5  the control plane could not be reached or answered an unexpected status
    6  the control plane's answer was not usable; the raw answer was saved
    other  setup_irods.py's own exit code

Not done here
-------------

- Existing servers in the zone are not told about a new provider's hostname;
  their catalog_provider_hosts lists are the zone administrator's to update.
- The account the control plane created is not removed when setup fails or
  when this server is later retired; `iadmin rmuser` does that.
- No TLS options. The control plane is reached with the system certificate
  store; a plain http:// address is accepted for testing on a trusted network,
  with a warning.
- No choice of default resource name, vault path, port range, or operating
  system service account. If one of these turns out to matter, it becomes an
  option.
- The redeem answer is not retrievable a second time; if this script fails
  after redeeming, the facts are in the written file or, when the answer could
  not be used, in bootstrap_irods.redeem.json.

Only the Python standard library is used, because a fresh server has nothing
else installed.
"""

import json
import os
import re
import socket
import subprocess
import sys
import urllib.error
import urllib.request
from urllib.parse import urlsplit, urlunsplit

SETUP_IRODS = "/var/lib/irods/scripts/setup_irods.py"
SERVER_CONFIG_TEMPLATE = "/var/lib/irods/packaging/server_config.json.template"
VAULT_DIRECTORY = "/var/lib/irods/Vault"
SERVICE_ACCOUNT_USER = "irods"
SERVICE_ACCOUNT_GROUP = "irods"
SERVER_PORT_RANGE_START = 20000
SERVER_PORT_RANGE_END = 20199
PROVIDER_DEFAULT_RESOURCE = "demoResc"
PASSWORD_MAX_LENGTH = 42  # irods.password_obfuscation.maximum_password_length
HTTP_TIMEOUT_SECONDS = 30
DEFAULT_SCHEME = "https"
DEFAULT_PORT = 1248
REDEEM_PATH = "/bootstrap/redeem"
RAW_RESPONSE_FILE = "bootstrap_irods.redeem.json"

UUID_RE = re.compile(r"^[0-9a-f]{8}-[0-9a-f]{4}-4[0-9a-f]{3}-[89ab][0-9a-f]{3}-[0-9a-f]{12}$")
USERNAME_RE = re.compile(r"^[A-Za-z0-9_.@-]{1,63}$")

# Zone facts the control plane must send; each is copied into server_config.
FACT_KEYS = (
    "catalog_service_role", "schema_name", "schema_version", "zone_name", "zone_port", "zone_user",
    "zone_auth_scheme", "zone_key", "negotiation_key", "catalog_provider_hosts", "client_server_policy",
    "default_hash_scheme", "match_hash_policy", "encryption",
)
ROLES = ("consumer", "provider")

EXIT_USAGE = 2
EXIT_PREFLIGHT = 3
EXIT_REFUSED = 4
EXIT_UNREACHABLE = 5
EXIT_UNUSABLE = 6


class BootstrapError(Exception):
    def __init__(self, code, message):
        super().__init__(message)
        self.code = code
        self.message = message


# ---------------------------------------------------------------------------
# Steps
# ---------------------------------------------------------------------------

def parse_control_plane(text):
    """Turn the handed-over control plane address into the URL to POST the key to.

    Accepts scheme://host:port, host:port, or host. Refuses anything with a
    path, query, fragment, or userinfo so the key cannot be smuggled into the
    address by mistake.
    """
    parts = urlsplit(text if "://" in text else "{}://{}".format(DEFAULT_SCHEME, text))
    if parts.scheme not in ("https", "http"):
        raise BootstrapError(EXIT_USAGE, "the control plane address must be https://host:port (or http:// for testing)")
    if not parts.hostname:
        raise BootstrapError(EXIT_USAGE, "the control plane address needs a host name")
    if parts.path not in ("", "/") or parts.query or parts.fragment or parts.username or parts.password:
        raise BootstrapError(EXIT_USAGE, "the control plane address is just scheme, host, and port; "
                                         "the key is the second argument, not part of the address")
    try:
        port = parts.port or DEFAULT_PORT
    except ValueError as e:
        raise BootstrapError(EXIT_USAGE, "the control plane address has an invalid port: {}".format(e))
    host = parts.hostname
    if ":" in host:  # IPv6 literal
        host = "[{}]".format(host)
    return urlunsplit((parts.scheme, "{}:{}".format(host, port), REDEEM_PATH, "", ""))


def parse_bootstrap_key(text):
    if not UUID_RE.match(text):
        raise BootstrapError(EXIT_USAGE, "the bootstrap key must be a lower-case uuid4 such as "
                                         "0b2f6c1e-7d3a-4f8b-9e21-5c4d3b2a1f00")
    return text


def preflight(run_setup):
    """Refuse to spend the key when setup could not possibly follow. Returns the FQDN."""
    if sys.version_info < (3, 9):
        raise BootstrapError(EXIT_PREFLIGHT, "Python 3.9 or newer is required")
    for path, what in ((SETUP_IRODS, "setup_irods.py"), (SERVER_CONFIG_TEMPLATE, "the server_config template")):
        if not os.path.isfile(path):
            raise BootstrapError(EXIT_PREFLIGHT, "{} not found at {}; is the iRODS server package installed?".format(what, path))
    if run_setup and os.geteuid() != 0:
        raise BootstrapError(EXIT_PREFLIGHT, "setup_irods.py must run as root; rerun with sudo or use --dry-run")
    fqdn = socket.getfqdn()
    if not fqdn or fqdn == "localhost":
        raise BootstrapError(EXIT_PREFLIGHT, "cannot determine this machine's fully qualified domain name")
    return fqdn


def redeem(post_url, key, host):
    """POST the key and this host's name; return the control plane's JSON answer.

    Raises on refusal, an account name collision, or unreachability.
    """
    request = urllib.request.Request(
        post_url, data=json.dumps({"bootstrap_key": key, "host": host}).encode("utf-8"),
        headers={"Content-Type": "application/json"}, method="POST")
    try:
        with urllib.request.urlopen(request, timeout=HTTP_TIMEOUT_SECONDS) as response:
            raw = response.read()
            status = response.status
    except urllib.error.HTTPError as e:
        raw = e.read()
        try:
            message = json.loads(raw.decode("utf-8")).get("error", raw.decode("utf-8", "replace"))
        except (ValueError, AttributeError):
            message = raw.decode("utf-8", "replace")[:200]
        if e.code == 403:
            raise BootstrapError(EXIT_REFUSED, "the control plane refused the key: {}".format(message))
        if e.code == 409:
            raise BootstrapError(EXIT_REFUSED, "the control plane refused this host: {}; ask the zone "
                                               "administrator to remove that account or rename this host".format(message))
        raise BootstrapError(EXIT_UNREACHABLE, "the control plane answered {}: {}".format(e.code, message))
    except (urllib.error.URLError, OSError) as e:
        raise BootstrapError(EXIT_UNREACHABLE, "cannot reach the control plane at {}: {}".format(
            post_url, getattr(e, "reason", e)))
    try:
        answer = json.loads(raw.decode("utf-8"))
    except ValueError:
        save_raw_response(raw)
        raise BootstrapError(EXIT_UNUSABLE, "the control plane's answer ({}) is not JSON; saved to {}".format(
            status, RAW_RESPONSE_FILE))
    return answer


def save_raw_response(raw):
    write_private(RAW_RESPONSE_FILE, raw if isinstance(raw, bytes) else raw.encode("utf-8"))


def parse_answer(answer):
    """Check the answer has the facts and the account setup needs; return (facts, account)."""
    facts = answer.get("zone_facts") if isinstance(answer, dict) else None
    account = answer.get("service_account") if isinstance(answer, dict) else None
    problems = []
    if not isinstance(facts, dict):
        problems.append("no zone_facts object")
    else:
        for k in FACT_KEYS:
            if k not in facts:
                problems.append("missing {}".format(k))
        if facts.get("catalog_service_role") not in ROLES:
            problems.append("catalog_service_role is not consumer or provider")
        if facts.get("catalog_service_role") == "provider" and not isinstance(facts.get("database"), dict):
            problems.append("a provider needs database")
        hosts = facts.get("catalog_provider_hosts")
        if not isinstance(hosts, list) or not hosts:
            problems.append("catalog_provider_hosts is not a non-empty list")
    if not isinstance(account, dict):
        problems.append("no service_account object")
    else:
        name, password = account.get("name"), account.get("password")
        if not isinstance(name, str) or not USERNAME_RE.match(name):
            problems.append("service_account.name is not an iRODS user name")
        elif isinstance(facts, dict) and facts.get("zone_user") != name:
            problems.append("zone_user is not the service account")
        if not isinstance(password, str) or not 1 <= len(password) <= PASSWORD_MAX_LENGTH:
            problems.append("service_account.password is not 1 to {} characters".format(PASSWORD_MAX_LENGTH))
    if problems:
        save_raw_response(json.dumps(answer, indent=2, sort_keys=True))
        raise BootstrapError(EXIT_UNUSABLE, "the control plane's answer is unusable ({}); saved to {}".format(
            "; ".join(problems), RAW_RESPONSE_FILE))
    return facts, account


def default_resource_name(role, fqdn):
    """setup_irods.py's own default: demoResc for a provider, <shortname>Resource for a consumer."""
    return PROVIDER_DEFAULT_RESOURCE if role == "provider" else fqdn.split(".")[0] + "Resource"


def build_configuration(facts, template, fqdn, account):
    """Assemble the json_configuration_file document for setup_irods.py."""
    role = facts["catalog_service_role"]
    resource = default_resource_name(role, fqdn)

    server_config = json.loads(json.dumps(template))  # deep copy; never touch the caller's template
    for k in FACT_KEYS:
        server_config[k] = facts[k]
    server_config["host"] = fqdn
    server_config["server_port_range_start"] = SERVER_PORT_RANGE_START
    server_config["server_port_range_end"] = SERVER_PORT_RANGE_END
    server_config["default_resource_name"] = resource
    server_config.setdefault("federation", [])
    if role == "provider":
        if fqdn not in server_config["catalog_provider_hosts"]:
            server_config["catalog_provider_hosts"] = list(server_config["catalog_provider_hosts"]) + [fqdn]
        server_config.setdefault("plugin_configuration", {})["database"] = facts["database"]

    advanced = server_config["advanced_settings"]
    service_account_environment = {
        "schema_name": "service_account_environment",
        "schema_version": "v5",
        "irods_host": fqdn,
        "irods_port": facts["zone_port"],
        "irods_default_resource": resource,
        "irods_home": "/{}/home/{}".format(facts["zone_name"], account["name"]),
        "irods_cwd": "/{}/home/{}".format(facts["zone_name"], account["name"]),
        "irods_user_name": account["name"],
        "irods_zone_name": facts["zone_name"],
        "irods_client_server_policy": facts["client_server_policy"],
        "irods_encryption_algorithm": facts["encryption"]["algorithm"],
        "irods_encryption_key_size": facts["encryption"]["key_size"],
        "irods_encryption_num_hash_rounds": facts["encryption"]["num_hash_rounds"],
        "irods_encryption_salt_size": facts["encryption"]["salt_size"],
        "irods_default_hash_scheme": facts["default_hash_scheme"],
        "irods_match_hash_policy": facts["match_hash_policy"],
        "irods_maximum_size_for_single_buffer_in_megabytes": advanced["maximum_size_for_single_buffer_in_megabytes"],
        "irods_default_number_of_transfer_threads": advanced["default_number_of_transfer_threads"],
        "irods_transfer_buffer_size_for_parallel_transfer_in_megabytes":
            advanced["transfer_buffer_size_for_parallel_transfer_in_megabytes"],
        "irods_connection_pool_refresh_time_in_seconds": server_config["connection_pool_refresh_time_in_seconds"],
    }

    return {
        "host_system_information": {
            "service_account_user_name": SERVICE_ACCOUNT_USER,
            "service_account_group_name": SERVICE_ACCOUNT_GROUP,
        },
        "server_config": server_config,
        "default_resource_name": resource,
        "default_resource_directory": VAULT_DIRECTORY,
        "service_account_environment": service_account_environment,
        "admin_password": account["password"],
    }


def configuration_file_name(facts):
    return "bootstrap_irods.{}.json".format(facts["zone_name"])


def write_private(path, data):
    """Create or replace path with mode 0600 before any secret lands in it."""
    fd = os.open(path, os.O_WRONLY | os.O_CREAT | os.O_TRUNC, 0o600)
    with os.fdopen(fd, "wb") as f:
        f.write(data)
    os.chmod(path, 0o600)


def setup_command(configuration_path):
    return [sys.executable, SETUP_IRODS, "--json_configuration_file", configuration_path]


def what_the_file_holds(role):
    secrets = ["the zone key", "the negotiation key", "this server's account password"]
    if role == "provider":
        secrets.append("the database password")
    return ", ".join(secrets[:-1]) + ", and " + secrets[-1]


# ---------------------------------------------------------------------------
# Entry point
# ---------------------------------------------------------------------------

def usage():
    return "usage: {} [--dry-run] <control-plane> <bootstrap-key>\n       see the docstring at the top of this file".format(
        os.path.basename(sys.argv[0] or "bootstrap_irods.py"))


def main(argv, stdout=None, stderr=None):
    stdout = stdout or sys.stdout
    stderr = stderr or sys.stderr
    args = list(argv[1:])
    dry_run = False
    if args and args[0] == "--dry-run":
        dry_run = True
        args = args[1:]
    if len(args) != 2 or any(a.startswith("-") for a in args):
        print(usage(), file=stderr)
        return EXIT_USAGE

    try:
        post_url = parse_control_plane(args[0])
        key = parse_bootstrap_key(args[1])
        if post_url.startswith("http://"):
            print("warning: the control plane address is plain http; the zone's secrets will travel unencrypted",
                  file=stderr)
        fqdn = preflight(run_setup=not dry_run)
        with open(SERVER_CONFIG_TEMPLATE) as f:
            template = json.load(f)

        answer = redeem(post_url, key, fqdn)
        facts, account = parse_answer(answer)
        print("redeemed a {} bootstrap key for zone {}; this server's account is {}".format(
            facts["catalog_service_role"], facts["zone_name"], account["name"]), file=stdout)

        configuration = build_configuration(facts, template, fqdn, account)
        path = os.path.abspath(configuration_file_name(facts))
        write_private(path, (json.dumps(configuration, indent=4, sort_keys=True) + "\n").encode("utf-8"))
        print("wrote {} (mode 0600): host {}, default resource {}".format(
            path, fqdn, configuration["default_resource_name"]), file=stdout)
    except BootstrapError as e:
        print("bootstrap_irods: {}".format(e.message), file=stderr)
        return e.code

    command = setup_command(path)
    if dry_run:
        print("dry run; would now run:\n    sudo " + " ".join(command), file=stdout)
        return 0
    print("running: " + " ".join(command), file=stdout)
    stdout.flush()
    rc = subprocess.call(command)
    if rc == 0:
        os.remove(path)
        print("setup succeeded; removed {}".format(path), file=stdout)
    else:
        print("setup_irods.py exited {}; kept {}, which holds {}.\n"
              "Fix the cause and rerun setup from it without asking for another key:\n"
              "    sudo {}".format(rc, path, what_the_file_holds(facts["catalog_service_role"]), " ".join(command)),
              file=stderr)
    return rc


if __name__ == "__main__":
    sys.exit(main(sys.argv))
