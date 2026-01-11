# Test rules for optional chaining operator (?. syntax)
# Note: These are syntax-only tests; actual execution will be tested later
# in Phase 3 (Code Generation). This file verifies parser can handle the syntax.

# Test 1: Basic optional field access
test_optional_field_access {
    obj = get_user_object();
    name = obj?.name;
}

# Test 2: Optional chaining on null variable
test_optional_on_optional_var {
    user : @optional User;
    email = user?.email;
}

# Test 3: Chained optional access
test_chained_optional_access {
    config = get_config();
    path = config?.database?.location?.path;
}

# Test 4: Optional field with fallback
test_optional_with_fallback {
    obj = get_object();
    name = obj?.name || "Unknown";
}

# Test 5: Optional index access
test_optional_index_access {
    arr = get_optional_array();
    first = arr?[0];
}

# Test 6: Chained index access
test_chained_index_access {
    matrix = get_matrix();
    value = matrix?[0]?[1];
}

# Test 7: Optional method call (no args)
test_optional_method_call {
    obj = get_object();
    result = obj?.toString();
}

# Test 8: Optional method call (with args)
test_optional_method_call_args {
    obj = get_object();
    substring = obj?.substring(0, 5);
}

# Test 9: Mixed field and index access
test_mixed_chain {
    data = get_data();
    value = data?.items?[0]?.name?.length;
}

# Test 10: Optional in condition
test_optional_in_condition {
    user = get_optional_user();
    if (user?.admin == true) {
        writeLine("stdout", "User is admin");
    }
}

# Test 11: Optional in function call
test_optional_in_function_arg {
    obj = get_object();
    result = process(obj?.id, obj?.name);
}

# Test 12: Multiple optional chains in expression
test_multiple_chains {
    obj1 = get_object1();
    obj2 = get_object2();
    combined = obj1?.value + obj2?.value;
}

# Test 13: Optional chaining with type annotation
test_optional_with_type {
    user : @optional User;
    name : @optional string = user?.name;
}

# Test 14: Optional chaining in tuple construction
test_optional_in_tuple {
    obj = get_object();
    tuple = (obj?.id, obj?.name, obj?.email);
}

# Test 15: Optional access on nested optional
test_nested_optional_type {
    data : @optional @optional Data;
    value = data?.field;
}

# Test 16: Regular field access (control test)
test_regular_field_access {
    user = get_user();
    name = user.name;
}

# Test 17: Mixed regular and optional access
test_mixed_regular_optional {
    config = get_config();
    value = config.database?.host;
}

# Test 18: Optional chaining in assignment
test_optional_in_assignment {
    obj = get_object();
    x = 5;
    y = obj?.getValue(x);
}

# Test 19: Optional with nonnull type
test_optional_on_nonnull {
    user : @nonnull User;
    name = user?.name;
}

# Test 20: Deep nesting optional chains
test_deep_nesting {
    root = get_root();
    deeply_nested = root?.level1?.level2?.level3?.level4?.level5?.data;
}
