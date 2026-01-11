# Optional Chaining Operator (?.) Design

## Executive Summary

The optional chaining operator allows safe property and method access on potentially null values. Returns null/none if the base object is null, otherwise evaluates normally. Built on top of existing @optional/@nonnull type annotations (from ticket i-c7e5).

---

## 1. Syntax

### 1.1 BNF/EBNF

```ebnf
OptionalChainingExpression ::= 
    | PrimaryExpression
    | OptionalChainingExpression "?." Identifier                 (* field access *)
    | OptionalChainingExpression "?[" Expression "]"             (* computed access *)
    | OptionalChainingExpression "?(" ArgumentList ")"           (* method call *)
    | OptionalChainingExpression "." Identifier                  (* regular field access *)
    | OptionalChainingExpression "[" Expression "]"              (* regular computed access *)

FieldAccess ::= 
    | Expression "." Identifier
    | Expression "?." Identifier

IndexAccess ::=
    | Expression "[" Expression "]"
    | Expression "?[" Expression "]"

MethodCall ::=
    | Expression "(" ArgumentList ")"
    | Expression "?(" ArgumentList ")"
```

### 1.2 Examples

```irods
# Field access with optional chaining
rule optional_field_access {
    obj = get_user_object();
    name = obj?.name;                    # null if obj is null
    admin = obj?.admin_info?.level;      # chaining
}

# Computed/index access
rule optional_index_access {
    arr = get_optional_array();
    elem = arr?[0];                      # null if arr is null
    nested = arr?[i]?.data;              # chaining
}

# Method calls with optional chaining
rule optional_method_call {
    obj = get_optional_object();
    result = obj?.tostring();            # null if obj is null
    len = arr?.length();                 # null propagation
}

# Null coalescing with optional chaining
rule null_coalescing {
    name = obj?.name || "Unknown";       # fallback if null
    count = arr?[0] || 0;                # default value
}
```

### 1.3 Token Definition

- **?** (Question mark, existing placeholder in new_ops[24])
- **?. (Optional chaining operator)** (two-character sequence: `?` followed by `.`)
  - Recognized as a single operator token in lexer
  - Precedence: 8 (same as `.` and `?`)
  - Arity: binary (left-associative)
  - Right operand: field name, index expression, or argument list

---

## 2. Type System Rules

### 2.1 Type Inference

**Rule 1: Optional Chaining on Optional Types**
```
If obj : @optional T and field : T → S, then
   obj?.field : @optional S
```

**Rule 2: Optional Chaining on Nonnull Types**
```
If obj : @nonnull T and field : T → S, then
   obj?.field : @nonnull S
   (but syntactically allowed for consistency)
```

**Rule 3: Chained Optional Chaining**
```
If obj : @optional T, field1 : T → @optional U, field2 : U → V, then
   obj?.field1?.field2 : @optional V
```

**Rule 4: Type Preservation Through Chains**
```
If chain : obj?.a?.b?.c has type @optional T, then
   T is the result type of accessing c on the final object type
```

### 2.2 Null Propagation

- If operand is null, result is immediately null without evaluating RHS
- Enables short-circuit evaluation for performance
- Prevents NullPointerException-style errors at runtime

### 2.3 Type Narrowing

After optional chaining check:
```irods
rule type_narrowing {
    obj : @optional User = get_user();
    
    # After ?. access, type is still @optional
    if (obj?.name != null) {
        # Type checker could narrow obj from @optional User to @nonnull User
        # But explicit check still required in iRODS semantics
        name = obj.name;  # Now safe
    }
}
```

### 2.4 Compatibility with Assignment

Optional chaining in assignment target:
```irods
# Valid: assigning to optional chain
obj?.field = value;          # No-op if obj is null

# Valid: coercion on optional chain result
result = obj?.field;         # result : @optional T
```

---

## 3. AST Structure

### 3.1 Node Types

**Node Type: N_OPTIONAL_CHAIN**
- New node type to represent optional chaining operations
- Distinct from regular `N_ATTR` (field access) and function application

### 3.2 AST Layout

For `obj?.field`:
```
N_OPTIONAL_CHAIN
├─ subtrees[0] = obj (expression node)
├─ subtrees[1] = field (identifier or expression for index)
└─ text = "optionalAccess" or specific operator type
```

For `obj?[index]`:
```
N_OPTIONAL_CHAIN
├─ subtrees[0] = obj
├─ subtrees[1] = index (expression)
└─ text = "optionalIndex"
```

For `obj?(args)`:
```
N_OPTIONAL_CHAIN
├─ subtrees[0] = obj (callable expression)
├─ subtrees[1] = argument_tuple
└─ text = "optionalCall"
```

### 3.3 Relationship to Existing Nodes

- **N_ATTR**: Used for regular field access (`obj.field`)
  - Preserved for backward compatibility
  - Can be used in typing phase to check if chaining needed

- **N_APPLICATION**: Function application
  - Optional call equivalent: `N_OPTIONAL_CHAIN` with special text

---

## 4. Code Generation Strategy

### 4.1 Parser Changes (parser.cpp)

**Location**: Around line 1095-1110 (existing `?` placeholder)

1. Modify tokenization to recognize `?.` as two-character operator sequence
2. In `nextRuleGenTerm()`: Handle `?` followed by `.` or `[` or `(`
3. Create `N_OPTIONAL_CHAIN` node instead of regular operator node
4. Set node text to indicate operation type: `"optionalAccess"`, `"optionalIndex"`, `"optionalCall"`

**Pseudo-code for parser modification**:
```c
// Around line 1095 in parser.cpp
if (TOKEN_TEXT("?")) {
    NEXT_TOKEN_BASIC;
    char *fn;
    if (TOKEN_TEXT(".")) {
        fn = "optionalAccess";
        TTEXT("identifier"); // next identifier
    } else if (TOKEN_TEXT("[")) {
        fn = "optionalIndex";
        // parse index expression
    } else if (TOKEN_TEXT("(")) {
        fn = "optionalCall";
        // parse arguments
    } else {
        error("Expected . [ or ( after ?");
    }
    
    NT2(Term, rulegen, getBinaryPrecedence(token));
    BUILD_NODE(N_OPTIONAL_CHAIN, fn, &start, 2, 2);
}
```

### 4.2 Typing Changes (typing.cpp)

**Location**: New case in `typeExpression3()` switch (around line 1740+)

```c
case N_OPTIONAL_CHAIN: {
    /* Type the base object */
    ExprType *baseType = typeExpression3(expr->subtrees[0], ..., r);
    
    /* Extract the non-null core type */
    ExprType *coreType = stripOptionalAnnotation(baseType);
    
    /* Type the access operation on the core type */
    ExprType *accessType = typeFieldAccess(coreType, expr);
    
    /* Result is optional if base was optional */
    ExprType *resultType = makeOptional(accessType);
    
    return expr->exprType = resultType;
}
```

### 4.3 Runtime Changes (arithmetics.cpp / functions.cpp)

**Location**: Function lookup and execution

1. Register optional chaining operators as built-in functions
2. `optionalAccess(obj, field)`: Check if obj is null, return null or obj.field
3. `optionalIndex(obj, index)`: Check if obj is null, return null or obj[index]
4. `optionalCall(obj, args)`: Check if obj is null, return null or obj(args)

**Pseudo-code**:
```c
Res *optionalAccessFunc(Node **args, ..., Env *env, ...) {
    Res *objRes = args[0];  // evaluated object
    
    if (objRes == NULL || isNull(objRes)) {
        return newRes(r);  // null result
    }
    
    // Regular field access on non-null object
    return performFieldAccess(objRes, args[1]);
}
```

---

## 5. Error Handling

### 5.1 Compile-Time Errors

1. **Invalid chaining on non-optional types**
   - Warning only (not error): `obj?.field` where obj is @nonnull
   - Allowed for consistency, optimizer can eliminate

2. **Type mismatch in chained access**
   - Error: `obj?.nonexistent` where type has no such field
   - Same as regular field access errors

3. **Invalid chaining syntax**
   - Error: `obj?.` with missing field/index/args
   - Error: `obj?(` with missing closing paren (regular error)

### 5.2 Runtime Behavior

1. **Null propagation**
   - If any intermediate value is null, short-circuit to null
   - No exception thrown

2. **Type mismatches at runtime**
   - Same as regular field access
   - Dynamic type checking may catch issues

### 5.3 Error Messages

```
ERROR [Line 5]: Invalid field access on optional chain
    obj?.nonexistent_field
        ^^ Field 'nonexistent_field' not found in type User

ERROR [Line 10]: Optional chaining requires optional or nonnull base type
    obj?.field
    ^^^ Expected @optional T or @nonnull T, got dynamic type
```

---

## 6. Implementation Phases

### Phase 1: Parser Support (Parser Design)
- Recognize `?.` as two-character token
- Create `N_OPTIONAL_CHAIN` AST nodes
- Preserve tree structure for type checking

### Phase 2: Type System (Typing Design)
- Infer optional types through chains
- Validate base type is optional or nonnull
- Propagate optionality through results

### Phase 3: Code Generation (Runtime Design)
- Implement optional access functions
- Handle null propagation
- Integrate with existing field/index access code

### Phase 4: Testing & Optimization
- Unit tests for AST structure
- Integration tests for type checking
- Runtime tests for null propagation
- Performance optimization (inline null checks)

---

## 7. Examples

### 7.1 Basic Field Access

```irods
rule test_basic_optional_access {
    user : @optional User = getUserById(123);
    name = user?.name;
    email = user?.email || "no-email";
}
```

**AST**:
```
N_OPTIONAL_CHAIN
├─ subtrees[0] = TK_VAR "user"
├─ subtrees[1] = TK_TEXT "name"
└─ text = "optionalAccess"
```

**Type**:
- user : @optional User
- User.name : string
- user?.name : @optional string

### 7.2 Chained Access

```irods
rule test_chained_optional {
    config : @optional Config = loadConfig();
    path = config?.database?.location?.path;
}
```

**AST**:
```
N_OPTIONAL_CHAIN
├─ subtrees[0] = N_OPTIONAL_CHAIN
│  ├─ subtrees[0] = N_OPTIONAL_CHAIN
│  │  ├─ subtrees[0] = TK_VAR "config"
│  │  ├─ subtrees[1] = TK_TEXT "database"
│  │  └─ text = "optionalAccess"
│  ├─ subtrees[1] = TK_TEXT "location"
│  └─ text = "optionalAccess"
├─ subtrees[1] = TK_TEXT "path"
└─ text = "optionalAccess"
```

**Type**:
- config : @optional Config
- Config.database : @optional Database
- Database.location : @optional Location
- Location.path : string
- **Final type**: @optional string

### 7.3 Index Access

```irods
rule test_optional_index {
    arr : @optional array = get_array();
    first = arr?[0];
    second = arr?[1] || "default";
}
```

**AST**:
```
N_OPTIONAL_CHAIN
├─ subtrees[0] = TK_VAR "arr"
├─ subtrees[1] = TK_INT 0
└─ text = "optionalIndex"
```

---

## 8. Integration Points

### 8.1 @optional/@nonnull Annotations

- Extends existing type annotation system
- Uses `OPTION_OPTIONAL_TYPE` and `OPTION_NONNULL_TYPE` flags
- Compatible with tuple types, union types, etc.

### 8.2 Type Narrowing (Future)

Could leverage optional chaining for type narrowing:
```irods
rule type_narrowing_example {
    obj : @optional User;
    if (obj != null) {
        # obj type could narrow to @nonnull User
        name = obj.name;  # Safe without ?.
    }
}
```

### 8.3 Null Coalescing Operator (Future)

Could complement optional chaining:
```irods
rule null_coalescing {
    name = user?.name ?? "Unknown";  # ?? operator (future)
}
```

---

## 9. Testing Strategy

### Parser Tests (AST Verification)
- Syntax parse only (no execution)
- Verify node types and structure
- Check precedence and associativity
- Validate error cases

### Type System Tests
- Infer types through optional chains
- Validate @optional propagation
- Check error detection for invalid fields

### Runtime Tests (Phase 3+)
- Null propagation behavior
- Field/index/method access on non-null
- Exception handling (if applicable)
- Performance benchmarks

---

## 10. Future Enhancements

1. **Optional method calls**: `obj?.method()` with null-safe argument passing
2. **Optional chaining in patterns**: Use in destructuring and pattern matching
3. **Optional chaining in assignments**: `obj?.field = value` (conditional assignment)
4. **Type narrowing**: Automatic type narrowing after null check
5. **Null coalescing**: `??` operator for default values
6. **Optional chaining in comprehensions**: `[x?.field | x in optional_list]`
7. **Performance optimization**: Inline null checks, eliminate redundant checks in chains

---

## References

- JavaScript Optional Chaining: https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Operators/Optional_chaining
- TypeScript Optional Chaining: https://www.typescriptlang.org/docs/handbook/release-notes/typescript-3-7.html#optional-chaining
- Kotlin Safe Call Operator: https://kotlinlang.org/docs/null-safety.html#safe-calls
- Swift Optional Chaining: https://docs.swift.org/swift-book/language-guide/optional-chaining.html
