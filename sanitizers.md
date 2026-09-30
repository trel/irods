# ASan and UBSan investigation

## Plan

1. Configure an out-of-source Clang build with iRODS's ASan and UBSan options,
   including the C++ unit-test targets, and install the local packages.
2. Exercise the installed server with simple iCommands and targeted Python
   tests; inspect sanitizer output and fix reproducible findings in this tree.
3. Run the C++ unit tests and Python core tests, repeat affected exercises after
   fixes, and record results and any remaining limitations here.

## Progress

- The checkout has extensive pre-existing untracked work. Preserve those files;
  this investigation will use a separate build directory and stage only its
  own changes.
- Build instructions: `.opencode/skills/build-irods/SKILL.md`; installed-tree
  test instructions: `skills/run-irods-tests/SKILL.md`. CMake exposes
  `IRODS_ENABLE_ADDRESS_SANITIZER` and
  `IRODS_ENABLE_UNDEFINED_BEHAVIOR_SANITIZER`.
- Configured `build-sanitizers` with Clang 16, RelWithDebInfo, ASan, UBSan and
  `IRODS_UNIT_TESTS_BUILD=YES`; the full build succeeded (1,259 steps).
- Linking unit tests generated repeated unsigned-overflow reports from GCC 14
  libstdc++ headers (`basic_string.h:495`, `string_view:575`). A GCC path added
  to the existing UBSan ignorelist did not suppress inlined standard-library
  code, so it was reverted; these external reports must be filtered when
  inspecting results. The rebuilt packages were generated successfully.
- The previously installed runtime package depended on unavailable `libfmt8`.
  Installing our packages with `dpkg -i` replaced it with packages depending
  on installed `libfmt9`. The unrelated `libc-bin` trigger's `ldconfig -r /`
  currently segfaults; the four iRODS packages configured successfully.
- After starting the instrumented server, `ils` worked. UBSan exposed real
  project findings: a string-literal-to-plugin-factory function-pointer type
  mismatch, intentional wraparound in the iRODS string hash, calls via a
  variadic query function pointer in `miscUtil.cpp`, and access to an
  uninitialized rule-engine global in `irods_plugin_base.hpp`.
- Corrected the plugin factory's deduced string-literal argument type and
  marked intentional unsigned hash wraparound for Clang's overflow check.
- Also corrected all three authentication plugin factories to return the
  declared base pointer type; Clang's function-type check revealed their
  derived-pointer return declarations did not match the loader signature.
- The targeted Python `test_imkdir` suite passed all 9 tests when run with
  `UBSAN_OPTIONS=log_path=/tmp/irods_ubsan_output` to keep diagnostics out of
  the iCommands' expected stderr.
- Ran all 73 C++ unit binaries with `IRODS_PLUGIN_DIRECTORY` set to the
  installed plugin directory. With `ASAN_OPTIONS=detect_leaks=0` (see below),
  61 binaries passed; 12 reported failures, including tests dependent on
  build-tree fixtures, conflicting catalog state, and environment-specific
  resources. There were no ASan logs in `/tmp`.
- Leak checking in short-lived CLI processes unexpectedly exits with code 1,
  sometimes truncating `izonereport` JSON output without a readable ASan log.
  Setting `detect_leaks=0` restores normal CLI behavior while retaining ASan
  memory-error checks; the first full Python core-suite attempt then reached
  the tests but stopped at an existing `alice` user in the dirty catalog.
- Recreated the local `ICAT` PostgreSQL database and ran the installed setup
  script successfully. The full Python core suite then passed the access-time
  tests and dozens of rule/microservice cases, with no reported test failure,
  before the command's 3,600-second limit terminated it around
  `test_rulemsiGetMoreRows_r`. This is a partial core-suite run, not a pass.
- On that run UBSan also found an invalid enum load from checking a negative
  packing-type lookup *after* assigning it to an enum, a zero-sized VLA in
  rule-language array traversal, a microservice factory called with the wrong
  argument list, and intentional unsigned wraparound in Bernstein hashing.
  Those are now addressed. GenQuery calls through a variadic pointer were
  changed to use the client/server function signatures selected by connection
  type. A main-server plugin call now handles the rule-engine manager being
  absent during startup.
- Reinstalled the final sanitizer packages after these changes. The
  `irods_packstruct` unit binary passes (24 assertions), and the installed
  Python `test_imkdir` suite passes (9 tests). The `test_ils` suite passed 17
  tests against the previous build of these last packing-type changes.

## Findings and verification

The following UBSan findings in this tree still require a larger API-dispatch
redesign and were not fixed in this investigation:

- `server/core/src/rsApiHandler.cpp:209,215,222`: calls through the generic
  `api_entry::call_wrapper` variadic function pointer do not match the
  individual plugin wrapper signatures.
- Rule-language generic traversal (`restruct.templates.hpp:75`): a
  type-erased copy-function pointer differs from the concrete function type.

There are repeated inlined libstdc++ unsigned-wrap reports from GCC 14
headers, outside this checkout. No ASan memory-error report was observed in
the exercised paths. Leak detection was disabled for the short-lived test
clients because their premature exit broke the test harness; it was not an
ASan memory-error finding.

## Full core-suite follow-up (2026-09-30)

Run every identifier in the installed `core_tests_list.json` in bounded slices
of five test methods. The test-only orchestration script is outside the
checkout at `/tmp/opencode/run_sanitized_core_batch.py`; it starts the
installed test-mode server and invokes the unchanged installed test runner's
`run_tests_from_names()`. Run as `irods`, passing `INDEX OFFSET 5`; the
printed `NEXT` cursor is the next slice. Record outcomes below before moving
the cursor. No tests in the checkout or installed tree are edited.

```
su - irods -c 'UBSAN_OPTIONS=log_path=/tmp/irods_ubsan_output:print_stacktrace=1 ASAN_OPTIONS=detect_leaks=0:log_path=/tmp/irods_asan_output python3 /tmp/opencode/run_sanitized_core_batch.py INDEX OFFSET 5'
```

The existing installed ASan/UBSan build is used. Leak detection remains
disabled for short-lived clients because it previously caused CLI processes
to exit early without actionable leak diagnostics; address and UB checks
remain active. Inspect newly produced sanitizer logs between batches, fix
reproducible project-code findings, and make one commit per distinct finding.

| Identifier index | Offset | Result | Next cursor |
| --- | --- | --- | --- |
| 0 (`test_access_time_updates`) | 0–4 | 5 passed | `0 5` |
| 0 (`test_access_time_updates`) | 5 | 1 passed | `1 0` |
| 1 (`test_all_rules.Test_AllRules`) | 0–4 | 5 passed | `1 5` |
| 1 (`test_all_rules.Test_AllRules`) | 5–9 | 4 passed, 1 skipped (PREP) | `1 10` |
| 1 (`test_all_rules.Test_AllRules`) | 10–14 | 5 passed | `1 15` |
| 1 (`test_all_rules.Test_AllRules`) | 15–19 | 5 passed | `1 20` |
| 1 (`test_all_rules.Test_AllRules`) | 20–24 | 4 passed, 1 skipped (large file) | `1 25` |
| 1 (`test_all_rules.Test_AllRules`) | 25–29 | 3 passed, 2 skipped (upstream) | `1 30` |
| 1 (`test_all_rules.Test_AllRules`) | 30–34 | 5 passed | `1 35` |
| 1 (`test_all_rules.Test_AllRules`) | 35–39 | 5 passed | `1 40` |
| 1 (`test_all_rules.Test_AllRules`) | 40–44 | 5 passed | `1 45` |
| 1 (`test_all_rules.Test_AllRules`) | 45–49 | 5 passed | `1 50` |
| 1 (`test_all_rules.Test_AllRules`) | 50–54 | 5 passed | `1 55` |
| 1 (`test_all_rules.Test_AllRules`) | 55–59 | 5 passed | `1 60` |
| 1 (`test_all_rules.Test_AllRules`) | 60–64 | 5 passed | `1 65` |
| 1 (`test_all_rules.Test_AllRules`) | 65–69 | 5 passed | `1 70` |
| 1 (`test_all_rules.Test_AllRules`) | 70–74 | 5 passed | `1 75` |
| 1 (`test_all_rules.Test_AllRules`) | 75–79 | 5 passed | `1 80` |
| 1 (`test_all_rules.Test_AllRules`) | 80–84 | 5 passed | `1 85` |
| 1 (`test_all_rules.Test_AllRules`) | 85–89 | 5 passed | `1 90` |
| 1 (`test_all_rules.Test_AllRules`) | 90–94 | 5 passed | `1 95` |
| 1 (`test_all_rules.Test_AllRules`) | 95–99 | 5 passed | `1 100` |
| 1 (`test_all_rules.Test_AllRules`) | 100–104 | 5 passed | `1 105` |
| 1 (`test_all_rules.Test_AllRules`) | 105–109 | 5 passed | `1 110` |
| 1 (`test_all_rules.Test_AllRules`) | 110–114 | 5 passed | `1 115` |
| 1 (`test_all_rules.Test_AllRules`) | 115–119 | 5 passed; UBSan finding | `1 120` |
| 1 (`test_all_rules.Test_AllRules`) | 120–124 | 5 passed | `2 0` |
| 2 (`test_all_rules.Test_JSON_microservices`) | 0–4 | 5 passed | `2 5` |
| 2 (`test_all_rules.Test_JSON_microservices`) | 5–9 | 5 passed | `2 10` |
| 2 (`test_all_rules.Test_JSON_microservices`) | 10–11 | 2 passed | `3 0` |
| 3 (`test_all_rules.Test_msiDataObjRepl_checksum_keywords`) | 0–4 | 5 passed | `3 5` |
| 3 (`test_all_rules.Test_msiDataObjRepl_checksum_keywords`) | 5–9 | 5 passed | `3 10` |
| 3 (`test_all_rules.Test_msiDataObjRepl_checksum_keywords`) | 10–11 | 2 passed | `4 0` |
| 4 (`test_all_rules.test_msi_replica_truncate`) | 0–4 | 5 passed | `4 5` |
| 4 (`test_all_rules.test_msi_replica_truncate`) | 5–9 | 5 passed | `5 0` |
| 5 (`test_auth.Test_Auth`) | 0–1 | 1 passed, 1 failed (PAM) | retry `5 0` |
| 5 (`test_auth.Test_Auth`) | 0–3 | 4 passed after fix | `6 0` |
| 6 (`test_auth.test_iinit`) | 0–4 | 5 passed | `6 5` |
| 6 (`test_auth.test_iinit`) | 5–8 | 4 passed | `7 0` |
| 7 (`test_catalog`) | 0–2 | 3 passed | `8 0` |
| 8 (`test_client_hints`) | 0 | failed (server disconnect) | retry `8 0` |
| 8 (`test_client_hints`) | 0 | 1 passed after fix | `9 0` |
| 9 (`test_collection_mtime`) | 0–1 | 2 passed | `10 0` |
| 10 (`test_configuration`) | 0–4 | 5 passed | `11 0` |
| 11 (`test_delay_queue.Test_Delay_Queue`) | 0–3 | 3 passed, long-job test failed | retry `11 3` |
| 11 (`test_delay_queue.Test_Delay_Queue`) | 3 | same timing failure; no new sanitizer report | `11 4` |
| 11 (`test_delay_queue.Test_Delay_Queue`) | 4–8 | 5 passed | `11 9` |
| 11 (`test_delay_queue.Test_Delay_Queue`) | 9–11 | 3 passed | `12 0` |
| 12 (`test_delay_queue.Test_Execution_Frequency`) | 0–2 | 3 passed | `13 0` |
| 13 (`test_dynamic_peps`) | 0–4 | 5 passed | `13 5` |
| 13 (`test_dynamic_peps`) | 5–7 | 2 passed, `StructFileExtAndRegInp` failed | retry `13 7` |
| 13 (`test_dynamic_peps`) | 7 | failed again; no new sanitizer report | `13 8` |
| 13 (`test_dynamic_peps`) | 8–12 | 5 passed | `14 0` |
| 14 (`test_genquery2_microservices`) | 0–1 | 2 passed | `15 0` |
| 15 (`test_iadmin.Test_Iadmin`) | 0–4 | 5 passed | `15 5` |
| 15 (`test_iadmin.Test_Iadmin`) | 5–9 | 5 passed | `15 10` |
| 15 (`test_iadmin.Test_Iadmin`) | 10–14 | 5 passed | `15 15` |
| 15 (`test_iadmin.Test_Iadmin`) | 15–19 | 5 passed | `15 20` |
| 15 (`test_iadmin.Test_Iadmin`) | 20–24 | 5 passed | `15 25` |
| 15 (`test_iadmin.Test_Iadmin`) | 25–29 | 5 passed | `15 30` |
| 15 (`test_iadmin.Test_Iadmin`) | 30–34 | 4 passed, `parent_context` test failed | retry `15 34` |
| 15 (`test_iadmin.Test_Iadmin`) | 34 | failed again; no new sanitizer report | `15 35` |
| 15 (`test_iadmin.Test_Iadmin`) | 35–39 | 5 passed | `15 40` |
| 15 (`test_iadmin.Test_Iadmin`) | 40–44 | 5 passed | `15 45` |
| 15 (`test_iadmin.Test_Iadmin`) | 45–49 | 5 passed; UBSan finding | `15 50` |

`Test_AllRules` contains 125 methods. The previously recorded
`rsApiHandler.cpp` function-type mismatch is still emitted by normal API
requests. Subsequent batches use cursor-specific sanitizer log prefixes to
isolate additional findings.

The `1 5` UBSan logs contain the existing API wrapper call-type report and
also reproduce the generic cache-copy callback type mismatch in the
rule-language engine (`restruct.templates.hpp:75`). Replaced the unsafe
function-pointer cast with a typed cache-copy adapter in `cache.proto.hpp` and
`traversal.instance.hpp`. Rebuilt the ASan/UBSan server package, installed it,
and reran the same five tests: 4 passed, 1 skipped. UBSan reports for
`restruct.templates.hpp:75` are absent from the rerun's
`/tmp/irods_ubsan_verify_cache.*` files; the separate `rsApiHandler.cpp`
report remains. No ASan report was generated.

The batch coordinator at `/tmp/opencode/run_sanitized_batches.py` invokes the
same five-method runner sequentially, applies a 1,200-second limit per batch,
and stops if it sees a new project-code UBSan or ASan report. The four batches
at offsets 35–54 passed with only the known generic API-dispatch report.

The batch at offset 115 exposed `functions.cpp:2576`: `construct()` calls
`memcpy` with null `args` for a valid zero-argument rule expression. The
implementation now leaves `subtrees` null for zero arguments and copies only
nonempty arrays. Rebuilt and installed the ASan/UBSan server package. Reran
the same five rule tests: all passed; the new
`/tmp/irods_ubsan_verify_empty_args.*` logs do not contain this report. No
ASan report was generated. Continue at cursor `1 120`.

Index 5 stopped at `test_authentication_PAM_with_server_params`. Running the
installed `irodsPamAuthCheck` directly reproduces an ASan SEGV at a null PC
in `pam_unix.so`'s `crypt_r` call. The Clang ASan runtime exports a crypt_r
interceptor, but `libcrypt` is normally loaded only when PAM loads pam_unix,
after the interceptor initialized its real-function pointer. For the
sanitizer build, link the helper against libcrypt with `--no-as-needed` so it
is present at process startup. Rebuilt and reinstalled the server package;
the helper now authenticates successfully under ASan, and the complete
`Test_Auth` class passes all 4 tests. The fresh rerun's scoped sanitizer logs
(`irods_batch_5_0_2265820`) contain no new ASan report. The host also lacked
an `/etc/pam.d/irods` service file; configured the local PAM service and its
test account outside this checkout.

PR #9093 (`cf968dee9`, `4abd24832`, `045832006`, `2bb56d6d4`) was
cherry-picked in order, preserving its four upstream commits. Set the new
`IRODS_GIT_SHA1_TO_REPORT` CMake cache variable to a stable value in the
sanitizer build so future commits do not regenerate version files and
invalidate the ccache build. The in-progress PAM fix was stashed for the
cherry-pick and restored afterward.

The first rerun of index 5 found an *old* ASan log using the same prefix as
the failed attempt; the coordinator now includes its process ID in each log
prefix to avoid treating reports from earlier attempts as new findings.

Index 8 (`test_client_hints`) disconnected with `SYS_INTERNAL_ERR`. Both
`rsIESClientHints.cpp` and `rsClientHints.cpp` allocate response buffers via
`new[]` but their callers use `freeBBuf()`/`clearBBuf()`, which call `free()`.
With `alloc_dealloc_mismatch=0` **for diagnosis only**, this test passes. Both
server handlers now allocate with `malloc()` and check allocation failures.
Rebuilt the ASan/UBSan packages and installed both `irods-server` and
`irods-runtime` (the latter owns `libirods_server.so`, which contains these
handlers). Retested with the allocation-mismatch check enabled: 1 passed;
the fresh `/tmp/irods_batch_8_0_2298139*` logs contain no new sanitizer
report. Continue at cursor `9 0`.

The first delay-queue batch failed only the long-running/timing-sensitive
test (`test_delay_queue_with_long_job`); its sanitizer logs contain the known
API-wrapper mismatch and no new ASan/UBSan report. The test intentionally
checks short jobs while a 150-second job is running. Retry that single test
without changing the test suite and investigate the failure separately.

Retried the single long-job test: the short jobs had finished, but some jobs
scheduled 15 seconds later were already running when the test checked that
all five remained queued. No new sanitizer finding; keep this as a timing
failure and continue at `11 4` without editing tests.

At `13 7`, `test_if_StructFileExtAndRegInp_is_exposed__issue_7413`
reproducibly returns `SYS_INTERNAL_ERR` from `ibun -x -f` after creating the
tar. The isolated rerun with `alloc_dealloc_mismatch=0` still fails, so this
is distinct from the earlier client-hints mismatch. Its scoped UBSan logs
contain only the known API-wrapper function-type reports; no ASan log was
created. Record this as an unresolved test failure and continue at `13 8`.

At `15 34`, `test_modify_resource_changing_parent_context_string__issue__4022`
reproducibly returns `SYS_INTERNAL_ERR` from `iadmin modresc parent_context`.
The isolated rerun still fails with `alloc_dealloc_mismatch=0`. The scoped
sanitizer logs contain only the known API-dispatch report and no ASan log;
keep this as an unresolved test failure and continue at `15 35`.

At `15 45`, the replication resource rebalance tests trigger UBSan at
`irods_repl_retry.cpp:57`: the post-decrement in the retry-loop condition
wraps an unsigned zero counter. Move the decrement inside the loop after
checking for a positive count. Rebuilt and installed the ASan/UBSan server
package; the five reproducing tests pass, and the scoped logs at
`/tmp/irods_batch_15_45_2320023_ubsan.*` no longer contain this report.
Continue at `15 50`.
