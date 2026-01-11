@backwardCompatible false

#============================================================================
# MIGRATION GUIDE: Legacy Syntax to Modern Equivalents
#============================================================================
# This file demonstrates 7 major migration patterns from iRODS v1.x legacy
# syntax to v2.0+ modern syntax. Each section shows "BEFORE" (legacy) and
# "AFTER" (modern) patterns that are functionally equivalent.
#
# Pattern 1: Variable Declaration & Scoping
# Pattern 2: Error Handling (implicit → try/catch)
# Pattern 3: Optional/Nullable Types (@optional/@nonnull)
# Pattern 4: Null Safety (unsafe → optional chaining ?.)
# Pattern 5: Control Flow (ifExec/whileExec/forEachExec → if/while/foreach)
# Pattern 6: Pattern Matching & Type Narrowing
# Pattern 7: Type Inference (T_DYNAMIC → explicit types)
#============================================================================

#============================================================================
# PATTERN 1: Variable Declaration & Scoping
#
# LEGACY: Session variables ($var) for global, local variables (*var) for scope
# MODERN: Type-annotated variables with scope qualifiers
#============================================================================

# BEFORE (Legacy): Session variable with implicit type
legacySessionVar1() :-
  $collName = "/home/admin" |
  $size = 1024 |
  msiDebugPrint($collName)

# AFTER (Modern): Explicit types with session variables
modernSessionVar1() :-
  $collName : string = "/home/admin" |
  $size : int = 1024 |
  msiDebugPrint($collName)

# BEFORE (Legacy): Untyped local variable
legacyLocalVar1($input) :-
  *output = "" |
  *temp = 0 |
  true

# AFTER (Modern): Typed local variables
modernLocalVar1($input : string) : string :-
  *output : string = "" |
  *temp : int = 0 |
  $result : string = *output |
  true

#============================================================================
# PATTERN 2: Error Handling (Implicit → try/catch)
#
# LEGACY: Silent failures with status checks, error handling in guards
# MODERN: try/catch exception handling with explicit recovery
#============================================================================

# BEFORE (Legacy): Status variable pattern (implicit error handling)
legacyErrorHandling1($path) :-
  msiDataObjCreate($path, "w", $fd) |
  if ($fd > 0) then {
    msiDataObjClose($fd, $status) |
    msiDebugPrint("Success")
  } else {
    msiDebugPrint("Creation failed")
  }

# AFTER (Modern): try/catch for explicit error handling
modernErrorHandling1($path : string) :-
  try {
    msiDataObjCreate($path, "w", $fd) |
    msiDataObjClose($fd, $status) |
    msiDebugPrint("Success")
  } catch (error => $e) {
    msiDebugPrint("Creation failed: " ++ string($e.code))
  }

# BEFORE (Legacy): Multiple status checks (error-prone)
legacyErrorHandling2($query) :-
  genQuery("*s", "*q", $qh) |
  if ($qh > 0) then {
    fetchQueryResults($qh, $rows) |
    if ($rows > 0) then {
      msiDebugPrint("Found results")
    }
  }

# AFTER (Modern): try/catch chain with proper recovery
modernErrorHandling2($query : string) :-
  try {
    $qh : int = genQuery("select DATA_NAME", "*q") |
    $rows : int = fetchQueryResults($qh, *results) |
    msiDebugPrint("Found results")
  } catch (error => $e) {
    msiDebugPrint("Query failed") |
    throw($e)
  } finally {
    if (defined($qh)) then {
      closeQueryHandle($qh)
    }
  }

#============================================================================
# PATTERN 3: Optional/Nullable Types
#
# LEGACY: Use special values (empty string, -1, null) for "missing" data
# MODERN: @optional/@nonnull type annotations
#============================================================================

# BEFORE (Legacy): Implicit nullability with magic values
legacyOptional1($maybeValue) :-
  if ($maybeValue == "" || $maybeValue == -1) then {
    msiDebugPrint("Value is missing")
  } else {
    msiDebugPrint("Value: " ++ $maybeValue)
  }

# AFTER (Modern): Explicit @optional type
modernOptional1($maybeValue : @optional string) :-
  if (defined($maybeValue)) then {
    msiDebugPrint("Value: " ++ $maybeValue)
  } else {
    msiDebugPrint("Value is missing")
  }

# BEFORE (Legacy): Mix of typed and untyped (error-prone)
legacyOptional2($data) :-
  *result = "" |
  *count = 0 |
  if ($data != "") then {
    *result = processData($data) |
    *count = 1
  } |
  msiDebugPrint(*result)

# AFTER (Modern): Type-safe with @optional
modernOptional2($data : string) : @optional string :-
  $result : @optional string = null |
  if (defined($data) && $data != "") then {
    $result = processData($data)
  } |
  $result

# BEFORE (Legacy): Nonnull without documentation
legacyRequired1($required) :-
  if ($required == "") then {
    msiDebugPrint("ERROR: required param missing") |
    false
  } else {
    msiDebugPrint("Processing: " ++ $required) |
    true
  }

# AFTER (Modern): @nonnull documents requirement
modernRequired1($required : @nonnull string) :-
  msiDebugPrint("Processing: " ++ $required) |
  true

#============================================================================
# PATTERN 4: Null Safety (Unsafe → Optional Chaining ?.)
#
# LEGACY: Explicit null checks everywhere
# MODERN: Optional chaining operator ?. for safe navigation
#============================================================================

# BEFORE (Legacy): Manual null checks for nested data
legacyNullSafety1($user) :-
  if ($user != "") then {
    $name = getUsername($user) |
    if ($name != "") then {
      $profile = getUserProfile($name) |
      if ($profile != "") then {
        msiDebugPrint($profile)
      }
    }
  }

# AFTER (Modern): Optional chaining with ?.
modernNullSafety1($user : @optional string) :-
  $profile : @optional string = getUsername($user)?.profile |
  if (defined($profile)) then {
    msiDebugPrint($profile)
  }

# BEFORE (Legacy): Unsafe navigation with guard expressions
legacyNullSafety2($record) :-
  *admin = "" |
  if ($record != "") then {
    *data = getRecord($record) |
    if (*data != "") then {
      *admin = extractField(*data, "admin")
    }
  } |
  msiDebugPrint(*admin)

# AFTER (Modern): Safe navigation and pattern matching
modernNullSafety2($record : string) :-
  $admin : @optional string = getRecord($record)?
    .data?.admin |
  match ($admin) with
    | defined($a) => msiDebugPrint($a)
    | undefined => msiDebugPrint("No admin found")

#============================================================================
# PATTERN 5: Control Flow (Legacy Functions → Modern Keywords)
#
# LEGACY: ifExec(cond, act1, act2, act3, act4)
#         whileExec(cond, actions)
#         forEachExec(var, actions1, actions2)
# MODERN: if/then/else, while, foreach
#============================================================================

# BEFORE (Legacy): ifExec with 4 branches
legacyIfExec1($x) :-
  ifExec($x > 0,
    msiDebugPrint("positive"),
    msiDebugPrint("negative"),
    msiDebugPrint("zero"),
    msiDebugPrint("other")
  )

# AFTER (Modern): if/then/else (no 3rd/4th actions)
modernIfExec1($x : int) :-
  if ($x > 0) then {
    msiDebugPrint("positive")
  } else if ($x < 0) then {
    msiDebugPrint("negative")
  } else if ($x == 0) then {
    msiDebugPrint("zero")
  } else {
    msiDebugPrint("other")
  }

# BEFORE (Legacy): whileExec for loops
legacyWhileExec1($count) :-
  *i = 0 |
  whileExec(*i < $count,
    msiDebugPrint(*i) |
    *i = *i + 1
  )

# AFTER (Modern): while statement
modernWhileExec1($count : int) :-
  *i : int = 0 |
  while (*i < $count) {
    msiDebugPrint(*i) |
    *i = *i + 1
  }

# BEFORE (Legacy): forEachExec over collection
legacyForEachExec1($items) :-
  forEachExec($item in $items,
    msiDebugPrint($item),
    msiDebugPrint("Done")
  )

# AFTER (Modern): foreach statement
modernForEachExec1($items : list<string>) :-
  foreach ($item in $items) {
    msiDebugPrint($item)
  } |
  msiDebugPrint("Done")

# BEFORE (Legacy): Nested whileExec (complex)
legacyNestedLoop1($rows, $cols) :-
  *r = 0 |
  whileExec(*r < $rows,
    *c = 0 |
    whileExec(*c < $cols,
      msiDebugPrint(*r ++ "," ++ *c) |
      *c = *c + 1
    ) |
    *r = *r + 1
  )

# AFTER (Modern): Nested foreach (cleaner)
modernNestedLoop1($rows : int, $cols : int) :-
  foreach (*r in range(0, $rows)) {
    foreach (*c in range(0, $cols)) {
      msiDebugPrint(*r ++ "," ++ *c)
    }
  }

#============================================================================
# PATTERN 6: Pattern Matching & Type Narrowing
#
# LEGACY: Guard expressions and implicit type coercion
# MODERN: Pattern matching with explicit type narrowing
#============================================================================

# BEFORE (Legacy): Manual type checking and coercion
legacyTypeNarrow1($data) :-
  if ($data != "") then {
    *intVal = castInt($data) |
    if (*intVal >= 0) then {
      msiDebugPrint("Positive int: " ++ *intVal)
    } else {
      msiDebugPrint("Negative int: " ++ *intVal)
    }
  }

# AFTER (Modern): Pattern matching with type narrowing
modernTypeNarrow1($data : string) :-
  match (tryParseInt($data)) with
    | success($n : int) => {
        match ($n) with
          | $x if $x >= 0 => msiDebugPrint("Positive: " ++ $x)
          | $x => msiDebugPrint("Negative: " ++ $x)
    }
    | failure => msiDebugPrint("Not an integer")

# BEFORE (Legacy): Union type simulation with multiple checks
legacyUnion1($value) :-
  if (isString($value)) then {
    msiDebugPrint("String: " ++ $value)
  } else if (isInt($value)) then {
    *intVal = castInt($value) |
    msiDebugPrint("Int: " ++ *intVal)
  } else {
    msiDebugPrint("Unknown type")
  }

# AFTER (Modern): Pattern matching on union types
modernUnion1($value : int | string) :-
  match ($value) with
    | $s : string => msiDebugPrint("String: " ++ $s)
    | $i : int => msiDebugPrint("Int: " ++ $i)

# BEFORE (Legacy): Complex destructuring without pattern support
legacyDestructure1($tuple) :-
  *first = getTupleElement($tuple, 0) |
  *second = getTupleElement($tuple, 1) |
  *third = getTupleElement($tuple, 2) |
  msiDebugPrint(*first ++ ", " ++ *second ++ ", " ++ *third)

# AFTER (Modern): Tuple unpacking with pattern matching
modernDestructure1($tuple : tuple<string, int, string>) :-
  {$name, $id, $path} = $tuple |
  msiDebugPrint($name ++ ", " ++ $id ++ ", " ++ $path)

#============================================================================
# PATTERN 7: Type Inference (T_DYNAMIC → Explicit Types)
#
# LEGACY: Untyped variables default to T_DYNAMIC (lossy, error-prone)
# MODERN: Explicit type annotations or inference from context
#============================================================================

# BEFORE (Legacy): All untyped = T_DYNAMIC (unsafe)
legacyDynamic1($input) :-
  *result = processData($input) |
  *count = 0 |
  if (*result != "") then {
    *count = *count + 1
  } |
  msiDebugPrint(*count)

# AFTER (Modern): Explicit types with inference
modernDynamic1($input : string) : int :-
  *result : string = processData($input) |
  *count : int = 0 |
  if (*result != "") then {
    *count = *count + 1
  } |
  *count

# BEFORE (Legacy): Query results as untyped
legacyQueryDynamic1($coll) :-
  *rows = queryCollection($coll) |
  if (*rows > 0) then {
    *name = getQueryField(*rows, 0) |
    *size = getQueryField(*rows, 1) |
    msiDebugPrint(*name ++ ": " ++ *size)
  }

# AFTER (Modern): Explicit query result types
modernQueryDynamic1($coll : string) :-
  *rows : list<tuple<string, int>> = 
    select DATA_NAME, DATA_SIZE 
    where COLL_NAME = $coll |
  foreach ({$name, $size} in *rows) {
    msiDebugPrint($name ++ ": " ++ string($size))
  }

# BEFORE (Legacy): Function arguments and returns untyped
legacyDynamic2($a, $b) :-
  *sum = *a + *b |
  *product = *a * *b |
  msiDebugPrint(*sum)

# AFTER (Modern): Full type signatures
modernDynamic2($a : int, $b : int) : int :-
  *sum : int = $a + $b |
  *product : int = $a * $b |
  *sum

#============================================================================
# HELPER FUNCTIONS FOR EXAMPLES
#============================================================================

# Placeholder functions used in examples above
processData($data : string) : string :-
  $data

getUsername($user : string) : string :-
  $user

getRecord($id : string) : string :-
  $id

tryParseInt($s : string) : int | string :-
  $s

queryCollection($coll : string) : list<string> :-
  [ ]

castInt($s : string) : int :-
  0

isString($v) :-
  true

isInt($v) :-
  true

range($start : int, $end : int) : list<int> :-
  [ ]

#============================================================================
# SUMMARY OF MIGRATION PATTERNS
#============================================================================
#
# 1. Variable Declaration:
#    Legacy: $var (session), *var (local), untyped
#    Modern: $var : type, *var : type, explicit types
#
# 2. Error Handling:
#    Legacy: Silent failures, status variables, guard expressions
#    Modern: try/catch/finally with exceptions
#
# 3. Optional Types:
#    Legacy: Magic values ("", -1), implicit nullability
#    Modern: @optional/@nonnull type annotations
#
# 4. Null Safety:
#    Legacy: Manual null checks everywhere
#    Modern: Optional chaining ?. operator
#
# 5. Control Flow:
#    Legacy: ifExec(), whileExec(), forEachExec()
#    Modern: if/then/else, while, foreach
#
# 6. Pattern Matching:
#    Legacy: Guard expressions, type checks
#    Modern: match/with patterns, type narrowing
#
# 7. Type Inference:
#    Legacy: T_DYNAMIC for untyped (unsafe)
#    Modern: Explicit types with inference
#
#============================================================================
