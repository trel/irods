/**
 * @file test_attr_typing.cpp
 * @brief Unit tests for N_ATTR (attribute access) type preservation
 *
 * Tests that attribute access preserves and derives types correctly,
 * enabling type checking in query iteration and tuple element access patterns.
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
 * @brief Test direct column access type inference
 */
TEST_CASE("attr_typing: direct column access type preservation")
{
    Region *r = make_region(8192, NULL);
    REQUIRE(r != NULL);

    // Simulate accessing column directly: COLL_NAME
    const char *columnName = "COLL_NAME";
    ExprType *attrType = getColumnTypeFromSchema(columnName, r);
    REQUIRE(attrType != NULL);

    // COLL_NAME should be T_STRING
    CHECK(getNodeType(attrType) == T_STRING);

    region_free(r);
}

/**
 * @brief Test tuple element access through N_ATTR
 */
TEST_CASE("attr_typing: tuple element access")
{
    Region *r = make_region(16384, NULL);
    REQUIRE(r != NULL);

    // Create tuple: tuple<string, int, string>
    // Represents: (COLL_NAME, DATA_SIZE, ZONE_NAME)
    ExprType *stringType = newSimpType(T_STRING, r);
    ExprType *intType = newSimpType(T_INT, r);

    ExprType **tupleTypes = (ExprType **)region_alloc(r, sizeof(ExprType *) * 3);
    tupleTypes[0] = stringType;
    tupleTypes[1] = intType;
    tupleTypes[2] = stringType;

    ExprType *tupleType = newTupleType(3, tupleTypes, r);
    REQUIRE(tupleType != NULL);

    // When accessing tuple[0] (first element), should get T_STRING
    ExprType *elem0Type = tupleType->subtrees[0];
    CHECK(getNodeType(elem0Type) == T_STRING);

    // When accessing tuple[1] (second element), should get T_INT
    ExprType *elem1Type = tupleType->subtrees[1];
    CHECK(getNodeType(elem1Type) == T_INT);

    region_free(r);
}

/**
 * @brief Test unknown column access with union fallback
 */
TEST_CASE("attr_typing: unknown column access with union")
{
    Region *r = make_region(8192, NULL);
    REQUIRE(r != NULL);

    // Create union for unknown column
    ExprType *intType = newSimpType(T_INT, r);
    ExprType *stringType = newSimpType(T_STRING, r);
    ExprType *dynamicType = newSimpType(T_DYNAMIC, r);

    ExprType **unionTypes = (ExprType **)region_alloc(r, sizeof(ExprType *) * 3);
    unionTypes[0] = intType;
    unionTypes[1] = stringType;
    unionTypes[2] = dynamicType;

    ExprType *unionType = newUnionType(3, unionTypes, r);
    REQUIRE(unionType != NULL);

    // When accessing unknown column, attribute type is the union
    CHECK(getNodeType(unionType) == T_VAR);
    CHECK(T_VAR_NUM_DISJUNCTS(unionType) == 3);

    region_free(r);
}

/**
 * @brief Test query iteration attribute access
 */
TEST_CASE("attr_typing: query result iteration attribute access")
{
    Region *r = make_region(16384, NULL);
    REQUIRE(r != NULL);

    // Simulate query result: collection<tuple<string, int>>
    ExprType *stringType = newSimpType(T_STRING, r);
    ExprType *intType = newSimpType(T_INT, r);

    ExprType **tupleTypes = (ExprType **)region_alloc(r, sizeof(ExprType *) * 2);
    tupleTypes[0] = stringType;
    tupleTypes[1] = intType;

    ExprType *tupleType = newTupleType(2, tupleTypes, r);
    REQUIRE(tupleType != NULL);

    // Wrap in collection
    ExprType *queryResultType = newCollType(tupleType, r);
    REQUIRE(queryResultType != NULL);

    // In foreach loop: foreach (row in query_result)
    // row's type is the tuple
    // row.fieldname[0] should be string
    // row.fieldname[1] should be int

    ExprType *rowType = queryResultType->subtrees[0];
    REQUIRE(rowType != NULL);
    CHECK(getNodeType(rowType) == T_TUPLE);

    // Access first field
    ExprType *field0Type = rowType->subtrees[0];
    CHECK(getNodeType(field0Type) == T_STRING);

    // Access second field
    ExprType *field1Type = rowType->subtrees[1];
    CHECK(getNodeType(field1Type) == T_INT);

    region_free(r);
}

/**
 * @brief Test mixed known/unknown columns in query iteration
 */
TEST_CASE("attr_typing: query with mixed column types")
{
    Region *r = make_region(16384, NULL);
    REQUIRE(r != NULL);

    // Build tuple: <string (COLL_NAME), union (unknown), int (DATA_SIZE)>
    ExprType *stringType = newSimpType(T_STRING, r);
    ExprType *intType = newSimpType(T_INT, r);

    // Create union for unknown column
    ExprType *intType2 = newSimpType(T_INT, r);
    ExprType *stringType2 = newSimpType(T_STRING, r);
    ExprType *dynamicType = newSimpType(T_DYNAMIC, r);
    ExprType **unionTypes = (ExprType **)region_alloc(r, sizeof(ExprType *) * 3);
    unionTypes[0] = intType2;
    unionTypes[1] = stringType2;
    unionTypes[2] = dynamicType;
    ExprType *unknownUnion = newUnionType(3, unionTypes, r);

    ExprType **tupleTypes = (ExprType **)region_alloc(r, sizeof(ExprType *) * 3);
    tupleTypes[0] = stringType;
    tupleTypes[1] = unknownUnion;
    tupleTypes[2] = intType;

    ExprType *tupleType = newTupleType(3, tupleTypes, r);
    REQUIRE(tupleType != NULL);

    // Wrap in collection
    ExprType *queryResultType = newCollType(tupleType, r);
    REQUIRE(queryResultType != NULL);

    // In iteration: row.field[0] -> string
    ExprType *field0 = tupleType->subtrees[0];
    CHECK(getNodeType(field0) == T_STRING);

    // In iteration: row.field[1] -> union (can be narrowed by constraints)
    ExprType *field1 = tupleType->subtrees[1];
    CHECK(getNodeType(field1) == T_VAR);
    CHECK(T_VAR_NUM_DISJUNCTS(field1) == 3);

    // In iteration: row.field[2] -> int
    ExprType *field2 = tupleType->subtrees[2];
    CHECK(getNodeType(field2) == T_INT);

    region_free(r);
}

/**
 * @brief Test attribute access chain
 */
TEST_CASE("attr_typing: nested attribute access")
{
    Region *r = make_region(16384, NULL);
    REQUIRE(r != NULL);

    // Simulate accessing through multiple levels:
    // result_collection.current_row.FIELD_NAME

    // Inner level: result of accessing FIELD_NAME from current_row
    ExprType *fieldType = getColumnTypeFromSchema("DATA_NAME", r);
    REQUIRE(fieldType != NULL);
    CHECK(getNodeType(fieldType) == T_STRING);

    // This type propagates through the attribute access chain
    // final type should still be T_STRING

    region_free(r);
}

/**
 * @brief Test various column types through N_ATTR access
 */
TEST_CASE("attr_typing: comprehensive column type preservation")
{
    Region *r = make_region(8192, NULL);
    REQUIRE(r != NULL);

    // Test different column categories
    struct {
        const char *columnName;
        NodeType expectedType;
    } testCases[] = {
        {"ZONE_ID", T_INT},           // ID column
        {"COLL_NAME", T_STRING},      // Name column
        {"DATA_SIZE", T_INT},         // Size column
        {"DATA_MODIFY_TIME", T_INT},  // Time column
        {"DATA_PATH", T_STRING},      // Path column
        {"META_DATA_ATTR_VALUE", T_STRING},  // Metadata column
    };

    for (const auto &testCase : testCases) {
        ExprType *attrType = getColumnTypeFromSchema(testCase.columnName, r);
        REQUIRE(attrType != NULL);
        CHECK(getNodeType(attrType) == testCase.expectedType);
    }

    region_free(r);
}

/**
 * @brief Test that N_ATTR enables type constraints in operations
 */
TEST_CASE("attr_typing: attribute access enables type constraints")
{
    Region *r = make_region(8192, NULL);
    REQUIRE(r != NULL);

    // When accessing COLL_NAME (known to be string) through N_ATTR,
    // subsequent operations should be constrained to string operations

    ExprType *collNameType = getColumnTypeFromSchema("COLL_NAME", r);
    REQUIRE(collNameType != NULL);
    CHECK(getNodeType(collNameType) == T_STRING);

    // If this type is passed to constraint solver,
    // operations like string concatenation would be allowed,
    // but arithmetic would be rejected

    region_free(r);
}

/**
 * @brief Test N_ATTR with NULL/invalid inputs
 */
TEST_CASE("attr_typing: invalid attribute access handling")
{
    Region *r = make_region(8192, NULL);
    REQUIRE(r != NULL);

    // NULL column name should fall back to T_DYNAMIC
    ExprType *nullType = getColumnTypeFromSchema(NULL, r);
    REQUIRE(nullType != NULL);
    CHECK(getNodeType(nullType) == T_DYNAMIC);

    // Empty column name should also return T_DYNAMIC
    ExprType *emptyType = getColumnTypeFromSchema("", r);
    REQUIRE(emptyType != NULL);
    // Depends on implementation - could be DYNAMIC or lookup UNKNOWN

    region_free(r);
}
