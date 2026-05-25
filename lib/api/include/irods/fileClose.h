#ifndef FILE_CLOSE_H__
#define FILE_CLOSE_H__

#include "irods/rcConnect.h"

/** Input for closing an open filesystem descriptor. */
typedef struct FileCloseInp {
    int fileInx; ///< Open file descriptor to close.
    char in_pdmo[MAX_NAME_LEN]; ///< PDMO marker used during close processing.
} fileCloseInp_t;

#define fileCloseInp_PI "int fileInx; str in_pdmo[MAX_NAME_LEN];"

#ifdef __cplusplus
extern "C"
#endif
/**
 * Close an open filesystem descriptor.
 *
 * @param[in] conn Client connection handle.
 * @param[in] fileCloseInp Close request input.
 *
 * @return Operation status.
 */
int rcFileClose( rcComm_t *conn, fileCloseInp_t *fileCloseInp );

#endif
