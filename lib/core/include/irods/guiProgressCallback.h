#ifndef _GUI_PROGRESS_CALLBACK_H__
#define _GUI_PROGRESS_CALLBACK_H__

/// \file
/// \brief Declares progress reporting types for client operations.

#include "irods/rodsDef.h" // For MAX_NAME_LEN
#include "irods/rodsType.h" // For rodsLong_t

#ifdef __cplusplus
extern "C" {
#endif
/// \brief Reports progress for a client-side data operation.
typedef struct OperProgress {
    int oprType;                         ///< Operation type.
    int flag;                            ///< Operation-specific flags.
    rodsLong_t totalNumFiles;            ///< Total number of files in the operation.
    rodsLong_t totalFileSize;            ///< Total size of all files in the operation.
    rodsLong_t totalNumFilesDone;        ///< Number of files completed so far.
    rodsLong_t totalFileSizeDone;        ///< Number of bytes completed across all files.
    char curFileName[MAX_NAME_LEN];      ///< Name of the file currently being processed.
    rodsLong_t curFileSize;              ///< Size of the current file.
    rodsLong_t curFileSizeDone;          ///< Number of bytes completed for the current file.
} operProgress_t;

/// \brief Signature for GUI progress callbacks.
typedef void ( *guiProgressCallback )( operProgress_t *operProgress );

#ifdef __cplusplus
}
#endif
#endif
