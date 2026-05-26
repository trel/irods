#ifndef RODS_GEN_QUERY_H__
#define RODS_GEN_QUERY_H__

#include "irods/objInfo.h"

/// Maximum number of selected attributes in a general query.
#define MAX_SQL_ATTR    50
/// Maximum number of rows returned by a general query page.
#define MAX_SQL_ROWS   256

/* In genQueryInp_t, selectInp is a int index, int value pair. The index
 * represents the attribute index.
 * sqlCondInp is a int index, string value pair. The index
 * represents the attribute index.
 */

/// Input describing a general query request.
typedef struct GenQueryInp {
    int maxRows;             /**< Maximum number of rows to return, or zero to close the statement. */
    int continueInx;         /**< Continuation index returned by a previous query call. */
    int rowOffset;           /**< Zero-based row offset for paged query results. */
    int options;             /**< Bitwise query options such as total-row-count and auto-close. */
    keyValPair_t condInput;  /**< Additional keyword-based query options. */
    inxIvalPair_t selectInp; /**< Selected columns and per-column option bits. */
    inxValPair_t sqlCondInp; /**< Column conditions for the SQL `where` clause. */
} genQueryInp_t;

/// Result data for a selected general query column.
typedef struct SqlResult {
    int attriInx;        /**< Attribute index. */
    int len;             /**< Length of each value in bytes. */
    char *value;         /**< Contiguous value buffer arranged as `[rowCnt][len]`. */
} sqlResult_t;

/// Output returned by a general query request.
typedef struct GenQueryOut {
    int rowCnt;                        /**< Number of rows in this result page. */
    int attriCnt;                      /**< Number of selected attributes returned. */
    int continueInx;                   /**< Continuation index for fetching more rows. */
    int totalRowCount;                 /**< Total available row count when requested. */
    sqlResult_t sqlResult[MAX_SQL_ATTR]; /**< Per-column result buffers. */
} genQueryOut_t;

/*
Bits to set in the value array (genQueryInp.selectInp.value[i]) to
order the results by that column, either ascending or descending.  This
is done in the order of the value array, so the first one will be the
primary ordering column.
*/
#define ORDER_BY 0x400 /**< Orders results by the selected column in ascending order. */
#define ORDER_BY_DESC 0x800 /**< Orders results by the selected column in descending order. */

// A bitwise set of options that will be applied to the entire resultset
// (GenQueryInp::options).
#define RETURN_TOTAL_ROW_COUNT 0x20 /**< Requests the total available row count. */
#define NO_DISTINCT 0x40 /**< Disables the default `distinct` behavior. */
#define QUOTA_QUERY 0x80 /**< Executes the special quota query form. */
#define AUTO_CLOSE  0x100 /**< Closes the statement even if more rows are available. */
#define UPPER_CASE_WHERE  0x200 /**< Uppercases column names used in the `where` clause. */

/*
  These are some operations (functions) that can be applied to columns
  being returned (selected) by setting the input array,
  genQueryInp.selectInp.value[i], to these values.  Values 0 and 1 (or
  any not defined here) will just return the column content (i.e. the
  normal, default case).  The ORDER_BY and ORDER_BY_DESC bits are also
  stored in the same input array but are not normally used together
  with these since these will return one row.
*/
#define SELECT_MIN 2 /**< Applies the SQL `MIN` aggregate. */
#define SELECT_MAX 3 /**< Applies the SQL `MAX` aggregate. */
#define SELECT_SUM 4 /**< Applies the SQL `SUM` aggregate. */
#define SELECT_AVG 5 /**< Applies the SQL `AVG` aggregate. */
#define SELECT_COUNT 6 /**< Applies the SQL `COUNT` aggregate. */

/*
  For the integer values below (the COL_* defines), up to 10,000 is reserved
  the core tables The type can be determined by comparing with this value.
 */
#define MAX_CORE_TABLE_VALUE 10000 /**< Maximum column value reserved for core tables. */

/*
  These are the Table Column names used with the GenQuery.  Also see
  the rcatGeneralQuerySetup routine which associates these values with
  tables and columns. */

/* R_ZONE_MAIN: */
#define COL_ZONE_ID 101 /**< Query column for zone identifiers. */
#define COL_ZONE_NAME 102 /**< Query column for zone names. */
#define COL_ZONE_TYPE 103 /**< Query column for zone types. */
#define COL_ZONE_CONNECTION 104 /**< Query column for zone connection strings. */
#define COL_ZONE_COMMENT 105 /**< Query column for zone comments. */
#define COL_ZONE_CREATE_TIME 106 /**< Query column for zone creation times. */
#define COL_ZONE_MODIFY_TIME 107 /**< Query column for zone modification times. */

/* R_USER_MAIN: */
#define COL_USER_ID 201 /**< Query column for user identifiers. */
#define COL_USER_NAME 202 /**< Query column for user names. */
#define COL_USER_TYPE 203 /**< Query column for user types. */
#define COL_USER_ZONE 204 /**< Query column for user zones. */
#define COL_USER_INFO 206 /**< Query column for user info strings. */
#define COL_USER_COMMENT 207 /**< Query column for user comments. */
#define COL_USER_CREATE_TIME 208 /**< Query column for user creation times. */
#define COL_USER_MODIFY_TIME 209 /**< Query column for user modification times. */

#define COL_USER_DN_INVALID 205 /**< Backward-compatible query column for the legacy user DN field. */

/* R_RESC_MAIN: */
#define COL_R_RESC_ID 301 /**< Query column for resource identifiers. */
#define COL_R_RESC_NAME 302 /**< Query column for resource names. */
#define COL_R_ZONE_NAME 303 /**< Query column for resource zone names. */
#define COL_R_TYPE_NAME 304 /**< Query column for resource type names. */
#define COL_R_CLASS_NAME 305 /**< Query column for resource class names. */
#define COL_R_LOC 306 /**< Query column for resource locations. */
#define COL_R_VAULT_PATH 307 /**< Query column for resource vault paths. */
#define COL_R_FREE_SPACE 308 /**< Query column for resource free space values. */
#define COL_R_RESC_INFO  309 /**< Query column for resource info strings. */
#define COL_R_RESC_COMMENT 310 /**< Query column for resource comments. */
#define COL_R_CREATE_TIME 311 /**< Query column for resource creation times. */
#define COL_R_MODIFY_TIME 312 /**< Query column for resource modification times. */
#define COL_R_RESC_STATUS 313 /**< Query column for resource status values. */
#define COL_R_FREE_SPACE_TIME 314 /**< Query column for resource free space timestamps. */
#define COL_R_RESC_CHILDREN 315 /**< Query column for resource children. */
#define COL_R_RESC_CONTEXT 316 /**< Query column for resource context strings. */
#define COL_R_RESC_PARENT 317 /**< Query column for parent resources. */
#define COL_R_RESC_PARENT_CONTEXT 318 /**< Query column for parent resource context strings. */
#define COL_R_MODIFY_TIME_MILLIS           319 /**< Query column for resource modification times in milliseconds. */

/* R_DATA_MAIN: */
#define COL_D_DATA_ID 401 /**< Query column for data object identifiers. */
#define COL_D_COLL_ID 402 /**< Query column for parent collection identifiers. */
#define COL_DATA_NAME 403 /**< Query column for data object names. */
#define COL_DATA_REPL_NUM 404 /**< Query column for replica numbers. */
#define COL_DATA_VERSION 405 /**< Query column for data object versions. */
#define COL_DATA_TYPE_NAME 406 /**< Query column for data type names. */
#define COL_DATA_SIZE 407 /**< Query column for data sizes. */
//#define COL_D_RESC_GROUP_NAME 408		// gone in 4.1 #1472
#define COL_D_RESC_NAME 409 /**< Query column for data object resource names. */
#define COL_D_DATA_PATH 410 /**< Query column for physical data paths. */
#define COL_D_OWNER_NAME 411 /**< Query column for data owner names. */
#define COL_D_OWNER_ZONE 412 /**< Query column for data owner zones. */
#define COL_D_REPL_STATUS 413 /**< Query column for data replica status values. */
#define COL_D_DATA_STATUS 414 /**< Query column for data object status values. */
#define COL_D_DATA_CHECKSUM 415 /**< Query column for data object checksums. */
#define COL_D_EXPIRY 416 /**< Query column for data expiry timestamps. */
#define COL_D_MAP_ID 417 /**< Query column for data map identifiers. */
#define COL_D_COMMENTS 418 /**< Query column for data comments. */
#define COL_D_CREATE_TIME 419 /**< Query column for data creation times. */
#define COL_D_MODIFY_TIME 420 /**< Query column for data modification times. */
#define COL_DATA_MODE 421 /**< Query column for data mode values. */
#define COL_D_RESC_HIER 422 /**< Query column for resource hierarchies. */
#define COL_D_RESC_ID 423 /**< Query column for resource identifiers on replicas. */
#define COL_D_ACCESS_TIME 424 /**< Query column for data access times. */

/* R_COLL_MAIN */
#define COL_COLL_ID 500 /**< Query column for collection identifiers. */
#define COL_COLL_NAME 501 /**< Query column for collection names. */
#define COL_COLL_PARENT_NAME 502 /**< Query column for parent collection names. */
#define COL_COLL_OWNER_NAME 503 /**< Query column for collection owner names. */
#define COL_COLL_OWNER_ZONE 504 /**< Query column for collection owner zones. */
#define COL_COLL_MAP_ID 505 /**< Query column for collection map identifiers. */
#define COL_COLL_INHERITANCE 506 /**< Query column for collection inheritance flags. */
#define COL_COLL_COMMENTS 507 /**< Query column for collection comments. */
#define COL_COLL_CREATE_TIME 508 /**< Query column for collection creation times. */
#define COL_COLL_MODIFY_TIME 509 /**< Query column for collection modification times. */
#define COL_COLL_TYPE 510 /**< Query column for collection types. */
#define COL_COLL_INFO1 511 /**< Query column for collection info1 values. */
#define COL_COLL_INFO2 512 /**< Query column for collection info2 values. */

/* R_META_MAIN */
#define COL_META_DATA_ATTR_NAME 600 /**< Query column for data AVU attribute names. */
#define COL_META_DATA_ATTR_VALUE 601 /**< Query column for data AVU attribute values. */
#define COL_META_DATA_ATTR_UNITS 602 /**< Query column for data AVU units. */
#define COL_META_DATA_ATTR_ID 603 /**< Query column for data AVU identifiers. */
#define COL_META_DATA_CREATE_TIME 604 /**< Query column for data AVU creation times. */
#define COL_META_DATA_MODIFY_TIME 605 /**< Query column for data AVU modification times. */

#define COL_META_COLL_ATTR_NAME 610 /**< Query column for collection AVU attribute names. */
#define COL_META_COLL_ATTR_VALUE 611 /**< Query column for collection AVU attribute values. */
#define COL_META_COLL_ATTR_UNITS 612 /**< Query column for collection AVU units. */
#define COL_META_COLL_ATTR_ID 613 /**< Query column for collection AVU identifiers. */
#define COL_META_COLL_CREATE_TIME 614 /**< Query column for collection AVU creation times. */
#define COL_META_COLL_MODIFY_TIME 615 /**< Query column for collection AVU modification times. */

#define COL_META_NAMESPACE_COLL 620 /**< Query column for the collection AVU namespace. */
#define COL_META_NAMESPACE_DATA 621 /**< Query column for the data AVU namespace. */
#define COL_META_NAMESPACE_RESC 622 /**< Query column for the resource AVU namespace. */
#define COL_META_NAMESPACE_USER 623 /**< Query column for the user AVU namespace. */
#define COL_META_NAMESPACE_RESC_GROUP 624 /**< Query column for the resource group AVU namespace. */
#define COL_META_NAMESPACE_RULE 625 /**< Query column for the rule AVU namespace. */
#define COL_META_NAMESPACE_MSRVC 626 /**< Query column for the microservice AVU namespace. */
#define COL_META_NAMESPACE_MET2 627 /**< Query column for the metadata2 AVU namespace. */

#define COL_META_RESC_ATTR_NAME 630 /**< Query column for resource AVU attribute names. */
#define COL_META_RESC_ATTR_VALUE 631 /**< Query column for resource AVU attribute values. */
#define COL_META_RESC_ATTR_UNITS 632 /**< Query column for resource AVU units. */
#define COL_META_RESC_ATTR_ID 633 /**< Query column for resource AVU identifiers. */
#define COL_META_RESC_CREATE_TIME 634 /**< Query column for resource AVU creation times. */
#define COL_META_RESC_MODIFY_TIME 635 /**< Query column for resource AVU modification times. */

#define COL_META_USER_ATTR_NAME 640 /**< Query column for user AVU attribute names. */
#define COL_META_USER_ATTR_VALUE 641 /**< Query column for user AVU attribute values. */
#define COL_META_USER_ATTR_UNITS 642 /**< Query column for user AVU units. */
#define COL_META_USER_ATTR_ID 643 /**< Query column for user AVU identifiers. */
#define COL_META_USER_CREATE_TIME 644 /**< Query column for user AVU creation times. */
#define COL_META_USER_MODIFY_TIME 645 /**< Query column for user AVU modification times. */

#define COL_META_RESC_GROUP_ATTR_NAME 650 /**< Query column for resource group AVU attribute names. */
#define COL_META_RESC_GROUP_ATTR_VALUE 651 /**< Query column for resource group AVU attribute values. */
#define COL_META_RESC_GROUP_ATTR_UNITS 652 /**< Query column for resource group AVU units. */
#define COL_META_RESC_GROUP_ATTR_ID 653 /**< Query column for resource group AVU identifiers. */
#define COL_META_RESC_GROUP_CREATE_TIME 654 /**< Query column for resource group AVU creation times. */
#define COL_META_RESC_GROUP_MODIFY_TIME 655 /**< Query column for resource group AVU modification times. */

#define COL_META_RULE_ATTR_NAME 660 /**< Query column for rule AVU attribute names. */
#define COL_META_RULE_ATTR_VALUE 661 /**< Query column for rule AVU attribute values. */
#define COL_META_RULE_ATTR_UNITS 662 /**< Query column for rule AVU units. */
#define COL_META_RULE_ATTR_ID 663 /**< Query column for rule AVU identifiers. */
#define COL_META_RULE_CREATE_TIME 664 /**< Query column for rule AVU creation times. */
#define COL_META_RULE_MODIFY_TIME 665 /**< Query column for rule AVU modification times. */

#define COL_META_MSRVC_ATTR_NAME 670 /**< Query column for microservice AVU attribute names. */
#define COL_META_MSRVC_ATTR_VALUE 671 /**< Query column for microservice AVU attribute values. */
#define COL_META_MSRVC_ATTR_UNITS 672 /**< Query column for microservice AVU units. */
#define COL_META_MSRVC_ATTR_ID 673 /**< Query column for microservice AVU identifiers. */
#define COL_META_MSRVC_CREATE_TIME 674 /**< Query column for microservice AVU creation times. */
#define COL_META_MSRVC_MODIFY_TIME 675 /**< Query column for microservice AVU modification times. */

#define COL_META_MET2_ATTR_NAME 680 /**< Query column for metadata2 AVU attribute names. */
#define COL_META_MET2_ATTR_VALUE 681 /**< Query column for metadata2 AVU attribute values. */
#define COL_META_MET2_ATTR_UNITS 682 /**< Query column for metadata2 AVU units. */
#define COL_META_MET2_ATTR_ID 683 /**< Query column for metadata2 AVU identifiers. */
#define COL_META_MET2_CREATE_TIME 684 /**< Query column for metadata2 AVU creation times. */
#define COL_META_MET2_MODIFY_TIME 685 /**< Query column for metadata2 AVU modification times. */

/* R_OBJT_ACCESS */
#define COL_DATA_ACCESS_TYPE 700 /**< Query column for data access token types. */
#define COL_DATA_ACCESS_NAME 701 /**< Query column for data access token names. */
#define COL_DATA_TOKEN_NAMESPACE 702 /**< Query column for data access token namespaces. */
#define COL_DATA_ACCESS_USER_ID 703 /**< Query column for user IDs on data access entries. */
#define COL_DATA_ACCESS_DATA_ID 704 /**< Query column for data IDs on access entries. */

#define COL_COLL_ACCESS_TYPE 710 /**< Query column for collection access token types. */
#define COL_COLL_ACCESS_NAME 711 /**< Query column for collection access token names. */
#define COL_COLL_TOKEN_NAMESPACE 712 /**< Query column for collection access token namespaces. */
#define COL_COLL_ACCESS_USER_ID 713 /**< Query column for user IDs on collection access entries. */
#define COL_COLL_ACCESS_COLL_ID 714 /**< Query column for collection IDs on access entries. */

#define COL_RESC_ACCESS_TYPE 720 /**< Query column for resource access token types. */
#define COL_RESC_ACCESS_NAME 721 /**< Query column for resource access token names. */
#define COL_RESC_TOKEN_NAMESPACE 722 /**< Query column for resource access token namespaces. */
#define COL_RESC_ACCESS_USER_ID 723 /**< Query column for user IDs on resource access entries. */
#define COL_RESC_ACCESS_RESC_ID 724 /**< Query column for resource IDs on access entries. */

#define COL_META_ACCESS_TYPE 730 /**< Query column for metadata access token types. */
#define COL_META_ACCESS_NAME 731 /**< Query column for metadata access token names. */
#define COL_META_TOKEN_NAMESPACE 732 /**< Query column for metadata access token namespaces. */
#define COL_META_ACCESS_USER_ID 733 /**< Query column for user IDs on metadata access entries. */
#define COL_META_ACCESS_META_ID 734 /**< Query column for metadata IDs on access entries. */

#define COL_RULE_ACCESS_TYPE 740 /**< Query column for rule access token types. */
#define COL_RULE_ACCESS_NAME 741 /**< Query column for rule access token names. */
#define COL_RULE_TOKEN_NAMESPACE 742 /**< Query column for rule access token namespaces. */
#define COL_RULE_ACCESS_USER_ID 743 /**< Query column for user IDs on rule access entries. */
#define COL_RULE_ACCESS_RULE_ID 744 /**< Query column for rule IDs on access entries. */

#define COL_MSRVC_ACCESS_TYPE 750 /**< Query column for microservice access token types. */
#define COL_MSRVC_ACCESS_NAME 751 /**< Query column for microservice access token names. */
#define COL_MSRVC_TOKEN_NAMESPACE 752 /**< Query column for microservice access token namespaces. */
#define COL_MSRVC_ACCESS_USER_ID 753 /**< Query column for user IDs on microservice access entries. */
#define COL_MSRVC_ACCESS_MSRVC_ID 754 /**< Query column for microservice IDs on access entries. */

/* R_RESC_GROUP */
//#define COL_RESC_GROUP_RESC_ID 800	// gone in 4.1 #1472
//#define COL_RESC_GROUP_NAME 801
//#define COL_RESC_GROUP_ID 802

/* R_USER_GROUP / USER */
#define COL_USER_GROUP_ID 900 /**< Query column for user group identifiers. */
#define COL_USER_GROUP_NAME 901 /**< Query column for user group names. */

/* R_RULE_EXEC */
#define COL_RULE_EXEC_ID 1000 /**< Query column for rule execution identifiers. */
#define COL_RULE_EXEC_NAME 1001 /**< Query column for rule execution names. */
#define COL_RULE_EXEC_REI_FILE_PATH 1002 /**< Query column for rule execution REI file paths. */
#define COL_RULE_EXEC_USER_NAME   1003 /**< Query column for rule execution user names. */
#define COL_RULE_EXEC_ADDRESS 1004 /**< Query column for rule execution addresses. */
#define COL_RULE_EXEC_TIME    1005 /**< Query column for rule execution times. */
#define COL_RULE_EXEC_FREQUENCY 1006 /**< Query column for rule execution frequencies. */
#define COL_RULE_EXEC_PRIORITY 1007 /**< Query column for rule execution priorities. */
#define COL_RULE_EXEC_ESTIMATED_EXE_TIME 1008 /**< Query column for estimated execution times. */
#define COL_RULE_EXEC_NOTIFICATION_ADDR 1009 /**< Query column for notification addresses. */
#define COL_RULE_EXEC_LAST_EXE_TIME 1010 /**< Query column for last execution times. */
#define COL_RULE_EXEC_STATUS 1011 /**< Query column for rule execution statuses. */
#define COL_RULE_EXEC_CONTEXT 1012 /**< Query column for rule execution context strings. */
#define COL_RULE_EXEC_LOCK_HOST            1013 /**< Query column for rule execution lock hosts. */
#define COL_RULE_EXEC_LOCK_HOST_PID        1014 /**< Query column for rule execution lock host PIDs. */
#define COL_RULE_EXEC_LOCK_TIME            1015 /**< Query column for rule execution lock times. */

/* R_TOKN_MAIN */
#define COL_TOKEN_NAMESPACE 1100 /**< Query column for token namespaces. */
#define COL_TOKEN_ID 1101 /**< Query column for token identifiers. */
#define COL_TOKEN_NAME 1102 /**< Query column for token names. */
#define COL_TOKEN_VALUE 1103 /**< Query column for primary token values. */
#define COL_TOKEN_VALUE2 1104 /**< Query column for secondary token values. */
#define COL_TOKEN_VALUE3 1105 /**< Query column for tertiary token values. */
#define COL_TOKEN_COMMENT 1106 /**< Query column for token comments. */
#define COL_TOKEN_CREATE_TIME 1107 /**< Query column for token creation times. */
#define COL_TOKEN_MODIFY_TIME 1108 /**< Query column for token modification times. */

/* R_OBJT_AUDIT */
#define COL_AUDIT_OBJ_ID      1200 /**< Query column for audited object identifiers. */
#define COL_AUDIT_USER_ID     1201 /**< Query column for audit user identifiers. */
#define COL_AUDIT_ACTION_ID   1202 /**< Query column for audit action identifiers. */
#define COL_AUDIT_COMMENT     1203 /**< Query column for audit comments. */
#define COL_AUDIT_CREATE_TIME 1204 /**< Query column for audit creation times. */
#define COL_AUDIT_MODIFY_TIME 1205 /**< Query column for audit modification times. */

/* Range of the Audit columns; used sometimes to restrict access */
#define COL_AUDIT_RANGE_START 1200 /**< First reserved audit column value. */
#define COL_AUDIT_RANGE_END   1299 /**< Last reserved audit column value. */

/* R_COLL_USER_MAIN (r_user_main for Collection information) */
#define COL_COLL_USER_NAME    1300 /**< Query column for collection user names. */
#define COL_COLL_USER_ZONE    1301 /**< Query column for collection user zones. */

/* R_DATA_USER_MAIN (r_user_main for Data information specifically) */
#define COL_DATA_USER_NAME    1310 /**< Query column for data user names. */
#define COL_DATA_USER_ZONE    1311 /**< Query column for data user zones. */

/* R_DATA_USER_MAIN (r_user_main for Data information specifically) */
#define COL_RESC_USER_NAME    1320 /**< Query column for resource user names. */
#define COL_RESC_USER_ZONE    1321 /**< Query column for resource user zones. */

/* R_SERVER_LOAD */
#define COL_SL_HOST_NAME      1400 /**< Query column for server load host names. */
#define COL_SL_RESC_NAME      1401 /**< Query column for server load resource names. */
#define COL_SL_CPU_USED       1402 /**< Query column for server CPU usage. */
#define COL_SL_MEM_USED       1403 /**< Query column for server memory usage. */
#define COL_SL_SWAP_USED      1404 /**< Query column for server swap usage. */
#define COL_SL_RUNQ_LOAD      1405 /**< Query column for server run queue load. */
#define COL_SL_DISK_SPACE     1406 /**< Query column for server disk space. */
#define COL_SL_NET_INPUT      1407 /**< Query column for server network input. */
#define COL_SL_NET_OUTPUT     1408 /**< Query column for server network output. */
#define COL_SL_CREATE_TIME    1409 /**< Query column for server load timestamps. */

/* R_SERVER_LOAD_DIGEST */
#define COL_SLD_RESC_NAME     1500 /**< Query column for server load digest resource names. */
#define COL_SLD_LOAD_FACTOR   1501 /**< Query column for server load digest factors. */
#define COL_SLD_CREATE_TIME   1502 /**< Query column for server load digest timestamps. */

/* R_USER_AUTH (for GSI/KRB) */
#define COL_USER_AUTH_ID 1600 /**< Query column for user authentication identifiers. */
#define COL_USER_DN      1601 /**< Query column for user distinguished names. */

/* R_RULE_MAIN */
#define COL_RULE_ID           1700 /**< Query column for rule identifiers. */
#define COL_RULE_VERSION      1701 /**< Query column for rule versions. */
#define COL_RULE_BASE_NAME    1702 /**< Query column for rule base names. */
#define COL_RULE_NAME         1703 /**< Query column for rule names. */
#define COL_RULE_EVENT        1704 /**< Query column for rule events. */
#define COL_RULE_CONDITION    1705 /**< Query column for rule conditions. */
#define COL_RULE_BODY         1706 /**< Query column for rule bodies. */
#define COL_RULE_RECOVERY     1707 /**< Query column for rule recovery blocks. */
#define COL_RULE_STATUS       1708 /**< Query column for rule statuses. */
#define COL_RULE_OWNER_NAME   1709 /**< Query column for rule owner names. */
#define COL_RULE_OWNER_ZONE   1710 /**< Query column for rule owner zones. */
#define COL_RULE_DESCR_1      1711 /**< Query column for primary rule descriptions. */
#define COL_RULE_DESCR_2      1712 /**< Query column for secondary rule descriptions. */
#define COL_RULE_INPUT_PARAMS      1713 /**< Query column for rule input parameters. */
#define COL_RULE_OUTPUT_PARAMS     1714 /**< Query column for rule output parameters. */
#define COL_RULE_DOLLAR_VARS       1715 /**< Query column for rule dollar-variable usage. */
#define COL_RULE_ICAT_ELEMENTS     1716 /**< Query column for rule iCAT elements. */
#define COL_RULE_SIDEEFFECTS       1717 /**< Query column for rule side effects. */
#define COL_RULE_COMMENT      1718 /**< Query column for rule comments. */
#define COL_RULE_CREATE_TIME  1719 /**< Query column for rule creation times. */
#define COL_RULE_MODIFY_TIME  1720 /**< Query column for rule modification times. */

/* R_RULE_BASE_MAP (for storing versions of the rules */
#define COL_RULE_BASE_MAP_VERSION      1721 /**< Query column for rule base map versions. */
#define COL_RULE_BASE_MAP_BASE_NAME    1722 /**< Query column for rule base map base names. */
#define COL_RULE_BASE_MAP_OWNER_NAME   1723 /**< Query column for rule base map owner names. */
#define COL_RULE_BASE_MAP_OWNER_ZONE   1724 /**< Query column for rule base map owner zones. */
#define COL_RULE_BASE_MAP_COMMENT      1725 /**< Query column for rule base map comments. */
#define COL_RULE_BASE_MAP_CREATE_TIME  1726 /**< Query column for rule base map creation times. */
#define COL_RULE_BASE_MAP_MODIFY_TIME  1727 /**< Query column for rule base map modification times. */
#define COL_RULE_BASE_MAP_PRIORITY     1728 /**< Query column for rule base map priorities. */

/* R_RULE_DVM (Data Variable Mapping) */
#define COL_DVM_ID            1800 /**< Query column for DVM identifiers. */
#define COL_DVM_VERSION       1801 /**< Query column for DVM versions. */
#define COL_DVM_BASE_NAME     1802 /**< Query column for DVM base names. */
#define COL_DVM_EXT_VAR_NAME  1803 /**< Query column for DVM external variable names. */
#define COL_DVM_CONDITION     1804 /**< Query column for DVM conditions. */
#define COL_DVM_INT_MAP_PATH  1805 /**< Query column for DVM internal map paths. */
#define COL_DVM_STATUS        1806 /**< Query column for DVM statuses. */
#define COL_DVM_OWNER_NAME    1807 /**< Query column for DVM owner names. */
#define COL_DVM_OWNER_ZONE    1808 /**< Query column for DVM owner zones. */
#define COL_DVM_COMMENT       1809 /**< Query column for DVM comments. */
#define COL_DVM_CREATE_TIME   1810 /**< Query column for DVM creation times. */
#define COL_DVM_MODIFY_TIME   1811 /**< Query column for DVM modification times. */

/* R_RULE_DVM_MAP (for storing versions of the rules */
#define COL_DVM_BASE_MAP_VERSION      1812 /**< Query column for DVM base map versions. */
#define COL_DVM_BASE_MAP_BASE_NAME    1813 /**< Query column for DVM base map base names. */
#define COL_DVM_BASE_MAP_OWNER_NAME   1814 /**< Query column for DVM base map owner names. */
#define COL_DVM_BASE_MAP_OWNER_ZONE   1815 /**< Query column for DVM base map owner zones. */
#define COL_DVM_BASE_MAP_COMMENT      1816 /**< Query column for DVM base map comments. */
#define COL_DVM_BASE_MAP_CREATE_TIME  1817 /**< Query column for DVM base map creation times. */
#define COL_DVM_BASE_MAP_MODIFY_TIME  1818 /**< Query column for DVM base map modification times. */

/* R_RULE_FNM (Function Name Mapping) */
#define COL_FNM_ID            1900 /**< Query column for FNM identifiers. */
#define COL_FNM_VERSION       1901 /**< Query column for FNM versions. */
#define COL_FNM_BASE_NAME     1902 /**< Query column for FNM base names. */
#define COL_FNM_EXT_FUNC_NAME 1903 /**< Query column for FNM external function names. */
#define COL_FNM_INT_FUNC_NAME 1904 /**< Query column for FNM internal function names. */
#define COL_FNM_STATUS        1905 /**< Query column for FNM statuses. */
#define COL_FNM_OWNER_NAME    1906 /**< Query column for FNM owner names. */
#define COL_FNM_OWNER_ZONE    1907 /**< Query column for FNM owner zones. */
#define COL_FNM_COMMENT       1908 /**< Query column for FNM comments. */
#define COL_FNM_CREATE_TIME   1909 /**< Query column for FNM creation times. */
#define COL_FNM_MODIFY_TIME   1910 /**< Query column for FNM modification times. */

/* R_RULE_FNM_MAP (for storing versions of the rules */
#define COL_FNM_BASE_MAP_VERSION      1911 /**< Query column for FNM base map versions. */
#define COL_FNM_BASE_MAP_BASE_NAME    1912 /**< Query column for FNM base map base names. */
#define COL_FNM_BASE_MAP_OWNER_NAME   1913 /**< Query column for FNM base map owner names. */
#define COL_FNM_BASE_MAP_OWNER_ZONE   1914 /**< Query column for FNM base map owner zones. */
#define COL_FNM_BASE_MAP_COMMENT      1915 /**< Query column for FNM base map comments. */
#define COL_FNM_BASE_MAP_CREATE_TIME  1916 /**< Query column for FNM base map creation times. */
#define COL_FNM_BASE_MAP_MODIFY_TIME  1917 /**< Query column for FNM base map modification times. */

/* R_QUOTA_MAIN */
#define COL_QUOTA_USER_ID     2000 /**< Query column for quota user identifiers. */
#define COL_QUOTA_RESC_ID     2001 /**< Query column for quota resource identifiers. */
#define COL_QUOTA_LIMIT       2002 /**< Query column for quota limits. */
#define COL_QUOTA_OVER        2003 /**< Query column for quota overage values. */
#define COL_QUOTA_MODIFY_TIME 2004 /**< Query column for quota modification times. */

/* R_QUOTA_USAGE */
#define COL_QUOTA_USAGE_USER_ID     2010 /**< Query column for quota usage user identifiers. */
#define COL_QUOTA_USAGE_RESC_ID     2011 /**< Query column for quota usage resource identifiers. */
#define COL_QUOTA_USAGE             2012 /**< Query column for quota usage values. */
#define COL_QUOTA_USAGE_MODIFY_TIME 2013 /**< Query column for quota usage modification times. */

/* For use with quotas */
#define COL_QUOTA_RESC_NAME  2020 /**< Query column for quota resource names. */
#define COL_QUOTA_USER_NAME  2021 /**< Query column for quota user names. */
#define COL_QUOTA_USER_ZONE  2022 /**< Query column for quota user zones. */
#define COL_QUOTA_USER_TYPE  2023 /**< Query column for quota user types. */

#define COL_MSRVC_ID 2100 /**< Query column for microservice identifiers. */
#define COL_MSRVC_NAME 2101 /**< Query column for microservice names. */
#define COL_MSRVC_SIGNATURE 2102 /**< Query column for microservice signatures. */
#define COL_MSRVC_DOXYGEN 2103 /**< Query column for microservice Doxygen text. */
#define COL_MSRVC_VARIATIONS 2104 /**< Query column for microservice variations. */
#define COL_MSRVC_STATUS 2105 /**< Query column for microservice statuses. */
#define COL_MSRVC_OWNER_NAME 2106 /**< Query column for microservice owner names. */
#define COL_MSRVC_OWNER_ZONE 2107 /**< Query column for microservice owner zones. */
#define COL_MSRVC_COMMENT 2108 /**< Query column for microservice comments. */
#define COL_MSRVC_CREATE_TIME 2109 /**< Query column for microservice creation times. */
#define COL_MSRVC_MODIFY_TIME 2110 /**< Query column for microservice modification times. */
#define COL_MSRVC_VERSION 2111 /**< Query column for microservice versions. */
#define COL_MSRVC_HOST 2112 /**< Query column for microservice host names. */
#define COL_MSRVC_LOCATION 2113 /**< Query column for microservice locations. */
#define COL_MSRVC_LANGUAGE 2114 /**< Query column for microservice languages. */
#define COL_MSRVC_TYPE_NAME 2115 /**< Query column for microservice type names. */
#define COL_MSRVC_MODULE_NAME 2116 /**< Query column for microservice module names. */

#define COL_MSRVC_VER_OWNER_NAME 2150 /**< Query column for microservice version owner names. */
#define COL_MSRVC_VER_OWNER_ZONE 2151 /**< Query column for microservice version owner zones. */
#define COL_MSRVC_VER_COMMENT 2152 /**< Query column for microservice version comments. */
#define COL_MSRVC_VER_CREATE_TIME 2153 /**< Query column for microservice version creation times. */
#define COL_MSRVC_VER_MODIFY_TIME 2154 /**< Query column for microservice version modification times. */

/* Tickets */
#define COL_TICKET_ID 2200 /**< Query column for ticket identifiers. */
#define COL_TICKET_STRING 2201 /**< Query column for ticket strings. */
#define COL_TICKET_TYPE 2202 /**< Query column for ticket types. */
#define COL_TICKET_USER_ID 2203 /**< Query column for ticket owner user identifiers. */
#define COL_TICKET_OBJECT_ID 2204 /**< Query column for ticket object identifiers. */
#define COL_TICKET_OBJECT_TYPE 2205 /**< Query column for ticket object types. */
#define COL_TICKET_USES_LIMIT 2206 /**< Query column for ticket use limits. */
#define COL_TICKET_USES_COUNT 2207 /**< Query column for ticket use counts. */
#define COL_TICKET_EXPIRY_TS 2208 /**< Query column for ticket expiry timestamps. */
#define COL_TICKET_CREATE_TIME 2209 /**< Query column for ticket creation times. */
#define COL_TICKET_MODIFY_TIME 2210 /**< Query column for ticket modification times. */
#define COL_TICKET_WRITE_FILE_COUNT 2211 /**< Query column for ticket write file counts. */
#define COL_TICKET_WRITE_FILE_LIMIT 2212 /**< Query column for ticket write file limits. */
#define COL_TICKET_WRITE_BYTE_COUNT 2213 /**< Query column for ticket write byte counts. */
#define COL_TICKET_WRITE_BYTE_LIMIT 2214 /**< Query column for ticket write byte limits. */

#define COL_TICKET_ALLOWED_HOST_TICKET_ID 2220 /**< Query column for ticket IDs on allowed hosts. */
#define COL_TICKET_ALLOWED_HOST 2221 /**< Query column for allowed ticket hosts. */
#define COL_TICKET_ALLOWED_USER_TICKET_ID 2222 /**< Query column for ticket IDs on allowed users. */
#define COL_TICKET_ALLOWED_USER_NAME 2223 /**< Query column for allowed ticket user names. */
#define COL_TICKET_ALLOWED_GROUP_TICKET_ID 2224 /**< Query column for ticket IDs on allowed groups. */
#define COL_TICKET_ALLOWED_GROUP_NAME 2225 /**< Query column for allowed ticket group names. */

#define COL_TICKET_DATA_NAME 2226 /**< Query column for ticket data names. */
#define COL_TICKET_DATA_COLL_NAME 2227 /**< Query column for ticket data collection names. */
#define COL_TICKET_COLL_NAME 2228 /**< Query column for ticket collection names. */

#define COL_TICKET_OWNER_NAME 2229 /**< Query column for ticket owner names. */
#define COL_TICKET_OWNER_ZONE 2230 /**< Query column for ticket owner zones. */

#endif /* RODS_GEN_QUERY_H__ */
