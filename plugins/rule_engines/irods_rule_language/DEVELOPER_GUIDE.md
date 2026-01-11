# iRODS Rule Language Plugin - Developer Guide

## Overview

This directory contains the iRODS rule language plugin - the compiler and runtime for the iRODS policy-driven rule system.

**Key files:**
- `parser.cpp` - Main parser (modern syntax)
- `parser_legacy.cpp` - Legacy syntax support (deprecated)
- `typing.cpp` - Type inference and checking
- `arithmetics.cpp` - Expression evaluation
- `rules.cpp` - Rule execution engine
- `restructs.hpp` - AST node definitions
- `parser.hpp` - Parser macros and declarations

## Parser Architecture

### Parser Modules

The parser is organized into two modules:

#### Modern Parser (parser.cpp)
- Strict typing requirements
- Quoted string literals
- Modern control flow syntax (if/then/else, while, foreach)
- Modern function calls with type information

#### Legacy Parser (parser_legacy.cpp)
- Implicit type conversions
- Unquoted string arguments
- Legacy system functions (ifExec, whileExec, forEachExec)
- Deprecated syntax (being phased out)

### Dispatch Mechanism

Parser mode is controlled by the `@backwardCompatible` directive:

```
@backwardCompatible true   # Use legacy syntax exclusively
@backwardCompatible false  # Use modern syntax exclusively  
@backwardCompatible auto   # Auto-detect (default)
```

The mode is stored in `ParserContext::backwardCompatible`:
```c
context->backwardCompatible = 1  // Use legacy parser
context->backwardCompatible = -1 // Use modern parser
context->backwardCompatible = 0  // Auto-detect
```

### Parser Functions

Main entry points:
- `parseRuleSet()` - Parse entire rule file
- `parseRuleRuleGen()` - Parse a single rule
- `Rule()` - Parse rule definition (dispatches based on mode)
- `Actions()` - Parse action sequence (dispatches based on mode)

## Working with Backward-Compatible Code

### When to Use Each Module

**Use modern syntax (parser.cpp) for:**
- All new rules being written
- Rules in v2.0+ deployments
- Code that benefits from strict typing

**Use legacy syntax (parser_legacy.cpp) only for:**
- Migrating existing v1.x rules
- Maintaining compatibility with old deployments
- Understanding historical behavior

### Adding New Parser Features

To add a new modern parser feature:

1. **Implement in parser.cpp**
   - Add PARSER_FUNC_BEGIN/END block
   - Use BUILD_NODE and related macros
   - Follow existing code style

2. **Update parser.hpp** with forward declarations if needed

3. **Add tests** in `test_irods_rule_language_backward_compat.cpp`

4. **Document** in BACKWARD_COMPAT_REFACTORING.md if it affects legacy syntax

### Deprecating Legacy Features

To phase out a legacy syntax feature:

1. **v2.1**: Add warning when feature is used
   - Use `rodsLog(LOG_NOTICE, "Legacy syntax deprecated...")`
   - Keep feature working

2. **v2.2**: Make feature configurable (default disabled)
   - Add config option: `legacy_rule_syntax = disabled`
   - Keep feature available when explicitly enabled

3. **v3.0**: Remove feature entirely
   - Delete from parser_legacy.cpp
   - Remove from forward declarations
   - Update documentation

## Testing

### Running Tests

```bash
# Run backward compatibility tests
ctest -R "backward_compat" -V

# Run all rule language tests
ctest -R "rule" -V
```

### Adding Tests

1. **For legacy syntax**: Add to `test_backward_compat_rules.re`
2. **For unit tests**: Add to `test_irods_rule_language_backward_compat.cpp`
3. **For integration tests**: Follow existing test suite patterns

## Error Handling

The parser uses a two-level error system:

**Parser-level errors:**
- Set `context->error = 1` to signal parse failure
- Create `N_ERROR` nodes for specific issues
- Use `BUILD_NODE(N_ERROR, message, ...)` 

**Runtime errors:**
- Add to error chain with `addRErrorMsg(errmsg, code, message)`
- Use existing error codes from rodsErrorTable.h
- Document new codes in error comments

## Type System

The type system is implemented in `typing.cpp`:

- `typeExpression3()` - Main type inference function
- `ExprType` - Type representation
- `T_DYNAMIC` - Fallback type for untyped values

When adding new syntax:
1. Add type rules in `typing.cpp`
2. Handle both strict and implicit modes
3. Test with typed variables: `$x : int`, `$y : string`

## Memory Management

Use region-based allocation (preferred):
```c
Node *n = (Node*)region_alloc(region, sizeof(Node));
```

For short-lived objects, malloc is acceptable:
```c
ParserContext *pc = (ParserContext*)malloc(sizeof(ParserContext));
// ... use ...
free(pc);
```

## Related Documentation

- `BACKWARD_COMPAT_REFACTORING.md` - Refactoring details and migration guide
- `PLAN.md` - Project plan and related tasks
- `AGENTS.md` - Agent instructions for development

## Key Files Quick Reference

| File | Purpose | Size | Status |
|------|---------|------|--------|
| parser.cpp | Main parser | 3,348 lines | Active (modern) |
| parser_legacy.cpp | Legacy syntax | 317 lines | Deprecated (to v3.0) |
| parser.hpp | Parser macros | 435 lines | Stable |
| typing.cpp | Type inference | 1,228 lines | Active |
| restructs.hpp | AST definitions | 382 lines | Stable |
| rules.cpp | Rule execution | 711 lines | Active |
| functions.cpp | Built-in functions | varies | Active |

## Development Workflow

1. **Pick a ticket** from ready list
2. **Understand scope** - Legacy vs modern, error handling, etc.
3. **Implement changes** - Update relevant modules
4. **Write tests** - Add test cases to validate work
5. **Document** - Update this guide if architecture changes
6. **Commit** - Reference ticket in commit message

## Troubleshooting

### Parser crashes or hangs
- Check for infinite loops in LOOP_BEGIN/LOOP_END blocks
- Verify token queue isn't full (tokenQueue[1024])
- Enable DEBUG_VERBOSE in parser.cpp

### Type errors in new code
- Ensure all nodes have proper type information
- Check typing.cpp for how to handle new node types
- Use T_DYNAMIC only as fallback

### Memory leaks
- Run under valgrind: `valgrind --leak-check=full ./test`
- Ensure all region_alloc'd memory is freed
- Check malloc/free pairs in error paths

## Contact and Questions

For questions about:
- **Parser structure**: See parser.hpp and PLAN.md
- **Legacy syntax**: See parser_legacy.cpp and test_backward_compat_rules.re
- **Type system**: See typing.cpp and its comments
- **General architecture**: See BACKWARD_COMPAT_REFACTORING.md
