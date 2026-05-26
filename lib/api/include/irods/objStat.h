#ifndef OBJ_STAT_H__
#define OBJ_STAT_H__

/// \file
/// \brief Declares object status query types and functions.

#include "irods/rcConnect.h"
#include "irods/rodsType.h"

/// Minimal iRODS-specific status information for a data object or collection.
typedef struct rodsObjStat {
    rodsLong_t          objSize;        ///< Data size in bytes.
    objType_t           objType;        ///< Object type such as data object or collection.
    uint                dataMode;       ///< Object mode bits.
    char                dataId[NAME_LEN]; ///< Data object identifier.
    char                chksum[CHKSUM_LEN]; ///< Checksum string for the object.
    char                ownerName[NAME_LEN]; ///< Owner user name.
    char                ownerZone[NAME_LEN]; ///< Owner zone name.
    char                createTime[TIME_LEN]; ///< Creation timestamp.
    char                modifyTime[TIME_LEN]; ///< Last modification timestamp.
    specColl_t          *specColl;      ///< Special collection information, if applicable.
    char                rescHier[MAX_NAME_LEN]; ///< Resource hierarchy for the object.
} rodsObjStat_t;


/// Returns status information for the object identified by `dataObjInp->objPath`.
///
/// The returned `rodsObjStat_t` describes whether the path refers to a collection,
/// a data object, or no existing object. If `specColl` is non-null, the path is
/// associated with a special collection and may be translated for follow-up work.
#ifdef __cplusplus
extern "C"
#endif
int rcObjStat( rcComm_t *conn, dataObjInp_t *dataObjInp, rodsObjStat_t **rodsObjStatOut );

#endif
