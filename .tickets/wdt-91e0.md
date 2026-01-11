---
id: wdt-91e0
status: closed
deps: []
links: []
created: 2026-01-18T14:44:11Z
type: task
priority: 2
assignee: Terrell Russell
---
# Enable test: query_type_inference


## Notes

**2026-01-18T14:44:33Z**

Test: test_query_type_inference.cpp
Line 22: #include "irods/private/re/restructs.hpp"
Issue: Missing rule engine header (same as attr_typing)

Root Cause:
- Same issue as attr_typing - rule engine headers not in include path
- Test needs access to: restructs.hpp for AST node definitions

Fix Strategy:
- Apply same fix as attr_typing:
  1. Update CMakeLists.txt to add rule engine plugin include directories
  2. Link against rule engine plugin library
  3. Or create public wrapper headers

**2026-01-18T16:40:37Z**

Status: Fixed - headers [restructs.hpp, typing.hpp, utils.hpp, region.h], libraries [irods_common, irods_client, irods_plugin_dependencies, boost], stubs [test_query_type_inference_stubs.cpp], test [PASSED - 72 assertions in 10 test cases]
