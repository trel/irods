#ifndef FILE_LSEEK_H__
#define FILE_LSEEK_H__

#include "irods/rodsType.h"
#include "irods/rcConnect.h"
#include "irods/procApiRequest.h"
#include "irods/apiNumber.h"

/**
 * \var fileLseekInp_t
 * \brief Input struct for rcDataObjLseek and rsFileLseek operations
 * \since 1.0
 *
 * \ingroup capi_input_data_structures
 *
 * \remark none
 *
 * \note
 * Elements of fileLseekInp_t:
 * \li int fileInx - the opened file descriptor from rcFileOpen or rcFileCreate.
 * \li rodsLong_t offset - the roffset
 * \li int whence - Similar to lseek of UNIX. Valid values are:
 *        \n SEEK_SET - The offset is set to offset bytes.
 *        \n SEEK_CUR - The offset is set to its current location plus
 *             offset bytes.
 *        \n SEEK_END - The offset is set to the size of the file plus
 *             offset bytes.
 *
 * \sa none
 */
/**
 * \brief Input for file seek operations.
 */
typedef struct FileLseekInp {
    int fileInx; ///< Open file descriptor returned by a file open call.
    rodsLong_t offset; ///< Offset applied during the seek.
    int whence; ///< Origin used to interpret the offset.
} fileLseekInp_t;

/**
 * \var fileLseekOut_t
 * \brief Output struct for rcDataObjLseek and rsFileLseek operations
 * \since 1.0
 *
 * \remark none
 *
 * \note
 * Elements of fileLseekOut_t:
 * \li offset - the resulting offset location in bytes from the beginning
 *        of the file.
 *
 * \sa none
 */
/**
 * \brief Output from file seek operations.
 */
typedef struct FileLseekOut {
    rodsLong_t offset; ///< Resulting offset measured from the beginning of the file.
} fileLseekOut_t;

#define fileLseekInp_PI "int fileInx; double offset; int whence;"
#define fileLseekOut_PI "double offset;"

#ifdef __cplusplus
extern "C"
#endif
/**
 * Reposition an open file descriptor.
 *
 * @param[in] conn Client connection handle.
 * @param[in] fileLseekInp Seek request input.
 * @param[out] fileLseekOut Output containing the resulting file offset.
 *
 * @return Operation status.
 */
int rcFileLseek( rcComm_t *conn, fileLseekInp_t *fileLseekInp, fileLseekOut_t **fileLseekOut );

#endif
