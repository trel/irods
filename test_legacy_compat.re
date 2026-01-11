# Legacy Backward Compatibility Test Suite
# Tests for iRODS Rule Language v1.0 legacy syntax
# 
# This file contains a collection of legacy rule syntax patterns that must
# continue to work when disable_legacy_syntax is not set (default behavior).

@backwardCompatible true

# ============================================================================
# TEST GROUP 1: Legacy Variable Declarations & Implicit Typing
# ============================================================================

# Test 1.1: Basic implicit type variable (no @type annotation)
testImplicitTypeBasic() :-
  $x = 5 |
  $y = "hello" |
  $z = 3.14 |
  true

# Test 1.2: Variable assignment with type inference
testImplicitAssignment($input) :-
  $output = $input |
  true

# Test 1.3: Multiple variable declarations in sequence
testImplicitMultiVar() :-
  $a = 1 |
  $b = $a + 2 |
  $c = $b * 3 |
  true

# Test 1.4: Variables without pre-declaration
testUndeclaredVar() :-
  $untyped = 10 |
  $converted = $untyped + 5 |
  true

# ============================================================================
# TEST GROUP 2: Legacy System Functions (Control Flow)
# ============================================================================

# Test 2.1: ifExec with all branches
testIfExec($value) :-
  ifExec($value > 10,
    writeLine("stdout", "greater than 10"),
    writeLine("stdout", "not greater"),
    writeLine("stdout", "then branch"),
    writeLine("stdout", "else branch")) |
  true

# Test 2.2: Nested ifExec
testNestedIfExec($x) :-
  ifExec($x > 5,
    ifExec($x > 10,
      writeLine("stdout", "x > 10"),
      writeLine("stdout", "5 < x <= 10")),
    writeLine("stdout", "x <= 5")) |
  true

# Test 2.3: whileExec function
testWhileExec($limit) :-
  *$counter = 0 |
  whileExec(*$counter < $limit,
    *$counter = *$counter + 1 |
    writeLine("stdout", *$counter)) |
  true

# Test 2.4: forEachExec function
testForEachExec() :-
  *$list = list(1, 2, 3) |
  forEachExec(*$item in *$list,
    writeLine("stdout", *$item),
    writeLine("stdout", "done")) |
  true

# Test 2.5: forExec (legacy loop construct)
testForExec($start, $end) :-
  forExec(*$i = $start; *$i < $end; *$i = *$i + 1,
    writeLine("stdout", *$i)) |
  true

# ============================================================================
# TEST GROUP 3: Implicit Error Handling (No try/catch required)
# ============================================================================

# Test 3.1: MSI function call without explicit error handling
testImplicitMSIError($objPath) :-
  msiDataObjCreate($objPath, "r", *$fd) |
  msiDataObjClose(*$fd) |
  true

# Test 3.2: Multiple MSI calls with implicit error propagation
testImplicitErrorPropagation($file1, $file2) :-
  msiDataObjOpen($file1, "r", *$fd1) |
  msiDataObjRead(*$fd1, 4096, *$buf) |
  msiDataObjClose(*$fd1) |
  msiDataObjOpen($file2, "w", *$fd2) |
  msiDataObjWrite(*$fd2, *$buf, 0) |
  msiDataObjClose(*$fd2) |
  true

# Test 3.3: Bare function call (implicit error ignored)
testBareFunction() :-
  writeLine("stdout", "test") |
  true

# ============================================================================
# TEST GROUP 4: Legacy Expression Syntax
# ============================================================================

# Test 4.1: Implicit string concatenation
testStringConcat() :-
  $a = "hello" |
  $b = "world" |
  $c = $a ++ $b |
  true

# Test 4.2: String pattern matching (like operator)
testLikeOperator($str) :-
  ($str like "pattern*") |
  ($str not like ".*test.*") |
  true

# Test 4.3: Implicit numeric type coercion
testNumericCoercion() :-
  $x = 5 |
  $y = 3.14 |
  $z = $x + $y |
  $w = $x * $y |
  true

# Test 4.4: Boolean logic with implicit cast
testBooleanLogic($a, $b) :-
  (($a > 0) && ($b > 0)) |
  (($a == 0) || ($b == 0)) |
  !($a < 0) |
  true

# Test 4.5: Comparison operators with different types
testMixedTypeComparison() :-
  ($x = 5) |
  ($y = "5") |
  ($x == 5) |
  ($y like "5") |
  true

# ============================================================================
# TEST GROUP 5: Unquoted String Arguments
# ============================================================================

# Test 5.1: MSI with unquoted string literal (legacy)
testUnquotedMSI($object) :-
  msiDataObjCreate($object, default, *$fd) |
  true

# Test 5.2: Function call with path argument (no quotes)
testUnquotedPath($path) :-
  msiDataObjOpen($path, read, *$fd) |
  msiDataObjClose(*$fd) |
  true

# Test 5.3: Mixed quoted and unquoted arguments
testMixedQuoting() :-
  writeLine("stdout", "test message") |
  writeLine("stdout", unquoted_identifier) |
  true

# ============================================================================
# TEST GROUP 6: Macro-Expanded Constructs
# ============================================================================

# Test 6.1: assign function (legacy assignment)
testAssignFunction($value) :-
  assign(*$var, $value) |
  true

# Test 6.2: delayExec function
testDelayExec() :-
  delayExec("INST", writeLine("stdout", "delayed")) |
  true

# Test 6.3: remoteExec function
testRemoteExec($host) :-
  remoteExec($host, msiDebugPrint("remote")) |
  true

# ============================================================================
# TEST GROUP 7: Legacy Variable Syntax (Pointer Dereferencing)
# ============================================================================

# Test 7.1: Dereference operator usage
testDereference() :-
  *$x = 10 |
  *$y = *$x + 5 |
  writeLine("stdout", *$y) |
  true

# Test 7.2: Multiple levels of dereferencing
testMultiDereference($input) :-
  *$ref = $input |
  *$deref = *$ref |
  true

# ============================================================================
# TEST GROUP 8: Backward Compatible with Modern
# ============================================================================

# Test 8.1: Mixed legacy and modern syntax (simple)
testMixedSyntax($x) :-
  $modern = $x |
  *$legacy = $x |
  true

# Test 8.2: Legacy function with modern variable syntax
testLegacyFuncModernVars($input) :-
  ifExec($input > 0,
    $result = "positive",
    $result = "non-positive") |
  true

# Test 8.3: Modern control flow with legacy variables
testModernCtrlLegacyVars() :-
  if *$x > 0 then {
    *$y = *$x * 2
  } else {
    *$y = 0
  } |
  true

# ============================================================================
# TEST GROUP 9: Legacy Metadata & Attributes
# ============================================================================

# Test 9.1: Legacy attribute access
testAttributeAccess($obj) :-
  $meta = $obj.metadata |
  true

# Test 9.2: Legacy list operations
testListOps() :-
  *$list = list() |
  *$list = cons(1, *$list) |
  *$head = hd(*$list) |
  *$tail = tl(*$list) |
  true

# ============================================================================
# TEST GROUP 10: Error Recovery & Partial Syntax
# ============================================================================

# Test 10.1: Rule with valid legacy syntax
testValidLegacy() :-
  *$x = 5 |
  ifExec(*$x > 0,
    writeLine("stdout", "positive")) |
  true

# Test 10.2: Simple legacy rule
testSimpleLegacy($val) :-
  $result = $val + 1 |
  true

# Test 10.3: Complex legacy nesting
testComplexNesting($a, $b) :-
  ifExec($a > 0,
    ifExec($b > 0,
      whileExec(*$i < 10,
        *$i = *$i + 1 |
        writeLine("stdout", *$i)),
      writeLine("stdout", "b not positive")),
    writeLine("stdout", "a not positive")) |
  true
