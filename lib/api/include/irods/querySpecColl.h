#ifndef QUERY_SPEC_COLL_H__
#define QUERY_SPEC_COLL_H__

#include "irods/rcConnect.h"
#include "irods/objInfo.h"
#include "irods/rodsGenQuery.h"
#include "irods/dataObjInpOut.h"

/** Descriptor describing an open special collection query context. */
typedef struct specCollDesc {
    int l3descInx; ///< Low-level descriptor index associated with the special collection.
    int inuseFlag; ///< Non-zero when this descriptor slot is in use.
    dataObjInfo_t *dataObjInfo; ///< Data object information associated with the special collection.
    int parentInx; ///< Index of the parent special collection descriptor.
} specCollDesc_t;

/** Maximum number of tracked special collection descriptors. */
#define NUM_SPEC_COLL_DESC    100
/** Maximum number of rows returned per special collection query. */
#define MAX_SPEC_COLL_ROW     100

/** Packing instruction string for the special collection query input. */
#define QuerySpecCollInp_PI "str objPath[MAX_NAME_LEN]; int recurFlag; int type; int continueInx; struct SpecCollMeta_PI;"

#ifdef __cplusplus
extern "C"
#endif
/**
 * Query the contents of a special collection.
 *
 * @param[in] conn Client connection handle.
 * @param[in] querySpecCollInp Query input describing the target collection.
 * @param[out] genQueryOut Query results for the special collection.
 *
 * @return Operation status.
 */
int rcQuerySpecColl( rcComm_t *conn, dataObjInp_t *querySpecCollInp, genQueryOut_t **genQueryOut );

#endif
