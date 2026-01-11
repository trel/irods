---
id: i-bacb
status: closed
deps: [i-5cbe]
links: []
created: 2026-01-12T16:12:02Z
type: task
priority: 2
assignee: Terrell Russell
---
# reVariableMap.cpp: Add Region parameters to utility functions

Analysis: reVariableMap.cpp has 3 strdup() calls in utility functions:
- Line 43: setStrDupLeafValue() - string duplication for variable values
- Line 89, 101: getVarMap() - string duplication for variable map lookups

These functions currently have no Region context. They are called from variable lookup and assignment operations.

Files: /src/irods/plugins/rule_engines/irods_rule_language/src/reVariableMap.cpp:43,89,101

Strategy:
1. Trace callers of setStrDupLeafValue, getVarMap
2. Determine if Region context can be threaded through
3. Add Region* parameter to function signatures
4. Update all callers
5. Replace strdup with region_alloc + strcpy

Complexity: Medium (need to trace call chains)
Effort: 4-5 hours

## Notes

**2026-01-12T16:49:40Z**

Added documentation comments explaining why malloc/strdup should remain in place for setStrDupLeafValue() and getVarMap(). 

Key findings:
- setStrDupLeafValue: Called from generated reVariableMap.gen.cpp setValFromRuleExecInfo() which lacks Region context
- getVarMap: Utility function for variable lookup, returns caller-managed strings that are freed by callers (arithmetics.cpp:959,1841)

Future migration path: Requires adding Region parameter to generated code reVariableMap.gen.cpp, which depends on code generator updates. Documented in code comments for future work.
