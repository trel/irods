---
id: i-7ae3
status: closed
deps: [i-5cbe]
links: []
created: 2026-01-12T16:12:03Z
type: task
priority: 2
assignee: Terrell Russell
---
# msiHelper.cpp: Evaluate Region allocation for MSI string operations

Analysis: msiHelper.cpp has 1 calloc() call in msiStrCat MSI function:
- Line 824: String concatenation buffer allocation

msiStrCat is a microservice function that receives MSI parameters and doesn't have direct access to rule execution Region context.

File: /src/irods/plugins/rule_engines/irods_rule_language/src/msiHelper.cpp:824

Options:
1. Extract Region from ruleExecInfo_t if available
2. Use long-lived allocation (roi->Region or malloc)
3. Document decision to keep malloc for MSI functions

Investigation:
- Check if ruleExecInfo_t contains Region pointer
- Check how other MSI functions handle allocations
- Determine lifetime of allocated strings

Effort: 2-3 hours

## Notes

**2026-01-12T16:17:55Z**

Decision: Keep malloc for MSI string operations.

Investigation:
- ruleExecInfo_t does not provide Region context to MSI functions
- MSI function signature: int msiXxx(msParam_t*, ..., ruleExecInfo_t* rei)
- No Region parameter passed to MSI functions

msiStrCat behavior:
- Allocates buffer via calloc (line 824)
- Returns string via msParam_t->inOutStruct (out parameter)
- Caller responsible for freeing old value (line 835)
- String lifetime extends beyond rule execution scope

Conclusion: Region allocation unsuitable. Appropriate to keep malloc() for caller-managed memory. Added explanatory comment. Build successful.
