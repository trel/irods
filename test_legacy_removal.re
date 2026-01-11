# Test file: Verify that legacy parser functions are no longer accepted (v3.0+)
# These test cases should now fail with clear error messages

# Legacy ifExec - no longer accepted
# testIfExec() :-
#   ifExec($x > 0, msiDebugPrint("yes"), msiDebugPrint("no"), "", "")

# Legacy whileExec - no longer accepted  
# testWhileExec() :-
#   whileExec($x > 0, msiDebugPrint("loop"))

# Legacy forEachExec - no longer accepted
# testForEachExec() :-
#   forEachExec(*var, *item | "collection", msiDebugPrint(*item))

# Legacy forExec - no longer accepted
# testForExec() :-
#   forExec($i = 0; $i < 10; $i = $i + 1, msiDebugPrint($i))

# Legacy delayExec - no longer accepted
# testDelayExec() :-
#   delayExec("INST=1h", msiDebugPrint("delayed"))

# Legacy remoteExec - no longer accepted (requires second argument)
# testRemoteExec() :-
#   remoteExec("rods@host", msiDebugPrint("remote"), "", "")

# Modern equivalents that SHOULD work:

# Modern if/then/else syntax
testModernIf() :-
  if ($x > 0) then {
    msiDebugPrint("yes")
  } else {
    msiDebugPrint("no")
  }

# Modern while syntax
testModernWhile() :-
  while ($x > 0) {
    msiDebugPrint("loop")
  }

# Modern foreach syntax
testModernForEach() :-
  foreach (*var in *collection) {
    msiDebugPrint(*var)
  }

# Modern for syntax
testModernFor() :-
  for ($i = 0; $i < 10; $i = $i + 1) {
    msiDebugPrint($i)
  }

# Modern delay syntax
testModernDelay() :-
  delay("1h") {
    msiDebugPrint("delayed")
  }

# Modern remote syntax (single argument)
testModernRemote() :-
  remote("rods@host") {
    msiDebugPrint("remote")
  }
