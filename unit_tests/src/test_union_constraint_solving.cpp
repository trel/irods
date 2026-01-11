/**
 * @file test_union_constraint_solving.cpp
 * @brief Unit tests for union type handling in the constraint solver
 *
 * Tests how the constraint solver handles union types during type inference
 * and constraint solving for query results and attributes.
 *
 * @author iRODS Development Team
 * @date 2026-01-12
 */

#include <catch2/catch_all.hpp>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include <cstring>

#include "irods/private/re/restructs.hpp"
#include "irods/private/re/typing.hpp"
#include "irods/private/re/utils.hpp"
#include "irods/region.h"

/**
 * @brief Test union type narrowing with constraint solving
 * 
 * When a union type is used in a constraint with a concrete type,
 * the constraint solver should check if the concrete type matches
 * any disjunct in the union.
 */
TEST_CASE("union_constraint: concrete type matches union disjunct")
{
    Region *r = make_region(8192, NULL);
    REQUIRE(r != NULL);

    // Create a union: int | string | dynamic
    ExprType *intType = newSimpType(T_INT, r);
    ExprType *stringType = newSimpType(T_STRING, r);
    ExprType *dynamicType = newSimpType(T_DYNAMIC, r);

    ExprType **types = (ExprType **)region_alloc(r, sizeof(ExprType *) * 3);
    types[0] = intType;
    types[1] = stringType;
    types[2] = dynamicType;

    ExprType *unionType = newUnionType(3, types, r);
    REQUIRE(unionType != NULL);

    // Verify the union structure
    CHECK(getNodeType(unionType) == T_VAR);
    CHECK(T_VAR_NUM_DISJUNCTS(unionType) == 3);

    // When checking if int matches the union, it should succeed
    // (because int is one of the disjuncts)
    ExprType *intToCheck = newSimpType(T_INT, r);
    CHECK(getNodeType(intToCheck) == T_INT);

    // In the constraint solver, this would be handled by simplifyLocally()
    // For now, we just verify the union structure supports matching

    region_free(r);
}

/**
 * @brief Test union type creation for query results
 * 
 * Query results with unknown columns should produce union types
 * that represent "could be int or string or dynamic"
 */
TEST_CASE("union_constraint: query result with unknown columns")
{
    Region *r = make_region(8192, NULL);
    REQUIRE(r != NULL);

    // Simulate a query with mixed/unknown column types
    // If columns are [COLL_NAME (string), UNKNOWN_COL, DATA_SIZE (int)]
    // The result type should be tuple<string, union<...>, int>

    ExprType *stringType = newSimpType(T_STRING, r);
    ExprType *intType = newSimpType(T_INT, r);
    
    // For unknown column, create a union as fallback
    ExprType *dynamicType = newSimpType(T_DYNAMIC, r);
    ExprType **unionTypes = (ExprType **)region_alloc(r, sizeof(ExprType *) * 2);
    unionTypes[0] = intType;
    unionTypes[1] = stringType;
    ExprType *unknownUnion = newUnionType(2, unionTypes, r);
    REQUIRE(unknownUnion != NULL);

    // Create tuple: <string, union<int|string>, int>
    ExprType **tupleTypes = (ExprType **)region_alloc(r, sizeof(ExprType *) * 3);
    tupleTypes[0] = stringType;
    tupleTypes[1] = unknownUnion;
    tupleTypes[2] = intType;

    ExprType *tupleType = newTupleType(3, tupleTypes, r);
    REQUIRE(tupleType != NULL);

    CHECK(getNodeType(tupleType) == T_TUPLE);
    CHECK(tupleType->degree == 3);

    region_free(r);
}

/**
 * @brief Test type variable with union constraints
 * 
 * A type variable constrained by a union should narrow its possibilities
 * to the union's disjuncts
 */
TEST_CASE("union_constraint: type variable constrained by union")
{
    Region *r = make_region(8192, NULL);
    REQUIRE(r != NULL);

    // Create a plain type variable (no disjuncts initially)
    ExprType *typeVar = newTVar(r);
    REQUIRE(typeVar != NULL);
    CHECK(T_VAR_NUM_DISJUNCTS(typeVar) == 0);

    // Create union: int | string
    ExprType *intType = newSimpType(T_INT, r);
    ExprType *stringType = newSimpType(T_STRING, r);
    ExprType **types = (ExprType **)region_alloc(r, sizeof(ExprType *) * 2);
    types[0] = intType;
    types[1] = stringType;
    ExprType *unionType = newUnionType(2, types, r);
    REQUIRE(unionType != NULL);

    // In constraint solver, constraint "typeVar < union<int|string>"
    // would narrow typeVar's possibilities to {int, string}
    
    // For now, verify that both types exist in the union
    CHECK(T_VAR_NUM_DISJUNCTS(unionType) == 2);
    CHECK(getNodeType(T_VAR_DISJUNCT(unionType, 0)) == T_INT);
    CHECK(getNodeType(T_VAR_DISJUNCT(unionType, 1)) == T_STRING);

    region_free(r);
}

/**
 * @brief Test union operation type checking
 * 
 * Operations on union types should be valid only if all disjuncts
 * support the operation
 */
TEST_CASE("union_constraint: operation compatibility with union")
{
    Region *r = make_region(8192, NULL);
    REQUIRE(r != NULL);

    // Union: string | int
    // String concatenation should fail because int doesn't support it
    ExprType *stringType = newSimpType(T_STRING, r);
    ExprType *intType = newSimpType(T_INT, r);

    ExprType **types = (ExprType **)region_alloc(r, sizeof(ExprType *) * 2);
    types[0] = stringType;
    types[1] = intType;

    ExprType *unionType = newUnionType(2, types, r);
    REQUIRE(unionType != NULL);

    // Verify the union has both types
    CHECK(T_VAR_NUM_DISJUNCTS(unionType) == 2);

    // In constraint solver, attempting string operation on this union
    // would fail because int is not a string type

    region_free(r);
}

/**
 * @brief Test union type in collection
 * 
 * Query results are collections of tuples, where tuple elements
 * may be union types for unknown columns
 */
TEST_CASE("union_constraint: union in collection type")
{
    Region *r = make_region(8192, NULL);
    REQUIRE(r != NULL);

    // Create collection<tuple<string, union<int|string>>>
    // This represents: "collection of rows, where each row has
    // (string column, unknown column that could be int or string)"

    ExprType *stringType = newSimpType(T_STRING, r);
    ExprType *intType = newSimpType(T_INT, r);

    // Create union for unknown column
    ExprType **unionTypes = (ExprType **)region_alloc(r, sizeof(ExprType *) * 2);
    unionTypes[0] = intType;
    unionTypes[1] = stringType;
    ExprType *unionType = newUnionType(2, unionTypes, r);
    REQUIRE(unionType != NULL);

    // Create tuple
    ExprType **tupleTypes = (ExprType **)region_alloc(r, sizeof(ExprType *) * 2);
    tupleTypes[0] = stringType;
    tupleTypes[1] = unionType;
    ExprType *tupleType = newTupleType(2, tupleTypes, r);
    REQUIRE(tupleType != NULL);

    // Wrap in collection
    ExprType *collType = newCollType(tupleType, r);
    REQUIRE(collType != NULL);

    CHECK(getNodeType(collType) == T_CONS);
    CHECK(collType->degree == 1);

    region_free(r);
}

/**
 * @brief Test multiple union types
 * 
 * A query with multiple unknown columns should have multiple unions
 */
TEST_CASE("union_constraint: multiple unions in tuple")
{
    Region *r = make_region(8192, NULL);
    REQUIRE(r != NULL);

    // Create tuple: <string, union<int|string>, union<int|double>>
    ExprType *stringType = newSimpType(T_STRING, r);
    ExprType *intType = newSimpType(T_INT, r);
    ExprType *doubleType = newSimpType(T_DOUBLE, r);

    // First union: int | string
    ExprType **union1Types = (ExprType **)region_alloc(r, sizeof(ExprType *) * 2);
    union1Types[0] = intType;
    union1Types[1] = stringType;
    ExprType *union1 = newUnionType(2, union1Types, r);
    REQUIRE(union1 != NULL);

    // Second union: int | double
    ExprType **union2Types = (ExprType **)region_alloc(r, sizeof(ExprType *) * 2);
    union2Types[0] = intType;
    union2Types[1] = doubleType;
    ExprType *union2 = newUnionType(2, union2Types, r);
    REQUIRE(union2 != NULL);

    // Create tuple
    ExprType **tupleTypes = (ExprType **)region_alloc(r, sizeof(ExprType *) * 3);
    tupleTypes[0] = stringType;
    tupleTypes[1] = union1;
    tupleTypes[2] = union2;
    ExprType *tupleType = newTupleType(3, tupleTypes, r);
    REQUIRE(tupleType != NULL);

    CHECK(getNodeType(tupleType) == T_TUPLE);
    CHECK(tupleType->degree == 3);

    // Verify union disjuncts
    ExprType *elem1 = tupleTypes[1];
    ExprType *elem2 = tupleTypes[2];
    
    CHECK(getNodeType(elem1) == T_VAR);
    CHECK(T_VAR_NUM_DISJUNCTS(elem1) == 2);
    
    CHECK(getNodeType(elem2) == T_VAR);
    CHECK(T_VAR_NUM_DISJUNCTS(elem2) == 2);

    region_free(r);
}

/**
 * @brief Test dynamic fallback in union
 * 
 * When column type is completely unknown, union should include T_DYNAMIC
 */
TEST_CASE("union_constraint: dynamic fallback in union")
{
    Region *r = make_region(8192, NULL);
    REQUIRE(r != NULL);

    // Create union with T_DYNAMIC: int | string | dynamic
    ExprType *intType = newSimpType(T_INT, r);
    ExprType *stringType = newSimpType(T_STRING, r);
    ExprType *dynamicType = newSimpType(T_DYNAMIC, r);

    ExprType **types = (ExprType **)region_alloc(r, sizeof(ExprType *) * 3);
    types[0] = intType;
    types[1] = stringType;
    types[2] = dynamicType;

    ExprType *unionType = newUnionType(3, types, r);
    REQUIRE(unionType != NULL);

    // Verify dynamic is included
    CHECK(T_VAR_NUM_DISJUNCTS(unionType) == 3);
    
    // Find T_DYNAMIC in disjuncts
    bool hasDynamic = false;
    for (int i = 0; i < T_VAR_NUM_DISJUNCTS(unionType); i++) {
        if (getNodeType(T_VAR_DISJUNCT(unionType, i)) == T_DYNAMIC) {
            hasDynamic = true;
            break;
        }
    }
    CHECK(hasDynamic);

    region_free(r);
}
