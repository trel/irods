#ifndef FILE_RMDIR_H__
#define FILE_RMDIR_H__

#include "irods/rodsDef.h"
#include "irods/rcConnect.h"

// definition for flags of fileRmdirInp_t
#define RMDIR_RECUR     0x1

/** Input for removing a directory through a resource plugin. */
typedef struct {
    int flags; ///< Request flags controlling directory removal behavior.
    rodsHostAddr_t addr; ///< Host address of the target server.
    char dirName[MAX_NAME_LEN]; ///< Directory path to remove.
    char rescHier[MAX_NAME_LEN]; ///< Resource hierarchy associated with the request.
} fileRmdirInp_t;

#define fileRmdirInp_PI "int flag; struct RHostAddr_PI; str dirName[MAX_NAME_LEN]; str rescHier[MAX_NAME_LEN];"

#ifdef __cplusplus
extern "C"
#endif
/**
 * Remove a directory through a resource plugin.
 *
 * @param[in] conn Client connection handle.
 * @param[in] fileRmdirInp Directory removal request.
 *
 * @return Operation status.
 */
int rcFileRmdir( rcComm_t *conn, fileRmdirInp_t *fileRmdirInp );

#endif
