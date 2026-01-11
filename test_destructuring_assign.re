/*
 * Test file for destructuring assignment syntax
 * Tests the {a, b, c} = tuple_expr pattern
 */

/* Test 1: Basic tuple destructuring */
test_basic_destructuring {
    *tuple = (10, 20, 30);
    {*a, *b, *c} = *tuple;
    if (*a == 10 && *b == 20 && *c == 30) {
        writeLine("stdout", "TEST 1 PASS: Basic destructuring");
    } else {
        writeLine("stdout", "TEST 1 FAIL: Basic destructuring");
    }
}

/* Test 2: Destructuring with wildcards */
test_destructuring_wildcards {
    *tuple = (100, 200, 300, 400);
    {*x, _, *y, _} = *tuple;
    if (*x == 100 && *y == 300) {
        writeLine("stdout", "TEST 2 PASS: Destructuring with wildcards");
    } else {
        writeLine("stdout", "TEST 2 FAIL: Destructuring with wildcards");
    }
}

/* Test 3: Nested tuple destructuring */
test_destructuring_nested {
    *inner = (5, 6);
    *outer = (1, *inner, 3);
    {*a, {*b, *c}, *d} = *outer;
    if (*a == 1 && *b == 5 && *c == 6 && *d == 3) {
        writeLine("stdout", "TEST 3 PASS: Nested destructuring");
    } else {
        writeLine("stdout", "TEST 3 FAIL: Nested destructuring");
    }
}

/* Test 4: Destructuring with partial patterns (fewer vars than tuple elements) */
test_destructuring_partial {
    *tuple = (10, 20, 30, 40, 50);
    {*a, *b, *c} = *tuple;
    if (*a == 10 && *b == 20 && *c == 30) {
        writeLine("stdout", "TEST 4 PASS: Partial destructuring");
    } else {
        writeLine("stdout", "TEST 4 FAIL: Partial destructuring");
    }
}

/* Test 5: Destructuring with string tuple */
test_destructuring_strings {
    *tuple = ("hello", "world", "test");
    {*s1, *s2, *s3} = *tuple;
    if (*s1 == "hello" && *s2 == "world" && *s3 == "test") {
        writeLine("stdout", "TEST 5 PASS: String tuple destructuring");
    } else {
        writeLine("stdout", "TEST 5 FAIL: String tuple destructuring");
    }
}

/* Test 6: Mixed type tuple destructuring */
test_destructuring_mixed {
    *tuple = (42, "text", 3.14);
    {*num, *str, *float} = *tuple;
    if (*num == 42 && *str == "text" && abs(*float - 3.14) < 0.01) {
        writeLine("stdout", "TEST 6 PASS: Mixed type destructuring");
    } else {
        writeLine("stdout", "TEST 6 FAIL: Mixed type destructuring");
    }
}

/* Test 7: Destructuring in function composition */
get_pair {
    *result = (100, 200);
}

test_destructuring_function {
    get_pair;
    {*x, *y} = *result;
    if (*x == 100 && *y == 200) {
        writeLine("stdout", "TEST 7 PASS: Function result destructuring");
    } else {
        writeLine("stdout", "TEST 7 FAIL: Function result destructuring");
    }
}

/* Test 8: Empty pattern destructuring */
test_destructuring_empty {
    *tuple = ();
    {} = *tuple;
    writeLine("stdout", "TEST 8 PASS: Empty pattern destructuring");
}

/* Test 9: Single element destructuring */
test_destructuring_single {
    *tuple = (999,);
    {*val} = *tuple;
    if (*val == 999) {
        writeLine("stdout", "TEST 9 PASS: Single element destructuring");
    } else {
        writeLine("stdout", "TEST 9 FAIL: Single element destructuring");
    }
}

/* Test 10: Reuse destructured variables */
test_destructuring_reuse {
    *tuple1 = (1, 2, 3);
    *tuple2 = (10, 20, 30);
    {*a, *b, *c} = *tuple1;
    {*a, *b, *c} = *tuple2;
    if (*a == 10 && *b == 20 && *c == 30) {
        writeLine("stdout", "TEST 10 PASS: Destructured variable reuse");
    } else {
        writeLine("stdout", "TEST 10 FAIL: Destructured variable reuse");
    }
}
