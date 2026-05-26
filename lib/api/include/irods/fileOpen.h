#ifndef FILE_OPEN_H__
#define FILE_OPEN_H__

/// \file
/// \brief Declares the server-side file open API.

#include "irods/rodsType.h"
#include "irods/rodsDef.h"
#include "irods/objInfo.h"
#include "irods/rcConnect.h"

/// Skips permission checks for the open operation.
#define NO_CHK_PERM_FLAG        0x1
/// Reuses a unique remote connection for the open operation.
#define UNIQUE_REM_COMM_FLAG    0x2
/// Forces the open operation to proceed when supported.
#define FORCE_FLAG              0x4

/// Input describing a physical file open request.
typedef struct {
    char resc_name_[MAX_NAME_LEN];     ///< Resource name for the open request.
    char resc_hier_[MAX_NAME_LEN];     ///< Resource hierarchy for the open request.
    char objPath[MAX_NAME_LEN];        ///< Logical path of the data object.
    int otherFlags;                    ///< Additional control flags.
    rodsHostAddr_t addr;               ///< Host address of the resource server.
    char fileName[MAX_NAME_LEN];       ///< Physical file path.
    int flags;                         ///< Open flags.
    int mode;                          ///< Requested file mode.
    rodsLong_t dataSize;               ///< Expected data size.
    keyValPair_t condInput;            ///< Additional keyword options.
    char in_pdmo[MAX_NAME_LEN];        ///< Indicates whether the request originated from a PDMO.
} fileOpenInp_t;
/// Packing instruction for `fileOpenInp_t`.
#define fileOpenInp_PI "str resc_name_[MAX_NAME_LEN]; str resc_hier_[MAX_NAME_LEN]; str objPath[MAX_NAME_LEN]; int otherFlags; struct RHostAddr_PI; str fileName[MAX_NAME_LEN]; int flags; int mode; double dataSize; struct KeyValPair_PI; str in_pdmo[MAX_NAME_LEN];"

/// Opens a physical file on behalf of the connected client.
#ifdef __cplusplus
extern "C"
#endif
int rcFileOpen( rcComm_t *conn, fileOpenInp_t *fileOpenInp );

#endif
