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

UBSan found mismatched calls through the generic API wrapper function pointer
at `server/core/src/rsApiHandler.cpp:209,215,222`. The typed-dispatch change
described at the end of this document addresses that finding. The
rule-language generic traversal finding at `restruct.templates.hpp:75` was
also corrected and verified during the full-suite follow-up below.

There are repeated inlined libstdc++ unsigned-wrap reports from GCC 14
headers, outside this checkout. No ASan memory-error report was observed in
the initial targeted paths; later full-suite paths exposed ASan issues that
were fixed below. Leak detection was disabled for the short-lived test
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
| 105 (`test_misc.Test_Misc`) | 0–4 | 5 passed | `105 5` |
| 105 (`test_misc.Test_Misc`) | 5–9 | 5 passed; UBSan invalid enum | `105 10` |
| 105 (`test_misc.Test_Misc`) | 10–14 | 5 passed | `105 15` |
| 105 (`test_misc.Test_Misc`) | 15–18 | 3 passed, teardown failure in server lifecycle test | `105 19` |
| 105 (`test_misc.Test_Misc`) | 19 | failed: missing hostname alias | retry `105 19` |
| 105 (`test_misc.Test_Misc`) | 19 | 1 passed after hostname alias | `105 20` |
| 105 (`test_misc.Test_Misc`) | 20–24 | 5 passed | `105 25` |
| 105 (`test_misc.Test_Misc`) | 25–27 | 3 passed | `106 0` |
| 106 (`test_misc.test_server_side_libraries`) | 0 | 1 passed | `107 0` |
| 107 (`test_native_authentication.test_configurations`) | 0–4 | 5 passed | `107 5` |
| 107 (`test_native_authentication.test_configurations`) | 5–6 | 2 passed | `108 0` |
| 108 (`test_native_rule_engine_plugin`) | 0–4 | 5 passed | `108 5` |
| 108 (`test_native_rule_engine_plugin`) | 5–9 | 5 passed | `108 10` |
| 108 (`test_native_rule_engine_plugin`) | 10–14 | 5 passed | `108 15` |
| 108 (`test_native_rule_engine_plugin`) | 15–19 | 5 passed | `109 0` |
| 109 (`test_negotiation`) | 0–4 | 5 passed | `109 5` |
| 109 (`test_negotiation`) | 5–9 | 5 passed | `109 10` |
| 109 (`test_negotiation`) | 10 | 1 passed | `110 0` |
| 110 (`test_pam_password_authentication.test_configurations`) | 0–3 | 3 passed, expiration test failed | retry `110 3` |
| 110 (`test_pam_password_authentication.test_configurations`) | 3 | failed again; no new sanitizer report | `110 4` |
| 110 (`test_pam_password_authentication.test_configurations`) | 4–7 | 3 passed, reuse-expiration test failed | `110 8` |
| 110 (`test_pam_password_authentication.test_configurations`) | 8 | 1 passed | `111 0` |
| 111 (`test_prep_genquery_iterator`) | 0–4 | 5 passed | `111 5` |
| 111 (`test_prep_genquery_iterator`) | 5–9 | 5 passed | `111 10` |
| 111 (`test_prep_genquery_iterator`) | 10–14 | 5 passed | `111 15` |
| 111 (`test_prep_genquery_iterator`) | 15–19 | 5 passed | `112 0` |
| 112 (`test_python_rule_engine_plugin`) | 0–4 | 5 passed | `113 0` |
| 113 (`test_quotas`) | 0–4 | 5 passed | `113 5` |
| 113 (`test_quotas`) | 5–8 | 4 passed | `114 0` |
| 114 (`test_resource_configuration`) | 0 | 1 passed | `115 0` |
| 115 (`test_resource_tree`) | 0 | 1 passed; second method exceeded 1200s batch limit | `115 1` |
| 115 (`test_resource_tree`) | 1 | 1 passed alone (692s) | `116 0` |
| 116 (`test_resource_types.Test_Resource_Compound`) | 0–4 | 5 passed | `116 5` |
| 116 (`test_resource_types.Test_Resource_Compound`) | 5–9 | 5 passed | `116 10` |
| 116 (`test_resource_types.Test_Resource_Compound`) | 10–14 | 5 passed | `116 15` |
| 116 (`test_resource_types.Test_Resource_Compound`) | 15–19 | 5 passed | `116 20` |
| 116 (`test_resource_types.Test_Resource_Compound`) | 20–24 | 5 passed; UBSan callback mismatch | `116 25` |
| 116 (`test_resource_types.Test_Resource_Compound`) | 25–29 | 5 passed | `116 30` |
| 116 (`test_resource_types.Test_Resource_Compound`) | 30–34 | 5 passed | `116 35` |
| 116 (`test_resource_types.Test_Resource_Compound`) | 35–39 | 5 passed | `116 40` |
| 116 (`test_resource_types.Test_Resource_Compound`) | 40–44 | 5 passed | `116 45` |
| 116 (`test_resource_types.Test_Resource_Compound`) | 45–49 | 5 passed | `116 50` |
| 116 (`test_resource_types.Test_Resource_Compound`) | 50–53 | 4 run, 2 failures; ASan report | retry `116 50` |
| 116 (`test_resource_types.Test_Resource_Compound`) | 50–54 | 5 passed after fixes | `116 55` |
| 116 (`test_resource_types.Test_Resource_Compound`) | 55–59 | 5 passed | `116 60` |
| 116 (`test_resource_types.Test_Resource_Compound`) | 60–64 | 5 passed | `116 65` |
| 116 (`test_resource_types.Test_Resource_Compound`) | 65–69 | 5 passed | `116 70` |
| 116 (`test_resource_types.Test_Resource_Compound`) | 70–74 | 5 passed | `116 75` |
| 116 (`test_resource_types.Test_Resource_Compound`) | 75–79 | 5 passed | `116 80` |
| 116 (`test_resource_types.Test_Resource_Compound`) | 80–84 | 5 passed | `116 85` |
| 116 (`test_resource_types.Test_Resource_Compound`) | 85–86 | 2 failed; ASan overflow | retry `116 85` |
| 116 (`test_resource_types.Test_Resource_Compound`) | 85–86 | 2 passed after fix | `117 0` |
| 117 (`test_resource_types.Test_Resource_CompoundWithMockarchive`) | 0–4 | 5 passed | `117 5` |
| 117 (`test_resource_types.Test_Resource_CompoundWithMockarchive`) | 5–9 | 5 passed | `117 10` |
| 117 (`test_resource_types.Test_Resource_CompoundWithMockarchive`) | 10–14 | 5 passed | `117 15` |
| 117 (`test_resource_types.Test_Resource_CompoundWithMockarchive`) | 15–19 | 5 passed | `117 20` |
| 117 (`test_resource_types.Test_Resource_CompoundWithMockarchive`) | 20–24 | 5 passed | `117 25` |
| 117 (`test_resource_types.Test_Resource_CompoundWithMockarchive`) | 25–29 | 5 passed | `117 30` |
| 117 (`test_resource_types.Test_Resource_CompoundWithMockarchive`) | 30–34 | 5 passed | `117 35` |
| 117 (`test_resource_types.Test_Resource_CompoundWithMockarchive`) | 35–39 | 5 passed | `117 40` |
| 117 (`test_resource_types.Test_Resource_CompoundWithMockarchive`) | 40–44 | 5 passed | `117 45` |
| 117 (`test_resource_types.Test_Resource_CompoundWithMockarchive`) | 45–49 | 5 passed | `117 50` |
| 117 (`test_resource_types.Test_Resource_CompoundWithMockarchive`) | 50–54 | 5 passed | `117 55` |
| 117 (`test_resource_types.Test_Resource_CompoundWithMockarchive`) | 55–59 | 5 passed | `117 60` |
| 117 (`test_resource_types.Test_Resource_CompoundWithMockarchive`) | 60–64 | 5 passed | `117 65` |
| 117 (`test_resource_types.Test_Resource_CompoundWithMockarchive`) | 65–66 | 2 passed | `118 0` |
| 118 (`test_resource_types.Test_Resource_CompoundWithUnivmss`) | 0–4 | 5 passed | `118 5` |
| 118 (`test_resource_types.Test_Resource_CompoundWithUnivmss`) | 5–9 | 5 passed | `118 10` |
| 118 (`test_resource_types.Test_Resource_CompoundWithUnivmss`) | 10–14 | 5 passed | `118 15` |
| 118 (`test_resource_types.Test_Resource_CompoundWithUnivmss`) | 15–19 | 5 passed | `118 20` |
| 118 (`test_resource_types.Test_Resource_CompoundWithUnivmss`) | 20–24 | 5 passed | `118 25` |
| 118 (`test_resource_types.Test_Resource_CompoundWithUnivmss`) | 25–29 | 5 passed | `118 30` |
| 118 (`test_resource_types.Test_Resource_CompoundWithUnivmss`) | 30–34 | 5 passed | `118 35` |
| 118 (`test_resource_types.Test_Resource_CompoundWithUnivmss`) | 35–39 | 5 passed | `118 40` |
| 118 (`test_resource_types.Test_Resource_CompoundWithUnivmss`) | 40–44 | 5 passed | `118 45` |
| 118 (`test_resource_types.Test_Resource_CompoundWithUnivmss`) | 45–49 | 5 passed | `118 50` |
| 118 (`test_resource_types.Test_Resource_CompoundWithUnivmss`) | 50–54 | 5 passed | `118 55` |
| 118 (`test_resource_types.Test_Resource_CompoundWithUnivmss`) | 55–59 | 5 passed | `118 60` |
| 118 (`test_resource_types.Test_Resource_CompoundWithUnivmss`) | 60–64 | 5 passed | `118 65` |
| 118 (`test_resource_types.Test_Resource_CompoundWithUnivmss`) | 65–69 | 5 passed | `119 0` |
| 119 (`test_resource_types.Test_Resource_Deferred`) | 0–4 | 5 passed | `119 5` |
| 119 (`test_resource_types.Test_Resource_Deferred`) | 5–9 | 5 passed | `119 10` |
| 119 (`test_resource_types.Test_Resource_Deferred`) | 10–14 | 5 passed | `119 15` |
| 119 (`test_resource_types.Test_Resource_Deferred`) | 15–19 | 5 passed | `119 20` |
| 119 (`test_resource_types.Test_Resource_Deferred`) | 20–24 | 5 passed | `119 25` |
| 119 (`test_resource_types.Test_Resource_Deferred`) | 25–29 | 5 passed | `119 30` |
| 119 (`test_resource_types.Test_Resource_Deferred`) | 30–34 | 5 passed | `119 35` |
| 119 (`test_resource_types.Test_Resource_Deferred`) | 35–39 | 5 passed | `119 40` |
| 119 (`test_resource_types.Test_Resource_Deferred`) | 40–44 | 5 passed | `119 45` |
| 119 (`test_resource_types.Test_Resource_Deferred`) | 45–49 | 5 passed | `119 50` |
| 119 (`test_resource_types.Test_Resource_Deferred`) | 50–54 | 5 passed | `119 55` |
| 119 (`test_resource_types.Test_Resource_Deferred`) | 55–59 | 5 passed | `119 60` |
| 119 (`test_resource_types.Test_Resource_Deferred`) | 60–64 | 5 passed | `120 0` |
| 120 (`test_resource_types.Test_Resource_MultiLayered`) | 0–4 | 5 passed | `120 5` |
| 120 (`test_resource_types.Test_Resource_MultiLayered`) | 5–9 | 5 passed | `120 10` |
| 120 (`test_resource_types.Test_Resource_MultiLayered`) | 10–14 | 5 passed | `120 15` |
| 120 (`test_resource_types.Test_Resource_MultiLayered`) | 15–19 | 5 passed | `120 20` |
| 120 (`test_resource_types.Test_Resource_MultiLayered`) | 20–24 | 5 passed | `120 25` |
| 120 (`test_resource_types.Test_Resource_MultiLayered`) | 25–29 | 5 passed | `120 30` |
| 120 (`test_resource_types.Test_Resource_MultiLayered`) | 30–34 | 5 passed | `120 35` |
| 120 (`test_resource_types.Test_Resource_MultiLayered`) | 35–39 | 5 passed | `120 40` |
| 120 (`test_resource_types.Test_Resource_MultiLayered`) | 40–44 | 5 passed | `120 45` |
| 120 (`test_resource_types.Test_Resource_MultiLayered`) | 45–49 | 5 passed | `120 50` |
| 120 (`test_resource_types.Test_Resource_MultiLayered`) | 50–54 | 5 passed | `120 55` |
| 120 (`test_resource_types.Test_Resource_MultiLayered`) | 55–59 | 5 passed | `120 60` |
| 120 (`test_resource_types.Test_Resource_MultiLayered`) | 60–64 | 5 passed | `121 0` |
| 121 (`test_resource_types.Test_Resource_NonBlocking`) | 0–4 | 5 passed | `121 5` |
| 121 (`test_resource_types.Test_Resource_NonBlocking`) | 5–9 | 5 passed | `121 10` |
| 121 (`test_resource_types.Test_Resource_NonBlocking`) | 10–14 | 5 passed | `121 15` |
| 121 (`test_resource_types.Test_Resource_NonBlocking`) | 15–19 | 5 passed | `121 20` |
| 121 (`test_resource_types.Test_Resource_NonBlocking`) | 20–24 | 5 passed | `121 25` |
| 121 (`test_resource_types.Test_Resource_NonBlocking`) | 25–29 | 5 passed | `121 30` |
| 121 (`test_resource_types.Test_Resource_NonBlocking`) | 30–34 | 5 passed | `121 35` |
| 121 (`test_resource_types.Test_Resource_NonBlocking`) | 35–39 | 5 passed | `121 40` |
| 121 (`test_resource_types.Test_Resource_NonBlocking`) | 40–44 | 5 passed | `121 45` |
| 121 (`test_resource_types.Test_Resource_NonBlocking`) | 45–49 | 5 passed | `121 50` |
| 121 (`test_resource_types.Test_Resource_NonBlocking`) | 50–54 | 5 passed | `121 55` |
| 121 (`test_resource_types.Test_Resource_NonBlocking`) | 55–59 | 5 passed | `121 60` |
| 121 (`test_resource_types.Test_Resource_NonBlocking`) | 60–64 | 5 passed | `122 0` |
| 122 (`test_resource_types.Test_Resource_Passthru`) | 0–4 | 5 passed | `122 5` |
| 122 (`test_resource_types.Test_Resource_Passthru`) | 5–9 | 5 passed | `122 10` |
| 122 (`test_resource_types.Test_Resource_Passthru`) | 10–14 | 5 passed | `122 15` |
| 122 (`test_resource_types.Test_Resource_Passthru`) | 15–19 | 5 passed | `122 20` |
| 122 (`test_resource_types.Test_Resource_Passthru`) | 20–24 | 5 passed | `122 25` |
| 122 (`test_resource_types.Test_Resource_Passthru`) | 25–29 | 5 passed | `122 30` |
| 122 (`test_resource_types.Test_Resource_Passthru`) | 30–34 | 5 passed | `122 35` |
| 122 (`test_resource_types.Test_Resource_Passthru`) | 35–39 | 5 passed | `122 40` |
| 122 (`test_resource_types.Test_Resource_Passthru`) | 40–44 | 5 passed | `122 45` |
| 122 (`test_resource_types.Test_Resource_Passthru`) | 45–49 | 5 passed | `122 50` |
| 122 (`test_resource_types.Test_Resource_Passthru`) | 50–54 | 5 passed | `122 55` |
| 122 (`test_resource_types.Test_Resource_Passthru`) | 55–59 | 5 passed | `122 60` |
| 122 (`test_resource_types.Test_Resource_Passthru`) | 60–64 | 5 passed | `123 0` |
| 123 (`test_resource_types.Test_Resource_Random`) | 0–4 | 5 passed | `123 5` |
| 123 (`test_resource_types.Test_Resource_Random`) | 5–9 | 5 passed | `123 10` |
| 123 (`test_resource_types.Test_Resource_Random`) | 10–14 | 5 passed | `123 15` |
| 123 (`test_resource_types.Test_Resource_Random`) | 15–19 | 5 passed | `123 20` |
| 123 (`test_resource_types.Test_Resource_Random`) | 20–24 | 5 passed | `123 25` |
| 123 (`test_resource_types.Test_Resource_Random`) | 25–29 | 5 passed | `123 30` |
| 123 (`test_resource_types.Test_Resource_Random`) | 30–34 | 5 passed | `123 35` |
| 123 (`test_resource_types.Test_Resource_Random`) | 35–39 | 5 passed | `123 40` |
| 123 (`test_resource_types.Test_Resource_Random`) | 40–44 | 5 passed | `123 45` |
| 123 (`test_resource_types.Test_Resource_Random`) | 45–49 | 5 passed | `123 50` |
| 123 (`test_resource_types.Test_Resource_Random`) | 50–54 | 5 passed | `123 55` |
| 123 (`test_resource_types.Test_Resource_Random`) | 55–59 | 5 passed | `123 60` |
| 123 (`test_resource_types.Test_Resource_Random`) | 60–64 | 5 passed | `123 65` |
| 123 (`test_resource_types.Test_Resource_Random`) | 65 | 1 passed | `124 0` |
| 124 (`test_resource_types.Test_Resource_RandomWithinRandom`) | 0–4 | 5 passed | `124 5` |
| 124 (`test_resource_types.Test_Resource_RandomWithinRandom`) | 5–9 | 5 passed | `124 10` |
| 124 (`test_resource_types.Test_Resource_RandomWithinRandom`) | 10–14 | 5 passed | `124 15` |
| 124 (`test_resource_types.Test_Resource_RandomWithinRandom`) | 15–19 | 5 passed | `124 20` |
| 124 (`test_resource_types.Test_Resource_RandomWithinRandom`) | 20–24 | 5 passed | `124 25` |
| 124 (`test_resource_types.Test_Resource_RandomWithinRandom`) | 25–29 | 5 passed | `124 30` |
| 124 (`test_resource_types.Test_Resource_RandomWithinRandom`) | 30–34 | 5 passed | `124 35` |
| 124 (`test_resource_types.Test_Resource_RandomWithinRandom`) | 35–39 | 5 passed | `124 40` |
| 124 (`test_resource_types.Test_Resource_RandomWithinRandom`) | 40–44 | 5 passed | `124 45` |
| 124 (`test_resource_types.Test_Resource_RandomWithinRandom`) | 45–49 | 5 passed | `124 50` |
| 124 (`test_resource_types.Test_Resource_RandomWithinRandom`) | 50–54 | 5 passed | `124 55` |
| 124 (`test_resource_types.Test_Resource_RandomWithinRandom`) | 55–59 | 5 passed | `124 60` |
| 124 (`test_resource_types.Test_Resource_RandomWithinRandom`) | 60–64 | 5 passed | `124 65` |
| 124 (`test_resource_types.Test_Resource_RandomWithinRandom`) | 65 | 1 passed | `125 0` |
| 125 (`test_resource_types.Test_Resource_RandomWithinReplication`) | 0–4 | 5 passed | `125 5` |
| 125 (`test_resource_types.Test_Resource_RandomWithinReplication`) | 5–9 | 5 passed | `125 10` |
| 125 (`test_resource_types.Test_Resource_RandomWithinReplication`) | 10–14 | 5 passed | `125 15` |
| 125 (`test_resource_types.Test_Resource_RandomWithinReplication`) | 15–19 | 5 passed | `125 20` |
| 125 (`test_resource_types.Test_Resource_RandomWithinReplication`) | 20–24 | 5 passed | `125 25` |
| 125 (`test_resource_types.Test_Resource_RandomWithinReplication`) | 25–29 | 5 passed | `125 30` |
| 125 (`test_resource_types.Test_Resource_RandomWithinReplication`) | 30–34 | 5 passed | `125 35` |
| 125 (`test_resource_types.Test_Resource_RandomWithinReplication`) | 35–39 | 5 passed | `125 40` |
| 125 (`test_resource_types.Test_Resource_RandomWithinReplication`) | 40–44 | 5 passed | `125 45` |
| 125 (`test_resource_types.Test_Resource_RandomWithinReplication`) | 45–49 | 5 passed | `125 50` |
| 125 (`test_resource_types.Test_Resource_RandomWithinReplication`) | 50–54 | 5 passed | `125 55` |
| 125 (`test_resource_types.Test_Resource_RandomWithinReplication`) | 55–59 | 5 passed | `125 60` |
| 125 (`test_resource_types.Test_Resource_RandomWithinReplication`) | 60–64 | 5 passed | `125 65` |
| 125 (`test_resource_types.Test_Resource_RandomWithinReplication`) | 65–67 | 3 passed | `126 0` |
| 126 (`test_resource_types.Test_Resource_Replication`) | 0–4 | 5 passed | `126 5` |
| 126 (`test_resource_types.Test_Resource_Replication`) | 5–9 | 5 passed | `126 10` |
| 126 (`test_resource_types.Test_Resource_Replication`) | 10–14 | 5 passed | `126 15` |
| 126 (`test_resource_types.Test_Resource_Replication`) | 15–19 | 5 passed | `126 20` |
| 126 (`test_resource_types.Test_Resource_Replication`) | 20–24 | 5 passed | `126 25` |
| 126 (`test_resource_types.Test_Resource_Replication`) | 25–29 | 5 passed | `126 30` |
| 126 (`test_resource_types.Test_Resource_Replication`) | 30–34 | 5 passed | `126 35` |
| 126 (`test_resource_types.Test_Resource_Replication`) | 35–39 | 5 passed | `126 40` |
| 126 (`test_resource_types.Test_Resource_Replication`) | 40–44 | 5 passed | `126 45` |
| 126 (`test_resource_types.Test_Resource_Replication`) | 45–49 | 5 passed | `126 50` |
| 126 (`test_resource_types.Test_Resource_Replication`) | 50–54 | 5 passed | `126 55` |
| 126 (`test_resource_types.Test_Resource_Replication`) | 55–59 | 5 passed | `126 60` |
| 126 (`test_resource_types.Test_Resource_Replication`) | 60–64 | 5 passed | `126 65` |
| 126 (`test_resource_types.Test_Resource_Replication`) | 65–69 | 5 passed | `126 70` |
| 126 (`test_resource_types.Test_Resource_Replication`) | 70–72 | 3 passed; fourth exceeded 1200s batch limit | `126 73` |
| 126 (`test_resource_types.Test_Resource_Replication`) | 73 | 1 passed alone (1142s) | `126 74` |
| 126 (`test_resource_types.Test_Resource_Replication`) | 74 | 1 passed | `126 75` |
| 126 (`test_resource_types.Test_Resource_Replication`) | 75–79 | 5 passed | `127 0` |
| 127 (`test_resource_types.Test_Resource_ReplicationToTwoCompound`) | 0–4 | 5 passed | `127 5` |
| 127 (`test_resource_types.Test_Resource_ReplicationToTwoCompound`) | 5–9 | 5 passed | `127 10` |
| 127 (`test_resource_types.Test_Resource_ReplicationToTwoCompound`) | 10–14 | 5 passed | `127 15` |
| 127 (`test_resource_types.Test_Resource_ReplicationToTwoCompound`) | 15–19 | 5 passed | `127 20` |
| 127 (`test_resource_types.Test_Resource_ReplicationToTwoCompound`) | 20–24 | 5 passed | `127 25` |
| 127 (`test_resource_types.Test_Resource_ReplicationToTwoCompound`) | 25–29 | 5 passed | `127 30` |
| 127 (`test_resource_types.Test_Resource_ReplicationToTwoCompound`) | 30–34 | 5 passed | `127 35` |
| 127 (`test_resource_types.Test_Resource_ReplicationToTwoCompound`) | 35–39 | 5 passed | `127 40` |
| 127 (`test_resource_types.Test_Resource_ReplicationToTwoCompound`) | 40–44 | 5 passed | `127 45` |
| 127 (`test_resource_types.Test_Resource_ReplicationToTwoCompound`) | 45–49 | 5 passed | `127 50` |
| 127 (`test_resource_types.Test_Resource_ReplicationToTwoCompound`) | 50–54 | 5 passed | `127 55` |
| 127 (`test_resource_types.Test_Resource_ReplicationToTwoCompound`) | 55–59 | 5 passed | `127 60` |
| 127 (`test_resource_types.Test_Resource_ReplicationToTwoCompound`) | 60–64 | 5 passed | `127 65` |
| 127 (`test_resource_types.Test_Resource_ReplicationToTwoCompound`) | 65–66 | 2 passed | `128 0` |
| 128 (`test_resource_types.Test_Resource_ReplicationToTwoCompoundResourcesWithPreferArchive`) | 0–4 | 5 passed | `128 5` |
| 128 (`test_resource_types.Test_Resource_ReplicationToTwoCompoundResourcesWithPreferArchive`) | 5–9 | 5 passed | `128 10` |
| 128 (`test_resource_types.Test_Resource_ReplicationToTwoCompoundResourcesWithPreferArchive`) | 10–14 | 5 passed | `128 15` |
| 128 (`test_resource_types.Test_Resource_ReplicationToTwoCompoundResourcesWithPreferArchive`) | 15–19 | 5 passed | `128 20` |
| 128 (`test_resource_types.Test_Resource_ReplicationToTwoCompoundResourcesWithPreferArchive`) | 20–24 | 5 passed | `128 25` |
| 128 (`test_resource_types.Test_Resource_ReplicationToTwoCompoundResourcesWithPreferArchive`) | 25–29 | 5 passed | `128 30` |
| 128 (`test_resource_types.Test_Resource_ReplicationToTwoCompoundResourcesWithPreferArchive`) | 30–34 | 5 passed | `128 35` |
| 128 (`test_resource_types.Test_Resource_ReplicationToTwoCompoundResourcesWithPreferArchive`) | 35–39 | 5 passed | `128 40` |
| 128 (`test_resource_types.Test_Resource_ReplicationToTwoCompoundResourcesWithPreferArchive`) | 40–44 | 5 passed | `128 45` |
| 128 (`test_resource_types.Test_Resource_ReplicationToTwoCompoundResourcesWithPreferArchive`) | 45–49 | 5 passed | `128 50` |
| 128 (`test_resource_types.Test_Resource_ReplicationToTwoCompoundResourcesWithPreferArchive`) | 50–54 | 5 passed | `128 55` |
| 128 (`test_resource_types.Test_Resource_ReplicationToTwoCompoundResourcesWithPreferArchive`) | 55–59 | 5 passed | `128 60` |
| 128 (`test_resource_types.Test_Resource_ReplicationToTwoCompoundResourcesWithPreferArchive`) | 60–64 | 5 passed | `128 65` |
| 128 (`test_resource_types.Test_Resource_ReplicationToTwoCompoundResourcesWithPreferArchive`) | 65–66 | 2 passed | `129 0` |
| 129 (`test_resource_types.Test_Resource_ReplicationWithinReplication`) | 0–4 | 5 passed | `129 5` |
| 129 (`test_resource_types.Test_Resource_ReplicationWithinReplication`) | 5–9 | 5 passed | `129 10` |
| 129 (`test_resource_types.Test_Resource_ReplicationWithinReplication`) | 10–14 | 5 passed | `129 15` |
| 129 (`test_resource_types.Test_Resource_ReplicationWithinReplication`) | 15–19 | 5 passed | `129 20` |
| 129 (`test_resource_types.Test_Resource_ReplicationWithinReplication`) | 20–24 | 5 passed | `129 25` |
| 129 (`test_resource_types.Test_Resource_ReplicationWithinReplication`) | 25–29 | 5 passed | `129 30` |
| 129 (`test_resource_types.Test_Resource_ReplicationWithinReplication`) | 30–34 | 5 passed | `129 35` |
| 129 (`test_resource_types.Test_Resource_ReplicationWithinReplication`) | 35–39 | 5 passed | `129 40` |
| 129 (`test_resource_types.Test_Resource_ReplicationWithinReplication`) | 40–44 | 5 passed | `129 45` |
| 129 (`test_resource_types.Test_Resource_ReplicationWithinReplication`) | 45–49 | 5 passed | `129 50` |
| 129 (`test_resource_types.Test_Resource_ReplicationWithinReplication`) | 50–54 | 5 passed | `129 55` |
| 129 (`test_resource_types.Test_Resource_ReplicationWithinReplication`) | 55–59 | 5 passed | `129 60` |
| 129 (`test_resource_types.Test_Resource_ReplicationWithinReplication`) | 60–64 | 5 passed | `130 0` |
| 130 (`test_resource_types.Test_Resource_Unixfilesystem`) | 0–4 | 5 passed | `130 5` |
| 130 (`test_resource_types.Test_Resource_Unixfilesystem`) | 5–9 | 5 passed | `130 10` |
| 130 (`test_resource_types.Test_Resource_Unixfilesystem`) | 10–14 | 5 passed | `130 15` |
| 130 (`test_resource_types.Test_Resource_Unixfilesystem`) | 15–19 | 5 passed | `130 20` |
| 130 (`test_resource_types.Test_Resource_Unixfilesystem`) | 20–24 | 5 passed | `130 25` |
| 130 (`test_resource_types.Test_Resource_Unixfilesystem`) | 25–29 | 5 passed | `130 30` |
| 130 (`test_resource_types.Test_Resource_Unixfilesystem`) | 30–34 | 5 passed | `130 35` |
| 130 (`test_resource_types.Test_Resource_Unixfilesystem`) | 35–39 | 5 passed | `130 40` |
| 130 (`test_resource_types.Test_Resource_Unixfilesystem`) | 40–44 | 5 passed | `130 45` |
| 130 (`test_resource_types.Test_Resource_Unixfilesystem`) | 45–49 | 5 passed | `130 50` |
| 130 (`test_resource_types.Test_Resource_Unixfilesystem`) | 50–54 | 5 passed | `130 55` |
| 130 (`test_resource_types.Test_Resource_Unixfilesystem`) | 55–59 | 5 passed | `130 60` |
| 130 (`test_resource_types.Test_Resource_Unixfilesystem`) | 60–64 | 5 passed | `130 65` |
| 130 (`test_resource_types.Test_Resource_Unixfilesystem`) | 65–69 | 5 passed | `130 70` |
| 130 (`test_resource_types.Test_Resource_Unixfilesystem`) | 70–74 | 5 passed | `130 75` |
| 130 (`test_resource_types.Test_Resource_Unixfilesystem`) | 75–79 | 5 passed | `130 80` |
| 130 (`test_resource_types.Test_Resource_Unixfilesystem`) | 80–83 | 4 passed | `131 0` |
| 131 (`test_resource_types.Test_Resource_WeightedPassthru`) | 0–3 | 4 passed | `132 0` |
| 132 (`test_rule_engine_plugin_framework.Test_Plugin_Instance_CppDefault`) | 0–1 | 2 passed | `133 0` |
| 133 (`test_rule_engine_plugin_framework.Test_Plugin_Instance_Delay`) | 0–2 | 3 passed | `134 0` |
| 134 (`test_rule_engine_plugin_framework.Test_Rule_Engine_Plugin_Framework`) | 0–4 | 5 passed | `134 5` |
| 134 (`test_rule_engine_plugin_framework.Test_Rule_Engine_Plugin_Framework`) | 5–9 | 5 passed | `134 10` |
| 134 (`test_rule_engine_plugin_framework.Test_Rule_Engine_Plugin_Framework`) | 10 | 1 passed | `135 0` |
| 135 (`test_rule_engine_plugin_passthrough`) | 0 | 1 passed | `136 0` |
| 136 (`test_rulebase.Test_Remote_Exec`) | 0–4 | 5 passed | `136 5` |
| 136 (`test_rulebase.Test_Remote_Exec`) | 5 | 1 passed | `137 0` |
| 137 (`test_rulebase.Test_Resource_Session_Vars__3024`) | 0–4 | 5 passed | `137 5` |
| 137 (`test_rulebase.Test_Resource_Session_Vars__3024`) | 5–9 | 5 passed | `137 10` |
| 137 (`test_rulebase.Test_Resource_Session_Vars__3024`) | 10 | 1 passed | `138 0` |
| 138 (`test_rulebase.Test_Rulebase`) | 0–4 | 5 passed | `138 5` |
| 138 (`test_rulebase.Test_Rulebase`) | 5–9 | 5 passed | `138 10` |
| 138 (`test_rulebase.Test_Rulebase`) | 10–14 | 5 passed | `139 0` |
| 139 (`test_session_tokens.test_session_token_lifetime_configuration`) | 0 | 1 passed | `140 0` |
| 140 (`test_session_tokens.test_password_authentication_returning_session_tokens`) | 0–4 | 5 passed | `140 5` |
| 140 (`test_session_tokens.test_password_authentication_returning_session_tokens`) | 5–8 | 4 passed | `141 0` |
| 141 (`test_session_tokens.test_remove_session_tokens`) | 0–4 | 5 passed | `141 5` |
| 141 (`test_session_tokens.test_remove_session_tokens`) | 5–8 | 4 passed | `142 0` |
| 142 (`test_setting_user_password.test_modifying_user_password`) | 0–4 | 5 passed | `142 5` |
| 142 (`test_setting_user_password.test_modifying_user_password`) | 5 | 1 passed | `143 0` |
| 143 (`test_setting_user_password.test_igroupadmin_mkuser`) | 0–2 | 3 passed | `144 0` |
| 144 (`test_setting_user_password.test_invalid_configurations_and_options`) | 0 | 1 passed | `145 0` |
| 145 (`test_setting_user_password.test_ipasswd_with_both_passwords_set`) | 0–4 | 5 passed | `145 5` |
| 145 (`test_setting_user_password.test_ipasswd_with_both_passwords_set`) | 5 | 1 passed | `146 0` |
| 146 (`test_setting_user_password.test_ipasswd_with_only_native_password_set`) | 0–4 | 5 passed | `146 5` |
| 146 (`test_setting_user_password.test_ipasswd_with_only_native_password_set`) | 5 | 1 passed | `147 0` |
| 147 (`test_setting_user_password.test_ipasswd_with_only_irods_password_set`) | 0–4 | 5 passed | `147 5` |
| 147 (`test_setting_user_password.test_ipasswd_with_only_irods_password_set`) | 5–6 | 2 passed | `148 0` |
| 148 (`test_setting_user_password.test_ipasswd_with_no_password_set`) | 0–4 | 5 passed | `148 5` |
| 148 (`test_setting_user_password.test_ipasswd_with_no_password_set`) | 5 | 1 passed | `149 0` |
| 149 (`test_special_collections`) | 0 | 1 passed | `150 0` |
| 150 (`test_specific_queries`) | 0 | 1 passed | `151 0` |
| 151 (`test_ssl`) | 0 | 1 passed | `152 0` |
| 152 (`test_stacktrace`) | 0 | 1 passed | `153 0` |
| 153 (`test_symlink_operations`) | 0–4 | 5 passed | `153 5` |
| 153 (`test_symlink_operations`) | 5–8 | 4 passed | `154 0` |
| 154 (`test_targeting_specific_replica_number`) | 0–4 | 5 passed | `154 5` |
| 154 (`test_targeting_specific_replica_number`) | 5–9 | 5 passed | `154 10` |
| 154 (`test_targeting_specific_replica_number`) | 10–12 | 3 passed | `155 0` |

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

At `105 5`, `test_invalid_client_irodsProt_handled_cleanly__issue_4130`
intentionally sets `irodsProt=2`. `rcConnect.cpp` assigned that integer to
an enum before validating it, and UBSan reported an invalid enum load.
Validate the integer first, then convert only recognized protocol values;
release the partially allocated connection on rejection. Rebuilt and
installed sanitized runtime, icommands and server packages; all five
reproducing `test_misc` methods pass and the scoped UBSan logs
(`irods_batch_105_5_2590811`) no longer show the invalid enum load.
Continue at `105 10`.

At `116 85`, ASan found a heap-buffer-overflow in `getAllocLenForStr()` while
unpacking a packed message read via TCP. The TCP and SSL readers allocate one
extra byte for the packed input body but leave it uninitialized; a string
scan can run off the end. Terminate the received packed body in that reserved
byte, and use `size_t` for the allocation length. Rebuilt/installed the
sanitized runtime, reset the catalog to remove resources left by the aborted
test, and reran both compound-resource methods: passed with no new sanitizer
report (`irods_batch_116_85_2709418`). Continue at `117 0`.

The batch at `105 15` runs server PID/respawn lifecycle tests. The fourth
test's teardown could not remove `otherrods` because the server reported
`RE_UNABLE_TO_READ_SESSION_VAR` for `$rodsZoneProxy`; scoped logs contain no
new sanitizer report. A separate server restart did not restore that session
variable. Reset the test catalog/server before continuing at `105 19`.

After the factory-restart test, `/dev/shm` held a stale 30 MB rule-engine
cache in a 64 MB mount. The new factory logged `No space left on device` and
could not initialize its rule-variable mappings. Stopping the server and
removing stale iRODS shared-memory objects restored space (63 MB available)
and user administration; no test source was edited. The repository test
runner's `clear_irods_shared_memory_files()` logs these files but calls
`os.unlink()` on bare filenames, not paths in the shared-memory directory.

Index `105 19` requires the testing-environment hostname alias
`irods-catalog-provider`; configured that alias for the local server in
`/etc/hosts` (outside this checkout), then reran the single test: passed.
Continue at `105 20`.

Index `110 3`, a PAM-password expiration test, fails on both the batch and
isolated rerun: it sets a four-second lifetime, reauthenticates a session,
then expects *both* sessions to have expired. One session remains valid in
these sanitizer runs. Scoped logs contain only the known API wrapper UBSan
report, with no new ASan report; leave the test unchanged and continue at
`110 4`.

At `115 0`, `test_ilsresc_tree` passed but the second ASCII-output resource
tree test exceeded the per-batch 1,200-second limit. Its logs contain only
the existing generic API-wrapper report. Reset catalog/shared-memory state
after interruption and ran the second method alone: passed in 692 seconds,
with no new sanitizer report. Continue at `116 0`.

At `126 70`, the first three replication-resource cases passed but the
rebalance-update case exceeded the per-batch limit. Scoped logs contained
only the known API-wrapper report. Reset catalog state left by interruption
and ran methods 73 and 74 individually. Both passed (1142 and 32 seconds),
with no new sanitizer findings. Continue at `126 75`.

At `116 20`, UBSan reports calls to `gGuiProgressCB` in get/put utilities
through an incompatible function pointer. `iCommandProgStat()` was declared
to *return* `guiProgressCallback`, whereas the progress callback interface
returns `void`. Four iCommands cast away this mismatch. Match the `void`
signature and assign the function pointer without casts. Rebuilt and
installed sanitized runtime, icommands and server packages. The five
reproducing tests pass with no new sanitizer report
(`irods_batch_116_20_2674095`). Continue at `116 25`.

At `116 50`, ASan reports another stack-use-after-scope in the structfile
resource, this time at `compose_cache_dir_physical_path()` line 948. Like
the separate extraction finding, a reference to a temporary Boost path
component outlives the dereference expression. Copy the component by value;
rebuild/install and rerun the affected compound-resource batch before
committing this finding.

Rebuilt/installed the sanitized server. A prior aborted test left the
`origResc` resource name occupied, so reset the test catalog. All five
previously failing compound-resource methods at `116 50` now pass and
`/tmp/irods_batch_116_50_2689759_ubsan.*` contains no ASan report. That
batch uncovered a separate UBSan call-type mismatch at
`miscUtil.cpp:1335,1383`; address it in its own commit.

The special-collection query handle keeps either `rcQuerySpecColl` or
`rsQuerySpecColl` in a variadic `funcPtr`, then invokes it through that wrong
signature. Use a helper that selects the actual client/server signature for
all four call sites. Rebuilt and installed sanitized runtime, icommands and
server packages; all five compound-resource tests at offset 50 pass with no
new UBSan call-type report (`irods_batch_116_50_2694754`). Continue at
`116 55`.

Index `110 7`, another PAM password lifetime test, expected authentication
within a six-second window but later observed `CAT_PASSWORD_EXPIRED` in the
ASan/UBSan run. The other three methods in its batch passed; scoped logs
show no new sanitizer report. Do not change the test; continue at `110 8`.

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

## Core-suite completion and typed API dispatch

All 155 identifiers in the installed Python core test list were exercised in
batches, ending at cursor `155 0`. Every corrected sanitizer finding was
retested under an installed ASan/UBSan build. Three timing-sensitive tests
still failed without a new sanitizer report: the long delay-job test at
`11 3` and the short PAM-password-expiration cases at `110 3` and `110 7`.
The server-factory-respawn test at `105 18` also failed its teardown in this
64 MB `/dev/shm` environment; the old factory's 30 MB shared-memory cache
prevented the replacement factory from initializing. Stopping the server and
clearing stale iRODS shared memory restored subsequent tests.

The remaining `rsApiHandler.cpp` UBSan function-type report came from calling
typed API wrappers through `funcPtr`, an incompatible variadic function
pointer. Preserve each wrapper's concrete signature in `api_call_dispatcher`
when constructing built-in and plugin API entries. Its type-erased invocation
casts the packed argument pointers back to that signature before the call and
rejects an incorrect number of arguments. Rebuilt all sanitized targets,
regenerated the Debian packages, installed runtime, server, iCommands,
PostgreSQL plugin and development packages, then ran the installed upgrade
script to restore `version.json` after the package upgrade.

Under the new installed build, all nine `test_imkdir` cases passed, as did
five `test_iput` cases, five `test_itouch` cases and the SSL test. The
`irods_get_delay_rule_info` unit test passed 31 assertions, and the
`rc_switch_user` basic-usage unit test passed 10 assertions. Scoped ASan and
UBSan logs contained no API function-type report or new ASan finding; only
the already documented inlined libstdc++ unsigned-wrap reports remain.
Isolated reruns of `110 3` and `110 7` still failed their timing-dependent
authentication assertions without any new sanitizer report.
