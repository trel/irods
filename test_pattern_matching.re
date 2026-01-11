#!/usr/bin/env irule
# Test pattern matching with type checking

# Test 1: Basic type pattern matching
test_basic_type_pattern(*data, *result) {
    match *data with
        (int) => { *result = "matched integer"; }
        | (string) => { *result = "matched string"; }
        | (double) => { *result = "matched double"; }
        | (_) => { *result = "matched other type"; }
}

# Test 2: Type narrowing in pattern body
test_type_narrowing(*value, *result) {
    match *value with
        (int) => {
            # After matching (int), *value has int type
            *x = *value + 10;
            *result = "int narrowed: " ++ str(*x);
        }
        | (string) => {
            *result = "string: " ++ *value;
        }
}

# Test 3: Wildcard pattern
test_wildcard_pattern(*data, *result) {
    match *data with
        (int) => { *result = "int"; }
        | (string) => { *result = "string"; }
        | (_) => { *result = "default"; }
}

# Test 4: Safe casting pattern
test_safe_casting(*data, *result) {
    match *data with
        (int) => {
            # Safe to use as integer
            *result = "count=" ++ str(*data + 1);
        }
        | (string) => {
            *result = "text=" ++ *data;
        }
        | (_) => {
            *result = "unknown type";
        }
}

# Test 5: Pattern matching on dynamic types
test_dynamic_type_matching(*dynamic_val, *result) {
    # dynamic_val could be any type (T_DYNAMIC)
    match *dynamic_val with
        (int) => {
            *x = *dynamic_val * 2;
            *result = str(*x);
        }
        | (string) => {
            *result = "len=" ++ str(strlen(*dynamic_val));
        }
        | (double) => {
            *x = *dynamic_val * 3.14;
            *result = str(*x);
        }
        | (_) => {
            *result = "unknown";
        }
}

# Test 6: Multiple cases with same type
test_multiple_int_cases(*value, *result) {
    match *value with
        (int) => {
            if (*value > 100) {
                *result = "large";
            } else if (*value > 0) {
                *result = "positive";
            } else {
                *result = "non-positive";
            }
        }
        | (string) => {
            *result = "string value";
        }
}

# Test 7: Pattern with function call
test_pattern_with_call(*data, *result) {
    match *data with
        (int) => {
            *x = abs(*data);
            *result = str(*x);
        }
        | (double) => {
            *x = abs(*data);
            *result = str(*x);
        }
        | (string) => {
            *result = "string length: " ++ str(strlen(*data));
        }
}

# Test 8: Nested type patterns (for future enhancement)
test_nested_pattern(*data, *result) {
    match *data with
        (int) => { *result = "int"; }
        | (string) => { *result = "string"; }
}

# Test 9: Error handling with pattern matching
test_error_pattern(*code, *result) {
    match *code with
        (int) => {
            if (*code == 0) {
                *result = "success";
            } else {
                *result = "error code: " ++ str(*code);
            }
        }
        | (_) => {
            *result = "invalid error code type";
        }
}

# Test 10: Pattern matching combined with conditionals
test_conditional_pattern(*value, *result) {
    match *value with
        (int) => {
            *x = *value + 5;
            if (*x > 10) {
                *result = "large: " ++ str(*x);
            } else {
                *result = "small: " ++ str(*x);
            }
        }
        | (string) => {
            if (strlen(*value) > 5) {
                *result = "long string";
            } else {
                *result = "short string";
            }
        }
}

# Main test runner
main {
    writeLine("Testing pattern matching with type checking...");
    
    # Test with integer
    test_basic_type_pattern(42, *r1);
    writeLine("Test 1 (int): " ++ *r1);
    
    # Test with string
    test_basic_type_pattern("hello", *r2);
    writeLine("Test 1 (string): " ++ *r2);
    
    # Test with double
    test_basic_type_pattern(3.14, *r3);
    writeLine("Test 1 (double): " ++ *r3);
    
    # Test type narrowing
    test_type_narrowing(100, *r4);
    writeLine("Test 2 (narrowing): " ++ *r4);
    
    # Test wildcard
    test_wildcard_pattern(99, *r5);
    writeLine("Test 3 (wildcard): " ++ *r5);
    
    # Test safe casting
    test_safe_casting(5, *r6);
    writeLine("Test 4 (safe cast): " ++ *r6);
    
    # Test dynamic matching
    test_dynamic_type_matching(2.71, *r7);
    writeLine("Test 5 (dynamic): " ++ *r7);
    
    writeLine("Pattern matching tests completed");
}
