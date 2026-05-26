#ifndef SUB_STRUCT_FILE_RENAME_H__
#define SUB_STRUCT_FILE_RENAME_H__

#include "irods/objInfo.h"
#include "irods/rcConnect.h"

/// \brief Input for renaming a sub-file inside a structured file.
typedef struct SubStructFileRenameInp {
    subFile_t subFile;                  ///< Existing sub-file to rename.
    char newSubFilePath[MAX_NAME_LEN];  ///< New logical path for the sub-file.
    char resc_hier[ MAX_NAME_LEN ];     ///< Resource hierarchy used to resolve the request.
} subStructFileRenameInp_t;

/// \brief Packing instruction for \ref subStructFileRenameInp_t.
#define SubStructFileRenameInp_PI "struct SubFile_PI; str newSubFilePath[MAX_NAME_LEN]; str resc_hier[MAX_NAME_LEN];"

/// \brief Renames a sub-file in a structured file.
/// \param[in] conn Client connection handle.
/// \param[in] subStructFileRenameInp Rename operation input.
/// \return Integer error code.
int rcSubStructFileRename( rcComm_t *conn, subStructFileRenameInp_t *subStructFileRenameInp );

#endif
