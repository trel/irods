# iRODS Control Plane Service — Specification

Status: draft, agreed 2026-11-05.

## 1. Purpose

The control plane is a small HTTP service that fronts exactly one iRODS zone
(the *home zone*) and runs as that zone's service account on a catalog
provider. It offers three features, each a group of endpoints:

- **federation** — a remote zone applies to federate with the home zone, a
  rodsadmin approves or rejects, and on approval the service performs the
  home-zone side of the federation and hands the applicant everything it
  needs for its own side.
- **keyserver** — a rodsadmin can read the lifecycle of the home zone's
  `zone_key` (the current key, its expiry, and the already-minted next key).
  The zone-key information is what iRODS servers will later use to rotate
  the zone key automatically; rotation itself is not performed by this
  iteration.
- **bootstrap** — a rodsadmin creates a single-use *bootstrap key* bound to
  a catalog service role and hands it to the operator of a new server out
  of band. The operator redeems the key, unauthenticated, within its
  lifetime and receives in the same response the *zone facts* needed to
  fill in a `json_configuration_file` for `setup_irods.py` and join the
  home zone as a catalog consumer or an additional catalog provider.
  Creating the key is the approval; no one judges a redemption. Nothing is
  done inside the home zone's iRODS instance for a redemption.

The service assumes it is reached only through a TLS-terminating reverse
proxy, so the channel to every client is already encrypted. It therefore
does no cryptography of its own: secrets are plain JSON fields in requests
and responses, and the only use of randomness is generating keys and ids.

The first iteration is a standalone process. A later iteration may have the
iRODS server keep it alive as a child process. Nothing in this design assumes
either mode.

## 2. Deliverables

All files live at the top level of the repository.

- `controlplane.py` — the whole service in one Python file.
- `controlplane-spec.md` — this document, the authoritative specification.
- `controlplane-openapi.yaml` — an OpenAPI 3.0.3 description of the HTTP
  surface (section 15). A companion to this document, not a replacement.
- `test_controlplane.py` — the automated tests (section 14).

## 3. Runtime requirements

- Python 3.9 or newer.
- The only third-party dependency is `python-irodsclient`. Everything else
  is the standard library.
- Runs as the iRODS service account on a catalog provider of the home zone,
  because federation approval edits `/etc/irods/server_config.json` and
  signals the iRODS server, and because the keyserver and bootstrap features
  read that file for the zone's keys and, for a provider bootstrap, its
  database credentials.
- Speaks plain HTTP only and must be deployed behind a TLS-terminating
  reverse proxy. Admin credentials travel in HTTP Basic headers and every
  applicant-facing secret travels as a plain JSON field; the proxy's TLS is
  the only thing protecting them in transit. Exposing the service without a
  proxy is a misconfiguration (section 11).

## 4. Invocation and configuration

```
python3 controlplane.py /path/to/controlplane.json
```

The single argument is the configuration file. The process exits non-zero
with a message on stdout if the file is missing, is not valid JSON, fails
validation, or if the iRODS client environment cannot be loaded.

The file is one JSON object with shared top-level keys and one nested object
per feature. Every key has a default, so `{}` is a valid configuration that
listens on port 1248. Unknown keys at any level are rejected so typos are caught.
Relative paths resolve against the configuration file's directory.

### 4.1 Shared keys

| Key | Type | Default | Meaning |
|---|---|---|---|
| `port` | integer 1–65535 | `1248` | TCP port the service listens on, one above the conventional iRODS zone port |
| `bind_address` | string | `"0.0.0.0"` | Interface to bind |
| `state_file` | string | `controlplane-state.json` in the config file's directory | Persistent state (section 5.1) |
| `max_request_body_bytes` | integer > 0 | `65536` | Larger bodies are rejected with 413 |
| `irods_environment_file` | string | `$IRODS_ENVIRONMENT_FILE`, else `~/.irods/irods_environment.json` | Service account client environment |
| `server_config_file` | string | `/etc/irods/server_config.json` | Home zone server configuration: edited by federation approval, read by keyserver and bootstrap |
| `server_pid_file` | string | `/var/run/irods/irods-server.pid` | Used to send SIGHUP after a federation approval edits the server config |
| `federation` | object | `{}` | Section 4.2 |
| `keyserver` | object | `{}` | Section 4.3 |

### 4.2 `federation` keys

| Key | Type | Default | Meaning |
|---|---|---|---|
| `application_expiration_in_seconds` | integer > 0 | `604800` (7 days) | Pending federation applications older than this expire |
| `max_pending_applications` | integer > 0 | `100` | `/federation/apply` returns 503 when this many are pending |

### 4.3 `keyserver` keys

| Key | Type | Default | Meaning |
|---|---|---|---|
| `lifetime_of_zone_keys_in_seconds` | integer > 0 | `7776000` (90 days) | How long a `zone_key` is meant to stay current before rotation |
| `lifetime_of_bootstrap_keys_in_seconds` | integer > 0 | `3600` (1 hour) | Default lifetime of a bootstrap key; `/bootstrap/create` may override it per key |

Every bootstrap key is single use.

### 4.4 Example

```json
{
  "federation": {"application_expiration_in_seconds": 1209600},
  "keyserver": {"lifetime_of_bootstrap_keys_in_seconds": 7200}
}
```

### 4.5 iRODS connection information

The service does not take iRODS host, port, zone, or user in its own config.
It reads the service account's `irods_environment_file` (and the matching
`.irodsA` through python-irodsclient's normal mechanism) for:

- `irods_host`, `irods_port`, `irods_zone_name` — used both for the service
  account session and as the target when verifying admin credentials.
- `irods_user_name` — the service account.
- Any SSL settings present are honored as-is; a non-SSL environment is
  acceptable.

The *home zone facts* come from `server_config_file` and are read at request
time, never cached: `zone_name`, `zone_port`, `zone_user`, `zone_key`,
`negotiation_key`, `zone_auth_scheme`, `catalog_provider_hosts`,
`client_server_policy`, `default_hash_scheme`, `match_hash_policy`,
`encryption`, `schema_name`, `schema_version`, the `federation` array, and
`plugin_configuration.database`. Which of these each feature uses is stated
in section 7.

## 5. Data model

### 5.1 State file

One JSON file, written atomically (serialize to a temp file in the same
directory, `fsync`, `os.replace`) with mode `0600`, because it holds keys.
Loaded once at startup; if it does not exist it is created. A corrupt state
file is a startup error, not something to silently overwrite.

```json
{
  "schema_version": 1,
  "federation": {"applications": [ ...5.2... ]},
  "bootstrap":  {"keys": [ ...5.3... ]},
  "keyserver":  { ...5.4... }
}
```

Nothing is ever redacted or purged in this iteration. Purging can be added
later.

### 5.2 Federation application record

All timestamps are ISO 8601 in UTC with a `Z` suffix and whole seconds.

| Field | Set by | Notes |
|---|---|---|
| `application_id` | service | `uuid4` string. Also serves as the bearer for `status`. |
| `status` | service | one of `pending`, `approved`, `rejected`, `expired` |
| `submitted_at` | service | |
| `expires_at` | service | `submitted_at + federation.application_expiration_in_seconds` |
| `remote_address` | service | peer IP of the `apply` request (proxy's `X-Forwarded-For` first value if present, else socket peer) |
| `zone_name` | applicant | |
| `zone_port` | applicant | |
| `catalog_provider_hosts` | applicant | |
| `zone_key` | applicant or service | applicant-supplied value when present, else generated |
| `zone_key_generated` | service | boolean |
| `negotiation_key` | service | shared key for this federation pair |
| `name` | applicant | optional, informational only, null when absent |
| `email` | applicant | optional, informational only, never used to send mail, null when absent |
| `judged_at` | service | null until judged |
| `judged_by` | service | iRODS username of the admin, null until judged |
| `decision_note` | admin | free text, optional, max 1024 chars |

### 5.3 Bootstrap key record

| Field | Set by | Notes |
|---|---|---|
| `bootstrap_key` | service | `uuid4` string, the secret handed to the operator out of band |
| `status` | service | `available`, `redeemed`, `expired`, `revoked` |
| `catalog_service_role` | admin | `consumer` or `provider`; decides what `redeem` releases |
| `note` | admin | free text for the admin's own bookkeeping, optional, max 1024 chars, null when absent |
| `created_at` | service | |
| `created_by` | service | iRODS username of the admin who created it |
| `expires_at` | service | `created_at + lifetime`, where lifetime is the `lifetime_in_seconds` given at creation, else `keyserver.lifetime_of_bootstrap_keys_in_seconds` |
| `redeemed_at` | service | null until redeemed |
| `redeemed_from` | service | peer IP of the `redeem` request (proxy's `X-Forwarded-For` first value if present, else socket peer), null until redeemed |
| `revoked_at` | service | null unless revoked |
| `revoked_by` | service | iRODS username of the admin who revoked it, null unless revoked |

A key is consumed at the moment a redemption succeeds (`200`), in the same
lock and the same state write that delivers the facts. A redemption that
fails for any reason, including a home `server_config_file` that cannot be
read, does not consume the key. A redeemed, expired, or revoked key is
never revived.

### 5.4 Zone key state

```json
{
  "zone_key":      {"value": "...", "issued_at": "...", "expires_at": "..."},
  "next_zone_key": {"value": "...", "minted_at": "...", "expires_at": "..."}
}
```

`zone_key.value` is always the key currently in `server_config_file`; the
state records when the service first saw it (`issued_at`) and when it is
meant to be rotated (`expires_at = issued_at + lifetime_of_zone_keys_in_seconds`).
`next_zone_key` is minted by the service (section 5.6) and is the key the
zone will rotate to; its `expires_at` is `zone_key.expires_at + lifetime`,
the moment it would in turn be rotated out after becoming current.

The state is reconciled against `server_config_file` on every keyserver and
bootstrap request, under the lock:

1. No state yet (first start): adopt the file's `zone_key` with
   `issued_at = now`, mint `next_zone_key`. Logged at INFO.
2. The file's `zone_key` equals `zone_key.value`: nothing to do.
3. The file's `zone_key` equals `next_zone_key.value`: a rotation happened.
   `next_zone_key` becomes `zone_key` with `issued_at = now` and
   `expires_at = now + lifetime`, and a fresh `next_zone_key` is minted.
   Logged at INFO.
4. The file's `zone_key` equals neither: someone changed the key by hand.
   Adopt it as in rule 1 and mint a fresh `next_zone_key`. Logged at
   WARNING.

The service never writes `zone_key` into `server_config_file` in this
iteration; rotation is performed elsewhere, later. `negotiation_key` does
not rotate and is not tracked.

### 5.5 Validation rules

Derived from `schemas/configuration/v5/server_config.json.in`.

| Field | Rule |
|---|---|
| `zone_name` | `^[A-Za-z0-9_.]+$`, 1–63 chars, must not equal the home zone's name (federation only) |
| `zone_port` | integer 0–65535 |
| `catalog_provider_hosts` | non-empty array of unique strings; each an RFC 1123 hostname (labels of `[A-Za-z0-9-]`, 1–63 chars, no leading/trailing hyphen, total ≤ 253) or an IPv4/IPv6 literal |
| `zone_key` | `^[A-Za-z0-9_]+$`, 1–49 chars |
| `negotiation_key` | `^[A-Za-z0-9_]{32}$` |
| `catalog_service_role` | `consumer` or `provider` |
| `bootstrap_key` | string in canonical `uuid4` text form, lower case |
| `lifetime_in_seconds` | integer > 0 |
| `note` | string ≤ 1024 chars |
| `decision_note` | string ≤ 1024 chars |
| `name` | string, 1–256 chars, no other rule |
| `email` | string, 3–254 chars, exactly one `@` with at least one character on each side, no other rule, never verified |

### 5.6 Key generation

Generated `zone_key`, `negotiation_key`, and `next_zone_key` values use
`secrets.choice` over the alphabet `A-Za-z0-9_` (64 symbols) and are 32
characters long (192 bits). Bootstrap keys and application ids are `uuid4`.

## 6. Authentication

Every `list` and `judge` endpoint, and every `/keyserver` endpoint, requires
HTTP Basic authentication.

1. Missing or malformed `Authorization` header → `401` with
   `WWW-Authenticate: Basic realm="controlplane"`.
2. Decode `username:password`. The username is a plain home-zone username;
   `user#zone` forms are rejected with `401`.
3. Open a python-irodsclient session with the home zone's host, port, and
   zone from the service account environment, the supplied username and
   password, and `authentication_scheme` taken from the environment (default
   `native`). Perform a real connection (`session.server_version` or
   equivalent) so bad passwords fail here → `401`. An unreachable iRODS
   server → `502`.
4. Query `session.users.get(username).type`. Anything other than `rodsadmin`
   → `403`.
5. The session is kept for the duration of the request and used for any
   iRODS operations the request performs, so catalog changes are attributed
   to the judging admin, not the service account.

No caching of credentials or sessions between requests.

## 7. Endpoints

All request and response bodies are JSON with `Content-Type: application/json`.
Errors use `{"error": "<human readable message>"}`. Unknown paths → `404`;
there are no aliases for the feature paths at the top level. Wrong method
→ `405` with an `Allow` header. Bodies larger than `max_request_body_bytes`
→ `413`. Malformed JSON or validation failures → `400`. A trailing slash is
ignored.

Before handling any request, the service runs the lazy expiration sweep
(section 8).

The same surface is described machine-readably in `controlplane-openapi.yaml`
(section 15). Where the two disagree, this document wins and the OpenAPI file
is the one to fix.

| Method and path | Auth | Section |
|---|---|---|
| `GET /info` | none | 7.0 |
| `POST /federation/apply` | none | 7.1.1 |
| `GET /federation/status` | none | 7.1.2 |
| `GET /federation/list` | rodsadmin | 7.1.3 |
| `POST /federation/judge` | rodsadmin | 7.1.4 |
| `GET /keyserver/local_zone` | rodsadmin | 7.2.1 |
| `POST /bootstrap/create` | rodsadmin | 7.3.1 |
| `POST /bootstrap/redeem` | none | 7.3.2 |
| `POST /bootstrap/revoke` | rodsadmin | 7.3.3 |
| `GET /bootstrap/list` | rodsadmin | 7.3.4 |

### 7.0 `GET /info` — unauthenticated

Publishes what a federation applicant needs to know before applying. No
iRODS connection is made. Response `200`:

```json
{
  "service": "controlplane",
  "version": "0.1.0",
  "federation": {"application_expiration_in_seconds": 604800}
}
```

The home zone's name is deliberately not included. The response carries
`Cache-Control: no-store`.

### 7.1 Federation

#### 7.1.1 `POST /federation/apply` — unauthenticated

Request:

```json
{
  "zone_name": "otherZone",
  "zone_port": 1247,
  "catalog_provider_hosts": ["icat.other.example.org"],
  "zone_key": "OPTIONAL_EXISTING_ZONE_KEY",
  "name": "Optional Human Name",
  "email": "optional@example.org"
}
```

`name` and `email` are for the reviewing admin only. The service never uses
them for anything and never sends mail.

Behavior:

1. Validate per 5.6. Extra keys → `400`.
2. If `federation.max_pending_applications` pending federation records
   exist → `503`.
3. If a `pending` federation record with the same `zone_name` exists →
   `409`. Approved, rejected, and expired records do not block a new
   application.
4. Generate `negotiation_key`; generate `zone_key` if not supplied.
5. Persist, then respond `201`:

```json
{
  "application_id": "6f1c...",
  "status": "pending",
  "expires_at": "2026-11-08T15:04:05Z",
  "negotiation_key": "....32 chars....",
  "zone_key": "....",
  "status_url": "/federation/status?application_id=6f1c..."
}
```

`apply` performs no iRODS operations and needs no iRODS connection.

#### 7.1.2 `GET /federation/status?application_id=<uuid>` — unauthenticated

The `application_id` is unguessable and acts as the bearer token.

- Missing, empty, or repeated `application_id` → `400`. Unknown id → `404`.
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
  "submitted_at": "2026-11-01T15:04:05Z",
  "expires_at": "2026-11-08T15:04:05Z",
  "judged_at": "2026-11-02T09:00:00Z",
  "decision_note": null,
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
`negotiation_key` is never disclosed to a federation applicant; the
pair-specific key from the record is used.

`name` and `email` are never returned by `status`.

#### 7.1.3 `GET /federation/list?status=<filter>` — rodsadmin

- `status` is one of `pending` (default), `approved`, `rejected`, `expired`,
  `all`. Anything else, or a repeated parameter → `400`.
- Response `200`:

```json
{ "applications": [ { ...5.2... }, ... ] }
```

Ordered by `submitted_at` ascending. Records are returned in full because
the caller is a rodsadmin.

#### 7.1.4 `POST /federation/judge` — rodsadmin

Request:

```json
{
  "application_id": "6f1c...",
  "decision": "approve",
  "decision_note": "optional free text"
}
```

- Extra keys → `400`. `decision` must be `approve` or `reject` → else `400`.
- Unknown id → `404`.
- Status `approved` or `rejected` → `409`.
- Status `expired` → `410`.

The body is read and parsed before the credentials are checked, so a
malformed or oversized body answers `400` or `413` even without valid
credentials.

**Reject:** set `status=rejected`, `judged_at`, `judged_by`,
`decision_note`; persist; respond `200` with `{"application": {...record...}}`.
No iRODS actions.

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
   (`session.zones.get`). If one exists → `409`, nothing changed, except in
   the retry case described under step 4.
2. Create the remote zone in the home catalog with the admin's session,
   equivalent to `iadmin mkzone <zone_name> remote <host>:<zone_port>` where
   `host` is `catalog_provider_hosts[0]`. python-irodsclient's
   `ZoneManager.create` does not accept a connection string, so the service
   sends `GeneralAdminRequest("add", "zone", zone_name, "remote",
   "host:port", "")` directly via `GENERAL_ADMIN_AN`.
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
   - Save a backup copy as `<server_config_file>.controlplane-backup-<UTC timestamp>`.
   - Write atomically (temp file + `os.replace`, preserving the original
     file's mode), `json.dump(indent=4, sort_keys=True)`.
   - On failure roll back step 2 with `rmzone`.
4. Send `SIGHUP` to the PID in `server_pid_file` so the iRODS server reloads
   its configuration. A missing or stale PID file is a step failure; the
   catalog and file changes are **not** rolled back in that case, because
   they are correct and only the reload is outstanding. The error message
   says so and the record stays `pending` so the admin can retry. On retry
   the service detects that the catalog zone and the federation entry both
   already exist *with the record's keys*, skips steps 2 and 3, and only
   re-attempts step 4.
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

### 7.2 Keyserver

The endpoint requires a rodsadmin (section 6) and reconciles the zone key
state against `server_config_file` first (5.4).

#### 7.2.1 `GET /keyserver/local_zone` — rodsadmin

Returns the lifecycle of the home zone's `zone_key`. Response `200`:

```json
{
  "zone_name": "homeZone",
  "lifetime_of_zone_keys_in_seconds": 7776000,
  "zone_key": {
    "value": "<the key currently in server_config.json>",
    "issued_at": "2026-11-01T15:04:05Z",
    "expires_at": "2027-01-30T15:04:05Z"
  },
  "next_zone_key": {
    "value": "<the key the zone will rotate to>",
    "minted_at": "2026-11-01T15:04:05Z",
    "expires_at": "2027-04-30T15:04:05Z"
  }
}
```

A server wanting to rotate itself needs `next_zone_key.value` and
`zone_key.expires_at`; both are returned because the caller is a rodsadmin
and the channel is the proxy's TLS. No iRODS write is performed. The
response carries `Cache-Control: no-store`.

### 7.3 Bootstrap

Every bootstrap endpoint reconciles the zone key state (5.4) first, under
the lock.

#### 7.3.1 `POST /bootstrap/create` — rodsadmin

Creates one single-use bootstrap key for the admin to hand to the operator
of a new server out of band. Request:

```json
{
  "catalog_service_role": "consumer",
  "lifetime_in_seconds": 3600,
  "note": "new consumer for the imaging lab"
}
```

`catalog_service_role` is required and fixes what the key releases: a
`provider` key additionally releases the zone's database credentials.
`lifetime_in_seconds` is optional and defaults to
`keyserver.lifetime_of_bootstrap_keys_in_seconds`. `note` is optional and
is shown back to admins by `list`; the service never interprets it.

- Extra keys → `400`. Missing or invalid `catalog_service_role` → `400`.
  `lifetime_in_seconds` or `note` failing 5.5 → `400`.
- Otherwise persist and respond `201` with the 5.3 record:

```json
{
  "bootstrap_key": "0b2f...-...",
  "status": "available",
  "catalog_service_role": "consumer",
  "note": "new consumer for the imaging lab",
  "created_at": "2026-11-01T15:04:05Z",
  "created_by": "rods",
  "expires_at": "2026-11-01T16:04:05Z",
  "redeemed_at": null,
  "redeemed_from": null,
  "revoked_at": null,
  "revoked_by": null
}
```

There is no limit on the number of available keys; an admin can see and
reason about them in `/bootstrap/list`.

#### 7.3.2 `POST /bootstrap/redeem` — unauthenticated

Request:

```json
{ "bootstrap_key": "0b2f...-..." }
```

Behavior:

1. Validate per 5.5. Extra keys → `400`. Missing `bootstrap_key` → `400`.
2. Look up the key. Unknown, redeemed, or revoked → `403` with an error
   saying the bootstrap key is not valid (the three are not
   distinguished); expired → `403` saying so. Nothing is consumed. A
   refused redemption is logged at WARNING with the remote address.
3. Read the zone facts for the key's role from `server_config_file`. A
   failure here → `500`; the key is not consumed.
4. Mark the key `redeemed` with `redeemed_at` and `redeemed_from`, persist,
   and respond `200`:

```json
{
  "zone_facts": {
    "catalog_service_role": "provider",
    "schema_name": "server_config",
    "schema_version": "v5",
    "zone_name": "homeZone",
    "zone_port": 1247,
    "zone_user": "rods",
    "zone_auth_scheme": "native",
    "zone_key": "<the zone's current zone_key>",
    "negotiation_key": "<the zone's negotiation_key>",
    "catalog_provider_hosts": ["icat.home.example.org"],
    "client_server_policy": "CS_NEG_REFUSE",
    "default_hash_scheme": "SHA256",
    "match_hash_policy": "compatible",
    "encryption": {"algorithm": "AES-256-CBC", "key_size": 32, "num_hash_rounds": 16, "salt_size": 8},
    "database": {
      "technology": "postgres",
      "host": "db.home.example.org",
      "port": 5432,
      "name": "ICAT",
      "odbc_driver": "PostgreSQL ANSI",
      "username": "irods",
      "password": "<verbatim from the home provider>"
    }
  }
}
```

The facts are delivered exactly once. No endpoint returns them again; an
operator who loses the response asks the admin for a new key.

Rules for `zone_facts`:

- Every value is copied verbatim from the home provider's
  `server_config_file`; nothing is invented or rewritten. `zone_key` is the
  key currently in that file, which is also `zone_key.value` of 7.2.1.
- `database` is present only when the key's `catalog_service_role` is
  `provider` and is `plugin_configuration.database` copied whole, including
  `password`. A consumer key never releases it.
- `database.host` is whatever the home provider uses, which may be
  `localhost`; the operator of a new provider on another machine must
  substitute an address that reaches the same database. The spec calls this
  out rather than guessing.
- The next zone key and its expiry are not included. A new server that
  comes up after key rotation exists goes through the same rotation process
  as every existing server.
- Host-specific settings (`host`, `server_port_range_*`, the default
  resource, the service account user and group, the admin password) are the
  operator's to supply. Appendix A maps `zone_facts` onto a
  `json_configuration_file`.

The admin password of the zone is **not** part of the facts. The service
cannot reliably recover it (with newer iRODS authentication `.irodsA` is no
longer reversible), so the operator obtains it out of band for now.

`redeem` performs no iRODS operations and needs no iRODS connection.

#### 7.3.3 `POST /bootstrap/revoke` — rodsadmin

Withdraws a key that has not been redeemed. Request:

```json
{ "bootstrap_key": "0b2f...-..." }
```

- Extra keys → `400`. Missing `bootstrap_key` → `400`.
- Unknown key → `404`.
- Status `redeemed`, `expired`, or `revoked` → `409` with an error naming
  the status.
- Status `available` → set `status=revoked`, `revoked_at`, `revoked_by`;
  persist; respond `200` with `{"bootstrap_key": {...5.3 record...}}`.

The body is read and parsed before the credentials are checked, as for
`judge`.

#### 7.3.4 `GET /bootstrap/list?status=<filter>` — rodsadmin

- `status` is one of `available`, `redeemed`, `expired`, `revoked`, `all`
  (default). Anything else, or a repeated parameter → `400`.
- Response `200`:

```json
{ "bootstrap_keys": [ { ...5.3... }, ... ] }
```

Ordered by `created_at` ascending. Records are returned in full because the
caller is a rodsadmin, so the admin can see which keys are still available
and when each expires, which were redeemed, when, and from which address,
and which were revoked or expired unused.

## 8. Expiration

No background thread. At the start of every request, under the lock:

- every `pending` federation application whose `expires_at` is in the past
  becomes `expired`;
- every `available` bootstrap key whose `expires_at` is in the past becomes
  `expired`;

and the state file is persisted if anything changed. Expired records and
keys remain in the state file indefinitely. Zone keys are not swept; their
expiry is informational and is reconciled per 5.4.

## 9. Concurrency

`http.server.ThreadingHTTPServer` serves requests concurrently, with a
listen backlog of 128 so a burst of simultaneous connections is queued
rather than reset. One process-wide `threading.Lock` guards every
read-modify-write of the in-memory state and the state file, the whole of
the federation approve sequence (7.1.4) so two admins cannot approve
conflicting applications at once, the bootstrap key redemption of 7.3.2
so one key cannot be redeemed twice, and the zone key reconciliation of
5.4.
iRODS connections are per request and are not shared across threads.

## 10. Logging and signals

- One line per request to stdout: timestamp, remote address, method, path,
  status code, and the admin username for authenticated calls. Passwords
  and keys, including bootstrap keys and zone keys, are never logged.
- Apply log lines say which feature the application is for.
- Bootstrap key creation, redemption, and revocation, zone key adoption
  and rotation detection, approve/reject decisions, and every step of the
  federation approve sequence are logged at INFO; failures at ERROR with
  the exception text; a hand-changed zone key and a refused redemption at
  WARNING.
- `SIGTERM` and `SIGINT` stop the server cleanly after in-flight requests
  finish. The state is always consistent on disk because every change is
  persisted before its response is sent.

## 11. Security notes

- TLS termination in front of the service is a deployment requirement, not
  an option. Admin passwords, bootstrap keys, zone and negotiation keys,
  and database credentials all cross the wire as plain HTTP content and
  are protected in transit only by the proxy's TLS. Binding to `0.0.0.0`
  on plain HTTP without a proxy exposes every one of them. The service
  does nothing to protect itself from that misconfiguration; a later
  iteration may refuse to bind a non-loopback address without an explicit
  opt-in.
- Federation approval refuses applicant hosts that do not resolve, because
  an unresolvable remote zone host slows every home zone agent start by
  about 30 seconds. This also means a host must be resolvable from the
  service's host at judge time.
- `/federation/apply` and `/bootstrap/redeem` are the only unauthenticated
  writes. Both are bounded by `max_request_body_bytes` and strict field
  validation; `apply` further by `max_pending_applications` and its 409
  duplicate rule, `redeem` by the single-use key, so a request without a
  valid key changes nothing.
- `/federation/status` reveals the home zone's `zone_key` only for approved
  applications and only to a holder of the matching 122-bit random
  `application_id`.
- `name` and `email` on federation applications are stored and shown to
  admins as supplied. They are not verified, not used to contact anyone,
  and not returned to applicants.
- A bootstrap key is a 122-bit random bearer token, single use, with a
  default one-hour lifetime that the creating admin can shorten or extend
  per key. Creating it is the approval: whoever redeems it within its
  lifetime receives the zone facts with no further human step, so a key
  must be handed over as carefully as the secrets it releases.
  `/bootstrap/revoke` withdraws an unredeemed key at any time, and
  `/bootstrap/list` shows every redemption with its source address so an
  unexpected one is visible.
- `zone_facts` reveals the home zone's own `zone_key` and `negotiation_key`
  and, for a provider key, the database password. These are exactly what a
  new server in the zone must have. The role is fixed by the admin at
  creation, not chosen by the redeemer, so a consumer key can never fetch
  the database password.
- The federation feature never discloses the home zone's top-level
  `negotiation_key`; the bootstrap feature must, because a server in the
  same zone needs it. The two features are distinct code paths and the
  federation path reads only the pair-specific key from its record.
- `/keyserver/local_zone` returns the current and next zone keys to a
  rodsadmin. The state file holding them is mode `0600` and lives beside
  the configuration file; the service account's home directory is the
  intended place.
- The service never writes the zone key into `server_config.json` and never
  sends a reload for keyserver or bootstrap operations; only federation
  approval touches the iRODS server.
- Admin passwords are verified against iRODS on every call and never stored.

## 12. Out of scope for this iteration

- Performing zone key rotation: writing `next_zone_key` into
  `server_config.json` on expiry and reloading, and coordinating that
  across the zone's servers. The keyserver only publishes the schedule and
  the next key.
- Rotating `negotiation_key`.
- Returning the zone's admin password in `zone_facts`.
- Any iRODS action on redemption, such as pre-creating a resource.
- Returning the zone facts a second time after a lost `redeem` response;
  the operator asks for a new key.
- Application-layer encryption of applicant secrets independent of the
  transport; TLS termination is required instead.
- TLS in-process; refusing to bind a non-loopback address; purging or
  redacting old records; per-IP rate limiting; notifying applicants; more
  than one control plane process per home zone; unfederating; supervision
  by the iRODS server.

## 13. Smoke test plan

Run the service as the `irods` service account against the local zone with
`{"port": 9200}`.

1. `GET /info` → `200` showing `service: controlplane` and both feature
   blocks.
2. `GET /keyserver/local_zone` without auth → `401`; as a rodsuser → `403`;
   as `rods` → `200` whose `zone_key.value` equals the `zone_key` in
   `/etc/irods/server_config.json`, with `issued_at` about now and a
   `next_zone_key`. The state file exists with mode `0600`. Call again:
   same values, nothing re-minted.
3. Change `zone_key` in `server_config.json` by hand to the `next_zone_key`
   value (and `kill -HUP` the server); call again → the old next key is now
   `zone_key` with a fresh `issued_at`, a new `next_zone_key` was minted,
   and the log says a rotation was detected. Restore the original key the
   same way; call again → the log warns about a hand-changed key and the
   state re-adopts it.
4. `POST /bootstrap/create` as `rods` with `catalog_service_role:
   consumer` → `201` with a key whose `expires_at` is one hour after
   `created_at`; `GET /bootstrap/list` shows it `available`. Without auth
   → `401`; as a rodsuser → `403`; with no role → `400`.
5. `POST /bootstrap/redeem` with the key → `200` with `zone_facts`
   matching `server_config.json` and no `database`. Redeem again → `403`,
   the key is not valid. `GET /bootstrap/list` shows the key `redeemed`
   with `redeemed_at` and `redeemed_from`.
6. Create a `provider` key with `lifetime_in_seconds: 120` and a `note`,
   redeem it, and confirm the facts include `database` with the password
   from `server_config.json`. Confirm `iadmin lz`, `server_config.json`,
   and the server log are unchanged by all bootstrap steps.
7. Create a key with `lifetime_in_seconds: 1`, wait, `GET /bootstrap/list`
   shows it `expired`, redeem it → `403` saying expired.
8. Create a key and `POST /bootstrap/revoke` it → `200` with status
   `revoked`; redeem it → `403`; revoke it again → `409`; revoke an unknown
   key → `404`.
9. Clean up: stop the service and delete its state file; nothing in iRODS
   needs restoring.

Federation, against the same instance:

10. `POST /federation/apply` for a fake zone; expect `201` and both keys.
    Repeat for a second fake zone with a `name` and `email`; expect `201`.
    Repeat with an unknown field and with a malformed `email`; expect `400`.
11. `GET /federation/list` without auth → `401`; with a rodsuser → `403`;
    with `rods` → `200` showing both pending records, one with the name and
    email as supplied.
12. `GET /federation/status` → `pending`.
13. `POST /federation/judge` approve as `rods`; expect `200`, `iadmin lz`
    shows the new zone, `/etc/irods/server_config.json` has the new
    federation entry, and the server log shows the reload. Use a
    resolvable applicant hostname (`localhost` or an IP literal); the
    reload briefly refuses connections while the agent factory restarts.
14. `GET /federation/status` → `approved` with `applicant_federation_entry`
    whose `zone_key` matches `server_config.json` and whose
    `negotiation_key` matches the one returned at apply time.
15. `POST /federation/judge` again on the same id → `409`.
16. Apply with a 1-second federation expiration config, wait,
    `GET /federation/list?status=expired` shows it,
    `POST /federation/judge` → `410`.
17. Clean up: `iadmin rmzone`, restore `server_config.json` from the
    backup, `kill -HUP` the server.

## 14. Automated tests

`test_controlplane.py` (top level) is a `unittest` suite, also runnable
under pytest. It starts the real server in-process on an ephemeral port
against a fake iRODS, with temp copies of the config, environment, state,
and server_config.json, and a child process that records the SIGHUP it
receives. It covers the federation feature (validation, expiration,
authentication, listing, reject, and every approve step with its rollbacks
and the reload retry), the keyserver (adoption, steady state, rotation
detection, hand-change warning, state file mode), the bootstrap feature
(creation with the default and an overridden lifetime, every validation,
redemption under concurrency, redeemed, expired, and revoked keys, facts
for both roles, database only for provider keys, a missing fact leaving
the key unconsumed, revocation, listing, no iRODS writes), the nested
configuration, the state file, and `main()`.
Live tests run against a real zone only when
`CONTROLPLANE_LIVE_IRODS_ENV_FILE` and `CONTROLPLANE_LIVE_ADMIN_PASSWORD`
are set (plus `CONTROLPLANE_LIVE_RODSUSER` and
`CONTROLPLANE_LIVE_RODSUSER_PASSWORD` for the 403 case); they must run as
the iRODS service account so python-irodsclient can find `.irodsA`.

The suite also needs PyYAML and jsonschema (Ubuntu packages `python3-yaml`
and `python3-jsonschema`) for the contract checks of section 15.

```
PYTHONPATH=/src/python-irodsclient python3 -m pytest -q test_controlplane.py
PYTHONPATH=/src/python-irodsclient python3 -m coverage run --branch --include=controlplane.py -m pytest -q test_controlplane.py
python3 -m coverage report -m
```

## 15. OpenAPI description

`controlplane-openapi.yaml` describes sections 6 and 7 as an OpenAPI 3.0.3
document: every path, method, request body, response body, status code,
and header; the HTTP Basic security scheme on the rodsadmin operations;
named schemas for every object that crosses the wire; worked examples; and
operations grouped by the tags `service`, `federation`, `keyserver`, and
`bootstrap`. 3.0.3 was chosen over 3.1.0 for the widest tooling support;
the only 3.0 idiom used is `nullable: true`.

The file is validated with `openapi-spec-validator` and is enforced by the
test suite rather than merely shipped beside it. Every request the suite
sends through its fixture and every response it receives is checked against
the document: the method and path must be declared (an undeclared path must
answer `404`, an undeclared method `405` with the declared `Allow` header),
the status code must be declared for that operation, declared headers must
be present, the JSON body must validate against the declared schema, and
for a 2xx response the request body and query parameters must validate
against the declared request. Separate tests check that the declared
operations are exactly the routes the service serves, that every `$ref`
resolves, that every example validates against its own schema, and that
every declared operation reaches a handler. A new status code or field in
`controlplane.py` therefore fails the suite until the OpenAPI file
describes it.

The YAML file is the single source. A JSON rendering, when a tool wants one,
is a lossless one-liner:

```
python3 -c 'import json, sys, yaml; json.dump(yaml.safe_load(sys.stdin), sys.stdout, indent=2)' < controlplane-openapi.yaml > controlplane-openapi.json
```

## Appendix A. From `zone_facts` to a `json_configuration_file`

`setup_irods.py --json_configuration_file <file>` takes one JSON object with
`host_system_information`, `server_config`, `default_resource_name`,
optional `default_resource_directory`, `service_account_environment`, and
`admin_password`. The operator starts from
`packaging/server_config.json.template.in` (or a copy of a working
`server_config.json`) and sets:

| `server_config` key | Source |
|---|---|
| `catalog_service_role` | `zone_facts.catalog_service_role` |
| `zone_name`, `zone_port`, `zone_user`, `zone_auth_scheme` | `zone_facts` |
| `zone_key`, `negotiation_key` | `zone_facts` |
| `catalog_provider_hosts` | `zone_facts.catalog_provider_hosts`; a new provider adds its own host |
| `client_server_policy`, `default_hash_scheme`, `match_hash_policy`, `encryption` | `zone_facts`, so the zone stays consistent |
| `plugin_configuration.database` | `zone_facts.database` (provider only), with `host` corrected if it was `localhost` |
| `host`, `server_port_range_start`, `server_port_range_end`, `default_resource_name`, `federation` (`[]`) | the operator |

`service_account_environment` is the usual `irods_environment.json` for the
service account with `irods_host` set to the new server, `irods_zone_name`,
`irods_port`, and `irods_user_name` from the facts, and
`irods_client_server_policy` matching. `admin_password` is obtained out of
band. `host_system_information` names the service account user and group,
conventionally `irods`/`irods`.
