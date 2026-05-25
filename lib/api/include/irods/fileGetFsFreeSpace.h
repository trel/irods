#ifndef IRODS_FILE_GET_FS_FREESPACE_H
#define IRODS_FILE_GET_FS_FREESPACE_H

#include "irods/rodsDef.h"
#include "irods/rcConnect.h"

/** Input for querying free space on a target filesystem resource. */
typedef struct FileGetFsFreespaceInp
{
    rodsHostAddr_t addr; ///< Host address of the target server.
    char fileName[MAX_NAME_LEN]; ///< Filesystem path used for the query.
    char rescHier[MAX_NAME_LEN]; ///< Resource hierarchy associated with the request.
    char objPath[MAX_NAME_LEN]; ///< Logical path associated with the request.
    int flag; ///< Request flags controlling the operation.
} fileGetFsFreeSpaceInp_t;
#define fileGetFsFreeSpaceInp_PI "struct RHostAddr_PI; str fileName[MAX_NAME_LEN]; str rescHier[MAX_NAME_LEN]; str objPath[MAX_NAME_LEN]; int flag;"

/** Output containing the discovered free-space value. */
typedef struct FileGetFsFreespaceOut
{
    rodsLong_t size; ///< Available free space in bytes.
} fileGetFsFreeSpaceOut_t;
#define fileGetFsFreeSpaceOut_PI "double size;"

#ifdef __cplusplus
extern "C"
#endif
/**
 * Query free filesystem space for a resource.
 *
 * @param[in] conn Client connection handle.
 * @param[in] fileGetFsFreeSpaceInp Free-space request input.
 * @param[out] fileGetFsFreeSpaceOut Output structure containing the free-space value.
 *
 * @return Operation status.
 */
int rcFileGetFsFreeSpace( rcComm_t *conn, fileGetFsFreeSpaceInp_t *fileGetFsFreeSpaceInp, fileGetFsFreeSpaceOut_t **fileGetFsFreeSpaceOut );

#endif // IRODS_FILE_GET_FS_FREESPACE_H
