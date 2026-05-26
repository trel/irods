#ifndef PAM_AUTH_REQUEST_H__
#define PAM_AUTH_REQUEST_H__

#include "irods/rcConnect.h"

/** Input for requesting a temporary PAM-derived iRODS password. */
typedef struct {
    char *pamUser; ///< PAM user name to authenticate.
    char *pamPassword; ///< PAM password for the user.
    int timeToLive; ///< Requested lifetime of the generated iRODS password.
} pamAuthRequestInp_t;

#define pamAuthRequestInp_PI "str *pamUser; str *pamPassword; int timeToLive;"

/** Output containing a generated iRODS PAM password. */
typedef struct {
    char *irodsPamPassword; ///< Generated password to use for iRODS authentication.
} pamAuthRequestOut_t;

#define pamAuthRequestOut_PI "str *irodsPamPassword;"

#ifdef __cplusplus
extern "C"
#endif
/**
 * Request a temporary iRODS password via PAM authentication.
 *
 * @param[in] conn Client connection handle.
 * @param[in] pamAuthRequestInp PAM authentication request.
 * @param[out] pamAuthRequestOut Output containing the generated iRODS password.
 *
 * @return Operation status.
 */
int rcPamAuthRequest( rcComm_t *conn, pamAuthRequestInp_t *pamAuthRequestInp, pamAuthRequestOut_t **pamAuthRequestOut );

#endif
