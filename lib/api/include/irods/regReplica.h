#ifndef REG_REPLICA_H__
#define REG_REPLICA_H__

#include "irods/rcConnect.h"
#include "irods/objInfo.h"

/** Input for registering an existing replica in the catalog. */
typedef struct {
    dataObjInfo_t *srcDataObjInfo; ///< Source replica metadata used as the registration template.
    dataObjInfo_t *destDataObjInfo; ///< Destination replica metadata to register.
    keyValPair_t condInput; ///< Conditional input keywords for replica registration.
} regReplica_t;
/** Packing instruction string for ::regReplica_t. */
#define RegReplica_PI "struct *DataObjInfo_PI; struct *DataObjInfo_PI; struct KeyValPair_PI;"


#ifdef __cplusplus
extern "C"
#endif
/**
 * Register an existing replica in the catalog.
 *
 * @param[in] conn Client connection handle.
 * @param[in] regReplicaInp Replica registration request.
 *
 * @return Operation status.
 */
int rcRegReplica( rcComm_t *conn, regReplica_t *regReplicaInp );

#endif
