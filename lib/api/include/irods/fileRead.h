#ifndef FILE_READ_H__
#define FILE_READ_H__

#include "irods/rodsDef.h"
#include "irods/rcConnect.h"

/** Input for reading bytes from an open filesystem descriptor. */
typedef struct FileReadInp {
    int fileInx; ///< Open file descriptor returned by a file open call.
    int len; ///< Number of bytes requested from the file.
} fileReadInp_t;
#define fileReadInp_PI "int fileInx; int len;"


#ifdef __cplusplus
extern "C"
#endif
/**
 * Read bytes from an open filesystem descriptor.
 *
 * @param[in] conn Client connection handle.
 * @param[in] fileReadInp Read request input.
 * @param[out] fileReadOutBBuf Output buffer receiving the read bytes.
 *
 * @return Operation status.
 */
int rcFileRead( rcComm_t *conn, fileReadInp_t *fileReadInp, bytesBuf_t *fileReadOutBBuf );

#endif
