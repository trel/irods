% Test file for try/catch exception handling construct
% Comprehensive test rules for try/catch/finally blocks

% Test 1: Basic try/catch - catch error
testTryCatchBasic {
    try {
        failAction("test");
    } catch (*) {
        writeLine("stdout", "Caught error");
    }
}

% Test 2: Try without error
testTryCatchNoError {
    try {
        writeLine("stdout", "No error here");
    } catch (*) {
        writeLine("stdout", "Should not print");
    }
}

% Test 3: Multiple catch handlers
testMultipleCatch {
    try {
        failAction("test");
    } catch (CAT_INVALID_ARGUMENT) {
        writeLine("stdout", "Invalid argument");
    } catch (*) {
        writeLine("stdout", "Other error");
    }
}

% Test 4: Try/catch with finally
testTryCatchFinally {
    try {
        writeLine("stdout", "In try block");
    } catch (*) {
        writeLine("stdout", "In catch");
    } finally {
        writeLine("stdout", "In finally");
    }
}

% Test 5: Try/catch/finally with error
testTryCatchFinallyError {
    try {
        failAction("test");
    } catch (*) {
        writeLine("stdout", "Caught");
    } finally {
        writeLine("stdout", "Cleanup");
    }
}

% Test 6: Catch with error variable binding
testCatchWithVariable {
    try {
        failAction("test");
    } catch (error) {
        writeLine("stdout", "Error code: ");
    } catch (*) {
        writeLine("stdout", "Fallback");
    }
}

% Test 7: Nested try/catch
testNestedTryCatch {
    try {
        try {
            writeLine("stdout", "Inner try");
        } catch (*) {
            writeLine("stdout", "Inner catch");
        }
    } catch (*) {
        writeLine("stdout", "Outer catch");
    }
}

% Test 8: Try/catch in conditional
testTryCatchInCondition {
    if (true) {
        try {
            writeLine("stdout", "Conditional try");
        } catch (*) {
            writeLine("stdout", "Conditional catch");
        }
    }
}

% Test 9: Try/catch in loop
testTryCatchInLoop {
    for (*x in "1,2,3") {
        try {
            writeLine("stdout", *x);
        } catch (*) {
            writeLine("stdout", "Error in loop");
        }
    }
}

% Test 10: Multiple catch handlers with error variable
testMultipleCatchWithVar {
    try {
        failAction("test");
    } catch (CAT_INVALID_ARGUMENT) {
        writeLine("stdout", "Invalid argument error");
    } catch (error) {
        writeLine("stdout", "Caught error");
    } catch (*) {
        writeLine("stdout", "Wildcard catch");
    }
}

% Test 11: Try/catch return value
testTryCatchReturnValue {
    try {
        writeLine("stdout", "Try returns value");
        "success";
    } catch (*) {
        "error";
    }
}

% Test 12: Try/catch with variable assignments
testTryCatchVarAssign {
    *result = "initial";
    try {
        *result = "try block";
        writeLine("stdout", *result);
    } catch (*) {
        *result = "catch block";
        writeLine("stdout", *result);
    }
    writeLine("stdout", *result);
}
