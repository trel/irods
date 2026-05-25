#ifndef GET_TEMP_PASSWORD_FOR_OTHER_H__
#define GET_TEMP_PASSWORD_FOR_OTHER_H__

#include "irods/rcConnect.h"
#include "irods/authenticate.h"

/** Input for requesting a temporary password for another user. */
typedef struct {
    char *otherUser; ///< Target user for whom a temporary password is requested.
    char *unused;  ///< Reserved protocol field for future use.
} getTempPasswordForOtherInp_t;
#define getTempPasswordForOtherInp_PI "str *targetUser; str *unused;"

/** Output containing the string used to derive the temporary password. */
typedef struct {
    char stringToHashWith[MAX_PASSWORD_LEN]; ///< String that the caller must hash to derive the password.
} getTempPasswordForOtherOut_t;
#define getTempPasswordForOtherOut_PI "str stringToHashWith[MAX_PASSWORD_LEN];"


/**
 * Request a temporary password for another user.
 *
 * @param[in] conn Client connection handle.
 * @param[in] getTempPasswordForOtherInp Temporary-password request input.
 * @param[out] getTempPasswordForOtherOut Output containing the password seed string.
 *
 * @return Operation status.
 */
int rcGetTempPasswordForOther( rcComm_t *conn, getTempPasswordForOtherInp_t *getTempPasswordForOtherInp, getTempPasswordForOtherOut_t **getTempPasswordForOtherOut );

#endif
