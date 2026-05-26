#ifndef DATA_COPY_H__
#define DATA_COPY_H__

#include "irods/rcConnect.h"
#include "irods/dataObjInpOut.h"

/**
 * \brief Input passed to `rcDataCopy`.
 */
typedef struct DataCopyInp {
    dataOprInp_t dataOprInp;   ///< Data transfer options for the copy operation.
    portalOprOut_t portalOprOut; ///< Portal operation details for the transfer.
} dataCopyInp_t;
#define DataCopyInp_PI "struct DataOprInp_PI; struct PortalOprOut_PI;"


int rcDataCopy( rcComm_t *conn, dataCopyInp_t *dataCopyInp );

#endif
