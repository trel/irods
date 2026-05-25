#ifndef FILE_MKDIR_H__
#define FILE_MKDIR_H__

#include "irods/rodsDef.h"
#include "irods/rcConnect.h"

/** Input for creating a directory through a resource plugin. */
typedef struct {
    rodsHostAddr_t addr; ///< Host address of the target server.
    char dirName[MAX_NAME_LEN]; ///< Directory path to create.
    char rescHier[MAX_NAME_LEN]; ///< Resource hierarchy associated with the request.
    int mode; ///< Filesystem mode for the new directory.
    keyValPair_t condInput; ///< Conditional input keywords for the request.
} fileMkdirInp_t;
#define fileMkdirInp_PI "struct RHostAddr_PI; str dirName[MAX_NAME_LEN]; str rescHier[MAX_NAME_LEN]; int mode; struct KeyValPair_PI;"

#ifdef __cplusplus
extern "C"
#endif
/**
 * Create a directory through a resource plugin.
 *
 * @param[in] conn Client connection handle.
 * @param[in] fileMkdirInp Directory creation request.
 *
 * @return Operation status.
 */
int rcFileMkdir( rcComm_t *conn, fileMkdirInp_t *fileMkdirInp );

#endif
