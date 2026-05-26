#ifndef SUB_STRUCT_FILE_READ_H__
#define SUB_STRUCT_FILE_READ_H__

#include "irods/rcConnect.h"
#include "irods/rodsDef.h"
#include "irods/objInfo.h"

/// \brief Input for reading from an open sub-file inside a structured file.
typedef struct SubStructFileFdOpr {
    rodsHostAddr_t addr;            ///< Target host address for the operation.
    structFileType_t type;          ///< Structured file type.
    int fd;                         ///< Open descriptor for the sub-file.
    int len;                        ///< Number of bytes to read.
    char resc_hier[ MAX_NAME_LEN ]; ///< Resource hierarchy used to resolve the request.
} subStructFileFdOprInp_t;
/// \brief Packing instruction for \ref subStructFileFdOprInp_t.
#define SubStructFileFdOpr_PI "struct RHostAddr_PI; int type; int fd; int len;"

/// \brief Reads bytes from a sub-file in a structured file.
/// \param[in] conn Client connection handle.
/// \param[in] subStructFileReadInp Read operation input.
/// \param[out] subStructFileReadOutBBuf Buffer receiving the bytes read.
/// \return Integer error code.
int rcSubStructFileRead( rcComm_t *conn, subStructFileFdOprInp_t *subStructFileReadInp, bytesBuf_t *subStructFileReadOutBBuf );

#endif
