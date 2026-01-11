---
id: i-94ab
status: closed
deps: [i-5cbe]
links: []
created: 2026-01-12T16:12:01Z
type: task
priority: 2
assignee: Terrell Russell
---
# index.cpp: Evaluate hashtable initialization for region allocation

Analysis: index.cpp has 3 malloc calls during hashtable initialization:
- Line 30, 279: int values for hash keys
- Line 375: string key allocation for rule names

These allocations happen during plugin initialization and populate global hashtables that last for the plugin lifetime.

Files: /src/irods/plugins/rule_engines/irods_rule_language/src/index.cpp:30,279,375

Evaluation needed:
1. Confirm these are init-time only (grep callers)
2. Check if hashtable lifetime extends beyond rule execution
3. Determine if Region-based allocation is feasible or if malloc should be documented

Option A: Migrate to region_alloc (requires adding Region context to init)
Option B: Document decision to keep malloc (init-time, long-lived hashtables)

Effort: 3-4 hours
