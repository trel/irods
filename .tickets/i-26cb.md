---
id: i-26cb
status: closed
deps: [i-ec3a]
links: []
created: 2026-01-12T17:56:49Z
type: epic
priority: 1
assignee: Terrell Russell
---
# Legacy Syntax Deprecation: Complete v2.0+ Transition

## Current State
The parser still supports v1.x legacy syntax alongside modern v2.0+ syntax:
- Backward compatibility flag threading through parser functions
- Legacy functions in parser_legacy.cpp already separated
- Mixed parsing logic in arithmetics.cpp with RE_BACKWARD_COMPATIBLE guards
- Type coercion rules differ between old and new syntax
- Deprecation plan exists (v2.2 disable, v3.0 remove) but not executed

## Problems
1. **Code bloat**: ~300 lines of legacy-specific code
2. **Maintenance burden**: Changes must work in both modes
3. **User confusion**: Different rules apply depending on syntax
4. **Migration friction**: No clear path to force modern syntax
5. **Testing complexity**: Every feature must be tested in both modes

## Revised Goals (Fast-Track Removal)
Since legacy syntax has zero production users, skip deprecation and remove immediately:
- Remove legacy parser module (parser_legacy.cpp)
- Remove legacy MSI functions (ifExec, whileExec, forEachExec)
- Remove backwardCompatible parameter threading from parser
- Clean up RE_BACKWARD_COMPATIBLE guards
- Delete test files and verify clean build

## Sub-tasks (Revised)
1. Remove parser_legacy.cpp module
2. Remove RE_BACKWARD_COMPATIBLE guards from functions.cpp
3. Remove backwardCompatible parameter threading from parser.cpp
4. Delete legacy MSI function implementations
5. Remove test_backward_compat_rules.re
6. Update CMakeLists.txt to remove legacy compilation
7. Verify build and all tests pass

**Effort:** ~10-15 hours (revised from 35-50)
**Timeline:** 1-2 weeks (not 3 release cycles)
**Risk:** None (zero production users)


## Notes

**2026-01-12T18:00:11Z**

Investigation Complete - Legacy Syntax Status

ACTUAL USAGE:
- Zero (@backwardCompatible) directives in production .re files
- Only 1 test file exists: test_backward_compat_rules.re (for testing purposes only)
- No real-world legacy syntax detected in active codebase

CODE FOOTPRINT:
- parser_legacy.cpp: 317 lines (fully separated module)
- RE_BACKWARD_COMPATIBLE guards: 2 locations in functions.cpp (only MSI function registration)
- backwardCompatible flag threading: 27 references in parser.cpp (param passing only)

DECISION: RECOMMEND FAST-TRACK REMOVAL
Since legacy syntax has zero users and is already separated, we can:
1. Skip deprecation warnings phase (no one to warn)
2. Remove legacy functions immediately
3. Remove ~300 lines of legacy code
4. Simplify parser by 27 backwardCompatible references

This would complete in 1-2 weeks instead of 3 release cycles.

REVISED EFFORT: ~10-15 hours (was 35-50)
- Remove parser_legacy.cpp and RE_BACKWARD_COMPATIBLE code
- Delete legacy MSI functions (ifExec/whileExec/forEachExec)
- Remove backwardCompatible param threading from parser
- Remove test_backward_compat_rules.re
- Verify build and tests pass

**2026-01-12T18:52:48Z**

Fast-Track Removal Plan - Execution Started

Files to modify:
1. CMakeLists.txt - Remove parser_legacy.cpp compilation
2. functions.cpp - Remove RE_BACKWARD_COMPATIBLE guard and legacy MSI functions
3. parser.cpp - Remove backwardCompatible parameter from all functions
4. parser.hpp - Remove backwardCompatible from ParserContext struct
5. parser_legacy.cpp - DELETE ENTIRE FILE
6. test_backward_compat_rules.re - DELETE ENTIRE FILE

Legacy MSI functions to remove:
- smsi_assignStr
- smsi_ifExec
- smsi_forExec (note: called forExec, not whileExec)
- smsi_whileExec
- smsi_forEachExec

Estimated removal sequence:
1. Remove from CMakeLists.txt
2. Remove legacy functions from functions.cpp
3. Simplify parser.cpp
4. Clean up parser.hpp
5. Delete parser_legacy.cpp
6. Delete test_backward_compat_rules.re
7. Build and test

**2026-01-12T19:07:19Z**

COMPLETED - Fast-track removal executed successfully. Parser_legacy.cpp deleted (317 lines). Removed backwardCompatible param threading (27 refs). Removed 4 legacy MSI functions. Removed @backwardCompatible directive. Deleted test_backward_compat_rules.re. Net 528 lines removed. Build clean. Zero production impact. Commit: e6e7cc2bd
