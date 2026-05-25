#ifndef GET_TEMP_PASSWORD_H__
#define GET_TEMP_PASSWORD_H__

#include "irods/rcConnect.h"
#include "irods/authenticate.h"

/** Output containing the seed string used to derive a temporary password. */
typedef struct {
    char stringToHashWith[MAX_PASSWORD_LEN]; ///< String the caller hashes to derive the password.
} getTempPasswordOut_t;
#define getTempPasswordOut_PI "str stringToHashWith[MAX_PASSWORD_LEN];"

/**
 * Request a temporary password for the connected user.
 *
 * @param[in] conn Client connection handle.
 * @param[out] getTempPasswordOut Output containing the password seed string.
 *
 * @return Operation status.
 */
int rcGetTempPassword( rcComm_t *conn, getTempPasswordOut_t **getTempPasswordOut );

#endif
