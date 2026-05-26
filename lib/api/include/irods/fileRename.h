#ifndef IRODS_FILE_RENAME_H
#define IRODS_FILE_RENAME_H

/// \file
/// \brief Declares the server-side file rename API.

#include "irods/rodsDef.h"

struct RcComm;

/// Input describing a physical file rename operation.
typedef struct FileRenameInp {
    rodsHostAddr_t addr;              ///< Host address of the resource server.
    char oldFileName[MAX_NAME_LEN];   ///< Existing physical file path.
    char newFileName[MAX_NAME_LEN];   ///< Replacement physical file path.
    char rescHier[MAX_NAME_LEN];      ///< Resource hierarchy for the file.
    char objPath[MAX_NAME_LEN];       ///< Logical path of the associated data object.
} fileRenameInp_t;
/// Packing instruction for `fileRenameInp_t`.
#define fileRenameInp_PI "struct RHostAddr_PI; str oldFileName[MAX_NAME_LEN]; str newFileName[MAX_NAME_LEN]; str rescHier[MAX_NAME_LEN]; str objPath[MAX_NAME_LEN];"

/// Output returned by a physical file rename operation.
typedef struct FileRenameOut {
    char file_name[MAX_NAME_LEN]; ///< Renamed file path.
} fileRenameOut_t;
/// Packing instruction for `fileRenameOut_t`.
#define fileRenameOut_PI "str file_name[MAX_NAME_LEN];"

#ifdef __cplusplus
extern "C" {
#endif

/// Renames a file.
/// 
/// \ingroup server_filedriver
/// 
/// \param[in]  conn          A RcComm connection handle to the server.
/// \param[in]  fileRenameInp The input.
/// \param[out] fileRenameOut The output.
/// 
/// \return An integer.
/// \retval 0        On success.
/// \retval Non-zero On failure.
///
/// \since 3.3.0
int rcFileRename(struct RcComm* conn,
                 struct FileRenameInp* fileRenameInp,
                 struct FileRenameOut** fileRenameOut);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // IRODS_FILE_RENAME_H
