#ifndef END_TRANSACTION_H__
#define END_TRANSACTION_H__

#include "irods/rcConnect.h"

/** Input for ending a transaction. */
typedef struct {
    char *arg0; ///< Primary transaction control argument.
    char *arg1; ///< Secondary transaction control argument.
} endTransactionInp_t;
#define endTransactionInp_PI "str *arg0; str *arg1;"

#ifdef __cplusplus
extern "C"
#endif
/**
 * End a transaction on the connected server.
 *
 * @param[in] conn Client connection handle.
 * @param[in] endTransactionInp Transaction completion request.
 *
 * @return Operation status.
 */
int rcEndTransaction( rcComm_t *conn, endTransactionInp_t *endTransactionInp );

#endif
