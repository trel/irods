#ifndef USER_ADMIN_H__
#define USER_ADMIN_H__

/// \file
/// \brief Declares the user administration API.

#include "irods/rcConnect.h"

/// Positional arguments describing a user administration operation.
typedef struct {
    char *arg0; ///< First positional argument.
    char *arg1; ///< Second positional argument.
    char *arg2; ///< Third positional argument.
    char *arg3; ///< Fourth positional argument.
    char *arg4; ///< Fifth positional argument.
    char *arg5; ///< Sixth positional argument.
    char *arg6; ///< Seventh positional argument.
    char *arg7; ///< Eighth positional argument.
    char *arg8; ///< Ninth positional argument.
    char *arg9; ///< Tenth positional argument.
} userAdminInp_t;
/// Packing instruction for `userAdminInp_t`.
#define userAdminInp_PI "str *arg0; str *arg1; str *arg2; str *arg3; str *arg4; str *arg5; str *arg6; str *arg7;  str *arg8;  str *arg9;"

/// Executes a user administration operation using the supplied positional arguments.
#ifdef __cplusplus
extern "C"
#endif
int rcUserAdmin( rcComm_t *conn, userAdminInp_t *userAdminInp );

#endif
