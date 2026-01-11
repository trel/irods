/* Simple destructuring tests */

test_destruct_1 {
    *t = (10, 20);
    {*a, *b} = *t;
    if (*a == 10 && *b == 20) {
        writeLine("stdout", "PASS: Basic destructuring works");
    } else {
        writeLine("stdout", "FAIL: Basic destructuring - got *a=*a *b=*b");
    }
}

test_destruct_2 {
    *t = (1, 2, 3, 4, 5);
    {*x, *y} = *t;
    if (*x == 1 && *y == 2) {
        writeLine("stdout", "PASS: Partial destructuring works");
    } else {
        writeLine("stdout", "FAIL: Partial destructuring");
    }
}

test_destruct_3 {
    *inner = (5, 6);
    *outer = (1, *inner, 3);
    {*a, {*b, *c}, *d} = *outer;
    if (*a == 1 && *b == 5 && *c == 6 && *d == 3) {
        writeLine("stdout", "PASS: Nested destructuring works");
    } else {
        writeLine("stdout", "FAIL: Nested destructuring");
    }
}
