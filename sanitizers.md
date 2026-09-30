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
