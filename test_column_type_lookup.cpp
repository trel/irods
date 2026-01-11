/*
 * Test for getColumnTypeFromSchema() function
 * Tests column type lookup for GenQuery columns
 */

#include "irods/private/re/typing.hpp"
#include "irods/private/re/restructs.hpp"
#include "irods/region.h"
#include <cassert>
#include <cstdio>
#include <cstring>

int main() {
    printf("Testing getColumnTypeFromSchema() function...\n");
    
    Region *r = make_region(0, NULL);
    
    /* Test ID columns return T_INT */
    ExprType *type = getColumnTypeFromSchema("ZONE_ID", r);
    assert(type != NULL);
    assert(getNodeType(type) == T_INT);
    printf("✓ ZONE_ID -> T_INT\n");
    
    type = getColumnTypeFromSchema("USER_ID", r);
    assert(getNodeType(type) == T_INT);
    printf("✓ USER_ID -> T_INT\n");
    
    type = getColumnTypeFromSchema("RESC_ID", r);
    assert(getNodeType(type) == T_INT);
    printf("✓ RESC_ID -> T_INT\n");
    
    type = getColumnTypeFromSchema("DATA_ID", r);
    assert(getNodeType(type) == T_INT);
    printf("✓ DATA_ID -> T_INT\n");
    
    type = getColumnTypeFromSchema("COLL_ID", r);
    assert(getNodeType(type) == T_INT);
    printf("✓ COLL_ID -> T_INT\n");
    
    /* Test NAME columns return T_STRING */
    type = getColumnTypeFromSchema("ZONE_NAME", r);
    assert(getNodeType(type) == T_STRING);
    printf("✓ ZONE_NAME -> T_STRING\n");
    
    type = getColumnTypeFromSchema("USER_NAME", r);
    assert(getNodeType(type) == T_STRING);
    printf("✓ USER_NAME -> T_STRING\n");
    
    type = getColumnTypeFromSchema("RESC_NAME", r);
    assert(getNodeType(type) == T_STRING);
    printf("✓ RESC_NAME -> T_STRING\n");
    
    type = getColumnTypeFromSchema("COLL_NAME", r);
    assert(getNodeType(type) == T_STRING);
    printf("✓ COLL_NAME -> T_STRING\n");
    
    type = getColumnTypeFromSchema("DATA_NAME", r);
    assert(getNodeType(type) == T_STRING);
    printf("✓ DATA_NAME -> T_STRING\n");
    
    /* Test TIME columns return T_INT */
    type = getColumnTypeFromSchema("ZONE_CREATE_TIME", r);
    assert(getNodeType(type) == T_INT);
    printf("✓ ZONE_CREATE_TIME -> T_INT\n");
    
    type = getColumnTypeFromSchema("USER_MODIFY_TIME", r);
    assert(getNodeType(type) == T_INT);
    printf("✓ USER_MODIFY_TIME -> T_INT\n");
    
    type = getColumnTypeFromSchema("DATA_ACCESS_TIME", r);
    assert(getNodeType(type) == T_INT);
    printf("✓ DATA_ACCESS_TIME -> T_INT\n");
    
    /* Test SIZE/COUNT columns return T_INT */
    type = getColumnTypeFromSchema("DATA_SIZE", r);
    assert(getNodeType(type) == T_INT);
    printf("✓ DATA_SIZE -> T_INT\n");
    
    type = getColumnTypeFromSchema("RESC_FREE_SPACE", r);
    assert(getNodeType(type) == T_INT);
    printf("✓ RESC_FREE_SPACE -> T_INT\n");
    
    type = getColumnTypeFromSchema("DATA_REPL_NUM", r);
    assert(getNodeType(type) == T_INT);
    printf("✓ DATA_REPL_NUM -> T_INT\n");
    
    /* Test PATH/TEXT columns return T_STRING */
    type = getColumnTypeFromSchema("RESC_VAULT_PATH", r);
    assert(getNodeType(type) == T_STRING);
    printf("✓ RESC_VAULT_PATH -> T_STRING\n");
    
    type = getColumnTypeFromSchema("DATA_PATH", r);
    assert(getNodeType(type) == T_STRING);
    printf("✓ DATA_PATH -> T_STRING\n");
    
    type = getColumnTypeFromSchema("RESC_INFO", r);
    assert(getNodeType(type) == T_STRING);
    printf("✓ RESC_INFO -> T_STRING\n");
    
    /* Test TYPE columns return T_STRING */
    type = getColumnTypeFromSchema("RESC_TYPE_NAME", r);
    assert(getNodeType(type) == T_STRING);
    printf("✓ RESC_TYPE_NAME -> T_STRING\n");
    
    type = getColumnTypeFromSchema("DATA_TYPE_NAME", r);
    assert(getNodeType(type) == T_STRING);
    printf("✓ DATA_TYPE_NAME -> T_STRING\n");
    
    /* Test unknown columns return T_DYNAMIC */
    type = getColumnTypeFromSchema("UNKNOWN_COLUMN", r);
    assert(getNodeType(type) == T_DYNAMIC);
    printf("✓ UNKNOWN_COLUMN -> T_DYNAMIC\n");
    
    type = getColumnTypeFromSchema("FAKE_COL_NAME", r);
    assert(getNodeType(type) == T_DYNAMIC);
    printf("✓ FAKE_COL_NAME -> T_DYNAMIC\n");
    
    /* Test NULL column name returns T_DYNAMIC */
    type = getColumnTypeFromSchema(NULL, r);
    assert(getNodeType(type) == T_DYNAMIC);
    printf("✓ NULL -> T_DYNAMIC\n");
    
    /* Test metadata columns return T_STRING */
    type = getColumnTypeFromSchema("META_DATA_ATTR_NAME", r);
    assert(getNodeType(type) == T_STRING);
    printf("✓ META_DATA_ATTR_NAME -> T_STRING\n");
    
    type = getColumnTypeFromSchema("META_DATA_ATTR_VALUE", r);
    assert(getNodeType(type) == T_STRING);
    printf("✓ META_DATA_ATTR_VALUE -> T_STRING\n");
    
    type = getColumnTypeFromSchema("META_COLL_ATTR_NAME", r);
    assert(getNodeType(type) == T_STRING);
    printf("✓ META_COLL_ATTR_NAME -> T_STRING\n");
    
    /* Test status columns return T_INT */
    type = getColumnTypeFromSchema("DATA_REPL_STATUS", r);
    assert(getNodeType(type) == T_INT);
    printf("✓ DATA_REPL_STATUS -> T_INT\n");
    
    type = getColumnTypeFromSchema("DATA_STATUS", r);
    assert(getNodeType(type) == T_INT);
    printf("✓ DATA_STATUS -> T_INT\n");
    
    /* Test special columns */
    type = getColumnTypeFromSchema("RESC_MODIFY_TIME_MILLIS", r);
    assert(getNodeType(type) == T_INT);
    printf("✓ RESC_MODIFY_TIME_MILLIS -> T_INT\n");
    
    type = getColumnTypeFromSchema("DATA_RESC_HIER", r);
    assert(getNodeType(type) == T_STRING);
    printf("✓ DATA_RESC_HIER -> T_STRING\n");
    
    type = getColumnTypeFromSchema("DELAY_RULE_NAME", r);
    assert(getNodeType(type) == T_STRING);
    printf("✓ DELAY_RULE_NAME -> T_STRING\n");
    
    type = getColumnTypeFromSchema("DELAY_RULE_PRIORITY", r);
    assert(getNodeType(type) == T_INT);
    printf("✓ DELAY_RULE_PRIORITY -> T_INT\n");
    
    /* Clean up */
    region_free(r);
    
    printf("\n✓ All tests passed! (29 test cases)\n");
    return 0;
}
