#ifndef MOD_ACCESS_CONTROL_H__
#define MOD_ACCESS_CONTROL_H__

#include "irods/rcConnect.h"

#define MOD_RESC_PREFIX "resource:"  // Used to indicate a resource instead of requiring a change to the protocol
#define MOD_ADMIN_MODE_PREFIX "admin:" // To indicate admin mode, without protocol change.

typedef struct {
    int recursiveFlag; ///< Non-zero to apply access changes recursively.
    char *accessLevel; ///< Access level to apply.
    char *userName; ///< User receiving the access change.
    char *zone; ///< Zone of the target user.
    char *path; ///< Target object, collection, or resource path.
} modAccessControlInp_t;
#define modAccessControlInp_PI "int recursiveFlag; str *accessLevel; str *userName; str *zone; str *path;"

#ifdef __cplusplus
extern "C"
#endif
/**
 * Modify access control for a catalog target.
 *
 * @param[in] conn Client connection handle.
 * @param[in] modAccessControlInp Access-control modification request.
 *
 * @return Operation status.
 */
int rcModAccessControl( rcComm_t *conn, modAccessControlInp_t *modAccessControlInp );

#endif
