#ifndef AUTH_CHECK_H__
#define AUTH_CHECK_H__

#include "irods/rcConnect.h"

/** Input for checking a challenge-response authentication exchange. */
typedef struct {
    char *challenge; ///< Challenge string presented by the server.
    char *response; ///< Response string provided by the client.
    char *username; ///< User name being authenticated.
} authCheckInp_t;

/** Output describing the results of an authentication check. */
typedef struct {
    int  privLevel; ///< Privilege level assigned to the authenticated user.
    int  clientPrivLevel; ///< Privilege level associated with the client session.
    char *serverResponse; ///< Optional response returned by the server.
} authCheckOut_t;

#define authCheckInp_PI "str *challenge; str *response; str *username;"
#define authCheckOut_PI "int privLevel; int clientPrivLevel; str *serverResponse;"


#ifdef __cplusplus
extern "C"
#endif
/**
 * Validate a challenge-response authentication exchange.
 *
 * @param[in] conn Client connection handle.
 * @param[in] authCheckInp Authentication check input.
 * @param[out] authCheckOut Output describing the authentication result.
 *
 * @return Operation status.
 */
int rcAuthCheck( rcComm_t *conn, authCheckInp_t *authCheckInp, authCheckOut_t **authCheckOut );

#endif
