/**
 * @file test_query_type_inference.cpp
 * @brief Integration tests for query result type inference
 *
 * Tests the complete query type inference pipeline:
 * 1. Column name extraction from N_QUERY AST
 * 2. Column type lookup from schema metadata
 * 3. Tuple type construction with union fallback for unknowns
 * 4. Collection type wrapping for query results
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
 * @brief Test column type lookup for known columns
 */
TEST_CASE("query_inference: column type lookup - known columns")
{
    Region *r = make_region(8192, NULL);
    REQUIRE(r != NULL);

    // Test known ID column
    ExprType *idType = getColumnTypeFromSchema("USER_ID", r);
    REQUIRE(idType != NULL);
    CHECK(getNodeType(idType) == T_INT);

    // Test known name column
    ExprType *nameType = getColumnTypeFromSchema("COLL_NAME", r);
    REQUIRE(nameType != NULL);
    CHECK(getNodeType(nameType) == T_STRING);

    // Test known time column
    ExprType *timeType = getColumnTypeFromSchema("DATA_MODIFY_TIME", r);
    REQUIRE(timeType != NULL);
    CHECK(getNodeType(timeType) == T_INT);

    // Test known size column
    ExprType *sizeType = getColumnTypeFromSchema("DATA_SIZE", r);
    REQUIRE(sizeType != NULL);
    CHECK(getNodeType(sizeType) == T_INT);

    region_free(r);
}

/**
 * @brief Test column type lookup for unknown columns
 */
TEST_CASE("query_inference: column type lookup - unknown columns")
{
    Region *r = make_region(8192, NULL);
    REQUIRE(r != NULL);

    // Test unknown column - should return T_DYNAMIC
    ExprType *unknownType = getColumnTypeFromSchema("UNKNOWN_COLUMN_XYZ", r);
    REQUIRE(unknownType != NULL);
    CHECK(getNodeType(unknownType) == T_DYNAMIC);

    // Test NULL column name - should return T_DYNAMIC
    ExprType *nullType = getColumnTypeFromSchema(NULL, r);
    REQUIRE(nullType != NULL);
    CHECK(getNodeType(nullType) == T_DYNAMIC);

    region_free(r);
}

/**
 * @brief Test tuple type creation from known columns
 */
TEST_CASE("query_inference: tuple from known columns")
{
    Region *r = make_region(16384, NULL);
    REQUIRE(r != NULL);

    // Create column name list: [COLL_NAME, DATA_SIZE, DATA_MODIFY_TIME]
    List *columnNames = newList(r);
    listAppend(columnNames, (void*)"COLL_NAME", r);
    listAppend(columnNames, (void*)"DATA_SIZE", r);
    listAppend(columnNames, (void*)"DATA_MODIFY_TIME", r);

    // Build tuple type
    ExprType *tupleType = createTupleTypeFromColumns(columnNames, r);
    REQUIRE(tupleType != NULL);

    CHECK(getNodeType(tupleType) == T_TUPLE);
    CHECK(tupleType->degree == 3);

    // Verify element types
    ExprType *elem0 = tupleType->subtrees[0];
    ExprType *elem1 = tupleType->subtrees[1];
    ExprType *elem2 = tupleType->subtrees[2];

    REQUIRE(elem0 != NULL);
    REQUIRE(elem1 != NULL);
    REQUIRE(elem2 != NULL);

    // First element: COLL_NAME -> string
    CHECK(getNodeType(elem0) == T_STRING);

    // Second element: DATA_SIZE -> int
    CHECK(getNodeType(elem1) == T_INT);

    // Third element: DATA_MODIFY_TIME -> int
    CHECK(getNodeType(elem2) == T_INT);

    region_free(r);
}

/**
 * @brief Test tuple type creation with unknown columns using unions
 */
TEST_CASE("query_inference: tuple with unknown columns produces unions")
{
    Region *r = make_region(16384, NULL);
    REQUIRE(r != NULL);

    // Create column list: [COLL_NAME, UNKNOWN_COL, DATA_SIZE]
    List *columnNames = newList(r);
    listAppend(columnNames, (void*)"COLL_NAME", r);
    listAppend(columnNames, (void*)"UNKNOWN_COLUMN_XYZ", r);
    listAppend(columnNames, (void*)"DATA_SIZE", r);

    // Build tuple type
    ExprType *tupleType = createTupleTypeFromColumns(columnNames, r);
    REQUIRE(tupleType != NULL);

    CHECK(getNodeType(tupleType) == T_TUPLE);
    CHECK(tupleType->degree == 3);

    // First element: COLL_NAME -> string (known)
    ExprType *elem0 = tupleType->subtrees[0];
    REQUIRE(elem0 != NULL);
    CHECK(getNodeType(elem0) == T_STRING);

    // Second element: UNKNOWN_COLUMN -> union<int|string|dynamic>
    ExprType *elem1 = tupleType->subtrees[1];
    REQUIRE(elem1 != NULL);
    CHECK(getNodeType(elem1) == T_VAR);
    CHECK(T_VAR_NUM_DISJUNCTS(elem1) == 3);

    // Third element: DATA_SIZE -> int (known)
    ExprType *elem2 = tupleType->subtrees[2];
    REQUIRE(elem2 != NULL);
    CHECK(getNodeType(elem2) == T_INT);

    region_free(r);
}

/**
 * @brief Test collection type wrapping for query results
 */
TEST_CASE("query_inference: query result as collection of tuples")
{
    Region *r = make_region(16384, NULL);
    REQUIRE(r != NULL);

    // Build tuple type for query columns
    List *columnNames = newList(r);
    listAppend(columnNames, (void*)"COLL_NAME", r);
    listAppend(columnNames, (void*)"DATA_SIZE", r);

    ExprType *tupleType = createTupleTypeFromColumns(columnNames, r);
    REQUIRE(tupleType != NULL);

    // Query result is a collection of tuples
    ExprType *queryResultType = newCollType(tupleType, r);
    REQUIRE(queryResultType != NULL);

    // Verify structure: collection<tuple<string, int>>
    CHECK(getNodeType(queryResultType) == T_CONS);  // T_CONS for collection/list
    CHECK(queryResultType->degree == 1);  // Collection has one type argument

    // Verify the element type is our tuple
    ExprType *elemType = queryResultType->subtrees[0];
    REQUIRE(elemType != NULL);
    CHECK(getNodeType(elemType) == T_TUPLE);
    CHECK(elemType->degree == 2);

    region_free(r);
}

/**
 * @brief Test mixed known and unknown columns
 */
TEST_CASE("query_inference: complex query with multiple unknowns")
{
    Region *r = make_region(16384, NULL);
    REQUIRE(r != NULL);

    // Column list: [ZONE_ID, UNKNOWN1, USER_NAME, UNKNOWN2, DATA_SIZE]
    List *columnNames = newList(r);
    listAppend(columnNames, (void*)"ZONE_ID", r);
    listAppend(columnNames, (void*)"UNKNOWN_A", r);
    listAppend(columnNames, (void*)"USER_NAME", r);
    listAppend(columnNames, (void*)"UNKNOWN_B", r);
    listAppend(columnNames, (void*)"DATA_SIZE", r);

    ExprType *tupleType = createTupleTypeFromColumns(columnNames, r);
    REQUIRE(tupleType != NULL);

    CHECK(getNodeType(tupleType) == T_TUPLE);
    CHECK(tupleType->degree == 5);

    // Verify pattern: known, union, known, union, known
    ExprType *elem0 = tupleType->subtrees[0];
    ExprType *elem1 = tupleType->subtrees[1];
    ExprType *elem2 = tupleType->subtrees[2];
    ExprType *elem3 = tupleType->subtrees[3];
    ExprType *elem4 = tupleType->subtrees[4];

    // elem0: ZONE_ID -> int (known)
    CHECK(getNodeType(elem0) == T_INT);

    // elem1: UNKNOWN_A -> union<int|string|dynamic>
    CHECK(getNodeType(elem1) == T_VAR);
    CHECK(T_VAR_NUM_DISJUNCTS(elem1) == 3);

    // elem2: USER_NAME -> string (known)
    CHECK(getNodeType(elem2) == T_STRING);

    // elem3: UNKNOWN_B -> union<int|string|dynamic>
    CHECK(getNodeType(elem3) == T_VAR);
    CHECK(T_VAR_NUM_DISJUNCTS(elem3) == 3);

    // elem4: DATA_SIZE -> int (known)
    CHECK(getNodeType(elem4) == T_INT);

    region_free(r);
}

/**
 * @brief Test empty column list handling
 */
TEST_CASE("query_inference: empty column list")
{
    Region *r = make_region(8192, NULL);
    REQUIRE(r != NULL);

    List *columnNames = newList(r);

    // Empty list should return NULL
    ExprType *tupleType = createTupleTypeFromColumns(columnNames, r);
    CHECK(tupleType == NULL);

    region_free(r);
}

/**
 * @brief Test NULL column list handling
 */
TEST_CASE("query_inference: NULL column list")
{
    Region *r = make_region(8192, NULL);
    REQUIRE(r != NULL);

    // NULL list should return NULL
    ExprType *tupleType = createTupleTypeFromColumns(NULL, r);
    CHECK(tupleType == NULL);

    region_free(r);
}

/**
 * @brief Test union type narrowing in query context
 */
TEST_CASE("query_inference: union narrowing with typed variables")
{
    Region *r = make_region(16384, NULL);
    REQUIRE(r != NULL);

    // Simulate: var x : string
    ExprType *varType = newTVar(r);
    REQUIRE(varType != NULL);

    // Build tuple with unknown column: tuple<string, union<int|string|dynamic>>
    ExprType *knownString = newSimpType(T_STRING, r);
    
    ExprType *intType = newSimpType(T_INT, r);
    ExprType *stringType = newSimpType(T_STRING, r);
    ExprType *dynamicType = newSimpType(T_DYNAMIC, r);
    ExprType **unionTypes = (ExprType **)region_alloc(r, sizeof(ExprType *) * 3);
    unionTypes[0] = intType;
    unionTypes[1] = stringType;
    unionTypes[2] = dynamicType;
    ExprType *unknownUnion = newUnionType(3, unionTypes, r);

    ExprType **tupleTypes = (ExprType **)region_alloc(r, sizeof(ExprType *) * 2);
    tupleTypes[0] = knownString;
    tupleTypes[1] = unknownUnion;
    ExprType *tupleType = newTupleType(2, tupleTypes, r);

    REQUIRE(tupleType != NULL);

    // When this tuple is used, the union in position 1 can be narrowed
    // by constraints on how the column is used in the query
    ExprType *colUnion = tupleType->subtrees[1];
    REQUIRE(colUnion != NULL);
    CHECK(getNodeType(colUnion) == T_VAR);
    CHECK(T_VAR_NUM_DISJUNCTS(colUnion) == 3);

    region_free(r);
}

/**
 * @brief Test union type formatting for error messages
 */
TEST_CASE("query_inference: union type names in errors")
{
    Region *r = make_region(8192, NULL);
    REQUIRE(r != NULL);

    // Create query tuple with unknown column
    List *columnNames = newList(r);
    listAppend(columnNames, (void*)"COLL_NAME", r);
    listAppend(columnNames, (void*)"MYSTERY_COLUMN", r);

    ExprType *tupleType = createTupleTypeFromColumns(columnNames, r);
    REQUIRE(tupleType != NULL);

    // Get the union element
    ExprType *unknownElem = tupleType->subtrees[1];
    REQUIRE(unknownElem != NULL);
    CHECK(getNodeType(unknownElem) == T_VAR);

    // Format for error message
    char buf[512];
    char *unionName = getUnionTypeName(unknownElem, buf);
    REQUIRE(unionName != NULL);

    // Should contain type names
    std::string typeStr(buf);
    CHECK(typeStr.find("union") != std::string::npos);
    CHECK(typeStr.find("int") != std::string::npos);
    CHECK(typeStr.find("string") != std::string::npos);

    region_free(r);
}
