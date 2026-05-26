#ifndef GET_LIMITED_PASSWORD_H__
#define GET_LIMITED_PASSWORD_H__

/// \file

#include "irods/rcConnect.h"
#include "irods/authenticate.h"

/// \brief Input for requesting a limited-lifetime temporary password.
typedef struct {
    int  ttl; ///< Requested password lifetime.
    char *unused1;  ///< Reserved protocol field for future use.
} getLimitedPasswordInp_t;
/// Packing instruction for `getLimitedPasswordInp_t`.
#define getLimitedPasswordInp_PI "int ttl; str *unused1;"

/// \brief Output containing the seed string used to derive the limited password.
typedef struct {
    char stringToHashWith[MAX_PASSWORD_LEN]; ///< String the caller hashes to derive the password.
} getLimitedPasswordOut_t;
/// Packing instruction for `getLimitedPasswordOut_t`.
#define getLimitedPasswordOut_PI "str stringToHashWith[MAX_PASSWORD_LEN];"


/**
 * Request a limited-lifetime temporary password.
 *
 * @param[in] conn Client connection handle.
 * @param[in] getLimitedPasswordInp Limited-password request input.
 * @param[out] getLimitedPasswordOut Output containing the password seed string.
 *
 * @return Operation status.
 */
int rcGetLimitedPassword( rcComm_t *conn, getLimitedPasswordInp_t *getLimitedPasswordInp, getLimitedPasswordOut_t **getLimitedPasswordOut );

#endif
