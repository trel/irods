#ifndef FILE_CHMOD_H__
#define FILE_CHMOD_H__

#include "irods/rodsDef.h"
#include "irods/rcConnect.h"

/** Input for changing filesystem mode bits through a resource plugin. */
typedef struct {
    rodsHostAddr_t addr; ///< Host address of the target server.
    char fileName[MAX_NAME_LEN]; ///< Filesystem path whose mode will be changed.
    int mode; ///< New filesystem mode bits.
    char rescHier[MAX_NAME_LEN]; ///< Resource hierarchy associated with the request.
    char objPath[MAX_NAME_LEN]; ///< Logical path associated with the request.
} fileChmodInp_t;

#define fileChmodInp_PI "struct RHostAddr_PI; str fileName[MAX_NAME_LEN]; int mode; str rescHier[MAX_NAME_LEN]; str objPath[MAX_NAME_LEN];"

#ifdef __cplusplus
extern "C"
#endif
/**
 * Change filesystem mode bits through a resource plugin.
 *
 * @param[in] conn Client connection handle.
 * @param[in] fileChmodInp Mode-change request input.
 *
 * @return Operation status.
 */
int rcFileChmod( rcComm_t *conn, fileChmodInp_t *fileChmodInp );

#endif
