#ifndef SSL_END_H__
#define SSL_END_H__

#include "irods/rcConnect.h"

/** Input for ending SSL negotiation on a client connection. */
typedef struct {
    char *arg0; ///< Reserved argument for the SSL end request.
} sslEndInp_t;
/** Packing instruction string for ::sslEndInp_t. */
#define sslEndInp_PI "str *arg0;"

#ifdef __cplusplus
extern "C"
#endif
/**
 * End SSL negotiation for a client connection.
 *
 * @param[in] conn Client connection handle.
 * @param[in] sslEndInp SSL end request input.
 *
 * @return Operation status.
 */
int rcSslEnd( rcComm_t *conn, sslEndInp_t *sslEndInp );

#endif
