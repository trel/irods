---
id: i-96bc
status: closed
deps: []
links: []
created: 2026-01-12T17:05:43Z
type: task
priority: 2
assignee: Terrell Russell
parent: i-cc56
---
# applyRuleBase: migrate temp allocations to region

Lines 143-157: Convert 2 allocations (inActionCopy strdup, action malloc) in applyRuleBase to region_alloc. Function scope lifetime allows stack/region allocation.


## Notes

**2026-01-12T17:11:48Z**

Migrated 2 allocations in applyRuleBase (lines 143-157) from malloc/strdup to region_alloc. Both allocations scoped to function, freed at region_free. Build successful with 30 cores.
