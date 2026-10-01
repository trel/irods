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
| 15 (`test_iadmin.Test_Iadmin`) | 50–54 | 5 passed | `15 55` |
| 15 (`test_iadmin.Test_Iadmin`) | 55–59 | 5 passed | `15 60` |
| 15 (`test_iadmin.Test_Iadmin`) | 60–64 | 5 passed | `15 65` |
| 15 (`test_iadmin.Test_Iadmin`) | 65–69 | 5 passed | `16 0` |
| 16 (`test_iadmin.Test_Iadmin_Queries`) | 0–2 | 3 passed | `17 0` |
| 17 (`test_iadmin.Test_Iadmin_Resources`) | 0–4 | 5 passed | `17 5` |
| 17 (`test_iadmin.Test_Iadmin_Resources`) | 5–7 | 3 passed | `18 0` |
| 18 (`test_iadmin.Test_Iadmin_modrepl`) | 0–4 | 5 passed | `18 5` |
| 18 (`test_iadmin.Test_Iadmin_modrepl`) | 5 | 1 passed | `19 0` |
| 19 (`test_iadmin.Test_Issue3862`) | 0 | 1 passed | `20 0` |
| 20 (`test_iadmin.test_making_groups`) | 0–3 | 4 passed | `21 0` |
| 21 (`test_iadmin.test_mkzone_conn_str_validation`) | 0–4 | 5 passed | `21 5` |
| 21 (`test_iadmin.test_mkzone_conn_str_validation`) | 5–9 | 5 passed | `22 0` |
| 22 (`test_iadmin.test_moduser_group`) | 0–4 | 5 passed | `22 5` |
| 22 (`test_iadmin.test_moduser_group`) | 5–7 | 3 passed | `23 0` |
| 23 (`test_iadmin.test_moduser_remove_password__issue_2899`) | 0–4 | 5 passed | `24 0` |
| 24 (`test_iadmin.test_moduser_user`) | 0–4 | 5 passed | `24 5` |
| 24 (`test_iadmin.test_moduser_user`) | 5–9 | 5 passed | `25 0` |
| 25 (`test_iadmin.test_modzone_conn_str_validation`) | 0–4 | 5 passed | `25 5` |
| 25 (`test_iadmin.test_modzone_conn_str_validation`) | 5–8 | 4 passed | `26 0` |
| 26 (`test_iadmin_set_grid_configuration.test_get_grid_configuration`) | 0–4 | 5 passed | `26 5` |
| 26 (`test_iadmin_set_grid_configuration.test_get_grid_configuration`) | 5–6 | 2 passed | `27 0` |
| 27 (`test_iadmin_set_grid_configuration.test_set_grid_configuration`) | 0–4 | 5 passed | `27 5` |
| 27 (`test_iadmin_set_grid_configuration.test_set_grid_configuration`) | 5–9 | 5 passed | `27 10` |
| 27 (`test_iadmin_set_grid_configuration.test_set_grid_configuration`) | 10–14 | 5 passed | `27 15` |
| 27 (`test_iadmin_set_grid_configuration.test_set_grid_configuration`) | 15 | 1 passed | `28 0` |
| 28 (`test_ibun`) | 0–4 | 5 passed | `28 5` |
| 28 (`test_ibun`) | 5–6 | 1 skipped, remote archive extraction failed | `29 0` |
| 29 (`test_icd`) | 0–4 | 5 passed | `29 5` |
| 29 (`test_icd`) | 5–6 | 2 passed | `30 0` |
| 30 (`test_ichksum`) | 0–4 | 5 passed | `30 5` |
| 30 (`test_ichksum`) | 5–9 | 5 passed | `30 10` |
| 30 (`test_ichksum`) | 10–14 | 5 passed | `30 15` |
| 30 (`test_ichksum`) | 15–19 | 5 passed | `30 20` |
| 30 (`test_ichksum`) | 20–23 | 4 passed | `31 0` |
| 31 (`test_ichmod.Test_ichmod`) | 0–4 | 5 passed | `32 0` |
| 32 (`test_ichmod.test_collection_acl_inheritance`) | 0–1 | 2 passed | `33 0` |
| 33 (`test_icommands_file_operations.Test_ICommands_File_Operations_1`) | 0–4 | 5 passed | `33 5` |
| 33 (`test_icommands_file_operations.Test_ICommands_File_Operations_1`) | 5–9 | 5 passed | `34 0` |
| 34 (`test_icommands_file_operations.Test_ICommands_File_Operations_2`) | 0–3 | 4 passed | `35 0` |
| 35 (`test_icommands_file_operations.Test_ICommands_File_Operations_3`) | 0–4 | 5 passed | `35 5` |
| 35 (`test_icommands_file_operations.Test_ICommands_File_Operations_3`) | 5–9 | 5 passed | `36 0` |
| 36 (`test_icommands_file_operations.Test_ICommands_File_Operations_4`) | 0–4 | 5 passed | `36 5` |
| 36 (`test_icommands_file_operations.Test_ICommands_File_Operations_4`) | 5–8 | 4 passed | `37 0` |
| 37 (`test_icommands_file_operations.Test_ICommands_File_Operations_5`) | 0–4 | 5 passed | `37 5` |
| 37 (`test_icommands_file_operations.Test_ICommands_File_Operations_5`) | 5–9 | 5 passed | `38 0` |
| 38 (`test_icp.Test_Icp`) | 0–4 | 5 passed | `39 0` |
| 39 (`test_icp.test_overwriting`) | 0–4 | 5 passed | `40 0` |
| 40 (`test_iexit`) | 0–2 | 3 passed | `41 0` |
| 41 (`test_ifsck`) | 0–2 | 3 passed | `42 0` |
| 42 (`test_iget`) | 0–2 | 3 passed | `43 0` |
| 43 (`test_igroupadmin.Test_Igroupadmin`) | 0–1 | 2 passed | `44 0` |
| 44 (`test_igroupadmin.test_making_groups`) | 0–4 | 5 passed | `44 5` |
| 44 (`test_igroupadmin.test_making_groups`) | 5 | 1 passed | `45 0` |
| 45 (`test_ihelp`) | 0–4 | 5 passed | `45 5` |
| 45 (`test_ihelp`) | 5 | 1 passed | `46 0` |
| 46 (`test_ils`) | 0–4 | 5 passed | `46 5` |
| 46 (`test_ils`) | 5–9 | 5 passed | `46 10` |
| 46 (`test_ils`) | 10–14 | 5 passed | `46 15` |
| 46 (`test_ils`) | 15–16 | 2 passed | `47 0` |
| 47 (`test_ilsresc`) | 0–2 | 3 passed | `48 0` |
| 48 (`test_imeta_admin_mode`) | 0–2 | 3 passed | `49 0` |
| 49 (`test_imeta_error_handling`) | 0–4 | 5 passed | `49 5` |
| 49 (`test_imeta_error_handling`) | 5–9 | 5 passed | `49 10` |
| 49 (`test_imeta_error_handling`) | 10–14 | 5 passed | `49 15` |
| 49 (`test_imeta_error_handling`) | 15–19 | 5 passed | `49 20` |
| 49 (`test_imeta_error_handling`) | 20–24 | 5 passed | `49 25` |
| 49 (`test_imeta_error_handling`) | 25 | 1 passed | `50 0` |
| 50 (`test_imeta_help`) | 0 | 1 passed | `51 0` |
| 51 (`test_imeta_set.Test_ImetaCp`) | 0 | 1 passed | `52 0` |
| 52 (`test_imeta_set.Test_ImetaLsLongmode`) | 0–3 | 4 passed | `53 0` |
| 53 (`test_imeta_set.Test_ImetaSet`) | 0–4 | 5 passed | `53 5` |
| 53 (`test_imeta_set.Test_ImetaSet`) | 5–9 | 5 passed | `53 10` |
| 53 (`test_imeta_set.Test_ImetaSet`) | 10–14 | 5 passed | `53 15` |
| 53 (`test_imeta_set.Test_ImetaSet`) | 15–19 | 5 passed | `53 20` |
| 53 (`test_imeta_set.Test_ImetaSet`) | 20–21 | 2 passed | `54 0` |
| 54 (`test_imiscsvrinfo`) | 0–1 | 2 passed | `55 0` |
| 55 (`test_imkdir`) | 0–4 | 5 passed | `55 5` |
| 55 (`test_imkdir`) | 5–8 | 4 passed | `56 0` |
| 56 (`test_imv.Test_Imv`) | 0–2 | 3 passed | `57 0` |
| 57 (`test_imv.test_moving_and_renaming_collections_with_multibyte_characters__issue_6239`) | 0–4 | 5 passed | `57 5` |
| 57 (`test_imv.test_moving_and_renaming_collections_with_multibyte_characters__issue_6239`) | 5–9 | 5 passed | `57 10` |
| 57 (`test_imv.test_moving_and_renaming_collections_with_multibyte_characters__issue_6239`) | 10–13 | 4 passed | `58 0` |
| 58 (`test_imv.test_renaming_collections_with_special_characters__issue_6239`) | 0–1 | 2 passed | `59 0` |
| 59 (`test_ipasswd`) | 0–3 | 4 passed | `60 0` |
| 60 (`test_iphymv.Test_iPhymv`) | 0–4 | 5 passed | `60 5` |
| 60 (`test_iphymv.Test_iPhymv`) | 5–7 | 3 passed | `61 0` |
| 61 (`test_iphymv.test_invalid_parameters`) | 0 | 1 passed | `62 0` |
| 62 (`test_iphymv.test_iphymv_exit_codes`) | 0–4 | 5 passed | `62 5` |
| 62 (`test_iphymv.test_iphymv_exit_codes`) | 5–6 | 2 passed | `63 0` |
| 63 (`test_iphymv.test_iphymv_repl_status`) | 0–4 | 5 passed | `63 5` |
| 63 (`test_iphymv.test_iphymv_repl_status`) | 5–6 | 2 passed | `64 0` |
| 64 (`test_iphymv.test_iphymv_with_two_basic_ufs_resources`) | 0 | 1 passed | `65 0` |
| 65 (`test_ips`) | 0 | failed: missing test client binary | retry `65 0` |
| 65 (`test_ips`) | 0 | 1 passed after installing test helper | `66 0` |
| 66 (`test_iput.Test_Iput`) | 0–4 | 5 passed | `66 5` |
| 66 (`test_iput.Test_Iput`) | 5–9 | 5 passed | `66 10` |
| 66 (`test_iput.Test_Iput`) | 10–14 | 5 passed | `66 15` |
| 66 (`test_iput.Test_Iput`) | 15–16 | 2 passed | `67 0` |
| 67 (`test_iput.test_iput_with_checksums`) | 0–4 | 5 passed | `68 0` |
| 68 (`test_iput_options.Test_iPut_Options`) | 0–4 | 5 passed | `68 5` |
| 68 (`test_iput_options.Test_iPut_Options`) | 5–9 | 5 passed | `69 0` |
| 69 (`test_iput_options.Test_iPut_Options_Issue_3883`) | 0 | 1 passed | `70 0` |
| 70 (`test_ipwd`) | 0 | 1 passed | `71 0` |
| 71 (`test_iqmod`) | 0 | 1 passed | `72 0` |
| 72 (`test_iqstat`) | 0–1 | 2 passed | `73 0` |
| 73 (`test_iquery`) | 0–4 | 5 passed | `73 5` |
| 73 (`test_iquery`) | 5–9 | 5 passed | `73 10` |
| 73 (`test_iquery`) | 10–14 | 5 passed | `73 15` |
| 73 (`test_iquery`) | 15–19 | 5 passed | `73 20` |
| 73 (`test_iquery`) | 20–24 | 5 passed | `73 25` |
| 73 (`test_iquery`) | 25–26 | 2 passed | `74 0` |
| 74 (`test_iquest.Test_Iquest`) | 0–4 | 5 passed | `74 5` |
| 74 (`test_iquest.Test_Iquest`) | 5 | blocked by old `/tmp/issue_3714` | retry `74 5` |
| 74 (`test_iquest.Test_Iquest`) | 5–9 | 5 passed | `75 0` |
| 75 (`test_iquest.test_iquest_logical_or_operator_with_data_resc_hier`) | 0–4 | 5 passed | `75 5` |
| 75 (`test_iquest.test_iquest_logical_or_operator_with_data_resc_hier`) | 5–9 | 5 passed | `75 10` |
| 75 (`test_iquest.test_iquest_logical_or_operator_with_data_resc_hier`) | 10–14 | 5 passed | `75 15` |
| 75 (`test_iquest.test_iquest_logical_or_operator_with_data_resc_hier`) | 15–16 | 2 passed | `76 0` |
| 76 (`test_iquest.test_iquest_with_data_resc_hier`) | 0–4 | 5 passed | `77 0` |
| 77 (`test_ireg.Test_Ireg`) | 0–4 | 5 passed | `77 5` |
| 77 (`test_ireg.Test_Ireg`) | 5–9 | 5 passed | `77 10` |
| 77 (`test_ireg.Test_Ireg`) | 10 | 1 passed | `78 0` |
| 78 (`test_ireg.test_ireg_options`) | 0–2 | 3 passed | `79 0` |
| 79 (`test_ireg.test_ireg_replica`) | 0–4 | 5 passed | `79 5` |
| 79 (`test_ireg.test_ireg_replica`) | 5–7 | 3 passed | `80 0` |
| 80 (`test_irepl.Test_Irepl`) | 0–4 | 5 passed | `81 0` |
| 81 (`test_irepl.test_all_permission_levels__issue_7444_7465_7816`) | 0–4 | 5 passed | `81 5` |
| 81 (`test_irepl.test_all_permission_levels__issue_7444_7465_7816`) | 5–7 | 3 passed | `82 0` |
| 82 (`test_irepl.test_invalid_parameters`) | 0 | 1 passed | `83 0` |
| 83 (`test_irepl.test_irepl_repl_status`) | 0–4 | 5 passed | `83 5` |
| 83 (`test_irepl.test_irepl_repl_status`) | 5–9 | 5 passed | `83 10` |
| 83 (`test_irepl.test_irepl_repl_status`) | 10–13 | 4 passed | `84 0` |
| 84 (`test_irepl.test_irepl_replication_hierarchy`) | 0–1 | 2 passed; next method hit 1200s limit | retry `84 2` |
| 84 (`test_irepl.test_irepl_replication_hierarchy`) | 2 | 1 passed (440s) | `84 3` |
| 84 (`test_irepl.test_irepl_replication_hierarchy`) | 3 | 1 passed (441s) | `85 0` |
| 85 (`test_irepl.test_irepl_with_special_resource_configurations`) | 0–2 | 3 passed | `86 0` |
| 86 (`test_irepl.test_irepl_with_two_basic_ufs_resources`) | 0–3 | 4 passed | `87 0` |
| 87 (`test_irm`) | 0–4 | 5 passed | `87 5` |
| 87 (`test_irm`) | 5–8 | 4 passed | `88 0` |
| 88 (`test_irmdir`) | 0–4 | 5 passed | `88 5` |
| 88 (`test_irmdir`) | 5–7 | 3 passed | `89 0` |
| 89 (`test_irmtrash`) | 0 | 1 passed | `90 0` |
| 90 (`test_irsync`) | 0–4 | 5 passed | `90 5` |
| 90 (`test_irsync`) | 5–9 | 5 passed | `90 10` |
| 90 (`test_irsync`) | 10–14 | 5 passed | `90 15` |
| 90 (`test_irsync`) | 15–19 | 5 passed | `91 0` |
| 91 (`test_irule`) | 0–4 | 5 passed | `91 5` |
| 91 (`test_irule`) | 5–7 | 3 passed | `92 0` |
| 92 (`test_iscan`) | 0–4 | 5 passed | `92 5` |
| 92 (`test_iscan`) | 5 | 1 passed | `93 0` |
| 93 (`test_istream`) | 0–4 | 5 passed | `93 5` |
| 93 (`test_istream`) | 5–9 | 5 passed | `93 10` |
| 93 (`test_istream`) | 10–14 | 5 passed | `93 15` |
| 93 (`test_istream`) | 15–17 | 3 passed | `94 0` |
| 94 (`test_isysmeta`) | 0–3 | 4 passed | `95 0` |
| 95 (`test_iticket`) | 0–4 | 5 passed | `95 5` |
| 95 (`test_iticket`) | 5–9 | 5 passed | `95 10` |
| 95 (`test_iticket`) | 10–14 | 5 passed | `95 15` |
| 95 (`test_iticket`) | 15–19 | 5 passed | `95 20` |
| 95 (`test_iticket`) | 20–24 | 5 passed | `95 25` |
| 95 (`test_iticket`) | 25 | 1 passed | `96 0` |
| 96 (`test_itouch`) | 0–4 | 5 passed | `96 5` |
| 96 (`test_itouch`) | 5–9 | 5 passed | `96 10` |
| 96 (`test_itouch`) | 10–12 | 3 passed | `97 0` |
| 97 (`test_itree`) | 0–4 | 5 passed | `97 5` |
| 97 (`test_itree`) | 5–9 | 5 passed | `97 10` |
| 97 (`test_itree`) | 10 | 1 passed | `98 0` |
| 98 (`test_itrim.Test_Itrim`) | 0–4 | 5 passed | `98 5` |
| 98 (`test_itrim.Test_Itrim`) | 5–7 | 3 passed | `99 0` |
| 99 (`test_itrim.test_itrim_target_replica_selection_decision_making__issue_7515`) | 0–4 | 5 passed | `99 5` |
| 99 (`test_itrim.test_itrim_target_replica_selection_decision_making__issue_7515`) | 5–9 | 5 passed | `99 10` |
| 99 (`test_itrim.test_itrim_target_replica_selection_decision_making__issue_7515`) | 10–14 | 5 passed | `99 15` |
| 99 (`test_itrim.test_itrim_target_replica_selection_decision_making__issue_7515`) | 15 | 1 passed | `100 0` |
| 100 (`test_iunreg`) | 0–4 | 5 passed | `100 5` |
| 100 (`test_iunreg`) | 5–6 | 2 passed | `101 0` |
| 101 (`test_iuserinfo`) | 0–4 | 5 passed | `101 5` |
| 101 (`test_iuserinfo`) | 5–6 | 2 passed | `102 0` |
| 102 (`test_izonereport`) | 0–4 | 5 passed | `102 5` |
| 102 (`test_izonereport`) | 5–7 | 3 passed | `103 0` |
| 103 (`test_load_balanced_suite`) | 0 | 1 passed | `104 0` |
| 104 (`test_logical_quotas`) | 0 | failed: `calculate_logical_usage` returned `SYS_INTERNAL_ERR` | `104 1` |
| 104 (`test_logical_quotas`) | 1 | failed: same SQL bind error | `104 2` |
| 104 (`test_logical_quotas`) | 2–6 | 4 passed, 1 failed: same SQL bind error | `104 7` |
| 104 (`test_logical_quotas`) | 7–11 | 1 passed, 4 failed; ASan report | retry after fix |
| 104 (`test_logical_quotas`) | 0–4 | 5 passed after fix | `104 5` |
| 104 (`test_logical_quotas`) | 5–9 | 5 passed after fix | `104 10` |
| 104 (`test_logical_quotas`) | 10–14 | 5 passed after cleaning old test resources | `104 15` |
| 104 (`test_logical_quotas`) | 15–19 | 5 passed | `105 0` |

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

Index 28 reproduces the `ibun -x`/`SYS_INTERNAL_ERR` failure in another test
(`test_ibun_x_tarfile_on_remote_archive_resource`) after an archive replica
is trimmed. The scoped UBSan logs contain only the generic API-wrapper
report and no ASan log was created. The other six `test_ibun` cases either
passed or skipped. This appears related to the failure at `13 7`; continue
at `29 0` and investigate the extraction path separately.

Index 65 requires the existing `irods_test_issue_8733` client program, absent
from the earlier build with `IRODS_ENABLE_ALL_TESTS=OFF`. Reconfigured the
sanitizer build with `-DIRODS_ENABLE_ALL_TESTS=ON` (keeping
`IRODS_GIT_SHA1_TO_REPORT=d4e463c61`), built/installed the server package
including the instrumented helper and test microservice plugins, and reran
`test_ips`: 1 passed. No test source was modified.

Index 104 initially failed at `iadmin calculate_logical_usage` with
`SYS_INTERNAL_ERR` after enabling quotas and putting two data objects. No
new project-code UBSan report was generated in that batch. Continuing with
`--continue` exposed the actual ASan stack-use-after-scope on subsequent
quota tests. `PATH_SEPARATOR` expands to `get_virtual_path_separator().c_str()`
and is stored in the SQL bind-variable array; the temporary string is gone
by the time the SQL statement reads the pointer. Keep the string alive in
`db_calc_logical_usage_and_quota_op()` for both database branches. Rebuild,
install the PostgreSQL plugin and rerun before committing this finding.

Rebuilt and installed the sanitized PostgreSQL database plugin. The first
ten quota tests passed. Prior aborted runs had left three test-specific
resources in the catalog; after removing them, all remaining quota tests
passed. All 20 tests in `test_logical_quotas` pass on the fixed build, and
the new scoped logs contain no stack-use-after-scope report. Continue at
`105 0`.

Log correction: With both runtimes linked, ASan reports can be appended to
the `UBSAN_OPTIONS=log_path=...` files rather than producing separate files
using the ASan prefix. The earlier statements that no ASan log was created
for failed batches 13, 15 and 28 did not establish the absence of ASan
failures. Reinspection found stack-use-after-scope in `structfile.cpp:461`
for both tar-extraction failures (13 and 28), and stack-use-after-return in
`irodsAgent` for the resource parent-context failure (15). Investigate and
commit these as separate findings.

The tar-extraction bug is `const auto& first_component =
*relative_entry_path.begin()`: the iterator yields a temporary path component
whose lifetime does not extend to the later `first_component.string()` call.
Copied the component by value, rebuilt and installed the sanitized server,
then reran both reproductions: five `test_dynamic_peps` methods including
the formerly failing extraction and both final `test_ibun` cases pass.
Neither scoped log prefix (`irods_batch_13_7_2581390` or
`irods_batch_28_5_2582329`) contains a new sanitizer report.

The parent-context test's old ASan stack-use-after-return is in logging:
`set_request_client_version()` saved a pointer into the request handler's
stack-allocated `rsComm`. An exception unwound that frame, then the agent's
outer catch logged the error and dereferenced the stale version pointer.
Store a copy of `Version` in the logger instead. Rebuilt and installed the
runtime and server packages and reran the reproducing iadmin case. The
stack-use-after-return is absent from `/tmp/irods_parent_verify_ubsan.*`;
the test still fails because the now-working outer exception logger reveals
another, independent issue: `basic_string: construction from null is not
valid`. In `rsGeneralAdmin.cpp`, the `parent_context` branch validates the
new value but fails to assign `args[2]` before calling `applyRuleArg()`.
Fix this in a separate commit.

`rsGeneralAdmin.cpp` now supplies `generalAdminInp->arg4` as the validated
`parent_context` argument before invoking the rule. Rebuilt and installed
the sanitized runtime and server packages. Five iadmin cases including the
previously failing parent-context update pass, with no new sanitizer report
(`irods_batch_15_34_2586654`).

Index 74 initially failed when `test_iquest_resc_hier_with_like__3714`
encountered an empty, iRODS-owned `/tmp/issue_3714` directory left from an
earlier run. Removed that empty temporary directory (no test edit); all five
cases in the retried batch passed. Continue at `75 0`.

Index 84: the first two replication-hierarchy tests passed, but the third
(`test_irepl_Sf_Rd_foo`) exceeded the coordinator's 1,200-second per-batch
limit. Its scoped logs contain only the known API-wrapper report. Run this
large scenario on its own with a longer limit after resetting any catalog
state left by the interrupted test.

Reset the catalog using the local PostgreSQL setup input. The two remaining
replication-hierarchy methods each passed when run separately (~440 seconds
per method). Their scoped logs showed only the known API-wrapper UBSan report;
no ASan report was found. Continue at `85 0`.

At `15 45`, the replication resource rebalance tests trigger UBSan at
`irods_repl_retry.cpp:57`: the post-decrement in the retry-loop condition
wraps an unsigned zero counter. Move the decrement inside the loop after
checking for a positive count. Rebuilt and installed the ASan/UBSan server
package; the five reproducing tests pass, and the scoped logs at
`/tmp/irods_batch_15_45_2320023_ubsan.*` no longer contain this report.
Continue at `15 50`.
