---
id: i-3cc5
status: closed
deps: [i-5cbe]
links: []
created: 2026-01-12T16:12:05Z
type: task
priority: 2
assignee: Terrell Russell
---
# nre.reHelpers1.cpp & nre.reHelpers2.cpp: Debug/helper allocations

Analysis: These files contain 2 malloc/strdup calls related to debug stack globals and helper utilities:

File: /src/irods/plugins/rule_engines/irods_rule_language/src/nre.reHelpers1.cpp

These are debug/diagnostic utility allocations that don't relate to core rule execution.

Investigation needed:
1. Locate the actual allocations
2. Determine if they're debug-only
3. Assess impact of keeping malloc vs converting

Options:
1. Keep malloc for debug utilities (no performance impact)
2. Use region allocation if feasible
3. Document decision

Effort: 2-3 hours
