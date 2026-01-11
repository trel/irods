@backwardCompatible false

# Test 1: Valid rule
testValid1() :- 
  $x = 5 |
  true

# Test 2: Invalid syntax - missing closing paren (should recover and continue)
testInvalid1($x :- 
  $y = $x + 1 |
  true

# Test 3: Valid rule after error
testValid2() :-
  $a = 10 |
  $b = 20 |
  true

# Test 4: Another invalid rule - bad expression
testInvalid2($x) :-
  $y = $x + + 1 |
  true

# Test 5: Another valid rule
testValid3($x) :-
  $result = $x * 2 |
  true

# Test 6: Missing body
testInvalid3($p)

# Test 7: Final valid rule
testValid4() :-
  msiDebugPrint("recovery test complete") |
  true
