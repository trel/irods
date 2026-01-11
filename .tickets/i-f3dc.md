---
id: i-f3dc
status: closed
deps: []
links: []
created: 2026-01-11T05:57:27Z
type: task
priority: 0
assignee: Terrell Russell
---
# Add NULL checks after malloc() calls

Add validation after all unchecked malloc() returns in rule language plugin.

Files affected (15+ instances):
- parser.cpp: ParserContext (line 71), Pointer structs
- arithmetics.cpp: Microservice params (line 1090)
- cache.cpp: Memory buffers (lines 109, 120, 200)
- msiHelper.cpp: String allocation (line 824)


## Notes

**2026-01-11T06:05:02Z**

Added NULL checks after malloc/calloc in all rule engine plugin files: parser.cpp (5 sites), arithmetics.cpp (2 sites), conversion.cpp (12 sites), msiHelper.cpp (1 site), cache.cpp (2 existing), functions.cpp (13 sites), rules.cpp (2 sites), restructs.cpp (1 site), index.cpp (3 sites), irods_rule_language.cpp (1 site), nre.reLib1.cpp (2 sites). Total: 44 new NULL checks added. Compilation successful.
