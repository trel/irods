# iRODS Rule Language Template/Macro Test Suite
# Comprehensive testing of template definition and instantiation

# ============================================================================
# Basic Templates
# ============================================================================

# Simple single-expression template
template increment(x) {
    x + 1;
}

# Template with multiple parameters
template add(a, b) {
    a + b;
}

# Template with multiple parameters and expressions
template multiParam(key1, key2) {
    val1 = "value1";
    val2 = "value2";
    writeLine("stdout", val1);
    writeLine("stdout", val2);
}

# ============================================================================
# Templates with Return Types
# ============================================================================

# Template with explicit return type annotation
template getAndLog(key) : string {
    val = "test_value";
    writeLine("stdout", val);
    val;
}

# Template returning integer
template getLength(str) : int {
    len(str);
}

# Template returning boolean
template isValid(value) : bool {
    value > 0;
}

# ============================================================================
# Advanced Templates
# ============================================================================

# Template using another template (nested calls)
template composed(id) {
    getAndLog(id);
    writeLine("stdout", "composed");
}

# Template with conditional logic
template conditionalTemplate(val) {
    if (val > 10) then {
        writeLine("stdout", "greater");
    } else {
        writeLine("stdout", "lesser");
    }
}

# Template with try-catch error handling
template safeExecute(action) {
    try {
        action;
        writeLine("stdout", "Success");
    } catch (*e) {
        writeLine("stdout", "Error occurred");
    }
}

# Template with loop
template repeatN(n, msg) {
    i = 0;
    while (i < n) {
        writeLine("stdout", msg);
        i = i + 1;
    }
}

# Template with string concatenation
template formatMessage(prefix, msg) {
    prefix ++ ": " ++ msg;
}

# Template with match expression
template describeValue(x) {
    match x with
    | 0 => "zero"
    | 1 => "one"
    | _ => "other"
}

# ============================================================================
# Recursive Templates
# ============================================================================

# Simple recursive template (factorial)
template factorial(n) {
    if (n <= 1) then {
        1;
    } else {
        n * factorial(n - 1);
    }
}

# Fibonacci sequence
template fibonacci(n) {
    if (n <= 1) then {
        n;
    } else {
        fibonacci(n - 1) + fibonacci(n - 2);
    }
}

# ============================================================================
# Rule Tests
# ============================================================================

# Simple rule calling a template
simpleRule {
    increment(5);
    writeLine("stdout", "simpleRule completed");
}

# Rule calling multi-param template
multiParamRule {
    add(10, 20);
    writeLine("stdout", "multiParamRule completed");
}

# Rule calling template with return type
returnTypeRule {
    result = getAndLog("mykey");
    writeLine("stdout", result);
}

# Rule testing nested template calls
nestedRule {
    composed("id1");
    writeLine("stdout", "nestedRule completed");
}

# Rule testing conditional template
conditionalRule {
    conditionalTemplate(15);
    conditionalTemplate(5);
}

# Rule testing safe execution
safeExecutionRule {
    safeExecute(writeLine("stdout", "Action executed"));
}

# Rule testing formatted messages
messageFormattingRule {
    msg1 = formatMessage("INFO", "Server started");
    msg2 = formatMessage("WARN", "High memory usage");
    msg3 = formatMessage("ERROR", "Connection failed");
    writeLine("stdout", msg1);
    writeLine("stdout", msg2);
    writeLine("stdout", msg3);
}

# Rule testing value description
valueDescriptionRule {
    d0 = describeValue(0);
    d1 = describeValue(1);
    d2 = describeValue(42);
    writeLine("stdout", d0);
    writeLine("stdout", d1);
    writeLine("stdout", d2);
}

# Rule testing factorial
factorialRule {
    f0 = factorial(0);
    f1 = factorial(1);
    f5 = factorial(5);
    writeLine("stdout", "factorial(0) = 1");
    writeLine("stdout", "factorial(1) = 1");
    writeLine("stdout", "factorial(5) = 120");
}

# Rule testing fibonacci
fibonacciRule {
    f0 = fibonacci(0);
    f1 = fibonacci(1);
    f6 = fibonacci(6);
    writeLine("stdout", "fibonacci(0) = 0");
    writeLine("stdout", "fibonacci(1) = 1");
    writeLine("stdout", "fibonacci(6) = 8");
}

# Rule testing loops
loopingRule {
    repeatN(3, "iteration");
}

# ============================================================================
# Edge Cases
# ============================================================================

# Empty template (no-op)
template noop() {
    nop();
}

# Template with single parameter
template identity(x) {
    x;
}

# Template with many parameters
template multiArg(a, b, c, d, e) {
    a + b + c + d + e;
}

# Rule testing edge cases
edgeCaseRule {
    noop();
    x = identity(42);
    sum = multiArg(1, 2, 3, 4, 5);
    writeLine("stdout", "edgeCaseRule completed");
}
