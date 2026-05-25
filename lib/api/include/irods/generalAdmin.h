#ifndef IRODS_GENERAL_ADMIN_H
#define IRODS_GENERAL_ADMIN_H

/// \file
/// \brief Declares the general administrative API.

struct RcComm;

/// Positional arguments describing an administrative operation.
// NOLINTNEXTLINE(modernize-use-using)
typedef struct GeneralAdminInput
{
    const char *arg0; ///< First positional argument.
    const char *arg1; ///< Second positional argument.
    const char *arg2; ///< Third positional argument.
    const char *arg3; ///< Fourth positional argument.
    const char *arg4; ///< Fifth positional argument.
    const char *arg5; ///< Sixth positional argument.
    const char *arg6; ///< Seventh positional argument.
    const char *arg7; ///< Eighth positional argument.
    const char *arg8; ///< Ninth positional argument.
    const char *arg9; ///< Tenth positional argument.
} generalAdminInp_t;

/// Packing instruction for `generalAdminInp_t`.
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define generalAdminInp_PI "str *arg0; str *arg1; str *arg2; str *arg3; str *arg4; str *arg5; str *arg6; str *arg7;  str *arg8;  str *arg9;"

#ifdef __cplusplus
extern "C" {
#endif

// NOLINTBEGIN(modernize-use-trailing-return-type)

/// Execute an administrative operation.
///
/// See iadmin.cpp and C++ administration APIs for usage examples.
///
/// \param[in] conn            The connection handle used for communication.
/// \param[in] generalAdminInp The input data structure.
///
/// \return An integer indicating the status of the operation.
/// \retval 0       On success
/// \retval nonzero On failure
///
/// \deprecated Token addition and removal are deprecated as of 5.1.0.
/// \deprecated Authentication name addition and removal are deprecated as of 5.1.0.
int rcGeneralAdmin(struct RcComm* conn, struct GeneralAdminInput* generalAdminInp);

// NOLINTEND(modernize-use-trailing-return-type)

#ifdef __cplusplus
} // extern "C"
#endif

#endif // IRODS_GENERAL_ADMIN_H
