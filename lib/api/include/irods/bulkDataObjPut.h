#ifndef BULK_DATA_OBJ_PUT_H__
#define BULK_DATA_OBJ_PUT_H__

#include "irods/rodsGenQuery.h"
#include "irods/rodsDef.h"
#include "irods/rcConnect.h"

/** Temporary physical bundle directory keyword. */
#define TMP_PHY_BUN_DIR "tmpPhyBunDir"

/** Input for bulk data object put operations. */
typedef struct BulkOperationInp {
    /** Target collection path for the extracted files. */
    char objPath[MAX_NAME_LEN];
    /** Attribute array describing bundled file metadata such as checksums. */
    genQueryOut_t attriArray;   /* arrays of attrib - chksum */
    /** Conditional input keywords for the operation. */
    keyValPair_t condInput;   /* include chksum flag and value */
} bulkOprInp_t;

/** Tracks physical files renamed during a bulk operation. */
typedef struct RenamedPhyFiles {
    /** Number of renamed files recorded in this structure. */
    int count;
    /** Logical paths of affected objects. */
    char objPath[MAX_NUM_BULK_OPR_FILES][MAX_NAME_LEN];
    /** Original physical file paths. */
    char origFilePath[MAX_NUM_BULK_OPR_FILES][MAX_NAME_LEN];
    /** Replacement physical file paths. */
    char newFilePath[MAX_NUM_BULK_OPR_FILES][MAX_NAME_LEN];
} renamedPhyFiles_t;

/** Packing instruction string for ::bulkOprInp_t. */
#define BulkOprInp_PI "str objPath[MAX_NAME_LEN]; struct GenQueryOut_PI; struct KeyValPair_PI;"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Bulk put a tar bundle of local files into iRODS.
 *
 * @param[in] conn Client connection handle.
 * @param[in] bulkOprInp Operation input describing the destination path and
 *                       conditional keywords.
 * @param[in] bulkOprInpBBuf Buffer containing bundled local files in tar format.
 *
 * @return Operation status.
 */
int rcBulkDataObjPut(rcComm_t* conn, bulkOprInp_t* bulkOprInp, bytesBuf_t* bulkOprInpBBuf);

#ifdef __cplusplus
}
#endif

#endif  // BULK_DATA_OBJ_PUT_H__
