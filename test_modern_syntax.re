# Modern iRODS Rule Language Test Suite
# Tests for iRODS Rule Language v2.0 modern syntax features
#
# This file contains ONLY modern rule syntax patterns.
# No legacy constructs are allowed: no ifExec, whileExec, forExec, assign, 
# delayExec, remoteExec, unquoted strings, implicit typing, or pointer dereference.
#
# When parsed with disableLegacySyntax=true, this file should parse successfully.
# If it fails, there are modern syntax feature gaps or regressions.

# ============================================================================
# TEST GROUP 1: Modern Variable Declarations with Type Annotations
# ============================================================================

# Test 1.1: Explicitly typed integer variable
testModernTypedInt() :-
  declare int $x = 5 |
  true

# Test 1.2: Explicitly typed string variable
testModernTypedString() :-
  declare string $name = "test" |
  true

# Test 1.3: Explicitly typed float variable
testModernTypedFloat() :-
  declare float $pi = 3.14159 |
  true

# Test 1.4: Explicitly typed bool variable
testModernTypedBool() :-
  declare bool $flag = true |
  true

# Test 1.5: Multiple typed variables in sequence
testModernMultiVarTyped() :-
  declare int $a = 1 |
  declare int $b = 2 |
  declare int $c = $a + $b |
  true

# Test 1.6: Typed variable with parameter
testModernTypedParam(declare int $input) :-
  declare int $output = $input + 10 |
  true

# ============================================================================
# TEST GROUP 2: Modern Control Flow - if/then/else
# ============================================================================

# Test 2.1: Simple if/then without else
testModernIfThen(declare int $value) :-
  if $value > 10 then {
    writeLine("stdout", "greater than 10")
  } |
  true

# Test 2.2: Complete if/then/else
testModernIfThenElse(declare int $value) :-
  if $value > 10 then {
    writeLine("stdout", "greater than 10")
  } else {
    writeLine("stdout", "not greater")
  } |
  true

# Test 2.3: Nested if/then/else
testModernNestedIf(declare int $x) :-
  if $x > 5 then {
    if $x > 10 then {
      writeLine("stdout", "x > 10")
    } else {
      writeLine("stdout", "5 < x <= 10")
    }
  } else {
    writeLine("stdout", "x <= 5")
  } |
  true

# Test 2.4: if/then with compound condition
testModernIfCompound(declare int $a, declare int $b) :-
  if ($a > 0) && ($b > 0) then {
    writeLine("stdout", "both positive")
  } |
  true

# ============================================================================
# TEST GROUP 3: Modern Control Flow - while loops
# ============================================================================

# Test 3.1: Modern while loop
testModernWhile(declare int $limit) :-
  declare int $counter = 0 |
  while $counter < $limit {
    writeLine("stdout", "counter: " ++ str($counter)) |
    $counter = $counter + 1
  } |
  true

# Test 3.2: While with condition
testModernWhileCondition() :-
  declare int $i = 0 |
  while $i < 5 {
    $i = $i + 1
  } |
  true

# ============================================================================
# TEST GROUP 4: Modern Control Flow - for loops
# ============================================================================

# Test 4.1: Modern for-each loop with list
testModernForEach(declare list $items) :-
  for $item in $items {
    writeLine("stdout", str($item))
  } |
  true

# Test 4.2: For-each with collection
testModernForEachCollection() :-
  declare list $data = list(1, 2, 3) |
  for $elem in $data {
    writeLine("stdout", str($elem))
  } |
  true

# ============================================================================
# TEST GROUP 5: Modern Function Calls with Type Safety
# ============================================================================

# Test 5.1: MSI function call with proper quoting
testModernMSI(declare string $objPath) :-
  msiDataObjOpen("r", $objPath, declare int $fd) |
  msiDataObjClose($fd) |
  true

# Test 5.2: Multiple MSI calls with error handling
testModernMSISequence(declare string $file1, declare string $file2) :-
  try {
    msiDataObjOpen("r", $file1, declare int $fd1) |
    msiDataObjRead($fd1, 4096, declare string $buf) |
    msiDataObjClose($fd1) |
    msiDataObjOpen("w", $file2, declare int $fd2) |
    msiDataObjWrite($fd2, $buf, 0) |
    msiDataObjClose($fd2)
  } catch error $err {
    writeLine("stdout", "error: " ++ str($err))
  } |
  true

# Test 5.3: Function call with explicitly typed parameters
testModernFuncCall(declare string $path) :-
  writeLine("stdout", "modern test: " ++ $path) |
  true

# ============================================================================
# TEST GROUP 6: Modern Error Handling - try/catch blocks
# ============================================================================

# Test 6.1: Basic try/catch with type
testModernTryCatch() :-
  try {
    writeLine("stdout", "attempting operation")
  } catch error $err {
    writeLine("stdout", "caught error")
  } |
  true

# Test 6.2: Try/catch with multiple handlers
testModernTryCatchMulti() :-
  try {
    msiDataObjOpen("r", "nonexistent", declare int $fd)
  } catch invalid_params $err {
    writeLine("stdout", "invalid params")
  } catch error $err2 {
    writeLine("stdout", "other error")
  } |
  true

# Test 6.3: Nested try/catch
testModernTryCatchNested() :-
  try {
    try {
      writeLine("stdout", "inner")
    } catch error $inner {
      writeLine("stdout", "inner error")
    }
  } catch error $outer {
    writeLine("stdout", "outer error")
  } |
  true

# ============================================================================
# TEST GROUP 7: Modern Expression Syntax
# ============================================================================

# Test 7.1: String concatenation with ++
testModernStringConcat() :-
  declare string $a = "hello" |
  declare string $b = "world" |
  declare string $c = $a ++ " " ++ $b |
  writeLine("stdout", $c) |
  true

# Test 7.2: Numeric expressions
testModernNumericExpr() :-
  declare int $x = 5 |
  declare int $y = 3 |
  declare int $sum = $x + $y |
  declare int $product = $x * $y |
  true

# Test 7.3: Comparison and logical operators
testModernLogicalExpr(declare int $a, declare int $b) :-
  if (($a > 0) && ($b > 0)) || ($a == 0) then {
    writeLine("stdout", "condition met")
  } |
  true

# Test 7.4: Type conversion functions
testModernTypeConv() :-
  declare int $num = 42 |
  declare string $str = str($num) |
  declare int $parsed = int($str) |
  true

# ============================================================================
# TEST GROUP 8: Modern Collections (Lists and Maps)
# ============================================================================

# Test 8.1: Modern list creation and iteration
testModernList() :-
  declare list $items = list(1, 2, 3, 4, 5) |
  for $item in $items {
    writeLine("stdout", str($item))
  } |
  true

# Test 8.2: List operations with modern syntax
testModernListOps() :-
  declare list $empty = list() |
  declare list $single = list(1) |
  declare list $multi = list("a", "b", "c") |
  true

# ============================================================================
# TEST GROUP 9: Modern Lambda/Anonymous Functions (if supported)
# ============================================================================

# Test 9.1: Anonymous function usage
testModernAnon() :-
  declare list $data = list(1, 2, 3) |
  for $x in $data {
    declare int $result = $x * 2 |
    writeLine("stdout", str($result))
  } |
  true

# ============================================================================
# TEST GROUP 10: Modern Metadata and Attribute Access (Modern Syntax)
# ============================================================================

# Test 10.1: Metadata access with modern syntax
testModernMetadata(declare string $object) :-
  try {
    msiGetObjType($object, declare string $type) |
    writeLine("stdout", "type: " ++ $type)
  } catch error $err {
    writeLine("stdout", "failed to get type")
  } |
  true

# Test 10.2: Query with result handling
testModernQuery() :-
  try {
    msiExecGenQuery("select DATA_NAME where DATA_COLL_NAME = '/tmp'", declare list $result) |
    for $row in $result {
      writeLine("stdout", str($row))
    }
  } catch error $err {
    writeLine("stdout", "query failed")
  } |
  true

# ============================================================================
# TEST GROUP 11: Modern Scoped Variables
# ============================================================================

# Test 11.1: Function scope isolation
testModernScope() :-
  declare int $outer = 10 |
  if true then {
    declare int $inner = 20 |
    writeLine("stdout", str($inner))
  } |
  true

# Test 11.2: Block scope variables
testModernBlockScope() :-
  declare int $x = 1 |
  {
    declare int $y = 2 |
    declare int $z = $x + $y |
    writeLine("stdout", str($z))
  } |
  true

# ============================================================================
# TEST GROUP 12: Modern Composition of Features
# ============================================================================

# Test 12.1: Complex modern rule combining features
testModernComplex(declare string $file, declare int $iterations) :-
  try {
    declare list $lines = list() |
    msiDataObjOpen("r", $file, declare int $fd) |
    declare int $counter = 0 |
    while $counter < $iterations {
      try {
        msiDataObjRead($fd, 4096, declare string $line) |
        $lines = list_append($lines, $line) |
        $counter = $counter + 1
      } catch end_of_file $eof {
        $counter = $iterations
      }
    } |
    msiDataObjClose($fd) |
    writeLine("stdout", "read " ++ str(list_length($lines)) ++ " lines")
  } catch error $err {
    writeLine("stdout", "operation failed")
  } |
  true

# Test 12.2: Modern rule with nested control structures
testModernNested() :-
  declare list $data = list(1, 2, 3, 4, 5) |
  declare int $sum = 0 |
  for $x in $data {
    if $x > 2 then {
      declare int $val = $x * 2 |
      $sum = $sum + $val
    }
  } |
  writeLine("stdout", "sum: " ++ str($sum)) |
  true

# ============================================================================
# TEST GROUP 13: Modern Operator Precedence
# ============================================================================

# Test 13.1: Proper precedence in expressions
testModernPrecedence() :-
  declare int $a = 2 |
  declare int $b = 3 |
  declare int $c = 4 |
  declare int $result = $a + $b * $c |
  true

# Test 13.2: Parenthesized expressions
testModernParens() :-
  declare int $a = 2 |
  declare int $b = 3 |
  declare int $c = 4 |
  declare int $result = ($a + $b) * $c |
  true

# ============================================================================
# TEST GROUP 14: Modern Comments
# ============================================================================

# Test 14.1: Comment preservation
testModernComments() :-
  # This is a comment inside a rule
  declare int $x = 5 |  # Inline comment
  writeLine("stdout", str($x)) |
  true

# ============================================================================
# TEST GROUP 15: Entry Point Rules (No Parameters)
# ============================================================================

# Test 15.1: Parameterless modern rule
testModernNoParams() :-
  declare int $x = 10 |
  writeLine("stdout", "test complete") |
  true

# Test 15.2: Another entry point
testModernEntry2() :-
  declare string $msg = "success" |
  writeLine("stdout", $msg) |
  true
