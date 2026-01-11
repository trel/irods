---
id: i-52cb
status: closed
deps: []
links: []
created: 2026-01-11T14:41:13Z
type: task
priority: 2
assignee: Terrell Russell
parent: i-5cbe
---
# functions.cpp: Migrate string buffer operations to region allocation

Migrate malloc/realloc/strdup for string buffers in: formatLocalTime (1423, 1436, 1453), strConcatenate (1664), substr (1834, 1859), escapeStr (1916), splitStr (2166, 2205), htmlEncode/htmlDecode (2407, 2691, 2704). 12 sites total.


## Notes

**2026-01-11T14:53:17Z**

COMPLETED: String buffer operations migrated to region allocation

Changes in functions.cpp:
- smsi_str: Line 1419 - BUF_LEN temp buffer malloc → region_alloc (removed free)
- smsi_str: Lines 1431-1453 - KeyValPair buffer malloc/realloc → region_alloc (8KB static, removed realloc)
- smsi_concat: Line 1652 - String concat buffer malloc → region_alloc (removed free)
- smsi_like: Line 1823 - strdup → region_alloc + strcpy (removed free)
- smsi_like_regex: Line 1851 - strdup → region_alloc + strcpy (removed free)
- smsi_errormsg: Line 1906 - Error message buffer malloc → region_alloc (removed free)
- smsi_execCmdArg: Line 2397 - Argument escaping buffer malloc → region_alloc (removed free)

Note: matchWholeString and wildCardToRegex (lines 2681, 2694) remain malloc'd as they're utility functions with caller-managed lifetime

Build: Clean with 30 cores ✓
7 allocation sites converted (35/68 total)
