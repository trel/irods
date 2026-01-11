---
id: i-3ad9
status: closed
deps: [i-ec3a]
links: []
created: 2026-01-12T17:56:52Z
type: epic
priority: 1
assignee: Terrell Russell
---
# MSI Type Safety: Integrate Runtime Type Checking for Microservices

## Current State
Microservice interface (MSI) functions are dynamically loaded with no type information:
- Rules call MSI functions without argument type checking
- Return types are unknown at compile time
- Memory ownership semantics are implicit
- No validation that MSI modules provide required signatures
- Type system treats all MSI calls as T_DYNAMIC

## Problems
1. **No compile-time safety**: Wrong arguments compile but fail at runtime
2. **Memory errors**: No tracking of who owns returned values
3. **API evolution**: Can't detect when MSI modules change signatures
4. **User errors**: Hard to debug type mismatches in MSI calls
5. **Performance**: Runtime discovery of function signatures on every call

## Goals
- Extract MSI type signatures from function registry
- Validate argument types at rule compile time
- Infer return types for MSI calls
- Create optional strict MSI type checking mode
- Document MSI type annotation requirements
- Build comprehensive MSI type registry

## Sub-tasks (Planning)
1. Design MSI type signature metadata format
2. Extract MSI function type information from function registry
3. Implement MSI argument type validation at call sites
4. Add MSI return type inference
5. Create MSI type safety checking mode
6. Document MSI type annotation requirements
7. Build MSI type registry from existing microservices

**Effort:** ~50-70 hours across 7 tasks
**Depends on:** Type system redesign (i-ec3a)
**Blocks:** Grammar evolution (i-d794) for safe MSI integration

