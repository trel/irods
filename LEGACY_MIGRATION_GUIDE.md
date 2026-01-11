# iRODS Rule Language: Legacy Syntax Migration Guide

## Overview

The iRODS Rule Language evolved from v1.x (legacy) to v2.0+ (modern). This guide helps users upgrade rules to modern syntax and understand deprecation timeline.

**Deprecation Timeline:**
- **v2.0-v2.4:** Accept both legacy and modern syntax (auto-detect mode)
- **v2.5+:** Accept both but issue warnings for legacy syntax
- **v3.0+:** Reject legacy syntax, require modern syntax only

---

## Pattern 1: Variable Declaration & Scoping

### Problem
Legacy syntax mixed typed and untyped variables with implicit scope semantics.

### Legacy Syntax
```irods
# Session variables ($var) - global scope, untyped
$collName = "/home/admin"
$size = 1024

# Local variables (*var) - local scope, untyped
*output = ""
*temp = 0
```

### Modern Syntax
```irods
# Explicit types with scope qualifiers
$collName : string = "/home/admin"
$size : int = 1024

*output : string = ""
*temp : int = 0
```

### Benefits
✓ Type safety - Compiler catches type errors
✓ Clear intent - Types document variable purpose
✓ IDE support - Better autocomplete and error detection
✓ Performance - No runtime type checking needed

### Migration Steps
1. Add type annotations to all variables
2. Use `:` operator before type name
3. Run type checker to validate
4. Test with `@backwardCompatible false`

### Examples
**Before:**
```irods
getData($path) :-
  *result = queryPath($path) |
  msiDebugPrint(*result)
```

**After:**
```irods
getData($path : string) : string :-
  *result : string = queryPath($path) |
  msiDebugPrint(*result) |
  *result
```

---

## Pattern 2: Error Handling (Implicit → try/catch)

### Problem
Legacy used "magic" status variables or return codes; errors were silent unless explicitly checked.

### Legacy Syntax
```irods
# Silent failure - no error checking
msiDataObjCreate($path, "w", $fd)
msiDataObjClose($fd, $status)

# Explicit status checking (verbose)
if ($fd > 0) then {
  # success path
} else {
  # error path  
}
```

### Modern Syntax
```irods
try {
  msiDataObjCreate($path, "w", $fd) |
  msiDataObjClose($fd, $status)
} catch (error => $e) {
  # error handling with exception info
  msiDebugPrint("Error: " ++ string($e.code))
} finally {
  # cleanup always runs
  closeResource($fd)
}
```

### Benefits
✓ Explicit error control - Clear error flow
✓ Automatic cleanup - finally block guarantees execution
✓ Exception context - $e.code, $e.message available
✓ Less boilerplate - No manual status checking

### Migration Steps
1. Identify silent operation calls (no status check)
2. Wrap in try block
3. Add catch block for error handling
4. Add finally block for cleanup if needed
5. Replace status variable checks with exception handling

### Examples
**Before:**
```irods
processFile($file) :-
  msiOpen("objType", $file, "r", $fd) |
  if ($fd > 0) then {
    msiClose($fd, $status) |
    msiDebugPrint("Success")
  } else {
    msiDebugPrint("Failed")
  }
```

**After:**
```irods
processFile($file : string) :-
  try {
    msiOpen("objType", $file, "r", $fd) |
    msiDebugPrint("Success")
  } catch (error => $e) {
    msiDebugPrint("Failed: " ++ string($e.code))
  } finally {
    if (defined($fd)) then {
      msiClose($fd, $status)
    }
  }
```

---

## Pattern 3: Optional/Nullable Types

### Problem
Legacy used sentinel values (empty string "", -1, etc.) to represent "missing" data; nullable types were implicit.

### Legacy Syntax
```irods
# Check for sentinel values
if ($maybeValue == "" || $maybeValue == -1) then {
  # missing
} else {
  # present
}

# No type documentation of nullability
function($param) :-
  if ($param != "") then { ... }
```

### Modern Syntax
```irods
# @optional type annotation
$maybeValue : @optional string = null

if (defined($maybeValue)) then {
  # present
} else {
  # missing
}

# Clear type contract
function($param : @optional string) :-
  if (defined($param)) then { ... }
```

### Benefits
✓ Type safety - Compiler enforces null checking
✓ Self-documenting - @optional in signature
✓ No sentinel values - No magic numbers/strings
✓ IDE support - Null-check hints and warnings

### @optional vs @nonnull

| Annotation | Meaning | Compiler Check |
|-----------|---------|-----------------|
| `@optional` | May be null/absent | Forces defined() check before use |
| `@nonnull` | Never null | Compiler rejects null assignments |
| (default) | Depends on context | Dynamic type inference |

### Migration Steps
1. Find sentinel value checks (`== ""`, `== -1`, `== null`)
2. Add `@optional` to function parameters that check for absent values
3. Use `defined()` to check for presence instead of sentinel checks
4. Remove sentinel value initialization (use `null` instead)
5. Type-check with compiler to find missed cases

### Examples
**Before:**
```irods
processUser($user) :-
  *profile = getProfile($user) |
  if (*profile != "") then {
    *name = extractName(*profile) |
    *email = extractEmail(*profile)
  } else {
    *name = "unknown" |
    *email = ""
  }
```

**After:**
```irods
processUser($user : string) :-
  $profile : @optional string = getProfile($user) |
  if (defined($profile)) then {
    $name : string = extractName($profile) |
    $email : string = extractEmail($profile)
  } else {
    $name : string = "unknown" |
    $email : string = ""
  }
```

---

## Pattern 4: Null Safety (Unsafe → Optional Chaining)

### Problem
Deeply nested data required multiple manual null checks at each level.

### Legacy Syntax
```irods
# Manual null check at each level
if ($user != "") then {
  $profile = getProfile($user) |
  if ($profile != "") then {
    $admin = getAdmin($profile) |
    if ($admin != "") then {
      msiDebugPrint($admin)
    }
  }
}
```

### Modern Syntax
```irods
# Optional chaining: ?. operator skips on null
$admin : @optional string = getProfile($user)?.admin?.name |
if (defined($admin)) then {
  msiDebugPrint($admin)
}
```

### Benefits
✓ Readability - Flat vs nested
✓ Safety - Automatic null propagation
✓ Conciseness - Fewer lines of code
✓ Maintainability - Less indentation/nesting

### Optional Chaining with Pattern Matching

Combine with pattern matching for maximum expressivity:

```irods
match (getProfile($user)?.admin?.perms) with
  | defined($p : list<string>) => {
      foreach ($perm in $p) {
        msiDebugPrint($perm)
      }
  }
  | undefined => msiDebugPrint("No permissions")
```

### Migration Steps
1. Find nested null checks (multiple `if` statements)
2. Replace with `?.` operator for property access
3. Use `defined()` only at the end, not in middle
4. Combine with pattern matching if multiple branches needed
5. Verify semantics match (short-circuit vs fallthrough)

### Examples
**Before:**
```irods
logEvent($event) :-
  *user = getEventUser($event) |
  if (*user != "") then {
    *profile = getUserProfile(*user) |
    if (*profile != "") then {
      *level = getPermLevel(*profile) |
      if (*level != "") then {
        msiDebugPrint(*level)
      }
    }
  }
```

**After:**
```irods
logEvent($event : string) :-
  $level : @optional string = 
    getEventUser($event)?.profile?.level |
  if (defined($level)) then {
    msiDebugPrint($level)
  }
```

---

## Pattern 5: Control Flow (Legacy Functions → Modern Keywords)

### Problem
Legacy used function-based control flow (ifExec, whileExec, forEachExec) with callback style; modern uses keywords for clarity.

### Legacy Syntax
```irods
# ifExec(cond, true_action, false_action, action3, action4)
ifExec($x > 0,
  msiDebugPrint("positive"),
  msiDebugPrint("negative"),
  msiDebugPrint("other1"),
  msiDebugPrint("other2")
)

# whileExec(cond, body)
whileExec(*i < 10,
  msiDebugPrint(*i) |
  *i = *i + 1
)

# forEachExec(pattern, loop_body, post_action)
forEachExec($item in $items,
  msiDebugPrint($item),
  msiDebugPrint("Done")
)
```

### Modern Syntax
```irods
# if/then/else keyword syntax
if ($x > 0) then {
  msiDebugPrint("positive")
} else if ($x < 0) then {
  msiDebugPrint("negative")
} else {
  msiDebugPrint("other")
}

# while keyword syntax
while (*i < 10) {
  msiDebugPrint(*i) |
  *i = *i + 1
}

# foreach keyword syntax
foreach ($item in $items) {
  msiDebugPrint($item)
} |
msiDebugPrint("Done")
```

### Comparison Table

| Feature | Legacy | Modern |
|---------|--------|--------|
| Condition | Function arg | if keyword |
| True branch | Function arg | {} block |
| False branch | Function arg | else keyword |
| Extra branches | Function args 3-4 | else if chains |
| Loop condition | Function arg | while keyword |
| Loop body | Function arg | {} block |
| Iteration | forEachExec | foreach keyword |
| Post-loop | Function arg | After foreach block |

### Benefits
✓ Readability - Keywords are clearer than functions
✓ Scoping - {} blocks create proper scope
✓ IDE support - Syntax highlighting for keywords
✓ Error messages - Better line number reporting
✓ Nesting - Easier to read deeply nested control flow

### Migration Steps
1. Find all `ifExec()` calls
2. Convert to `if/then/else` keyword syntax
3. Find all `whileExec()` calls
4. Convert to `while` keyword syntax
5. Find all `forEachExec()` calls
6. Convert to `foreach` keyword syntax
7. Test control flow behavior matches

### Examples

**ifExec migration:**
```irods
# Before:
test($x) :-
  ifExec($x > 0,
    msiDebugPrint("A"),
    msiDebugPrint("B"),
    msiDebugPrint("C"),
    msiDebugPrint("D")
  )

# After:
test($x : int) :-
  if ($x > 0) then {
    msiDebugPrint("A")
  } else if ($x < 0) then {
    msiDebugPrint("B")
  } else if ($x == 0) then {
    msiDebugPrint("C")
  } else {
    msiDebugPrint("D")
  }
```

**whileExec migration:**
```irods
# Before:
sum($n, *result) :-
  *i = 0 |
  *sum = 0 |
  whileExec(*i < $n,
    *sum = *sum + *i |
    *i = *i + 1
  ) |
  msiDebugPrint(*sum)

# After:
sum($n : int, *result : int) :-
  *i : int = 0 |
  *sum : int = 0 |
  while (*i < $n) {
    *sum = *sum + *i |
    *i = *i + 1
  } |
  msiDebugPrint(*sum)
```

**forEachExec migration:**
```irods
# Before:
process($items) :-
  forEachExec($item in $items,
    msiDebugPrint($item),
    msiDebugPrint("Done")
  )

# After:
process($items : list<string>) :-
  foreach ($item in $items) {
    msiDebugPrint($item)
  } |
  msiDebugPrint("Done")
```

---

## Pattern 6: Pattern Matching & Type Narrowing

### Problem
Legacy used guard expressions and implicit type coercion; modern uses explicit pattern matching for type narrowing.

### Legacy Syntax
```irods
# Guard expressions (unclear type intent)
if (isInt($x)) then {
  $val = castInt($x) |
  process($val)
}

# Type coercion (implicit, error-prone)
$result = $x + $y  # Automatic casting

# Manual type checks at multiple points
if ($type == "string") then {
  msiDebugPrint("String: " ++ $value)
} else if ($type == "int") then {
  msiDebugPrint("Int: " ++ $value)
}
```

### Modern Syntax
```irods
# Pattern matching with type narrowing
match (tryParseInt($x)) with
  | success($n : int) => process($n)
  | failure => msiDebugPrint("Not int")

# Explicit type annotations (no coercion)
$result : int = $x + $y

# Union type matching
match ($value : int | string) with
  | $s : string => msiDebugPrint("String: " ++ $s)
  | $i : int => msiDebugPrint("Int: " ++ $i)
```

### Pattern Matching Features

```irods
# 1. Literal patterns
match ($status) with
  | 0 => msiDebugPrint("Success")
  | -1 => msiDebugPrint("Error")
  | _ => msiDebugPrint("Unknown")

# 2. Variable capture
match (processData($x)) with
  | {$name, $value} => msiDebugPrint($name ++ ": " ++ $value)

# 3. Type narrowing
match ($data) with
  | $s : string => msiDebugPrint("String: " ++ $s)
  | $i : int => msiDebugPrint("Int: " ++ $i)

# 4. Guard expressions
match ($n : int) with
  | $x if $x > 0 => msiDebugPrint("Positive")
  | $x if $x < 0 => msiDebugPrint("Negative")
  | _ => msiDebugPrint("Zero")

# 5. Destructuring tuples
match ($user) with
  | {$name, $id, $email} => msiDebugPrint($name)

# 6. Option/Maybe patterns
match (getOptional()) with
  | defined($value) => process($value)
  | undefined => msiDebugPrint("Missing")
```

### Benefits
✓ Type safety - Exhaustiveness checking
✓ Clarity - Intent is explicit
✓ Performance - No runtime type checks
✓ Debugging - Better error messages
✓ Maintainability - Easier refactoring

### Migration Steps
1. Find guard expressions (if isType())
2. Convert to pattern matching with match/with
3. Add type annotations to match patterns
4. Remove implicit type coercion - add explicit casts
5. Use union types for multi-type values
6. Test all pattern cases execute correctly

### Examples
**Before:**
```irods
validateInput($input) :-
  if (isInt($input)) then {
    *val = castInt($input) |
    if (*val > 0) then {
      msiDebugPrint("Valid positive int")
    } else {
      msiDebugPrint("Zero or negative")
    }
  } else if (isString($input)) then {
    msiDebugPrint("String: " ++ $input)
  } else {
    msiDebugPrint("Unknown type")
  }
```

**After:**
```irods
validateInput($input : int | string) :-
  match ($input) with
    | $i : int => {
        match ($i) with
          | $x if $x > 0 => msiDebugPrint("Valid positive int")
          | _ => msiDebugPrint("Zero or negative")
    }
    | $s : string => msiDebugPrint("String: " ++ $s)
```

---

## Pattern 7: Type Inference (T_DYNAMIC → Explicit Types)

### Problem
Legacy defaulted untyped variables to T_DYNAMIC, losing type information and preventing compiler optimizations.

### Legacy Syntax
```irods
# Untyped = T_DYNAMIC (unsafe, no type checking)
getData($input) :-
  *result = processData($input) |
  *count = 0 |
  if (*result != "") then {
    *count = *count + 1  # Type unknown, dynamic casting at runtime
  } |
  msiDebugPrint(*count)  # Could be string, int, etc.

# Query results lose column type information
queryData($coll) :-
  *rows = queryCollection($coll) |
  *first = getTupleElement(*rows, 0) |
  msiDebugPrint(*first)  # Type unknown
```

### Modern Syntax
```irods
# Explicit types (type-safe, compiler checks)
getData($input : string) : int :-
  *result : string = processData($input) |
  *count : int = 0 |
  if (*result != "") then {
    *count = *count + 1  # Type known, compiler validates
  } |
  *count  # Return type explicit

# Query results preserve column types
queryData($coll : string) : list<tuple<string, int>> :-
  *rows : list<tuple<string, int>> = 
    select COLL_NAME, DATA_SIZE 
    where COLL_NAME = $coll |
  *rows
```

### Type Inference

When explicit types omitted, compiler infers from context:

```irods
# Type inferred from RHS
getData($input) :-
  *result = processData($input) |  # Type inferred from processData return type
  *count = 0 |                      # Type inferred: int (from 0)
  *items = []                       # Type inferred: list<dynamic>

# Type inferred from usage
msiDebugPrint($x) |                 # $x used as string (msiDebugPrint expects)
*result = $x + 1                    # $result inferred: int (from + 1)
```

### Benefits
✓ Type safety - Compiler catches type errors early
✓ Performance - No runtime type checking
✓ IDE support - Better autocomplete and refactoring
✓ Documentation - Types document intent
✓ Debugging - Stack traces show correct types

### Query Result Types

GenQuery results should be explicitly typed:

```irods
# Old: untyped
getData() :-
  *rows = select DATA_NAME, DATA_SIZE where COLL_NAME = "/home"

# New: explicitly typed
getData() : list<tuple<string, int>> :-
  *rows : list<tuple<string, int>> = 
    select DATA_NAME, DATA_SIZE where COLL_NAME = "/home" |
  *rows

# Pattern-matched extraction
processResults($rows : list<tuple<string, int>>) :-
  foreach ({$name, $size} in $rows) {
    msiDebugPrint($name ++ ": " ++ string($size))
  }
```

### Migration Steps
1. Add type annotations to all function parameters
2. Add type annotations to all function returns
3. Add type annotations to all local variables
4. Remove T_DYNAMIC usages - replace with explicit types
5. Query results: type as `list<tuple<...>>`
6. Run type checker to validate
7. Remove dynamic type casts where possible

### Examples
**Before:**
```irods
processFile($path) :-
  *handle = openFile($path) |
  *size = getFileSize(*handle) |
  if (*size > 0) then {
    *data = readFile(*handle, *size) |
    *result = parseData(*data)
  } else {
    *result = ""
  } |
  closeFile(*handle) |
  msiDebugPrint(*result)
```

**After:**
```irods
processFile($path : string) : @optional string :-
  *handle : int = openFile($path) |
  *size : int = getFileSize(*handle) |
  if (*size > 0) then {
    *data : string = readFile(*handle, *size) |
    *result : string = parseData(*data)
  } else {
    *result : @optional string = null
  } |
  closeFile(*handle) |
  if (defined(*result)) then {
    msiDebugPrint(*result)
  } |
  *result
```

---

## Troubleshooting Common Migration Issues

### Issue 1: "Type Error: Expected int, got T_DYNAMIC"

**Cause:** Variable type unknown because uninitialized or from dynamic source

**Solution:**
```irods
# Before (error):
*count = getData($x)  # What type does getData return?

# After (fixed):
*count : int = getData($x)  # Explicitly declare type
# OR add return type to getData:
getData($x : string) : int :- ...
```

### Issue 2: "Undefined Type Constructor"

**Cause:** Optional chaining without @optional type annotation

**Solution:**
```irods
# Before (error):
$name : string = getUser($id)?.name  # May be null!

# After (fixed):
$name : @optional string = getUser($id)?.name
# OR
getUser($id : int) : @nonnull {...}  # Guarantee non-null
```

### Issue 3: "Pattern Match Not Exhaustive"

**Cause:** Not all union type cases handled in match

**Solution:**
```irods
# Before (error):
match ($val : int | string) with
  | $i : int => msiDebugPrint($i)
  # Missing string case!

# After (fixed):
match ($val : int | string) with
  | $i : int => msiDebugPrint($i)
  | $s : string => msiDebugPrint($s)
```

### Issue 4: "Sentinel Value Comparison"

**Cause:** Using magic values instead of @optional types

**Solution:**
```irods
# Before (bad):
if ($value != "") then { ... }  # String empty = missing?

# After (better):
if (defined($value)) then { ... }  # Explicit nullability

# Best: declare @optional in signature
getData($id : int) : @optional string :- ...
```

### Issue 5: "Control Flow Functions Not Found"

**Cause:** Using legacy ifExec/whileExec/forEachExec keywords

**Solution:**
```irods
# Before (error):
ifExec($x > 0, action1, action2, action3, action4)

# After (fixed):
if ($x > 0) then {
  action1
} else if ($x < 0) then {
  action2
} else if ($x == 0) then {
  action3
} else {
  action4
}
```

---

## Deprecation Warnings

When using legacy syntax in v2.5+, compiler will warn:

```
WARNING: Rule uses legacy syntax (deprecated in v2.5, removed in v3.0)
  - Line 5: ifExec() function is deprecated, use if/then/else instead
  - Line 10: whileExec() function is deprecated, use while instead
  - Line 15: Untyped variable $x is deprecated, add type annotation
```

To suppress warnings temporarily during migration:

```irods
@legacySyntaxWarnings false

# Rules here won't generate warnings (v2.5 only)

@legacySyntaxWarnings true
```

---

## Automated Migration Tools (Future)

Planned for v2.5+:

```bash
# Convert file to modern syntax (best-effort)
irods-rule-migrate --output rules.v2.re rules.v1.re

# Check for deprecated syntax without converting
irods-rule-check --strict rules.re

# Generate migration report
irods-rule-report --deprecated rules.re > migration_report.txt
```

---

## Testing Your Migrations

1. **Parse check:** Verify modern syntax parses without warnings
   ```bash
   irule -F rules.modern.re --check
   ```

2. **Type check:** Verify all types are explicit
   ```bash
   irods-rule-check --strict rules.modern.re
   ```

3. **Behavior verification:** Run rule and verify output matches
   ```bash
   irule rules.modern.re ruleName
   ```

4. **Regression testing:** Compare results before/after
   ```bash
   diff <(irule -F rules.legacy.re) <(irule -F rules.modern.re)
   ```

---

## References

- **Modern Features:**
  - [Type System](QUERY_TYPING_FOUNDATION.md)
  - [Error Recovery](ERROR_RECOVERY.md)
  - [Backward Compatibility](BACKWARD_COMPAT_REFACTORING.md)

- **Test Files:**
  - `test_legacy_migration_guide.re` - Complete migration examples
  - `test_error_recovery.re` - Error handling patterns
  - `test_query_column_extraction.re` - Query typing patterns

---

## Getting Help

- Check `test_legacy_migration_guide.re` for working examples
- Review parser.cpp comments for type system internals
- Ask on iRODS community forums with migration questions
- File issues with migration tool ideas

---

**Last Updated:** 2026-01-13  
**Version:** 2.0.0 Compatible
