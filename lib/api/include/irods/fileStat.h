#ifndef IRODS_FILE_STAT_H
#define IRODS_FILE_STAT_H

#include "irods/rodsDef.h"
#include "irods/rodsType.h"

struct RcComm;
struct rodsStat;

/** Input for requesting filesystem metadata through a resource plugin. */
typedef struct FileStatInp
{
    rodsHostAddr_t addr; ///< Host address of the target server.
    char fileName[MAX_NAME_LEN]; ///< Filesystem path to stat.
    char rescHier[MAX_NAME_LEN]; ///< Resource hierarchy associated with the request.
    char objPath[MAX_NAME_LEN]; ///< Logical path associated with the request.
    rodsLong_t rescId; ///< Resource identifier associated with the request.
} fileStatInp_t;

#define fileStatInp_PI "struct RHostAddr_PI; str fileName[MAX_NAME_LEN]; str rescHier[MAX_NAME_LEN]; str objPath[MAX_NAME_LEN]; double rescId;"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Query filesystem metadata through a resource plugin.
 *
 * @param[in] conn Client connection handle.
 * @param[in] fileStatInp Stat request input.
 * @param[out] fileStatOut Output structure containing the filesystem metadata.
 *
 * @return Operation status.
 */
int rcFileStat(struct RcComm* conn, fileStatInp_t* fileStatInp, struct rodsStat** fileStatOut);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // IRODS_FILE_STAT_H
