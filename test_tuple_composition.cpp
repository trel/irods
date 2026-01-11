/*
 * Test for createTupleTypeFromColumns() function
 * Tests tuple type composition from column lists
 */

#include "irods/private/re/typing.hpp"
#include "irods/private/re/restructs.hpp"
#include "irods/irods_list.h"
#include "irods/region.h"
#include <cassert>
#include <cstdio>
#include <cstring>

int main() {
    printf("Testing createTupleTypeFromColumns() function...\n");
    
    Region *r = make_region(0, NULL);
    
    /* Test 1: Single column tuple */
    List *cols = newList(r);
    listAppend(cols, (void*)"COLL_NAME", r);
    
    ExprType *tupleType = createTupleTypeFromColumns(cols, r);
    assert(tupleType != NULL);
    assert(getNodeType(tupleType) == T_TUPLE);
    assert(tupleType->degree == 1);
    assert(getNodeType(tupleType->subtrees[0]) == T_STRING);
    printf("✓ Single column tuple: [COLL_NAME] -> tuple<string>\n");
    
    /* Test 2: Two column tuple with string types */
    cols = newList(r);
    listAppend(cols, (void*)"COLL_NAME", r);
    listAppend(cols, (void*)"DATA_NAME", r);
    
    tupleType = createTupleTypeFromColumns(cols, r);
    assert(tupleType != NULL);
    assert(getNodeType(tupleType) == T_TUPLE);
    assert(tupleType->degree == 2);
    assert(getNodeType(tupleType->subtrees[0]) == T_STRING);
    assert(getNodeType(tupleType->subtrees[1]) == T_STRING);
    printf("✓ Two string column tuple: [COLL_NAME, DATA_NAME] -> tuple<string, string>\n");
    
    /* Test 3: Mixed type tuple (int, string, int) */
    cols = newList(r);
    listAppend(cols, (void*)"USER_ID", r);
    listAppend(cols, (void*)"USER_NAME", r);
    listAppend(cols, (void*)"DATA_SIZE", r);
    
    tupleType = createTupleTypeFromColumns(cols, r);
    assert(tupleType != NULL);
    assert(getNodeType(tupleType) == T_TUPLE);
    assert(tupleType->degree == 3);
    assert(getNodeType(tupleType->subtrees[0]) == T_INT);
    assert(getNodeType(tupleType->subtrees[1]) == T_STRING);
    assert(getNodeType(tupleType->subtrees[2]) == T_INT);
    printf("✓ Mixed type tuple: [USER_ID, USER_NAME, DATA_SIZE] -> tuple<int, string, int>\n");
    
    /* Test 4: Query-like tuple with multiple column types */
    cols = newList(r);
    listAppend(cols, (void*)"COLL_NAME", r);
    listAppend(cols, (void*)"DATA_NAME", r);
    listAppend(cols, (void*)"DATA_SIZE", r);
    listAppend(cols, (void*)"DATA_CREATE_TIME", r);
    listAppend(cols, (void*)"DATA_MODIFY_TIME", r);
    
    tupleType = createTupleTypeFromColumns(cols, r);
    assert(tupleType != NULL);
    assert(getNodeType(tupleType) == T_TUPLE);
    assert(tupleType->degree == 5);
    assert(getNodeType(tupleType->subtrees[0]) == T_STRING); // COLL_NAME
    assert(getNodeType(tupleType->subtrees[1]) == T_STRING); // DATA_NAME
    assert(getNodeType(tupleType->subtrees[2]) == T_INT);    // DATA_SIZE
    assert(getNodeType(tupleType->subtrees[3]) == T_INT);    // DATA_CREATE_TIME
    assert(getNodeType(tupleType->subtrees[4]) == T_INT);    // DATA_MODIFY_TIME
    printf("✓ Query result tuple: [COLL_NAME, DATA_NAME, DATA_SIZE, DATA_CREATE_TIME, DATA_MODIFY_TIME]\n");
    printf("  -> tuple<string, string, int, int, int>\n");
    
    /* Test 5: Tuple with unknown column (should use T_DYNAMIC) */
    cols = newList(r);
    listAppend(cols, (void*)"USER_ID", r);
    listAppend(cols, (void*)"UNKNOWN_COL", r);
    listAppend(cols, (void*)"USER_NAME", r);
    
    tupleType = createTupleTypeFromColumns(cols, r);
    assert(tupleType != NULL);
    assert(getNodeType(tupleType) == T_TUPLE);
    assert(tupleType->degree == 3);
    assert(getNodeType(tupleType->subtrees[0]) == T_INT);     // USER_ID
    assert(getNodeType(tupleType->subtrees[1]) == T_DYNAMIC); // UNKNOWN_COL (graceful degradation)
    assert(getNodeType(tupleType->subtrees[2]) == T_STRING);  // USER_NAME
    printf("✓ Tuple with unknown column: [USER_ID, UNKNOWN_COL, USER_NAME]\n");
    printf("  -> tuple<int, dynamic, string> (graceful degradation)\n");
    
    /* Test 6: NULL input returns NULL */
    tupleType = createTupleTypeFromColumns(NULL, r);
    assert(tupleType == NULL);
    printf("✓ NULL column list returns NULL\n");
    
    /* Test 7: Empty list returns NULL */
    cols = newList(r);
    tupleType = createTupleTypeFromColumns(cols, r);
    assert(tupleType == NULL);
    printf("✓ Empty column list returns NULL\n");
    
    /* Test 8: Tuple with all ID columns */
    cols = newList(r);
    listAppend(cols, (void*)"ZONE_ID", r);
    listAppend(cols, (void*)"USER_ID", r);
    listAppend(cols, (void*)"RESC_ID", r);
    listAppend(cols, (void*)"DATA_ID", r);
    
    tupleType = createTupleTypeFromColumns(cols, r);
    assert(tupleType != NULL);
    assert(getNodeType(tupleType) == T_TUPLE);
    assert(tupleType->degree == 4);
    for (int i = 0; i < 4; i++) {
        assert(getNodeType(tupleType->subtrees[i]) == T_INT);
    }
    printf("✓ All-integer tuple: [ZONE_ID, USER_ID, RESC_ID, DATA_ID] -> tuple<int, int, int, int>\n");
    
    /* Test 9: Tuple with all name columns */
    cols = newList(r);
    listAppend(cols, (void*)"ZONE_NAME", r);
    listAppend(cols, (void*)"USER_NAME", r);
    listAppend(cols, (void*)"RESC_NAME", r);
    listAppend(cols, (void*)"COLL_NAME", r);
    listAppend(cols, (void*)"DATA_NAME", r);
    
    tupleType = createTupleTypeFromColumns(cols, r);
    assert(tupleType != NULL);
    assert(getNodeType(tupleType) == T_TUPLE);
    assert(tupleType->degree == 5);
    for (int i = 0; i < 5; i++) {
        assert(getNodeType(tupleType->subtrees[i]) == T_STRING);
    }
    printf("✓ All-string tuple: [ZONE_NAME, USER_NAME, RESC_NAME, COLL_NAME, DATA_NAME]\n");
    printf("  -> tuple<string, string, string, string, string>\n");
    
    /* Test 10: Large tuple with many columns (10 columns) */
    cols = newList(r);
    listAppend(cols, (void*)"DATA_ID", r);
    listAppend(cols, (void*)"DATA_NAME", r);
    listAppend(cols, (void*)"DATA_SIZE", r);
    listAppend(cols, (void*)"DATA_CREATE_TIME", r);
    listAppend(cols, (void*)"DATA_MODIFY_TIME", r);
    listAppend(cols, (void*)"DATA_TYPE_NAME", r);
    listAppend(cols, (void*)"DATA_CHECKSUM", r);
    listAppend(cols, (void*)"COLL_NAME", r);
    listAppend(cols, (void*)"RESC_ID", r);
    listAppend(cols, (void*)"DATA_PATH", r);
    
    tupleType = createTupleTypeFromColumns(cols, r);
    assert(tupleType != NULL);
    assert(getNodeType(tupleType) == T_TUPLE);
    assert(tupleType->degree == 10);
    assert(getNodeType(tupleType->subtrees[0]) == T_INT);    // DATA_ID
    assert(getNodeType(tupleType->subtrees[1]) == T_STRING); // DATA_NAME
    assert(getNodeType(tupleType->subtrees[2]) == T_INT);    // DATA_SIZE
    assert(getNodeType(tupleType->subtrees[3]) == T_INT);    // DATA_CREATE_TIME
    assert(getNodeType(tupleType->subtrees[4]) == T_INT);    // DATA_MODIFY_TIME
    assert(getNodeType(tupleType->subtrees[5]) == T_STRING); // DATA_TYPE_NAME
    assert(getNodeType(tupleType->subtrees[6]) == T_STRING); // DATA_CHECKSUM
    assert(getNodeType(tupleType->subtrees[7]) == T_STRING); // COLL_NAME
    assert(getNodeType(tupleType->subtrees[8]) == T_INT);    // RESC_ID
    assert(getNodeType(tupleType->subtrees[9]) == T_STRING); // DATA_PATH
    printf("✓ Large 10-column tuple composes correctly\n");
    
    /* Clean up */
    region_free(r);
    
    printf("\n✓ All tests passed! (10 test cases)\n");
    return 0;
}
