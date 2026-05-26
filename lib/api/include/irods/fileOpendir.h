#ifndef FILE_OPENDIR_H__
#define FILE_OPENDIR_H__

#include "irods/rodsDef.h"
#include "irods/rcConnect.h"

/** Input for opening a directory through a resource plugin. */
typedef struct {
    char resc_name_[MAX_NAME_LEN]; ///< Resource name associated with the request.
    char resc_hier_[MAX_NAME_LEN]; ///< Resource hierarchy associated with the request.
    char objPath[MAX_NAME_LEN]; ///< Logical path associated with the request.
    rodsHostAddr_t addr; ///< Host address of the target server.
    char dirName[MAX_NAME_LEN]; ///< Directory path to open.
} fileOpendirInp_t;

#define fileOpendirInp_PI "str resc_name_[MAX_NAME_LEN]; str resc_hier_[MAX_NAME_LEN]; str objPath[MAX_NAME_LEN]; struct RHostAddr_PI; str dirName[MAX_NAME_LEN];"


#ifdef __cplusplus
extern "C"
#endif
/**
 * Open a directory through a resource plugin.
 *
 * @param[in] conn Client connection handle.
 * @param[in] fileOpendirInp Directory open request.
 *
 * @return Operation status.
 */
int rcFileOpendir( rcComm_t *conn, fileOpendirInp_t *fileOpendirInp );

#endif
