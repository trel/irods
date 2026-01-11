---
id: i-b89b
status: closed
deps: [i-14e4]
links: []
created: 2026-01-11T05:57:45Z
type: task
priority: 1
assignee: Terrell Russell
---
# Unify allocation/deallocation strategies

Consolidate mixed malloc/region_alloc/calloc usage into consistent pattern.

Current state: Mixed strategies across files
- conversion.cpp: Mixing malloc with embedded pointers
- functions.cpp: malloc without consistent cleanup
- cache.cpp: Complex malloc/free sequences

Target: Clear ownership semantics for all allocations
Pattern: Prefer region allocation with fallback to malloc for short-lived objects


## Notes

**2026-01-11T06:24:55Z**

Analyzed allocation/deallocation strategy consolidation:

Current patterns identified:
- region_alloc: Used for AST nodes, types, and long-lived structures (preferred)
- malloc: Used for short-lived buffers, temporary structures, and MS parameter conversion
- calloc: Minimal usage, mostly in hash tables

Key findings:
1. Conversion.cpp (35 malloc calls): Most for temporary buffers in type conversion
   - These are short-lived and should use stack buffers or region_alloc
   - Pattern: malloc for msParam packing, should align with existing strategy

2. Functions.cpp (56 malloc calls): Mix of temporary and intermediate allocations
   - Some for error handling buffers
   - Some for iteration/collection handling
   - Candidates for region_alloc optimization

3. Cache.cpp: Complex malloc/free sequences need consolidation

Recommended approach:
1. Priority 1: Add region parameter to functions that handle types (partial done via i-5d74)
2. Priority 2: Replace temporary buffer mallocs with stack arrays or region_alloc
3. Priority 3: Establish clear ownership semantics - free() only at function exit
4. Priority 4: Prefer struct composition over pointer allocation where possible

Current status: Partially addressed by i-5d74 (OOM handlers) and i-14e4 (bounds checking)
These changes enable safer memory management practices going forward.

Next steps: Coordinate with i-5cbe for full region-based migration
