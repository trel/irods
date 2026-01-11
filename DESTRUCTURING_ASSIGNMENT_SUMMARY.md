# Destructuring Assignment Syntax - Implementation Summary

**Ticket**: i-b180
**Status**: COMPLETE
**Foundation**: i-2d10 (Tuple unpacking)

## Implementation Overview

Destructuring assignment syntax allows binding multiple variables from tuple values using pattern syntax:

```irods
{a, b, c} = getTuple();  // a, b, c get tuple elements
{x, _} = pair;           // Bind x, ignore second element  
{p, {q, r}} = nested;    // Nested pattern support
```

## Architecture

### 1. Parser Layer
**File**: `plugins/rule_engines/irods_rule_language/src/parser.cpp` (lines 1322-1351)

- **Function**: `UnpackingPattern()` - Parses destructuring patterns
- **Node Type**: `N_UNPACKING_PATTERN` (enum value 53)
- **Support**:
  - Variable patterns: `*a`, `*b`, `*c`
  - Wildcard patterns: `_` (ignored elements)
  - Nested patterns: `{a, {b, c}}`
  - Empty patterns: `{}`

**Parser Rule**:
```
Pattern := '{' [Element (',' Element)* '}']
Element := Variable | '_' | Pattern
```

### 2. Type System
**File**: `plugins/rule_engines/irods_rule_language/src/typing.cpp` (lines 1625-1656)

**Function**: `typeExpression3()` - Case `N_UNPACKING_PATTERN`

**Type Validation**:
- Patterns type as tuple: `Tuple<T1, T2, ...>`
- Variables in patterns get component types: `a: T1, b: T2, ...`
- Wildcards type as `T_DYNAMIC` (ignored)
- Nested patterns recursively typed
- Type error if pattern has more elements than tuple

**Example**:
```
Pattern: {*a, *b, *c}
Tuple:   (10, "hello", 3.14)
Types:   a: T_INT, b: T_STRING, c: T_DOUBLE
```

### 3. Code Generation / Evaluation
**File**: `plugins/rule_engines/irods_rule_language/src/arithmetics.cpp` (lines 1975-2015)

**Function**: `matchPattern()` - Case `N_UNPACKING_PATTERN`

**Expansion Strategy**:
```irods
{a, b, c} = tuple  →  matchPattern({a,b,c}, tuple)
                   →  a = tuple.0; b = tuple.1; c = tuple.2;
```

**Features**:
- Extracts tuple elements via array access `v->subtrees[i]`
- Recursively calls `matchPattern()` for nested patterns
- Skips wildcard `_` elements
- Supports partial unpacking (fewer vars than elements)
- Error if pattern has more elements than value tuple

**Integration Point**:
- Called from `smsi_assign()` in functions.cpp (line 980)
- Handles the left-hand side of assignment operator `=`

## Example Usage

### Basic Destructuring
```irods
test_destructuring {
    *coords = (10, 20);
    {*x, *y} = *coords;
    # *x = 10, *y = 20
}
```

### Wildcards
```irods
test_wildcards {
    *record = (42, "ignore", 3.14);
    {*id, _, *value} = *record;
    # *id = 42, *value = 3.14 (middle element ignored)
}
```

### Nested Patterns
```irods
test_nested {
    *point = (1, (2, 3), 4);
    {*a, {*b, *c}, *d} = *point;
    # *a = 1, *b = 2, *c = 3, *d = 4
}
```

### Partial Unpacking
```irods
test_partial {
    *data = (1, 2, 3, 4, 5);
    {*first, *second} = *data;
    # *first = 1, *second = 2 (remaining ignored)
}
```

## Type System Integration

### Pattern Variables
- Fresh variables introduced by patterns are typed
- Variables follow standard scoping rules
- Multiple assignments to same variable allowed

### Type Checking
```
assign(pattern, value):
  - pattern: pattern node (N_UNPACKING_PATTERN)
  - value: expression node
  - value type must be Tuple<...>
  - pattern degree ≤ tuple degree
  - recursive type checking for elements
```

### Error Handling
```
1. Non-tuple value: "unpacking pattern expects a tuple value"
2. Too many variables: "unpacking pattern has more elements (N) than tuple (M)"
3. Type mismatch: Variables won't unify with tuple element types
```

## Backward Compatibility
- New syntax, opt-in feature
- No breaking changes
- Works alongside traditional tuple projection (`tuple.0`, `tuple.1`)

## Future Extensions
- Spread patterns: `{head, ...tail}`
- Destructuring in for loops: `for {a, b} in pairs { ... }`
- Object/record destructuring (if types added)
- Array destructuring (if array types added)

## Testing

### Unit Tests (test_destructuring_assign.cpp)
12 comprehensive tests covering:
1. Basic three-element destructuring
2. Wildcard patterns
3. Nested destructuring
4. Type checking
5. Error conditions
6. Variable binding
7. Partial unpacking
8. Single element patterns
9. Empty patterns
10. Node type verification

### Integration Tests (test_destructuring_assign.re)
10 rule-based tests:
1. Basic tuple destructuring
2. Wildcards in patterns
3. Nested tuple patterns
4. Partial patterns
5. String tuples
6. Mixed-type tuples
7. Function result unpacking
8. Empty pattern destructuring
9. Single element destructuring
10. Variable reuse

## Files Modified

### Core Implementation (complete)
- `src/irods/plugins/rule_engines/irods_rule_language/src/parser.cpp` - UnpackingPattern parser (existing)
- `src/irods/plugins/rule_engines/irods_rule_language/src/typing.cpp` - Type checking (existing)
- `src/irods/plugins/rule_engines/irods_rule_language/src/arithmetics.cpp` - Code generation (existing)

### Test Files
- `src/irods/test_destructuring_assign.re` - Integration tests
- `src/irods/test_destructuring_assign.cpp` - Unit tests

## Implementation Status

**Phase 1 - Parser**: ✅ COMPLETE
- N_UNPACKING_PATTERN node type defined
- UnpackingPattern() parser function implemented
- Supports all pattern varieties

**Phase 2 - Type Checking**: ✅ COMPLETE
- Pattern type validation integrated
- Variable typing from tuple element types
- Error detection for incompatible patterns

**Phase 3 - Code Generation**: ✅ COMPLETE
- matchPattern() handles N_UNPACKING_PATTERN
- Tuple element extraction working
- Recursive pattern expansion for nested patterns

**All Phases**: ✅ COMPLETE

## References
- Ticket i-2d10: Tuple unpacking foundation
- restructs.hpp: N_UNPACKING_PATTERN (enum value 53)
- Type system: typeExpression3() in typing.cpp
- Evaluation: matchPattern() in arithmetics.cpp
- Assignment: smsi_assign() in functions.cpp
