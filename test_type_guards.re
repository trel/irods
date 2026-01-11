% ============================================================================
% Type Guard Predicates Test Suite (i-d43d)
% ============================================================================
%
% Comprehensive tests for type guard predicates enabling safe type narrowing
% in conditionals and pattern matching.
%

% Test 1: Basic is_int predicate
testIsInt(*value, *result) {
    if (is_int(*value)) {
        *result = "passed";
    } else {
        *result = "failed";
    }
}

% Test 2: Basic is_string predicate
testIsString(*value, *result) {
    if (is_string(*value)) {
        *result = "passed";
    } else {
        *result = "failed";
    }
}

% Test 3: Negation guard: !is_null
testIsNotNull(*maybeValue, *result) {
    if (!is_null(*maybeValue)) {
        *result = "not null";
    } else {
        *result = "is null";
    }
}

% Test 4: Type narrowing in if-then
testTypeNarrowingThen(*value, *result) {
    if (is_int(*value)) {
        % In this scope, *value should be narrowed to int
        *result = *value + 1;
    } else {
        *result = 0;
    }
}

% Test 5: Type narrowing in else branch
testTypeNarrowingElse(*value, *result) {
    if (!is_int(*value)) {
        *result = "not int";
    } else {
        % *value narrowed to int
        *result = *value * 2;
    }
}

% Test 6: Conjunction guards (&&)
testConjunctionGuards(*value, *result) {
    if (is_int(*value) && *value > 0) {
        *result = "positive int";
    } else {
        *result = "not positive int";
    }
}

% Test 7: Disjunction guards (||)
testDisjunctionGuards(*value, *result) {
    if (is_int(*value) || is_string(*value)) {
        *result = "int or string";
    } else {
        *result = "other type";
    }
}

% Test 8: is_bool predicate
testIsBool(*value, *result) {
    if (is_bool(*value)) {
        *result = "boolean";
    } else {
        *result = "not boolean";
    }
}

% Test 9: is_double predicate
testIsDouble(*value, *result) {
    if (is_double(*value)) {
        *result = "double";
    } else {
        *result = "not double";
    }
}

% Test 10: is_list predicate
testIsList(*value, *result) {
    if (is_list(*value)) {
        *result = "list type";
    } else {
        *result = "not list";
    }
}

% Test 11: is_optional with @optional annotation
testIsOptional(*maybeValue, *result) {
    if (is_optional(*maybeValue)) {
        *result = "optional";
    } else {
        *result = "not optional";
    }
}

% Test 12: is_nonnull predicate
testIsNonnull(*value, *result) {
    if (is_nonnull(*value)) {
        *result = "non-null";
    } else {
        *result = "null";
    }
}

% Test 13: is_datetime predicate
testIsDatetime(*value, *result) {
    if (is_datetime(*value)) {
        *result = "datetime";
    } else {
        *result = "not datetime";
    }
}

% Test 14: is_path predicate
testIsPath(*value, *result) {
    if (is_path(*value)) {
        *result = "path type";
    } else {
        *result = "not path";
    }
}

% Test 15: Multiple type checks in sequence
testMultipleTypeChecks(*value, *result) {
    if (is_int(*value)) {
        *result = "int";
    } else if (is_string(*value)) {
        *result = "string";
    } else if (is_bool(*value)) {
        *result = "bool";
    } else {
        *result = "unknown";
    }
}

% Test 16: is_dynamic for unresolved types
testIsDynamic(*value, *result) {
    if (is_dynamic(*value)) {
        *result = "dynamic";
    } else {
        *result = "typed";
    }
}

% Test 17: Guard with function call
testGuardWithFunctionCall(*str, *result) {
    if (is_string(*str) && strlen(*str) > 0) {
        *result = "non-empty string";
    } else {
        *result = "empty or non-string";
    }
}

% Test 18: Nested guards
testNestedGuards(*outer, *inner, *result) {
    if (is_int(*outer)) {
        if (is_int(*inner)) {
            *result = "both int";
        } else {
            *result = "outer int only";
        }
    } else {
        *result = "outer not int";
    }
}

% Test 19: Pattern matching with guards
testPatternWithGuard(*value, *result) {
    % Pattern matching provides type patterns like (int)
    % Guards can further refine the match
    match *value with
        | (int) =>
            if (*value > 0) {
                *result = "positive integer";
            } else {
                *result = "non-positive integer";
            }
        | (string) =>
            *result = "matched string";
        | _ =>
            *result = "unknown pattern";
}

% Test 20: Scope boundary - narrowing ends after if block
testScopeBoundary(*value, *result) {
    if (is_int(*value)) {
        % *value is int in this scope
        *x = *value + 1;
    }
    % After the if block, narrowing scope ends
    % *value reverts to original type (which may be different)
    if (is_int(*value)) {
        *result = "still int";
    } else {
        *result = "not int after scope";
    }
}

% Test 21: Guard result variable reuse
testGuardVariable(*value, *result) {
    *isInteger = is_int(*value);
    if (*isInteger) {
        *result = "guard passed";
    } else {
        *result = "guard failed";
    }
}

% Test 22: Complex boolean expression with guards
testComplexBoolean(*a, *b, *result) {
    if ((is_int(*a) && *a > 0) || (is_string(*b) && strlen(*b) > 0)) {
        *result = "at least one condition passed";
    } else {
        *result = "no condition passed";
    }
}

% Test 23: is_null on uninitialized variable
testIsNullUninitialized(*result) {
    % *undef should be uninitialized
    if (is_null(*undef)) {
        *result = "uninitialized is null";
    } else {
        *result = "uninitialized is not null";
    }
}

% Test 24: Chained guard checks
testChainedGuards(*value, *result) {
    if (is_nonnull(*value)) {
        if (is_int(*value)) {
            if (*value > 0) {
                *result = "non-null positive int";
            } else {
                *result = "non-null non-positive int";
            }
        } else {
            *result = "non-null non-int";
        }
    } else {
        *result = "null";
    }
}

% Test 25: Guard predicate with variable assignment
testGuardWithAssignment(*inputValue, *result) {
    *value = *inputValue;
    if (is_string(*value)) {
        *length = strlen(*value);
        *result = "string length:" ++ "*length";
    } else {
        *result = "not string";
    }
}

% Main test execution
main {
    % Basic type tests
    testIsInt(42, *r1);
    writeLine("stdout", "*r1");
    
    testIsString("hello", *r2);
    writeLine("stdout", "*r2");
    
    testIsBool(true, *r3);
    writeLine("stdout", "*r3");
    
    % Null tests
    testIsNotNull("value", *r4);
    writeLine("stdout", "*r4");
    
    % Type narrowing
    testTypeNarrowingThen(10, *r5);
    writeLine("stdout", "*r5");
    
    testConjunctionGuards(5, *r6);
    writeLine("stdout", "*r6");
    
    % Multiple type checks
    testMultipleTypeChecks(100, *r7);
    writeLine("stdout", "*r7");
    
    testMultipleTypeChecks("string", *r8);
    writeLine("stdout", "*r8");
    
    % Pattern with guards
    testPatternWithGuard(42, *r9);
    writeLine("stdout", "*r9");
    
    % Complex tests
    testChainedGuards(42, *r10);
    writeLine("stdout", "*r10");
}
