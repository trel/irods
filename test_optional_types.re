# Test rules for optional type syntax (@optional, @nonnull)

# Test 1: Basic @optional type parameter
test_optional_param(@optional int x) {
    writeLine("stdout", "Got optional int: " ++ str(x));
}

# Test 2: Basic @nonnull type parameter
test_nonnull_param(@nonnull string s) {
    writeLine("stdout", "Got nonnull string: " ++ s);
}

# Test 3: Optional function return type
testOptionalReturn() -> @optional int {
    return 42;
}

# Test 4: Nonnull function return type
testNonnullReturn() -> @nonnull string {
    return "hello";
}

# Test 5: Mixed parameters
test_mixed_params(@nonnull int a, @optional string b) {
    writeLine("stdout", "a=" ++ str(a) ++ ", b=" ++ if(b == "", "null", b));
}

# Test 6: Optional in tuple type
test_optional_tuple() {
    x = (@optional int, string);
    writeLine("stdout", "Tuple type created");
}

# Test 7: Multiple optional annotations should not conflict
test_multiple_optional(@optional int x, @optional string y) {
    writeLine("stdout", "Both optional");
}

# Test 8: Nonnull preservation in assignments
test_nonnull_assignment(@nonnull int x) {
    y = x;
    writeLine("stdout", "Assigned from nonnull: " ++ str(y));
}

# Test 9: Optional type in variable declaration
test_optional_var() {
    *optionalVal : @optional int;
    writeLine("stdout", "Optional variable declared");
}

# Test 10: Type annotation in function signature
testAnnotationInSignature(@optional int param1, @nonnull string param2) -> @nonnull int {
    return 1;
}
