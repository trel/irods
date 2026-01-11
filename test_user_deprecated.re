/**
 * Test rules for @deprecated user-defined functions
 * These test cases verify that deprecated function decorations
 * are properly parsed and warnings are emitted during type checking
 */

/* Test 1: Simple deprecated function with message */
@deprecated "Use newFunction instead"
simpleDeprecated() = nop;

/* Test 2: Deprecated function without message */
@deprecated
deprecatedNoMsg() = nop;

/* Test 3: Call to deprecated function (should warn) */
callDeprecated() = simpleDeprecated();

/* Test 4: Non-deprecated function (should not warn) */
nonDeprecated() = nop;

/* Test 5: Non-deprecated function calling deprecated (should warn once per call) */
callsDeprecatedIndirectly() = simpleDeprecated();

/* Test 6: Deprecated function with parameters */
@deprecated "Use computeNewWay(int, string) instead"
computeOldWay(int val, string name) = val;

/* Test 7: Function calling deprecated function with parameters */
testParamDeprecated(int x) = computeOldWay(x, "test");

/* Test 8: Deprecated function returning complex type */
@deprecated "Use getNewData() instead"
getOldData() = 1;

/* Test 9: Multiple calls to same deprecated function */
multipleCallsTest() {
    simpleDeprecated();
    simpleDeprecated();
    simpleDeprecated();
}

/* Test 10: Nested deprecated function calls */
@deprecated "Use nestedNew() instead"
nestedOld() = simpleDeprecated();

/* Test 11: Chain of deprecated functions */
@deprecated "Deprecated chain member 1"
deprecatedChain1() = nop;

@deprecated "Deprecated chain member 2"
deprecatedChain2() = deprecatedChain1();

callDeprecatedChain() = deprecatedChain2();
