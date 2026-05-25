#ifndef SSL_START_H__
#define SSL_START_H__

#include "irods/rcConnect.h"

/** Input for starting SSL negotiation on a client connection. */
typedef struct {
    /** Reserved argument for the SSL start request. */
    char *arg0;
} sslStartInp_t;
/** Packing instruction string for ::sslStartInp_t. */
#define sslStartInp_PI "str *arg0;"

#ifdef __cplusplus
extern "C"
#endif
/**
 * Start SSL negotiation for a client connection.
 *
 * @param[in] conn Client connection handle.
 * @param[in] sslStartInp SSL start request input.
 *
 * @return Operation status.
 */
int rcSslStart( rcComm_t *conn, sslStartInp_t *sslStartInp );

#endif
