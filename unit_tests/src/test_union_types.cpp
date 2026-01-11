/**
 * @file test_union_types.cpp
 * @brief Unit tests for union type representation and operations
 *
 * Tests the union type system design (i-0435) which uses T_VAR nodes with
 * disjuncts as a safe fallback for query and attribute types.
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
 * @brief Test basic union type creation
 */
TEST_CASE("union_types: create binary union")
{
    Region *r = make_region(8192, NULL);
    REQUIRE(r != NULL);

    // Create a union of int and string types
    ExprType *intType = newSimpType(T_INT, r);
    ExprType *stringType = newSimpType(T_STRING, r);
    REQUIRE(intType != NULL);
    REQUIRE(stringType != NULL);

    ExprType **types = (ExprType **)region_alloc(r, sizeof(ExprType *) * 2);
    types[0] = intType;
    types[1] = stringType;

    ExprType *unionType = newUnionType(2, types, r);
    REQUIRE(unionType != NULL);

    // Verify structure
    CHECK(getNodeType(unionType) == T_VAR);
    CHECK(T_VAR_NUM_DISJUNCTS(unionType) == 2);
    CHECK(T_VAR_DISJUNCT(unionType, 0) == intType);
    CHECK(T_VAR_DISJUNCT(unionType, 1) == stringType);

    region_free(r);
}

/**
 * @brief Test union with many types
 */
TEST_CASE("union_types: create multi-type union")
{
    Region *r = make_region(8192, NULL);
    REQUIRE(r != NULL);

    // Create union of 5 types
    ExprType *types_array[] = {
        newSimpType(T_INT, r),
        newSimpType(T_DOUBLE, r),
        newSimpType(T_STRING, r),
        newSimpType(T_BOOL, r),
        newSimpType(T_DYNAMIC, r),
    };

    for (int i = 0; i < 5; i++) {
        REQUIRE(types_array[i] != NULL);
    }

    ExprType **types = (ExprType **)region_alloc(r, sizeof(ExprType *) * 5);
    std::memcpy(types, types_array, sizeof(types_array));

    ExprType *unionType = newUnionType(5, types, r);
    REQUIRE(unionType != NULL);

    CHECK(T_VAR_NUM_DISJUNCTS(unionType) == 5);
    for (int i = 0; i < 5; i++) {
        CHECK(T_VAR_DISJUNCT(unionType, i) == types_array[i]);
    }

    region_free(r);
}

/**
 * @brief Test union type formatting for error messages
 */
TEST_CASE("union_types: format union type name")
{
    Region *r = make_region(8192, NULL);
    REQUIRE(r != NULL);

    // Create union: int | string | dynamic
    ExprType *intType = newSimpType(T_INT, r);
    ExprType *stringType = newSimpType(T_STRING, r);
    ExprType *dynamicType = newSimpType(T_DYNAMIC, r);

    ExprType **types = (ExprType **)region_alloc(r, sizeof(ExprType *) * 3);
    types[0] = intType;
    types[1] = stringType;
    types[2] = dynamicType;

    ExprType *unionType = newUnionType(3, types, r);
    REQUIRE(unionType != NULL);

    // Format the union type name
    char buf[512];
    char *result = getUnionTypeName(unionType, buf);
    REQUIRE(result != NULL);

    // Check format: union<int|string|dynamic>
    std::string typeStr(buf);
    CHECK(typeStr.find("union<") == 0);
    CHECK(typeStr.find("int") != std::string::npos);
    CHECK(typeStr.find("string") != std::string::npos);
    CHECK(typeStr.find("dynamic") != std::string::npos);
    CHECK(typeStr.find(">") != std::string::npos);

    region_free(r);
}

/**
 * @brief Test union with IRODS type
 */
TEST_CASE("union_types: union with IRODS type")
{
    Region *r = make_region(8192, NULL);
    REQUIRE(r != NULL);

    ExprType *stringType = newSimpType(T_STRING, r);
    ExprType *irodsType = newIRODSType("genQueryOut_t", r);
    REQUIRE(stringType != NULL);
    REQUIRE(irodsType != NULL);

    ExprType **types = (ExprType **)region_alloc(r, sizeof(ExprType *) * 2);
    types[0] = stringType;
    types[1] = irodsType;

    ExprType *unionType = newUnionType(2, types, r);
    REQUIRE(unionType != NULL);

    CHECK(T_VAR_NUM_DISJUNCTS(unionType) == 2);

    // Format for error messages
    char buf[512];
    char *result = getUnionTypeName(unionType, buf);
    REQUIRE(result != NULL);

    std::string typeStr(buf);
    CHECK(typeStr.find("string") != std::string::npos);
    CHECK(typeStr.find("irods") != std::string::npos);
    CHECK(typeStr.find("genQueryOut_t") != std::string::npos);

    region_free(r);
}

/**
 * @brief Test error handling for invalid union inputs
 */
TEST_CASE("union_types: invalid union inputs")
{
    Region *r = make_region(8192, NULL);
    REQUIRE(r != NULL);

    // NULL types array
    ExprType *result1 = newUnionType(5, NULL, r);
    CHECK(result1 == NULL);

    // Zero arity
    ExprType *intType = newSimpType(T_INT, r);
    ExprType **types = (ExprType **)region_alloc(r, sizeof(ExprType *));
    types[0] = intType;

    ExprType *result2 = newUnionType(0, types, r);
    CHECK(result2 == NULL);

    // Negative arity (should be treated as invalid)
    ExprType *result3 = newUnionType(-1, types, r);
    CHECK(result3 == NULL);

    region_free(r);
}

/**
 * @brief Test getUnionTypeName with NULL input
 */
TEST_CASE("union_types: format NULL union type")
{
    char buf[512];
    char *result = getUnionTypeName(NULL, buf);
    REQUIRE(result != NULL);
    CHECK(std::string(buf) == "invalid_union");
}

/**
 * @brief Test empty union type formatting
 */
TEST_CASE("union_types: format empty union")
{
    Region *r = make_region(8192, NULL);
    REQUIRE(r != NULL);

    // Create empty union (0 disjuncts)
    ExprType *emptyUnion = newTVar2(0, NULL, r);
    REQUIRE(emptyUnion != NULL);

    char buf[512];
    char *result = getUnionTypeName(emptyUnion, buf);
    REQUIRE(result != NULL);
    CHECK(std::string(buf) == "union<>");

    region_free(r);
}

/**
 * @brief Test union type with type variables
 */
TEST_CASE("union_types: union containing type variables")
{
    Region *r = make_region(8192, NULL);
    REQUIRE(r != NULL);

    ExprType *stringType = newSimpType(T_STRING, r);
    ExprType *typeVar = newTVar(r);
    REQUIRE(stringType != NULL);
    REQUIRE(typeVar != NULL);

    ExprType **types = (ExprType **)region_alloc(r, sizeof(ExprType *) * 2);
    types[0] = stringType;
    types[1] = typeVar;

    ExprType *unionType = newUnionType(2, types, r);
    REQUIRE(unionType != NULL);

    CHECK(T_VAR_NUM_DISJUNCTS(unionType) == 2);
    CHECK(getNodeType(T_VAR_DISJUNCT(unionType, 0)) == T_STRING);
    CHECK(getNodeType(T_VAR_DISJUNCT(unionType, 1)) == T_VAR);

    // Format should include both types
    char buf[512];
    char *result = getUnionTypeName(unionType, buf);
    REQUIRE(result != NULL);
    std::string typeStr(buf);
    CHECK(typeStr.find("string") != std::string::npos);
    CHECK(typeStr.find("?") != std::string::npos); // Type variable format

    region_free(r);
}

/**
 * @brief Test distinguishing union types from plain type variables
 */
TEST_CASE("union_types: distinguish union from type variable")
{
    Region *r = make_region(8192, NULL);
    REQUIRE(r != NULL);

    // Plain type variable (no disjuncts)
    ExprType *plainTVar = newTVar(r);
    REQUIRE(plainTVar != NULL);
    CHECK(T_VAR_NUM_DISJUNCTS(plainTVar) == 0);

    // Union type (multiple disjuncts)
    ExprType *intType = newSimpType(T_INT, r);
    ExprType *stringType = newSimpType(T_STRING, r);
    ExprType **types = (ExprType **)region_alloc(r, sizeof(ExprType *) * 2);
    types[0] = intType;
    types[1] = stringType;

    ExprType *unionType = newUnionType(2, types, r);
    REQUIRE(unionType != NULL);
    CHECK(T_VAR_NUM_DISJUNCTS(unionType) == 2);

    // Both are T_VAR, but union has disjuncts
    CHECK(getNodeType(plainTVar) == T_VAR);
    CHECK(getNodeType(unionType) == T_VAR);
    CHECK(T_VAR_NUM_DISJUNCTS(plainTVar) != T_VAR_NUM_DISJUNCTS(unionType));

    region_free(r);
}

/**
 * @brief Test buffer overflow protection in getUnionTypeName
 */
TEST_CASE("union_types: buffer overflow protection")
{
    Region *r = make_region(16384, NULL);
    REQUIRE(r != NULL);

    // Create a union with many types
    std::vector<ExprType *> type_vec;
    for (int i = 0; i < 20; i++) {
        type_vec.push_back(newSimpType(T_STRING, r));
    }

    ExprType **types = (ExprType **)region_alloc(r, sizeof(ExprType *) * 20);
    for (int i = 0; i < 20; i++) {
        types[i] = type_vec[i];
    }

    ExprType *unionType = newUnionType(20, types, r);
    REQUIRE(unionType != NULL);

    // Format with small buffer - should not overflow
    char buf[512];
    char *result = getUnionTypeName(unionType, buf);
    REQUIRE(result != NULL);

    // String should be null-terminated
    CHECK(buf[511] == '\0');

    region_free(r);
}
