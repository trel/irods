#ifndef GET_HOST_FOR_GET_H__
#define GET_HOST_FOR_GET_H__

#include "irods/rcConnect.h"
#include "irods/dataObjClose.h"

#define MAX_HOST_TO_SEARCH      10

/** Statistics describing the hosts considered for a GET operation. */
typedef struct {
    int numHost; ///< Number of hosts considered in the current result set.
    int totalCount; ///< Total number of hosts examined.
    int count[MAX_HOST_TO_SEARCH]; ///< Per-host usage counts.
} hostSearchStat_t;

#ifdef __cplusplus
extern "C"
#endif
/**
 * Select a host for servicing a GET operation.
 *
 * @param[in] conn Client connection handle.
 * @param[in] dataObjInp Data object request input.
 * @param[out] outHost Output containing the selected host.
 *
 * @return Operation status.
 */
int rcGetHostForGet( rcComm_t *conn, dataObjInp_t *dataObjInp, char **outHost );

#endif
