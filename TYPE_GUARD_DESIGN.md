# Type Guard Predicates Design

## Overview

Type guard predicates enable safe type narrowing in conditional expressions and pattern matching. They provide boolean-returning functions that test type properties and enable the type system to narrow variable types in dependent code.

## Goals

1. Enable safe type narrowing via guard conditions
2. Support type checking before dereference operations
3. Integrate seamlessly with pattern matching (i-adac)
4. Support optional type safety (@optional/@nonnull from i-c7e5)
5. Enable custom predicates for user-defined types

## Built-in Type Guard Predicates

### Core Predicates

#### `is_int(value: any) -> bool`
Returns true if value is of type int.
```re
myVar = *value;
if (is_int(myVar)) {
    % myVar is narrowed to int
    result = myVar + 1;
}
```

#### `is_string(value: any) -> bool`
Returns true if value is of type string.
```re
data = getDataFromQuery();
if (is_string(data)) {
    length = strlen(data);
}
```

#### `is_double(value: any) -> bool`
Returns true if value is of type double.

#### `is_bool(value: any) -> bool`
Returns true if value is of type bool.

#### `is_datetime(value: any) -> bool`
Returns true if value is of type datetime/int (time value).

#### `is_path(value: any) -> bool`
Returns true if value is of type path.

#### `is_list(value: any) -> bool`
Returns true if value is of list type.

#### `is_tuple(value: any) -> bool`
Returns true if value is of tuple type.

### Optional/Null Predicates

#### `is_optional(value: any) -> bool`
Returns true if value has optional type annotation (@optional).
```re
maybeInt = getOptionalInt();
if (!is_optional(maybeInt)) {
    % maybeInt is narrowed to non-optional
    result = maybeInt;
}
```

#### `is_nonnull(value: any) -> bool`
Returns true if value is guaranteed non-null (opposite of is_null).
```re
if (is_nonnull(userObj)) {
    userName = userObj.name;
}
```

#### `is_null(value: any) -> bool`
Returns true if value is null/absent.
```re
if (is_null(result)) {
    % Handle missing value
} else {
    % result is narrowed to non-null
}
```

### Type Pattern Predicates

#### `is_dynamic(value: any) -> bool`
Returns true if value has dynamic (unresolved) type.

#### `is_error_type(value: any) -> bool`
Returns true if value is error/exception type.

## Syntax Patterns

### Basic If-Guard

```re
rule guardExample {
    input: *value;
    output: *result;
    *result = 0;
    if (is_int(*value)) {
        *result = *value + 1;
    }
}
```

Type narrowing scope: the `then` branch narrows *value to int.

### Negation Guard

```re
rule checkRequired {
    input: *maybeValue;
    output: *hasValue;
    if (!is_null(*maybeValue)) {
        *hasValue = 1;
    } else {
        *hasValue = 0;
    }
}
```

Type narrowing: positive and negative branches both narrow types.

### Conjunction Guards

```re
rule stringLength {
    input: *value;
    output: *len;
    if (is_string(*value) && strlen(*value) > 0) {
        *len = strlen(*value);
    }
}
```

Type narrowing applies across &&, ||, all branches properly narrow.

### Pattern Match Integration

```re
rule processValue {
    input: *value;
    output: *result;
    match *value with
        | (int) => *result = is_int(*value) ? "matched int" : "error"
        | (string) => *result = "string match"
        | _ => *result = "unknown type"
}
```

Match cases implicitly narrow type; guards can further validate.

## Type Narrowing Rules

### General Rules

1. **Guard returns true**: type is narrowed in then-branch
   ```re
   if (is_int(x)) {
       % x is narrowed to T_INT in this scope
   }
   ```

2. **Guard returns false**: type excluded from else-branch
   ```re
   if (!is_string(x)) {
       % x is narrowed to non-string in this branch
   }
   ```

3. **Compound guards**: narrowing applies to all joined conditions
   ```re
   if (is_int(x) && x > 0) {
       % x narrowed to positive int
   }
   ```

4. **Negation**: inverts narrowing
   ```re
   if (!is_null(x)) {
       % x narrowed to non-null
   }
   ```

### Narrowing Implementation

Implementation uses Hashtable `varTypes` to track narrowed type bindings:

1. **Parser phase**: Identify guard predicates in conditional expressions
2. **Typing phase**: 
   - Evaluate guard return type
   - Create narrowing constraint for guard variable
   - Apply narrowing to then/else branches separately
   - Track via `typingConstraints` list with narrowing markers
3. **Code generation**: Use narrowed types for type checking in branches

### Narrowing Scope

Narrowing scope ends at:
- End of if-then block
- End of if-else block
- Start of unrelated statement
- Function/rule return

## Integration with Pattern Matching

Pattern matching (i-adac) provides type patterns:
```re
match value with
    | (int) => ...      % explicit type pattern
    | x => ...          % value pattern
```

Type guards complement patterns:
```re
match value with
    | (int) =>
        if (is_int(value)) {   % redundant but allowed
            value + 1
        }
    | x =>
        if (is_int(x)) {       % refines generic pattern
            x + 1
        }
```

## Integration with Optional Types

Optional types (i-c7e5) use annotations:
```re
@optional int maybeVal;
@nonnull int required;
```

Type guards work with optional types:
```re
rule processOptional {
    input: *opt : @optional string;
    output: *result;
    
    if (!is_null(*opt)) {
        % *opt is narrowed to @nonnull string
        *result = strlen(*opt);
    } else {
        *result = 0;
    }
}
```

## Custom Predicates

Users can define custom predicates:
```re
% Define custom predicate
isPositiveInt(x) = is_int(x) && x > 0;

rule useCustom {
    input: *value;
    if (isPositiveInt(*value)) {
        % *value is narrowed to positive int (by definition)
    }
}
```

Custom predicates composed of guards inherit narrowing properties.

## Error Handling

### Invalid Guard Usage

```re
% ERROR: guard predicate on non-existent variable
if (is_int(undefined)) { }

% ERROR: type narrowing narrowed-variable-without-decl
if (is_int(x)) { y = x; }  % x not declared in scope
```

### Type Mismatches

```re
% OK: guard can narrow any type
*value = ...;  % type unknown
if (is_string(*value)) { ... }

% OK: redundant narrowing
*str : string = ...;
if (is_string(*str)) { ... }  % always true, allowed
```

## Built-in Implementation Details

### Predicate Registration

Built-in predicates registered in `functions.cpp`:
- Type: T_BOOL return value
- Parameters: (any) accepting T_DYNAMIC
- Marked with flag to enable type narrowing

### Narrowing Metadata

Each guard predicate stores:
- Narrowed type (T_INT, T_STRING, etc.)
- Negation mode (for !is_type guards)
- Scope (function, rule)

### Code Generation

For each guard in conditional:
1. Insert guard predicate call
2. Track control flow (then/else branches)
3. Apply narrowed type bindings
4. Restore previous bindings at merge point

## Type System Extensions

### Type Expressions

Guards use type expressions in dereference:
```re
is_int(x) → type(x) = T_INT
is_optional(x) → has_annotation(x, @optional)
is_nonnull(x) → has_annotation(x, @nonnull)
```

### Constraint Generation

Type constraints generated for guards:
```
Guard: is_int(x)
→ Constraint: narrowing(x, T_INT, positive)
→ In then-branch: x ≤ T_INT
→ In else-branch: x ≠ T_INT
```

## Performance Characteristics

- **Guard predicate call**: O(1) type check
- **Narrowing lookup**: O(1) hashtable access
- **Scope management**: O(scope_depth) for restore
- **Code generation**: O(predicates_in_rule) linear scan

No runtime overhead: guards resolved at compile time.

## Testing Strategy

### Unit Tests (test_type_guards.cpp)

1. **Basic Predicates**: is_int, is_string, etc.
2. **Null Checks**: is_null, is_nonnull
3. **Optional Types**: is_optional integration
4. **Narrowing Scope**: guard scope boundaries
5. **Negation**: !is_type narrowing inversion
6. **Conjunction**: && narrowing across conditions
7. **Complex Expressions**: guards in nested conditions
8. **Integration**: guards with pattern matching
9. **Error Cases**: invalid guards, scope violations
10. **Performance**: large rule sets

### Integration Tests (test_type_guards.re)

1. Type narrowing in conditionals
2. Pattern matching with guards
3. Optional type safety
4. Error recovery
5. Shadowing and scope
6. Type coercion edge cases
7. Recursive rules with guards
8. Custom predicates

## Design Decision Rationale

### Why Predicates vs Casts?

Type casts are unsafe:
```re
(int)value  % unsafe conversion
```

Type guards are safe:
```re
if (is_int(value)) {
    % type system guarantees int
}
```

### Why Implicit Narrowing?

Narrowing is automatic in guard branches:
```re
if (is_int(x)) {
    % x is int, no cast needed
}
```

vs explicit:
```re
if (is_int(x)) {
    y : int = (int)x;  % verbose
}
```

### Why Function Calls?

Predicates as functions are:
- Easy to understand
- Composable
- Extensible
- Type-safe
- Familiar to developers

## Future Extensions

1. **Type Unions**: `is_numeric(x)` → is_int || is_double
2. **Generic Predicates**: `is_list(x)` checks element types
3. **Custom Patterns**: user-defined type guards with hints
4. **Performance Hints**: `#pragma narrow(x, int)` for manual hints
5. **Exhaustiveness Checking**: enum-like type patterns

## Summary

Type guard predicates provide:
- Safe type narrowing without casts
- Integration with pattern matching
- Optional type safety
- Extensible for custom types
- Zero runtime overhead
- Clear, familiar syntax
