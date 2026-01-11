---
id: wdt-9eee
status: open
deps: []
links: []
created: 2026-01-18T14:44:11Z
type: task
priority: 2
assignee: Terrell Russell
---
# Enable test: union_types


## Notes

**2026-01-18T14:44:33Z**

Test: test_union_types.cpp
Line 19: #include "irods/private/re/restructs.hpp"
Issue: Missing rule engine headers (same pattern)

Root Cause:
- Rule engine headers not in include path
- Tests type inference for union type expressions

Fix Strategy:
- Apply same fix as attr_typing group

**2026-01-18T17:06:43Z**

Status: Fixed - Added rule engine headers to test includes, created stubs for unneeded symbols, fixed T_LIST compilation bug in functions.cpp. Test builds and passes (70 assertions in 10 test cases).
