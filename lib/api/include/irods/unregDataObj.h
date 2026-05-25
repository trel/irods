#ifndef IRODS_UNREG_DATA_OBJ_H
#define IRODS_UNREG_DATA_OBJ_H

#include "irods/rcConnect.h"
#include "irods/objInfo.h"

/// \brief Input for unregistering a data object from the catalog.
typedef struct {
    dataObjInfo_t *dataObjInfo; ///< Data object information identifying what to unregister.
    keyValPair_t *condInput;    ///< Optional keyword-value options controlling the operation.
} unregDataObj_t;

/// \brief Packing instruction for \ref unregDataObj_t.
#define UnregDataObj_PI "struct *DataObjInfo_PI; struct *KeyValPair_PI;"

#ifdef __cplusplus
extern "C" {
#endif

/// \brief Unregisters all replicas of a data object from the catalog without removing the underlying storage data.
///
/// \param[in] conn Client connection handle.
/// \param[in] unregDataObjInp Operation input containing at least a valid object path in \p dataObjInfo.
///
/// \return An integer representing an iRODS error code.
/// \retval 0 on success.
/// \retval <0 on failure.
int rcUnregDataObj( rcComm_t *conn, unregDataObj_t *unregDataObjInp );

#ifdef __cplusplus
} // extern "C"
#endif

#endif // IRODS_UNREG_DATA_OBJ_H
