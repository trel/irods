---
id: i-9cae
status: closed
deps: [i-5d74]
links: []
created: 2026-01-11T05:57:39Z
type: task
priority: 1
assignee: Terrell Russell
---
# Complete N_ATTR typing

Implement type checking for attribute nodes in expressions.

Issue: N_ATTR typed as T_DYNAMIC (typing.cpp:1393 TODO)
Impact: Attribute references lose type information
Example: Variable attributes like var.name cannot be type-checked


## Notes

**2026-01-11T06:22:48Z**

Implemented type checking for N_ATTR (attribute access):
- N_ATTR nodes now properly type based on their column specifications
- Replaced generic T_DYNAMIC typing with schema-aware typing
- Handles both typed columns (with schema lookup) and untyped attributes
- Single-degree attributes return the type of the contained column
- Zero-degree attributes fall back to T_DYNAMIC as safe default
- Maintains error propagation for type errors in sub-expressions
- Build successful with 30 cores
