#ifndef FILE_UNLINK_H__
#define FILE_UNLINK_H__

#include "irods/rodsDef.h"
#include "irods/rcConnect.h"

/** Input for unlinking a filesystem path through a resource plugin. */
typedef struct {
    rodsHostAddr_t addr; ///< Host address of the target server.
    char fileName[MAX_NAME_LEN]; ///< Filesystem path to unlink.
    char rescHier[MAX_NAME_LEN]; ///< Resource hierarchy associated with the request.
    char objPath[MAX_NAME_LEN]; ///< Logical path associated with the request.
    char in_pdmo[MAX_NAME_LEN]; ///< PDMO marker used during unlink processing.
} fileUnlinkInp_t;
#define fileUnlinkInp_PI "struct RHostAddr_PI; str fileName[MAX_NAME_LEN]; str rescHier[MAX_NAME_LEN]; str objPath[MAX_NAME_LEN]; str in_pdmo[MAX_NAME_LEN];"

#ifdef __cplusplus
extern "C"
#endif
/**
 * Unlink a filesystem path through the resource layer.
 *
 * @param[in] conn Client connection handle.
 * @param[in] fileUnlinkInp Unlink request input.
 *
 * @return Operation status.
 */
int rcFileUnlink( rcComm_t *conn, fileUnlinkInp_t *fileUnlinkInp );

#endif
