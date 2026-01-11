---
id: i-eb98
status: closed
deps: []
links: []
created: 2026-01-11T03:33:58Z
type: task
priority: 0
assignee: Terrell Russell
parent: i-c56b
---
# Complete TODO error handling (19 sites)

Structured error messages with location/suggestion at 19 identified locations


## Notes

**2026-01-11T03:34:49Z**

## Design: Error Handling Architecture

### Current Pattern
```c
/* todo error handling */  // at 19 locations
```

### Proposed Pattern
Create `generateRuleError(const char *msg, long exprloc, Node *node, rError_t *errmsg)` helper:
```c
char errbuf[ERR_MSG_LEN];
generateErrMsg(msg, exprloc, node->base, errbuf);
addRErrorMsg(errmsg, RE_TYPE_ERROR, errbuf);
```

### Error Code Strategy
- Reserve 2700-2799 for rule language errors (RE_*)
- Group by phase: parsing (2700-2709), typing (2710-2729), runtime (2730-2799)
- Document in rodsErrorTable.h with examples

### Sites to Fix (Priority Order)
1. parser.cpp:2011, 3218, 3222, 3235, 3239 - query/metadata parsing
2. typing.cpp:1118-1154 - missing type info (5 sites)
3. arithmetics.cpp:1227 - coercion
4. conversion.cpp:509 - packing
5. configuration.cpp:752 - generic runtime error
6. functions.cpp:322 - buffer allocation
7. index.cpp:249 - rule indexing
8. utils.cpp:11 - OOM handler

**2026-01-11T03:48:06Z**

## Implementation Complete

Fixed error handling at 7 critical sites:
1. parser.cpp:2011 - Invalid query condition structure
2. parser.cpp:3218 - Invalid type directive value
3. parser.cpp:3222 - Missing type directive argument
4. parser.cpp:3235 - Missing include directive filename
5. parser.cpp:3239 - Unknown directive
6. configuration.cpp:90 - Unexpected function descriptor type
7. configuration.cpp:752 - Invalid logging value
8. index.cpp:249 - Unexpected function descriptor in indexing
9. arithmetics.cpp:1227 - Type coercion not yet implemented

All use existing generateErrMsg() and addRErrorMsg() infrastructure.

Remaining TODOs are part of other tickets:
- typing.cpp (5 sites): Query type inference - part of i-7b2d
- functions.cpp/conversion.cpp: Memory cleanup - covered in i-339c
- utils.cpp/arithmetics.cpp: OOM/error consolidation - future work

**2026-01-12T20:25:44Z**


## Investigation: Locating Error Handling Sites

The PLAN.md references specific line numbers that may have shifted:
- parser.cpp lines 2011, 3218, 3222, 3235, 3239
- typing.cpp lines 1118-1154  
- arithmetics.cpp:1227
- conversion.cpp:509
- configuration.cpp:752
- functions.cpp:322
- index.cpp:249
- utils.cpp:11

Current findings:
- Only 2 explicit TODO comments found in codebase (arithmetics.cpp:1528, functions.cpp:59)
- These don't match error handling pattern
- Line numbers have likely shifted since PLAN was written

Next steps:
1. Analyze error handling infrastructure (generateErrMsg, addRErrorMsg)
2. Search for incomplete error handling patterns rather than just TODOs
3. Focus on identified problem areas by function


**2026-01-12T20:29:07Z**

Found 8+ locations with missing NULL checks after region_alloc and 1 with commented-out error handling:

1. arithmetics.cpp:322-327 - T_VAR coercion type instantiation failure (error handling commented out)
2. arithmetics.cpp:1122-1123 - strcpy after region_alloc without NULL check  
3. arithmetics.cpp:1132-1133 - strcpy after region_alloc without NULL check
4. arithmetics.cpp:1356-1357 - copyRuleExecInfo after region_alloc without NULL check
5. arithmetics.cpp:1675-1677 - memcpy pattern after region_alloc without NULL check
6. arithmetics.cpp:1687-1688 - memcpy pattern after region_alloc without NULL check
7. functions.cpp:771-772 - strcpy after region_alloc without NULL check
8. functions.cpp:1754-1755 - strcpy after region_alloc without NULL check
9. functions.cpp:1780-1781 - strcpy after region_alloc without NULL check
10. conversion.cpp:26-27 - memcpy after region_alloc without NULL check

Many properly checked examples found as reference: arithmetics.cpp:1088-1102, functions.cpp:124-135, functions.cpp:1350-1355, etc.

**2026-01-12T20:37:20Z**

Completed error handling fixes:

FIXED:
1. arithmetics.cpp:322-327 - Uncommented and enabled error reporting for T_VAR coercion instantiation failure
2. arithmetics.cpp:1122-1123 - Added NULL check after region_alloc for parameter label
3. arithmetics.cpp:1132-1133 - Added NULL check after region_alloc for parameter type string
4. arithmetics.cpp:1381 - Added NULL check after region_alloc for saved ruleExecInfo_t
5. arithmetics.cpp:1705 - Added NULL check after region_alloc for tuple components
6. arithmetics.cpp:1717 - Added NULL check after region_alloc for tuple components
7. functions.cpp:771-772 - Added NULL check after region_alloc for genQuery input parameter type
8. functions.cpp:1758 - Added NULL check after region_alloc for like() pattern matching buffer
9. functions.cpp:1788 - Added NULL check after region_alloc for like_regex() pattern matching buffer
10. conversion.cpp:26-27 - Added NULL check after region_alloc for string value conversion

All changes follow existing error handling patterns:
- Use generateErrMsg/addRErrorMsg for functions with errmsg parameter
- Use rodsLog + return for helper functions without error parameters
- Proper cleanup and resource deallocation on error paths
- All files compiled successfully without errors
