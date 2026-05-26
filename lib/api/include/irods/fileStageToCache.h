#ifndef IRODS_FILE_STAGE_TO_CACHE_H
#define IRODS_FILE_STAGE_TO_CACHE_H

/// \file
/// \brief Declares the file stage-to-cache API.

#include "irods/rodsType.h"
#include "irods/rodsDef.h"
#include "irods/objInfo.h"
#include "irods/rcConnect.h"

/// Input for staging a file into the cache.
typedef struct FileStageSyncInp
{
    int mode;                         ///< Requested file mode.
    int flags;                        ///< Operation flags.
    rodsLong_t dataSize;              ///< Size of the file data.
    rodsHostAddr_t addr;              ///< Host address of the resource server.
    char filename[MAX_NAME_LEN];      ///< Source file path.
    char cacheFilename[MAX_NAME_LEN]; ///< Destination cache file path.
    char objPath[MAX_NAME_LEN];       ///< Logical path of the data object.
    char rescHier[MAX_NAME_LEN];      ///< Resource hierarchy for the operation.
    keyValPair_t condInput;           ///< Additional keyword options.
} fileStageSyncInp_t;
/// Packing instruction for `fileStageSyncInp_t`.
#define fileStageSyncInp_PI "int mode; int flags; double dataSize; struct RHostAddr_PI; str filename[MAX_NAME_LEN]; str cacheFilename[MAX_NAME_LEN]; str objPath[MAX_NAME_LEN]; str rescHier[MAX_NAME_LEN]; struct KeyValPair_PI;"

/// Output for a file synchronization operation.
typedef struct FileSyncOut
{
    char file_name[MAX_NAME_LEN]; ///< Path of the synchronized file.
} fileSyncOut_t;
/// Packing instruction for `fileSyncOut_t`.
#define fileSyncOut_PI "str file_name[MAX_NAME_LEN];"


/// Stages a file from archive storage into cache storage.
#ifdef __cplusplus
extern "C"
#endif
int rcFileStageToCache( rcComm_t *conn, fileStageSyncInp_t *fileStageToCacheInp );

#endif // IRODS_FILE_STAGE_TO_CACHE_H
