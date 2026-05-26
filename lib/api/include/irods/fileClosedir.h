#ifndef FILE_CLOSEDIR_H__
#define FILE_CLOSEDIR_H__

#include "irods/rcConnect.h"

/**
 * \brief Input passed to `rcFileClosedir`.
 */
typedef struct {
    int fileInx; ///< File descriptor index for the directory stream to close.
} fileClosedirInp_t;
#define fileClosedirInp_PI "int fileInx;"

#ifdef __cplusplus
extern "C"
#endif
int rcFileClosedir( rcComm_t *conn, fileClosedirInp_t *fileClosedirInp );

#endif
