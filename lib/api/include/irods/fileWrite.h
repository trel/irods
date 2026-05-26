#ifndef FILE_WRITE_H__
#define FILE_WRITE_H__

#include "irods/rcConnect.h"
#include "irods/rodsDef.h"

/** Input for writing bytes to an open filesystem descriptor. */
typedef struct {
    int fileInx; ///< Open file descriptor returned by a file open call.
    int len; ///< Number of bytes to write from the accompanying buffer.
} fileWriteInp_t;

#define fileWriteInp_PI "int fileInx; int len;"

#ifdef __cplusplus
extern "C"
#endif
/**
 * Write bytes to an open filesystem descriptor.
 *
 * @param[in] conn Client connection handle.
 * @param[in] fileWriteInp Write request input.
 * @param[in] fileWriteInpBBuf Buffer containing the bytes to write.
 *
 * @return Operation status.
 */
int rcFileWrite( rcComm_t *conn, const fileWriteInp_t *fileWriteInp, const bytesBuf_t *fileWriteInpBBuf );

#endif
