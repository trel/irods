# iRODS Federation Zone Registrar Service (FZRS) — Specification

Status: draft, agreed 2026-10-31; applicant identity and public-key
encryption added 2026-10-31.

## 1. Purpose

FZRS is a small HTTP service that fronts exactly one iRODS zone (the *home
zone*). A remote zone that wants to federate with the home zone submits an
*application*. A rodsadmin of the home zone reviews the queue and approves or
rejects each application. On approval FZRS performs the home-zone side of the
federation itself and hands the applicant everything it needs to configure its
own side.

The first iteration is a standalone process run as the iRODS service account on
a home-zone catalog provider. A later iteration may have the iRODS server keep
FZRS alive as a child process. Nothing in this design assumes either mode.

## 2. Deliverables

Both files live at the top level of the repository.

- `fzrs.py` — the whole service in one Python file.
- `fzrs-spec.md` — this document.

## 3. Runtime requirements

- Python 3.9 or newer.
- Third-party dependencies are `python-irodsclient` and `cryptography`
  (packaged as `python3-cryptography`). `cryptography` is used only to parse
  applicant public keys and to build the JWE envelope of section 5.5.
  Everything else is the standard library (`http.server`, `ssl` is not used,
  `json`, `secrets`, `uuid`, `threading`, `base64`, `re`, `logging`,
  `signal`, `os`, `hashlib`).
- Runs as the iRODS service account on a catalog provider of the home zone,
  because approval edits `/etc/irods/server_config.json` and signals the iRODS
  server.
- Speaks plain HTTP only. It should be deployed behind a TLS-terminating
  reverse proxy; admin credentials travel in HTTP Basic headers. Applicants
  who supply a public key receive their secrets encrypted end to end
  (section 5.5), so the applicant side does not depend on TLS.

## 4. Invocation and configuration

```
python3 fzrs.py /path/to/fzrs.json
```

The single argument is the configuration file. The process exits non-zero with
a message on stdout if the file is missing, is not valid JSON, fails
validation, or if the iRODS client environment cannot be loaded.

### 4.1 Configuration keys

| Key | Type | Default | Meaning |
|---|---|---|---|
| `port` | integer 1–65535 | required | TCP port FZRS listens on |
| `bind_address` | string | `"0.0.0.0"` | Interface to bind |
| `application_expiration_in_seconds` | integer > 0 | `604800` (7 days) | Pending applications older than this expire |
| `queue_file` | string | `fzrs-queue.json` in the config file's directory | Persistent queue |
| `max_pending_applications` | integer > 0 | `100` | `apply` returns 503 when this many are pending |
| `max_request_body_bytes` | integer > 0 | `65536` | Larger bodies are rejected with 413 |
| `irods_environment_file` | string | `$IRODS_ENVIRONMENT_FILE`, else `~/.irods/irods_environment.json` | Service account client environment |
| `server_config_file` | string | `/etc/irods/server_config.json` | Home zone server configuration to edit on approval |
| `server_pid_file` | string | `/var/run/irods/irods-server.pid` | Used to send SIGHUP after editing the server config |
| `require_public_key` | boolean | `false` | When true, `apply` without a `public_key` is rejected, so secrets are never delivered in plaintext |

Unknown keys are rejected so typos are caught. Relative paths resolve against
the configuration file's directory.

### 4.2 iRODS connection information

FZRS does not take iRODS host, port, zone, or user in its own config. It reads
the service account's `irods_environment_file` (and the matching `.irodsA`
through python-irodsclient's normal mechanism) for:

- `irods_host`, `irods_port`, `irods_zone_name` — used both for the service
  account session and as the target when verifying admin credentials.
- `irods_user_name` — the service account.
- Any SSL/encryption settings present are honored as-is; a non-SSL environment
  is acceptable.

The *home zone facts* needed to build the applicant's federation stanza come
from `server_config_file`: top-level `zone_name`, `zone_key`, `zone_port`, and
`catalog_provider_hosts`. The home zone's top-level `negotiation_key` is never
read (see 7.2).

## 5. Data model

### 5.1 Application record

Stored as a JSON object. All timestamps are ISO 8601 in UTC with a `Z` suffix.

| Field | Set by | Notes |
|---|---|---|
| `application_id` | FZRS | `uuid4` string. Also serves as the bearer for `status`. |
| `status` | FZRS | one of `pending`, `approved`, `rejected`, `expired` |
| `submitted_at` | FZRS | |
| `expires_at` | FZRS | `submitted_at + application_expiration_in_seconds` |
| `remote_address` | FZRS | peer IP of the `apply` request (proxy's `X-Forwarded-For` first value if present, else socket peer) |
| `zone_name` | applicant | |
| `zone_port` | applicant | |
| `catalog_provider_hosts` | applicant | |
| `zone_key` | applicant or FZRS | applicant-supplied value when present, else generated |
| `zone_key_generated` | FZRS | boolean |
| `negotiation_key` | FZRS | shared key for this federation pair |
| `name` | applicant | optional, informational only, null when absent |
| `email` | applicant | optional, informational only, never used to send mail, null when absent |
| `public_key` | applicant | optional, stored verbatim as supplied, null when absent |
| `public_key_fingerprint` | FZRS | `SHA256:<base64>` of the DER SubjectPublicKeyInfo, null when no key |
| `judged_at` | FZRS | null until judged |
| `judged_by` | FZRS | iRODS username of the admin, null until judged |
| `decision_note` | admin | free text, optional, max 1024 chars |

Nothing is ever redacted or purged in this iteration. Purging can be added
later.

### 5.2 Queue file

```json
{
  "schema_version": 1,
  "applications": [ { ...record... }, ... ]
}
```

Written atomically: serialize to a temp file in the same directory, `fsync`,
`os.replace`. Loaded once at startup; if it does not exist it is created empty.
A corrupt queue file is a startup error, not something to silently overwrite.

### 5.3 Validation rules

Derived from `schemas/configuration/v5/server_config.json.in`.

| Field | Rule |
|---|---|
| `zone_name` | `^[A-Za-z0-9_.]+$`, 1–63 chars, must not equal the home zone's name |
| `zone_port` | integer 0–65535 |
| `catalog_provider_hosts` | non-empty array of unique strings; each an RFC 1123 hostname (labels of `[A-Za-z0-9-]`, 1–63 chars, no leading/trailing hyphen, total ≤ 253) or an IPv4/IPv6 literal |
| `zone_key` | `^[A-Za-z0-9_]+$`, 1–49 chars |
| `negotiation_key` | `^[A-Za-z0-9_]{32}$` |
| `decision_note` | string ≤ 1024 chars |
| `name` | string, 1–256 chars, no other rule |
| `email` | string, 3–254 chars, exactly one `@` with at least one character on each side, no other rule, never verified |
| `public_key` | string ≤ 16384 chars; parses as PEM SubjectPublicKeyInfo (`-----BEGIN PUBLIC KEY-----`) or OpenSSH single-line (`ssh-rsa AAAA...`, trailing comment ignored); must be RSA with modulus ≥ 2048 bits |

### 5.4 Key generation

Both generated keys use `secrets.choice` over the alphabet
`A-Za-z0-9_` (64 symbols) and are 32 characters long (192 bits).

### 5.5 Encrypted payloads

When an application carries a `public_key`, every applicant-facing secret is
delivered as one JWE compact serialization (RFC 7516) under the field name
`encrypted_payload`, and the corresponding plaintext fields are omitted.

- Protected header: `{"alg":"RSA-OAEP-256","enc":"A256GCM"}`.
- Content key: 32 random bytes from `secrets.token_bytes`, wrapped with
  RSA-OAEP using SHA-256 for both the hash and MGF1.
- Content: the UTF-8 JSON document described by the endpoint, encrypted with
  AES-256-GCM, a fresh 96-bit IV, a 128-bit tag, and the base64url-encoded
  protected header as additional authenticated data, exactly as JWE requires.
- Serialization: five base64url segments without padding, joined by `.`.

A new content key and IV are generated on every response; the same plaintext
yields a different ciphertext each time.

The applicant decrypts with any JOSE library, with `step crypto jwe decrypt`,
or with this `cryptography`-only snippet:

```python
import base64, json, sys
from cryptography.hazmat.primitives import hashes, serialization
from cryptography.hazmat.primitives.asymmetric import padding
from cryptography.hazmat.primitives.ciphers.aead import AESGCM

def b64d(s):
    return base64.urlsafe_b64decode(s + "=" * (-len(s) % 4))

private_key = serialization.load_pem_private_key(open(sys.argv[1], "rb").read(), None)
header, enc_key, iv, ciphertext, tag = sys.argv[2].split(".")
cek = private_key.decrypt(b64d(enc_key), padding.OAEP(
    padding.MGF1(hashes.SHA256()), hashes.SHA256(), None))
plaintext = AESGCM(cek).decrypt(b64d(iv), b64d(ciphertext) + b64d(tag), header.encode())
print(json.dumps(json.loads(plaintext), indent=2))
```

Fingerprints are `SHA256:` followed by the SHA-256 of the key's DER-encoded
SubjectPublicKeyInfo in the standard base64 alphabet (`+` and `/`, not
base64url) with `=` padding removed. A key supplied in PEM or OpenSSH form
yields the same fingerprint, and an applicant can confirm the key the
registrar holds with:

```
openssl pkey -pubin -in key.pem -outform DER | openssl dgst -sha256 -binary | base64 | tr -d '='
```

## 6. Authentication

`list` and `judge` require HTTP Basic authentication.

1. Missing or malformed `Authorization` header → `401` with
   `WWW-Authenticate: Basic realm="fzrs"`.
2. Decode `username:password`. The username is a plain home-zone username;
   `user#zone` forms are rejected with `401`.
3. Open a python-irodsclient session with the home zone's host, port, and zone
   from the service account environment, the supplied username and password,
   and `authentication_scheme` taken from the environment (default `native`).
   Perform a real connection (`session.server_version` or equivalent) so bad
   passwords fail here → `401`.
4. Query `session.users.get(username).type`. Anything other than `rodsadmin`
   → `403`.
5. The session is kept for the duration of the request and used for any iRODS
   operations the request performs, so catalog changes are attributed to the
   judging admin, not the service account.

No caching of credentials or sessions between requests.

## 7. Endpoints

All request and response bodies are JSON with `Content-Type: application/json`.
Errors use `{"error": "<human readable message>"}`. Unknown paths → `404`.
Wrong method → `405`. Bodies larger than `max_request_body_bytes` → `413`.
Malformed JSON or validation failures → `400`.

Before handling any request, FZRS runs the lazy expiration sweep (section 8).

### 7.1 `POST /apply` — unauthenticated

Request:

```json
{
  "zone_name": "otherZone",
  "zone_port": 1247,
  "catalog_provider_hosts": ["icat.other.example.org"],
  "zone_key": "OPTIONAL_EXISTING_ZONE_KEY",
  "name": "Optional Human Name",
  "email": "optional@example.org",
  "public_key": "-----BEGIN PUBLIC KEY-----\n...\n-----END PUBLIC KEY-----\n"
}
```

`name` and `email` are for the reviewing admin only. FZRS never uses them
for anything and never sends mail. `public_key` is the applicant's RSA
public key; when present, every secret FZRS returns to the applicant is
encrypted to it (section 5.5).

Behavior:

1. Validate per 5.3. Extra keys → `400`. An unparseable, non-RSA, or
   too-short `public_key` → `400`.
2. If `require_public_key` is true and no `public_key` was supplied → `400`
   with an error saying this registrar requires one.
3. If `max_pending_applications` pending records exist → `503`.
4. If a `pending` record with the same `zone_name` exists → `409`. Approved,
   rejected, and expired records do not block a new application.
5. Generate `negotiation_key`; generate `zone_key` if not supplied.
6. Persist, then respond `201`. Without a public key:

```json
{
  "application_id": "6f1c...",
  "status": "pending",
  "expires_at": "2026-11-07T15:04:05Z",
  "negotiation_key": "....32 chars....",
  "zone_key": "....",
  "status_url": "/status?application_id=6f1c..."
}
```

With a public key, `negotiation_key` and `zone_key` are replaced by one
field whose plaintext is `{"negotiation_key": "...", "zone_key": "..."}`:

```json
{
  "application_id": "6f1c...",
  "status": "pending",
  "expires_at": "2026-11-07T15:04:05Z",
  "public_key_fingerprint": "SHA256:...",
  "encrypted_payload": "eyJhbGciOi...",
  "status_url": "/status?application_id=6f1c..."
}
```

`apply` performs no iRODS operations and needs no iRODS connection.

### 7.2 `GET /status?application_id=<uuid>` — unauthenticated

The `application_id` is unguessable and acts as the bearer token.

- Unknown id → `404`.
- Otherwise `200` with `application_id`, `status`, `submitted_at`,
  `expires_at`, `judged_at`, `decision_note`.
- When `status` is `approved` the response also includes
  `applicant_federation_entry`: the object to append to the `federation`
  array of the **applicant's** `server_config.json`, plus the
  `iadmin mkzone` command the applicant must run:

```json
{
  "application_id": "6f1c...",
  "status": "approved",
  "applicant_federation_entry": {
    "catalog_provider_hosts": ["icat.home.example.org"],
    "negotiation_key": "<shared negotiation_key>",
    "zone_key": "<home zone's zone_key>",
    "zone_key_signing_hash_scheme": "sha256",
    "zone_name": "homeZone",
    "zone_port": 1247
  },
  "applicant_mkzone_command": "iadmin mkzone homeZone remote icat.home.example.org:1247"
}
```

The home zone's `zone_key`, hosts, name, and port are read from
`server_config_file` at request time. The home zone's own top-level
`negotiation_key` is never disclosed; the pair-specific key from the record is
used.

When the record carries a `public_key`, the two fields above are replaced by
`public_key_fingerprint` and one `encrypted_payload` whose plaintext is the
JSON document `{"applicant_federation_entry": {...}, "applicant_mkzone_command":
"..."}` with exactly the content shown above:

```json
{
  "application_id": "6f1c...",
  "status": "approved",
  "public_key_fingerprint": "SHA256:...",
  "encrypted_payload": "eyJhbGciOi..."
}
```

`name`, `email`, and the public key itself are never returned by `status`.

### 7.3 `GET /list?status=<filter>` — rodsadmin

- `status` is one of `pending` (default), `approved`, `rejected`, `expired`,
  `all`. Anything else → `400`.
- Response `200`:

```json
{ "applications": [ { ...full record, see 5.1... }, ... ] }
```

Ordered by `submitted_at` ascending. Records are returned in full, including
keys, `name`, `email`, `public_key`, and `public_key_fingerprint`, because the
caller is a rodsadmin. The fingerprint lets the admin confirm the key with the
applicant out of band before approving.

### 7.4 `POST /judge` — rodsadmin

Request:

```json
{
  "application_id": "6f1c...",
  "decision": "approve",
  "decision_note": "optional free text"
}
```

- `decision` must be `approve` or `reject` → else `400`.
- Unknown id → `404`.
- Status `approved` or `rejected` → `409`.
- Status `expired` → `410`.

**Reject:** set `status=rejected`, `judged_at`, `judged_by`, `decision_note`;
persist; respond `200` with the full record. No iRODS actions.

**Approve** runs these steps in order under the global lock. If any step
fails, earlier steps are rolled back where possible, the record stays
`pending`, and the response is `500` (or `400` for step 0, `409` for step 1)
with an `error` naming the step. The admin can retry after fixing the cause.

0. Verify every entry of the applicant's `catalog_provider_hosts` resolves
   (`socket.getaddrinfo`). If one does not → `400`, nothing changed. iRODS
   agents retry DNS for roughly 30 seconds per unresolvable remote zone host
   at startup (`getaddrinfo_with_retry` in `lib/core/src/rcMisc.cpp`), so
   federating with an unresolvable host would stall every connection to the
   home zone.
1. Verify no zone named `zone_name` exists in the home catalog
   (`session.zones.get`). If one exists → `409`, nothing changed.
2. Create the remote zone in the home catalog with the admin's session,
   equivalent to `iadmin mkzone <zone_name> remote <host>:<zone_port>` where
   `host` is `catalog_provider_hosts[0]`. python-irodsclient's
   `ZoneManager.create` does not accept a connection string, so FZRS sends
   `GeneralAdminRequest("add", "zone", zone_name, "remote", "host:port", "")`
   directly via `GENERAL_ADMIN_AN`.
3. Edit `server_config_file`:
   - Load JSON. If the `federation` array already has an entry whose
     `zone_name` matches → treat as failure of this step (and roll back
     step 2 with `rmzone`).
   - Append:
     ```json
     {
       "catalog_provider_hosts": <applicant hosts>,
       "negotiation_key": <record negotiation_key>,
       "zone_key": <record zone_key>,
       "zone_key_signing_hash_scheme": "sha256",
       "zone_name": <applicant zone_name>,
       "zone_port": <applicant zone_port>
     }
     ```
   - Save a backup copy as `<server_config_file>.fzrs-backup-<UTC timestamp>`.
   - Write atomically (temp file + `os.replace`, preserving the original
     file's mode), `json.dump(indent=4, sort_keys=True)`.
   - On failure roll back step 2 with `rmzone`.
4. Send `SIGHUP` to the PID in `server_pid_file` so the iRODS server reloads
   its configuration. A missing or stale PID file is a step failure; the
   catalog and file changes are **not** rolled back in that case, because
   they are correct and only the reload is outstanding. The error message
   says so and the record stays `pending` so the admin can retry; on retry
   step 1 will report the zone already exists, so the retry path for this
   specific case is: FZRS detects that the catalog zone and the federation
   entry both already exist *with the record's keys*, skips steps 2 and 3,
   and only re-attempts step 4.
5. Set `status=approved`, `judged_at`, `judged_by`, `decision_note`. Persist.
6. Respond `200`:

```json
{
  "application": { ...full record... },
  "home_federation_entry": { ...what was appended in step 3... },
  "applicant_federation_entry": { ...same object `status` returns... },
  "applicant_mkzone_command": "iadmin mkzone homeZone remote icat.home.example.org:1247"
}
```

The judge response is always plaintext, whether or not the applicant supplied
a public key. The admin is already authenticated over the same channel and
can read the home zone's `zone_key` from server_config.json; the encrypted
form is only for the applicant-facing `apply` and `status` responses.

## 8. Expiration

No background thread. At the start of every request, every `pending` record
whose `expires_at` is in the past is set to `expired` and the queue is
persisted if anything changed. Expired records remain in the queue
indefinitely.

## 9. Concurrency

`http.server.ThreadingHTTPServer` serves requests concurrently. One
process-wide `threading.Lock` guards every read-modify-write of the in-memory
queue and the queue file, and the whole of the approve sequence (7.4) holds it
so two admins cannot approve conflicting applications at once. iRODS
connections are per request and are not shared across threads.

## 10. Logging and signals

- One line per request to stdout: timestamp, remote address, method, path,
  status code, and the admin username for authenticated calls. Passwords and
  keys are never logged.
- Approve/reject decisions and every step of the approve sequence are logged
  at INFO; failures at ERROR with the exception text.
- `SIGTERM` and `SIGINT` stop the server cleanly after in-flight requests
  finish. The queue is always consistent on disk because every change is
  persisted before its response is sent.

## 11. Security notes

- Approval refuses applicant hosts that do not resolve, because an
  unresolvable remote zone host slows every home zone agent start by about
  30 seconds. This also means a host must be resolvable from the FZRS host
  at judge time.
- `apply` is the only unauthenticated write; it is bounded by
  `max_pending_applications`, `max_request_body_bytes`, strict field
  validation, and the 409 duplicate rule.
- `status` reveals the home zone's `zone_key` only for approved applications
  and only to a holder of the matching 122-bit random `application_id`.
- When the applicant supplies an RSA public key, the home zone's `zone_key`,
  the shared `negotiation_key`, and the applicant's `zone_key` are only ever
  sent to the applicant inside a JWE encrypted to that key, in both the
  `apply` and the `status` responses, so a plaintext HTTP path does not
  expose them. Without a key they travel in plaintext, and
  `require_public_key` lets a deployment forbid that.
- `name` and `email` are stored and shown to admins as supplied. They are
  not verified, not used to contact anyone, and not returned to applicants.
- The home zone's own top-level `negotiation_key` is never read or returned.
- Admin passwords are verified against iRODS on every call and never stored.
- TLS is the reverse proxy's job. Binding to `0.0.0.0` on plain HTTP without
  a proxy exposes admin passwords and any plaintext applicant secrets; the
  README section at the top of `fzrs.py` says so. Public-key encryption
  protects applicant secrets, not admin credentials.

## 12. Out of scope for this iteration

- TLS in-process.
- Purging or redacting old records.
- Per-IP rate limiting.
- Notifying the applicant (email, webhook); they poll `status`. The `email`
  field is informational only.
- Non-RSA applicant keys (Ed25519, X25519, ECDSA) and signed or
  authenticated applications.
- Removing a federation (`unfederate`).
- Supervision by the iRODS server as a child process.

## 13. Smoke test plan

1. Run FZRS as the `irods` service account against the local zone with a
   config of `{"port": 9100}`.
2. `POST /apply` for a fake zone; expect `201` and both keys. Repeat with a
   `name`, `email`, and an RSA public key generated with
   `openssl genrsa -out key.pem 2048 && openssl rsa -in key.pem -pubout`;
   expect `201` with `encrypted_payload` and no plaintext keys, and decrypt
   it with the snippet in 5.5. Repeat with a 1024-bit key and with garbage;
   expect `400`.
3. `GET /list` without auth → `401`; with a rodsuser → `403`; with `rods` →
   `200` showing the pending record.
4. `GET /status` → `pending`.
5. `POST /judge` approve as `rods`; expect `200`, `iadmin lz` shows the new
   zone, `/etc/irods/server_config.json` has the new federation entry, and the
   server log shows the reload. Use a resolvable applicant hostname
   (`localhost` or an IP literal); the reload briefly refuses connections
   while the agent factory restarts.
6. `GET /status` → `approved` with `applicant_federation_entry`; for the
   keyed application, `encrypted_payload` instead, decrypting to the same
   entry. `GET /list` shows name, email, and the fingerprint matching
   `openssl` output.
7. `POST /judge` again on the same id → `409`.
8. `POST /apply` with a 1-second expiration config, wait, `GET /list?status=expired`
   shows it, `POST /judge` → `410`.
9. Clean up: `iadmin rmzone`, restore `server_config.json` from the backup,
   `kill -HUP` the server.

## 14. Automated tests

`test_fzrs.py` (top level) is a `unittest` suite, also runnable under pytest.
It starts the real server in-process on an ephemeral port against a fake
iRODS, with temp copies of the config, environment, and server_config.json,
and a child process that records the SIGHUP it receives. Four live tests run
against a real zone only when `FZRS_LIVE_IRODS_ENV_FILE` and
`FZRS_LIVE_ADMIN_PASSWORD` are set (plus `FZRS_LIVE_RODSUSER` and
`FZRS_LIVE_RODSUSER_PASSWORD` for the 403 case); they must run as the iRODS
service account so python-irodsclient can find `.irodsA`.

```
PYTHONPATH=/src/python-irodsclient python3 -m pytest -q test_fzrs.py
PYTHONPATH=/src/python-irodsclient python3 -m coverage run --branch --include=fzrs.py -m pytest -q test_fzrs.py
python3 -m coverage report -m
```

Only `main()`'s serve loop (exercised in a subprocess), the broken-pipe
branch, and the temp-file cleanup branch of `atomic_write_json` are outside
line coverage.
