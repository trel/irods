#ifndef SUB_STRUCT_FILE_LSEEK_H__
#define SUB_STRUCT_FILE_LSEEK_H__

#include "irods/rcConnect.h"
#include "irods/objInfo.h"
#include "irods/rodsDef.h"
#include "irods/rodsType.h"
#include "irods/fileLseek.h"

/// \brief Input for seeking within an open sub-file inside a structured file.
typedef struct SubStructFileLseekInp {
    rodsHostAddr_t addr;            ///< Target host address for the operation.
    structFileType_t type;          ///< Structured file type.
    int fd;                         ///< Open descriptor for the sub-file.
    rodsLong_t offset;              ///< Byte offset used by the seek operation.
    int whence;                     ///< Seek origin such as SEEK_SET, SEEK_CUR, or SEEK_END.
    char resc_hier[ MAX_NAME_LEN ]; ///< Resource hierarchy used to resolve the request.
} subStructFileLseekInp_t;
/// \brief Packing instruction for \ref subStructFileLseekInp_t.
#define SubStructFileLseekInp_PI "struct RHostAddr_PI; int type; int fd; double offset; int whence;"

/// \brief Seeks within a sub-file in a structured file.
/// \param[in] conn Client connection handle.
/// \param[in] subStructFileLseekInp Seek operation input.
/// \param[out] subStructFileLseekOut Result containing the updated offset.
/// \return Integer error code.
int rcSubStructFileLseek( rcComm_t *conn, subStructFileLseekInp_t *subStructFileLseekInp, fileLseekOut_t **subStructFileLseekOut );

#endif
