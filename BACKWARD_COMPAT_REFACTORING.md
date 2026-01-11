# Backward Compatibility Parser Refactoring

## Overview

This document describes the refactoring of backward-compatible parser functions from the monolithic `parser.cpp` into a separate `parser_legacy.cpp` module.

## Motivation

The iRODS rule language evolved from v1.x (legacy) to v2.0+ (modern). The original parser contained inline conditionals to support both syntaxes, making the code difficult to understand and maintain. This refactoring:

1. **Separates concerns** - Legacy functions are now in one module
2. **Enables deprecation** - Legacy code can be easily disabled or removed
3. **Improves maintainability** - Modern parser.cpp is cleaner without legacy logic
4. **Facilitates testing** - Legacy syntax can be tested in isolation

## Refactoring Phases

### Phase 1: Extract Legacy Functions (COMPLETED)

**Commit:** be4f924c2

**Changes:**
- Created `parser_legacy.cpp` with 4 backward-compatible functions
- Removed 220 lines of implementations from parser.cpp
- Updated CMakeLists.txt to compile new module
- Modified parser.cpp to store backward compatibility flag in context

**Functions Extracted:**
1. `ExprBackwardCompatible()` - Expression parsing with implicit type conversions
2. `TermBackwardCompatible()` - Term/value parsing for legacy syntax
3. `TermSystemBackwardCompatible()` - Legacy system functions (ifExec, whileExec, etc.)
4. `ActionArgumentBackwardCompatible()` - Unquoted string argument parsing

**Code Metrics:**
- parser.cpp: 3,568 → 3,348 lines (220 lines removed)
- parser_legacy.cpp: 317 lines (new)
- Net change: +97 lines (due to added documentation)

### Phase 2: Testing and Validation (IN PROGRESS)

**Objectives:**
- Create test infrastructure for backward-compatible rules
- Verify all legacy syntax still parses correctly
- Document test coverage

**Deliverables:**
1. `test_backward_compat_rules.re` - Test rule file with legacy syntax examples
2. `test_irods_rule_language_backward_compat.cpp` - Unit test template
3. Test documentation

**Test Coverage:**
- Legacy expressions with implicit type conversions
- Legacy system functions (ifExec, whileExec, forEachExec, etc.)
- Unquoted string arguments
- @backwardCompatible directive processing
- Mixed modern/legacy syntax

### Phase 3: Deprecation Warnings (PLANNED)

**Objectives:**
- Add warnings when legacy syntax is detected
- Document deprecation timeline
- Enable/disable legacy syntax via configuration

**Timeline:**
- v2.1: Support with warnings
- v2.2: Disabled by default (config option to enable)
- v3.0: Remove entirely

### Phase 4: Future Cleanup (PLANNED)

**Objectives:**
- Remove legacy functions in v3.0
- Clean up conditional logic in parser.cpp
- Remove backwardCompatible parameter from parser interface

## Implementation Details

### Context Integration

The `backwardCompatible` flag is now stored in `ParserContext`:

```c
typedef struct {
    // ... existing fields ...
    int backwardCompatible;  // 1: legacy, -1: modern, 0: auto-detect
} ParserContext;
```

This allows any parser function to access the mode without requiring parameter threading.

### Function Declarations

All backward-compatible functions have forward declarations in parser.cpp:

```c
PARSER_FUNC_PROTO( TermBackwardCompatible );
PARSER_FUNC_PROTO1( ExprBackwardCompatible, int level );
PARSER_FUNC_PROTO1( TermSystemBackwardCompatible, int lev );
PARSER_FUNC_PROTO( ActionArgumentBackwardCompatible );
```

These are resolved at link time when `parser_legacy.cpp` is compiled.

### Legacy Syntax Features

#### Implicit Type Conversions

Legacy expressions support automatic casting:
```
$result = $x + $y      # Casts to double
$result = $x like "p"  # Casts to string
$result = $x && $y     # Casts to boolean
```

#### System Functions

Legacy control flow syntax:
- `ifExec(cond, action1, action2, action3, action4)` → modern: `if (cond) then { ... } else { ... }`
- `whileExec(cond, actions)` → modern: `while (cond) { ... }`
- `forEachExec(var, actions1, actions2)` → modern: `foreach (var) { ... }`

#### Unquoted Strings

Legacy arguments don't require quotes:
```
msiDataObjCreate(path_without_quotes, mode, $fd)
```

Modern requires:
```
msiDataObjCreate("path_with_quotes", "mode", $fd)
```

## Testing

### Test Files

1. **test_backward_compat_rules.re**
   - Example rules using legacy syntax
   - Test cases for each legacy feature
   - Can be parsed by parser with `@backwardCompatible true`

2. **test_irods_rule_language_backward_compat.cpp**
   - Unit test template for parser testing
   - Placeholder for full integration tests
   - Documents required test coverage

### Running Tests

#### Manual Testing
```bash
# Parse legacy rules (once test infrastructure is available)
irods-rule-engine-parser test_backward_compat_rules.re
```

#### Unit Tests
```bash
# Run backward compatibility tests
ctest -R "backward_compat" -V
```

## Configuration

### @backwardCompatible Directive

Rules can specify compatibility mode:

```
@backwardCompatible true   # Use legacy syntax only
@backwardCompatible false  # Use modern syntax only
@backwardCompatible auto   # Auto-detect (default)
```

Mode applies to all subsequent rules until changed.

### Future Config Option (v2.2)

Planned for v2.2:
```
legacy_rule_syntax = disabled  # Or: auto, enabled
```

## Deprecation Warnings (Future)

When v2.1 is released, add warnings:

```c
// In parser_legacy.cpp
if (using_legacy_feature) {
    rodsLog(LOG_NOTICE, 
        "Legacy syntax in use. Will be disabled in v2.2 and removed in v3.0");
}
```

## Migration Guide

### For Users

Replace legacy constructs:

| Legacy | Modern |
|--------|--------|
| `ifExec(cond, a1, a2, a3, a4)` | `if (cond) then { a1 } else { a2 }` |
| `whileExec(cond, actions)` | `while (cond) { actions }` |
| `forEachExec(var, actions)` | `foreach (var) { actions }` |
| `msiFunc(unquoted_arg)` | `msiFunc("quoted_arg")` |
| `$x + $y` | Use type annotations: `$x : int + $y : int` |

### For Developers

- Use modern syntax in new rules
- Test with `@backwardCompatible false` to catch legacy syntax
- Update CI/CD to gradually deprecate legacy rules

## Related Tickets

- **i-c56b** - iRODS Rule Language: Fix Core Weaknesses (Epic)
- **i-eb98** - Complete TODO error handling
- **i-7b2d** - Type query nodes instead of T_DYNAMIC
- **i-339c** - Audit and fix memory leaks
- **i-b269** - Improve parser error recovery

## References

- **PLAN.md** - Detailed project plan
- **AGENTS.md** - Development guidelines
- **parser.hpp** - Parser macros and structures
- **parser_legacy.cpp** - Legacy function implementations
- **restructs.hpp** - AST node definitions
