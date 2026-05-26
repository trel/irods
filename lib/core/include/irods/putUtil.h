/*** Copyright (c), The Regents of the University of California            ***
 *** For more information please refer to files in the COPYRIGHT directory ***/
/* putUtil.h - Header for for putUtil.c */

/// \file
/// \brief Declares client-side helpers for putting data into iRODS.

#ifndef PUT_UTIL_H__
#define PUT_UTIL_H__

#include "irods/rodsClient.h"
#include "irods/parseCommandLine.h"
#include "irods/rodsPath.h"

#ifdef __cplusplus
extern "C" {
#endif

/// \brief Indicates that bulk put support is not in use.
#define NON_BULK_OPR         0x0
/// \brief Indicates that bulk put support is handling small files.
#define BULK_OPR_SMALL_FILES 0x1 // bulk opr for small files
/// \brief Indicates that bulk put support is handling large files.
#define BULK_OPR_LARGE_FILES 0x2 // bulk opr for large files

/// \brief Default root directory for physical bundle creation.
#define DEF_PHY_BUN_ROOT_DIR "/tmp"

/// \brief Tracks state for a bulk put operation.
typedef struct {
    int flags;                                     ///< Bulk operation flags.
    int count;                                     ///< Number of files accumulated for the bundle.
    int forceFlagAdded;                            ///< Indicates whether the force flag was added.
    int size;                                      ///< Total size of accumulated data.
    char cwd[MAX_NAME_LEN];                        ///< Working directory for the operation.
    char phyBunDir[MAX_NAME_LEN];                  ///< Physical bundle directory path.
    char cachedTargPath[MAX_NAME_LEN];             ///< Cached target path.
    char cachedSubPhyBunDir[MAX_NAME_LEN];         ///< Cached subdirectory under the bundle directory.
    char phyBunPath[MAX_NUM_BULK_OPR_FILES][MAX_NAME_LEN]; ///< Paths of physical bundle members.
    bytesBuf_t bytesBuf;                           ///< Buffer holding bundled data.
} bulkOprInfo_t;

/// \brief Uploads one or more local paths into iRODS.
int
putUtil( rcComm_t **myConn, rodsEnv *myEnv, rodsArguments_t *myRodsArgs,
         rodsPathInp_t *rodsPathInp );
/// \brief Uploads a single local file into iRODS.
int
putFileUtil( rcComm_t *conn, char *srcPath, char *targPath,
             rodsLong_t srcSize, rodsArguments_t *myRodsArgs,
             dataObjInp_t *dataObjOprInp );
/// \brief Initializes input conditions for a put operation.
int
initCondForPut( rcComm_t *conn, rodsEnv *myRodsEnv, rodsArguments_t *rodsArgs,
                 dataObjInp_t *dataObjOprInp, bulkOprInp_t *bulkOprInp,
                 rodsRestart_t *rodsRestart );
/// \brief Uploads a local directory into a target collection.
int
putDirUtil( rcComm_t **myConn, char *srcDir, char *targColl,
            rodsEnv *myRodsEnv, rodsArguments_t *rodsArgs, dataObjInp_t *dataObjOprInp,
            bulkOprInp_t *bulkOprInp, rodsRestart_t *rodsRestart, bulkOprInfo_t *bulkOprInfo );
/// \brief Uploads a directory using bulk put semantics.
int
bulkPutDirUtil( rcComm_t **myConn, char *srcDir, char *targColl,
                 rodsEnv *myRodsEnv, rodsArguments_t *rodsArgs, dataObjInp_t *dataObjOprInp,
                 bulkOprInp_t *bulkOprInp, rodsRestart_t *rodsRestart );
/// \brief Builds the physical bundle directory path for a user.
int
getPhyBunDir( char *phyBunRootDir, char *userName, char *outPhyBunDir );
/// \brief Adds a file to an in-progress bulk put operation.
int
bulkPutFileUtil( rcComm_t *conn, char *srcPath, char *targPath,
                  rodsLong_t srcSize, int createMode,
                  rodsArguments_t *myRodsArgs, bulkOprInp_t *bulkOprInp,
                  bulkOprInfo_t *bulkOprInfo );
/// \brief Archives bundled files and uploads the archive.
int
tarAndBulkPut( rcComm_t *conn, bulkOprInp_t *bulkOprInp,
                bulkOprInfo_t *bulkOprInfo, rodsArguments_t *rodsArgs );
/// \brief Sends a prepared bulk put payload to the server.
int
sendBulkPut( rcComm_t *conn, bulkOprInp_t *bulkOprInp,
              bulkOprInfo_t *bulkOprInfo, rodsArguments_t *rodsArgs );
/// \brief Clears the tracked state for a bulk put operation.
int
clearBulkOprInfo( bulkOprInfo_t *bulkOprInfo );
/// \brief Sets the force flag when restarting a bulk put operation.
int
setForceFlagForRestart( bulkOprInp_t *bulkOprInp, bulkOprInfo_t *bulkOprInfo );
/// \brief Checks restart state before resuming an upload.
int
chkStateForResume( rcComm_t * conn, rodsRestart_t * rodsRestart,
                    char * targPath, rodsArguments_t * rodsArgs, objType_t objType,
                    keyValPair_t * condInput, int deleteFlag );
/// \brief Records restart state for a resumable upload.
int
setStateForResume( rcComm_t *conn, rodsRestart_t *rodsRestart,
                    char *restartPath, objType_t objType, keyValPair_t *condInput,
                    int deleteFlag );
#ifdef __cplusplus
}
#endif
#endif	// PUT_UTIL_H__
