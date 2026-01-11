---
id: i-ec3a
status: closed
deps: []
links: []
created: 2026-01-12T17:56:43Z
type: epic
priority: 1
assignee: Terrell Russell
---
# Type System Redesign: Move Beyond T_DYNAMIC

## Current State
The rule language type system falls back to T_DYNAMIC (wildcard type) for:
- Query result types (N_QUERY nodes)
- Attribute access (N_ATTR nodes)
- WHERE clause conditions (N_QUERY_COND_JUNCTION)
- Unknown column references
- Symbol links and MSI functions

This disables type checking for a large portion of typical rule code, making it impossible to catch type errors at compile time.

## Root Cause
Query type inference requires knowledge of the iRODS metadata schema, which isn't accessible during offline parsing. The language needs a type system that can:
1. Infer types from column metadata at parse time
2. Support union types for queries with unknown columns
3. Provide gradual typing (strict mode vs. permissive mode)
4. Handle MSI function type annotations

## Goals
- Eliminate T_DYNAMIC from hot code paths
- Introduce union types for safe fallbacks
- Add optional/nullable type support
- Enable strict type checking mode for policy code
- Support MSI type annotations

## Sub-tasks (Planning)
1. Design union type representation
2. Add column type metadata infrastructure
3. Implement query result tuple typing
4. Implement attribute typing
5. Add optional/nullable types
6. Create strict type checking mode
7. Integrate MSI type signatures
8. Add type constraint annotations

**Effort:** ~80-120 hours across 8 tasks


## Notes

**2026-01-13T00:59:46Z**

Status Review (2026-01-12):

COMPLETED SUBTASKS:
✓ i-0435 - Design union type representation for safe type fallback
✓ i-649a - Implement query result tuple type inference  
✓ i-44a1 - Implement N_ATTR typing with type preservation
✓ i-ab93 - Integrate MSI type signatures from function registry
✓ i-8d13 - Add column type metadata infrastructure for genQuery

OPEN SUBTASKS:
- i-dedf - Create strict type checking mode with compilation flags
- i-9b21 - Add optional/nullable type annotations (@optional, @nonnull)
- i-19d0 - Implement type constraint annotations and validation

BLOCKING DEPENDENCIES:
- i-72d9 - Parser Architecture Refactoring: Reduce Macro Coupling
- i-d794 - Grammar Evolution: Add Modern Language Features
- i-3ad9 - MSI Type Safety: Integrate Runtime Type Checking

Current Work: Assessing remaining tasks and understanding which can be started next.
