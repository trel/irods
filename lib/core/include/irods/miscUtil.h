/*** Copyright (c), The Regents of the University of California            ***
 *** For more information please refer to files in the COPYRIGHT directory ***/
/* miscUtil.h - Header file for miscUtil.c */

#ifndef MISC_UTIL_H__
#define MISC_UTIL_H__

#include "irods/rodsPath.h"
#include "irods/parseCommandLine.h"
#include "irods/guiProgressCallback.h"

/** Sentinel value indicating that the process umask has not been initialized. */
#define	INIT_UMASK_VAL	99999999

/** Results returned for a collection metadata query. */
typedef struct CollSqlResult {
    int rowCnt;                  ///< Number of rows in the current result set.
    int attriCnt;                ///< Number of selected attributes.
    int continueInx;             ///< Continuation index for paged queries.
    int totalRowCount;           ///< Total number of rows available.
    sqlResult_t collName;        ///< Collection name column.
    sqlResult_t collType;        ///< Collection type column.
    sqlResult_t collInfo1;       ///< First collection info column.
    sqlResult_t collInfo2;       ///< Second collection info column.
    sqlResult_t collOwner;       ///< Collection owner column.
    sqlResult_t collCreateTime;  ///< Collection create time column.
    sqlResult_t collModifyTime;  ///< Collection modify time column.
} collSqlResult_t;

/** Metadata describing a collection entry. */
typedef struct CollMetaInfo {
    char *collName;      ///< Collection name.
    char *collOwner;     ///< Collection owner.
    specColl_t specColl; ///< Special collection information.
} collMetaInfo_t;

/** Results returned for a data object metadata query. */
typedef struct DataObjSqlResult {
    int rowCnt;                ///< Number of rows in the current result set.
    int attriCnt;              ///< Number of selected attributes.
    int continueInx;           ///< Continuation index for paged queries.
    int totalRowCount;         ///< Total number of rows available.
    sqlResult_t collName;      ///< Collection name column.
    sqlResult_t dataName;      ///< Data object name column.
    sqlResult_t dataMode;      ///< Data mode column.
    sqlResult_t dataSize;      ///< Data size column.
    sqlResult_t createTime;    ///< Create time column.
    sqlResult_t modifyTime;    ///< Modify time column.
    sqlResult_t chksum;        ///< Checksum column used by rsync logic.
    sqlResult_t replStatus;    ///< Replica status column used by rsync logic.
    sqlResult_t dataId;        ///< Data identifier column used by rsync logic.
    sqlResult_t resource;      ///< Resource name column.
    sqlResult_t resc_id;       ///< Resource identifier column.
    sqlResult_t resc_hier;     ///< Resource hierarchy column.
    sqlResult_t phyPath;       ///< Physical path column.
    sqlResult_t ownerName;     ///< Owner name column.
    sqlResult_t replNum;       ///< Replica number column.
    sqlResult_t dataType;      ///< Data type column.
} dataObjSqlResult_t;

/** Metadata describing a data object entry. */
typedef struct DataObjMetaInfo {
    char *collName;    ///< Parent collection name.
    char *dataName;    ///< Data object name.
    char *dataSize;    ///< Data object size string.
    char *createTime;  ///< Create time string.
    char *modifyTime;  ///< Modify time string.
    char *chksum;      ///< Checksum string.
    char *replStatus;  ///< Replica status string.
    char *dataId;      ///< Data identifier string.
} dataObjMetaInfo_t;

/** States used by `collHandle_t`. */
typedef enum CollState {
    COLL_CLOSED,            ///< The collection handle is closed.
    COLL_OPENED,            ///< The collection handle is open.
    COLL_DATA_OBJ_QUERIED,  ///< Data objects have been queried.
    COLL_COLL_OBJ_QUERIED   ///< Child collections have been queried.
} collState_t;

/** Connection kinds supported by `queryHandle_t`. */
typedef enum ConnType {
    RC_COMM, ///< Client connection.
    RS_COMM  ///< Server connection.
} connType_t;

/** Query callback table shared by client and server collection code. */
typedef struct QueryHandle {
    void *conn;            ///< Either `rsComm_t` or `rcComm_t`.
    connType_t connType;   ///< Connection type stored in `conn`.
    funcPtr querySpecColl; ///< `rcQuerySpecColl` or `rsQuerySpecColl`.
    funcPtr genQuery;      ///< `rcGenQuery` or `rsGenQuery`.
    funcPtr getHierForId;  ///< Client or server resource hierarchy lookup.
} queryHandle_t;

/** Request verbose metadata during collection reads. */
#define LONG_METADATA_FG           0x1
/** Request very verbose metadata during collection reads. */
#define VERY_LONG_METADATA_FG      0x2
/** Query collections recursively. */
#define RECUR_QUERY_FG             0x4
/** Query data objects before child collections. */
#define DATA_QUERY_FIRST_FG        0x8
/** Preserve replica information without trimming. */
#define NO_TRIM_REPL_FG            0x10
/** Include `condInput` conditions in generated queries. */
#define INCLUDE_CONDINPUT_IN_QUERY 0x20

/** Client-side state for iterating over a collection. */
typedef struct CollHandle {
    collState_t state;                     ///< Current collection iteration state.
    int inuseFlag;                         ///< Indicates whether this handle is active.
    int flags;                             ///< Flags controlling query behavior.
    int rowInx;                            ///< Current row index within the result set.
    rodsObjStat_t *rodsObjStat;            ///< Object stat information for the collection.
    queryHandle_t queryHandle;             ///< Query callbacks and connection state.
    genQueryInp_t genQueryInp;             ///< Generated query input.
    dataObjInp_t dataObjInp;               ///< Data object query input.
    dataObjSqlResult_t dataObjSqlResult;   ///< Cached data object query results.
    collSqlResult_t collSqlResult;         ///< Cached collection query results.
    char linkedObjPath[MAX_NAME_LEN];      ///< Linked object path for mounted collections.
    char prevdataId[NAME_LEN];             ///< Previously returned data identifier.
} collHandle_t;

/** Output record returned by collection iteration functions. */
typedef struct CollEnt {
    objType_t objType;    ///< Type of object represented by this entry.
    int replNum;          ///< Replica number for data objects.
    int replStatus;       ///< Replica status for data objects.
    uint dataMode;        ///< Mode bits for the data object.
    rodsLong_t dataSize;  ///< Data size in bytes.
    char *collName;       ///< Collection name for the entry.
    char *dataName;       ///< Data object name.
    char *dataId;         ///< Data object identifier.
    char *createTime;     ///< Create time string.
    char *modifyTime;     ///< Modify time string.
    char *chksum;         ///< Checksum string.
    char *resource;       ///< Resource name.
    char *resc_hier;      ///< Resource hierarchy string.
    char *phyPath;        ///< Physical path.
    char *ownerName;      ///< Owner name.
    char *dataType;       ///< Data type string.
    specColl_t specColl;  ///< Special collection details for collection entries.
} collEnt_t;

/** Compiled pathname matching patterns. */
typedef struct PathnamePatterns {
    char *pattern_buf;   ///< Backing buffer containing parsed patterns.
    char **patterns;     ///< Array of individual pattern strings.
    int num_patterns;    ///< Number of patterns in `patterns`.
} pathnamePatterns_t;

#ifdef __cplusplus
extern "C" {
#endif

/** Recursively creates directories from `startDir` to `destDir`. */
int
mkdirR( char *startDir, char *destDir, int mode );
/** Recursively removes directories from `destDir` back toward `startDir`. */
int
rmdirR( char *startDir, char *destDir );
/** Creates the collection identified by `collection`. */
int
mkColl( rcComm_t *conn, char *collection );
/** Creates each missing component between `startColl` and `destColl`. */
int
mkCollR( rcComm_t *conn, char *startColl, char *destColl );
/** Determines the iRODS object type referenced by `rodsPath`. */
int
getRodsObjType( rcComm_t *conn, rodsPath_t *rodsPath );
/** Builds a query condition matching all descendants of `collection`. */
int
genAllInCollQCond( char *collection, char *collQCond );
/** Queries child collections contained in `collection`. */
int
queryCollInColl( queryHandle_t *queryHandle, char *collection,
                 int flags, genQueryInp_t *genQueryInp,
                 genQueryOut_t **genQueryOut );
/** Queries data objects contained in `collection`. */
int
queryDataObjInColl( queryHandle_t *queryHandle, char *collection,
                    int flags, genQueryInp_t *genQueryInp,
                    genQueryOut_t **genQueryOut, keyValPair_t *condInput );
/** Populates a general query input for data object queries. */
int
setQueryInpForData( int flags, genQueryInp_t *genQueryInp );

/** Prints transfer timing information for an object. */
int
printTiming( rcComm_t *conn, char *objPath, rodsLong_t fileSize,
             char *localFile, struct timeval *startTime, struct timeval *endTime );
/** Prints elapsed time for an object operation. */
int
printTime( char *objPath, struct timeval *startTime,
           struct timeval *endTime );
/** Initializes process timing output. */
int
initSysTiming( char *procName, char *action, int envVarFlag );
/** Prints collected process timing information. */
int
printSysTiming( char *procName, char *action, int envVarFlag );
/** Prints a message explaining why synchronization was skipped. */
int
printNoSync( char *objPath, rodsLong_t fileSize, char *reason );
/** Queries ACLs for the data object identified by `dataId`. */
int
queryDataObjAcl( rcComm_t *conn, char *dataId, char *zoneHint,
                 genQueryOut_t **genQueryOut ); // JMC - backport 4516
/** Queries ACLs for the collection identified by `collName`. */
int
queryCollAcl( rcComm_t *conn, char *collName, char *zoneHint,
              genQueryOut_t **genQueryOut ); // JMC - backport 4516
/** Queries ACLs for a specific collection in a hinted zone. */
int
queryCollAclSpecific( rcComm_t *conn, char *collName, char *zoneHint,
                      genQueryOut_t **genQueryOut );
/** Queries the inheritance setting for `collName`. */
int
queryCollInheritance( rcComm_t *conn, char *collName,
                      genQueryOut_t **genQueryOut );
/** Extracts object-type information from query result columns. */
int
extractRodsObjType( rodsPath_t *rodsPath, sqlResult_t *dataId,
                    sqlResult_t *replStatus, sqlResult_t *chksum, sqlResult_t *dataSize,
                    int inx, int rowCnt );
/** Converts a general query output into `collSqlResult_t`. */
int
genQueryOutToCollRes( genQueryOut_t **genQueryOut,
                      collSqlResult_t *collSqlResult );
/** Stores a string value into a SQL result column. */
int
setSqlResultValue( sqlResult_t *sqlResult, int attriInx, char *valueStr,
                   int rowCnt );
/** Clears the contents of a collection SQL result. */
int
clearCollSqlResult( collSqlResult_t *collSqlResult );
/** Clears the contents of a data object SQL result. */
int
clearDataObjSqlResult( dataObjSqlResult_t *dataObjSqlResult );
/** Converts a general query output into `dataObjSqlResult_t`. */
int
genQueryOutToDataObjRes( genQueryOut_t **genQueryOut,
                         dataObjSqlResult_t *dataObjSqlResult );
/** Opens a collection for iterative reads. */
int
rclOpenCollection( rcComm_t *conn, char *collection,
                   int flag, collHandle_t *collHandle );
/** Reads the next entry from an open collection. */
int
rclReadCollection( rcComm_t *conn, collHandle_t *collHandle,
                   collEnt_t *collEnt );
/** Reads the next cached entry from `collHandle`. */
int
readCollection( collHandle_t *collHandle, collEnt_t *collEnt );
/** Clears a collection handle and optionally frees special collection state. */
int
clearCollHandle( collHandle_t *collHandle, int freeSpecColl );
/** Closes a collection handle opened by `rclOpenCollection`. */
int
rclCloseCollection( collHandle_t *collHandle );
/** Returns the next collection metadata entry. */
int
getNextCollMetaInfo( collHandle_t *collHandle, collEnt_t *outCollEnt );
/** Returns the next data object metadata entry. */
int
getNextDataObjMetaInfo( collHandle_t *collHandle, collEnt_t *outCollEnt );
/** Generates collection results for the current handle state. */
int
genCollResInColl( queryHandle_t *queryHandle, collHandle_t *collHandle );
/** Generates data object results for the current handle state. */
int
genDataResInColl( queryHandle_t *queryHandle, collHandle_t *collHandle );
/** Converts command-line arguments into collection query flags. */
int
setQueryFlag( rodsArguments_t *rodsArgs );
/** Initializes a query handle for a client connection. */
int
rclInitQueryHandle( queryHandle_t *queryHandle, rcComm_t *conn );
/** Frees dynamically allocated strings stored in `collEnt`. */
int
freeCollEnt( collEnt_t *collEnt );
/** Resets a collection entry to an empty state. */
int
clearCollEnt( collEnt_t *collEnt );
/** Applies mode bits to the local path `inPath`. */
int
myChmod( char *inPath, uint dataMode );
/** Returns the zone hint stored in a general query input. */
char *
getZoneHintForGenQuery( genQueryInp_t *genQueryInp );
/** Resolves the type of `inZoneName` relative to `icatZone`. */
int
getZoneType( rcComm_t *conn, char *icatZone, char *inZoneName,
             char *outZoneType );
/** Computes collection size information for progress reporting. */
int
getCollSizeForProgStat( rcComm_t *conn, char *srcColl,
                        operProgress_t *operProgress );
/** Computes local directory size information for progress reporting. */
int
getDirSizeForProgStat( rodsArguments_t *rodsArgs, char *srcDir,
                       operProgress_t *operProgress );
/** Returns the progress callback used by iCommands. */
guiProgressCallback
iCommandProgStat( operProgress_t *operProgress );
/** Returns the number of opened collections under `collHandle`. */
int
getOpenedCollLen( collHandle_t *collHandle );
/** Removes the subdirectory `mydir`. */
int
rmSubDir( char *mydir );
/** Removes files contained directly in `mydir`. */
int
rmFilesInDir( char *mydir );
/** Creates parent directories for `filePath`. */
int
mkdirForFilePath( char* filePath );
/** Captures file metadata from `srcPath` into `condInput`. */
int
getFileMetaFromPath( char *srcPath, keyValPair_t *condInput );
/** Captures file metadata from a stat buffer into `condInput`. */
int
getFileMetaFromStat( rodsStat_t *statbuf, keyValPair_t *condInput );
/** Parses pathname patterns from a caller-provided buffer. */
pathnamePatterns_t *
readPathnamePatterns( char *buf, int buflen );
/** Frees a pathname pattern set created by `readPathnamePatterns`. */
void
freePathnamePatterns( pathnamePatterns_t *pp );
/** Tests whether `name` in `dirname` matches any configured pattern. */
int
matchPathname( pathnamePatterns_t *pp, char *name, char *dirname );
/** Looks up a resource hierarchy string from a leaf resource identifier. */
int get_resc_hier_from_leaf_id(queryHandle_t* _query_handle, rodsLong_t _resc_id, char* _resc_hier );

/// resolveRodsTarget - based on rodsPathInp->srcPath and
/// rodsPathInp->destPath, fill in targPath.
///
/// oprType -
///      MOVE_OPR - do not create the target coll or dir because rename will
///        take care of it.
///      RSYNC_OPR - uses the destPath and the targPath if the src is a
///        collection
///      All other oprType will be treated as normal.
///
int resolveRodsTarget( rcComm_t *conn, rodsPathInp_t *rodsPathInp, int oprType );

/// resolveRodsTargetDryRun - same as resolveRodsTarget, but do not create
/// target collection, regardless of oprType.
int resolveRodsTargetDryRun(rcComm_t* conn, rodsPathInp_t* rodsPathInp, int oprType);

#ifdef __cplusplus
}
#endif
#endif	// MISC_UTIL_H__
