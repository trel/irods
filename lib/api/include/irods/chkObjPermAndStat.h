#ifndef CHK_OBJ_PERM_AND_STAT_H__
#define CHK_OBJ_PERM_AND_STAT_H__

#include "irods/rcConnect.h"

// definition for flags
#define CHK_COLL_FOR_BUNDLE_OPR      0x1

/** Input for checking object permission and status. */
typedef struct {
    char objPath[MAX_NAME_LEN]; ///< Path of the object or collection to inspect.
    char permission[NAME_LEN]; ///< Permission string to verify.
    int flags; ///< Request flags controlling additional checks.
    int status; ///< Status field used for request and response handling.
    keyValPair_t condInput; ///< Conditional input keywords for the request.
} chkObjPermAndStat_t;

#define ChkObjPermAndStat_PI "str objPath[MAX_NAME_LEN]; str permission[NAME_LEN]; int flags; int status; struct KeyValPair_PI;"

/* rcChkObjPermAndStat - Unregister a iRODS dataObject.
 * Input -
 *   rcComm_t *conn - The client connection handle.
 *   chkObjPermAndStat_t *chkObjPermAndStatInp - the dataObjInfo to unregister
 *
 * OutPut -
 *   int status - status of the operation.
 */
/**
 * Check object permissions and status.
 *
 * @param[in] conn Client connection handle.
 * @param[in] chkObjPermAndStatInp Permission and status request input.
 *
 * @return Operation status.
 */
int rcChkObjPermAndStat( rcComm_t *conn, chkObjPermAndStat_t *chkObjPermAndStatInp );

#endif
