---
id: wdt-5d86
status: closed
deps: []
links: []
created: 2026-01-18T14:44:11Z
type: task
priority: 2
assignee: Terrell Russell
---
# Enable test: union_constraint_solving


## Notes

**2026-01-18T14:44:33Z**

Test: test_union_constraint_solving.cpp
Line 19: #include "irods/private/re/restructs.hpp"
Issue: Missing rule engine headers (same pattern)

Root Cause:
- Rule engine headers not in include path
- Test tests type system for union types

Fix Strategy:
- Apply same fix as attr_typing and query_type_inference

**2026-01-18T16:50:18Z**

Status: Fixed - headers [restructs.hpp, typing.hpp, utils.hpp], libraries [libboost_regex], test configuration updated
