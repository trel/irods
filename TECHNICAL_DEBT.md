# iRODS Rule Language: Technical Debt Remediation

## Epic: i-716c - Address Remaining Technical Debt

Follow-up to epic i-c56b (completed). Addresses remaining weaknesses in memory safety, type system, and error handling.

**Total Tickets**: 15 organized in 4 priority levels
**Status**: All open and ready for work

---

## Priority 0 (CRITICAL - Start This Week)

### i-f3dc: Add NULL checks after malloc() calls
- **Type**: Task
- **Effort**: 8-10 hours
- **Files**: parser.cpp, arithmetics.cpp, cache.cpp, msiHelper.cpp
- **Issue**: 15+ malloc() calls without NULL validation
- **Impact**: NULL pointer dereference crashes under memory pressure
- **Dependencies**: None

### i-e575: Implement cleanup on early returns
- **Type**: Task
- **Effort**: 6-8 hours
- **Files**: functions.cpp (8+ instances)
- **Issue**: Memory leaks in error paths
- **Impact**: Memory leaks when iterator initialization fails
- **Dependencies**: i-f3dc

### i-5d74: Add OOM handler to type system
- **Type**: Task
- **Effort**: 10-12 hours
- **Files**: utils.cpp, typing.cpp
- **Issue**: No handler for malloc/region exhaustion
- **Impact**: Type system crashes under resource pressure
- **Dependencies**: i-f3dc, i-e575

---

## Priority 1 (HIGH - This Sprint)

### i-41a8: Complete N_QUERY_COND_JUNCTION typing
- **Type**: Task
- **Effort**: 6-8 hours
- **Files**: typing.cpp
- **Issue**: WHERE clause AND/OR junctions type as T_DYNAMIC
- **Impact**: Loss of type safety for complex WHERE clauses
- **Pattern**: Similar to N_QUERY_COND (i-4dd9)
- **Dependencies**: i-5d74

### i-9cae: Complete N_ATTR typing
- **Type**: Task
- **Effort**: 4-6 hours
- **Files**: typing.cpp
- **Issue**: Attribute references type as T_DYNAMIC
- **Impact**: Attributes lose type information
- **Dependencies**: i-5d74

### i-14e4: Add array bounds checking to type macros
- **Type**: Task
- **Effort**: 4-6 hours
- **Files**: restructs.hpp
- **Issue**: Macros without bounds validation
- **Impact**: Buffer overread, segmentation faults
- **Dependencies**: i-5d74

### i-b89b: Unify allocation/deallocation strategies
- **Type**: Task
- **Effort**: 12-15 hours
- **Files**: conversion.cpp, functions.cpp, cache.cpp
- **Issue**: Mixed malloc/region_alloc/calloc without clear ownership
- **Impact**: Use-after-free, double-free risks
- **Dependencies**: i-14e4

### i-d219: Fix unsafe type conversions
- **Type**: Task
- **Effort**: 6-8 hours
- **Files**: conversion.cpp
- **Issue**: Missing validation and default cases
- **Impact**: Runtime type errors, crashes
- **Dependencies**: i-14e4

### i-ea46: Implement string type coercion
- **Type**: Task
- **Effort**: 6-8 hours
- **Files**: arithmetics.cpp
- **Issue**: TODO - string coercion not implemented
- **Impact**: Runtime failures on string conversion
- **Dependencies**: i-f3dc

---

## Priority 2 (MEDIUM - Next Sprint)

### i-5509: Replace goto-based error handling
- **Type**: Task
- **Effort**: 15-20 hours
- **Files**: utils.cpp, arithmetics.cpp, index.cpp
- **Issue**: Goto-based error handling, cleanup paths unclear
- **Impact**: Difficult to verify all paths clean up
- **Pattern**: RE_ERROR macros → RAII/explicit cleanup guards
- **Dependencies**: i-d219

### i-411c: Improve parser error recovery
- **Type**: Task
- **Effort**: 10-12 hours
- **Files**: parser.cpp
- **Issue**: Parser hangs on malformed input
- **Impact**: Poor diagnostics for invalid rules
- **Dependencies**: i-5509

### i-e60d: Add resource exhaustion testing
- **Type**: Task
- **Effort**: 8-10 hours
- **Files**: test/
- **Scope**: OOM conditions, stack exhaustion, deep recursion
- **Acceptance**: Tests verify graceful degradation under pressure
- **Dependencies**: i-411c

---

## Priority 3 (LONG-TERM - Future Work)

### i-5cbe: Migrate to region-based allocation exclusively
- **Type**: Task
- **Effort**: 20-30 hours
- **Rationale**: Region allocation is more efficient and safer
- **Phase 1**: Audit allocation sites
- **Phase 2**: Convert malloc to region_alloc
- **Phase 3**: Verify performance improvements
- **Dependencies**: i-b89b

### i-52a5: Integrate static analysis tools
- **Type**: Task
- **Effort**: 5-8 hours
- **Tools**: cppcheck, clang-static-analyzer, ASAN, UBSAN
- **Target**: CI/CD pipeline integration
- **Dependencies**: i-5cbe

### i-6528: Implement comprehensive fuzz testing
- **Type**: Task
- **Effort**: 12-16 hours
- **Target**: Parser and type system edge cases
- **Goal**: Find crashes, hangs, and memory corruption
- **Dependencies**: i-52a5

---

## Dependency Graph

```
i-f3dc (NULL checks)
├── i-e575 (early return cleanup)
│   └── i-5d74 (OOM handler)
│       ├── i-41a8 (query junction typing)
│       ├── i-9cae (N_ATTR typing)
│       └── i-14e4 (array bounds)
│           ├── i-b89b (unify allocation)
│           │   └── i-5cbe (region-only allocation)
│           │       └── i-52a5 (static analysis)
│           │           └── i-6528 (fuzz testing)
│           └── i-d219 (type conversions)
│               └── i-5509 (goto → RAII)
│                   └── i-411c (parser recovery)
│                       └── i-e60d (resource testing)
└── i-ea46 (string coercion)

i-716c [EPIC]
├── (Priority 0 × 3)
├── (Priority 1 × 6)
├── (Priority 2 × 3)
└── (Priority 3 × 3)
```

---

## Ready to Start (No Dependencies)

Tickets ready for immediate work:
- **i-f3dc**: Add NULL checks after malloc() calls

---

## Effort Estimate

| Priority | Count | Hours | Weeks |
|----------|-------|-------|-------|
| P0       | 3     | 24-30 | 1     |
| P1       | 6     | 38-45 | 1-2   |
| P2       | 3     | 28-34 | 1     |
| P3       | 3     | 37-54 | 2-3   |
| **TOTAL**| **15**| **127-163** | **5-7** |

---

## Success Criteria

✓ All malloc() calls have NULL checks
✓ No memory leaks in error paths
✓ Type system handles resource exhaustion gracefully
✓ All incomplete type annotations completed
✓ Array bounds checking on all array access
✓ Unified allocation strategy throughout
✓ Safe type conversions with validation
✓ Modern error handling patterns (no goto)
✓ Parser recovers from errors gracefully
✓ Comprehensive test coverage under stress
✓ Static analysis tools integrated
✓ Fuzz testing reveals no crashes

