#ifndef STRUCT_FILE_SYNC_H__
#define STRUCT_FILE_SYNC_H__

#include "irods/rcConnect.h"
#include "irods/rodsDef.h"
#include "irods/objInfo.h"

/// \brief Input for synchronizing a structured file or special collection.
typedef struct StructFileOprInp {
    rodsHostAddr_t addr;    ///< Target host address for the operation.
    int oprType;            ///< Operation type defined by syncMountedColl.h.
    int flags;              ///< Operation flags.
    specColl_t *specColl;   ///< Structured file or special collection to synchronize.
    keyValPair_t condInput; ///< Optional keyword-value options such as checksum control.
} structFileOprInp_t;

/// \brief Packing instruction for \ref structFileOprInp_t.
#define StructFileOprInp_PI "struct RHostAddr_PI; int oprType; int flags; struct *SpecColl_PI; struct KeyValPair_PI;"

/// \brief Synchronizes a structured file or special collection.
/// \param[in] conn Client connection handle.
/// \param[in] structFileOprInp Operation input.
/// \return Integer error code.
int rcStructFileSync( rcComm_t *conn, structFileOprInp_t *structFileOprInp );

#endif
