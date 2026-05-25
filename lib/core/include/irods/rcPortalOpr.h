/*** Copyright (c), The Regents of the University of California            ***
 *** For more information please refer to files in the COPYRIGHT directory ***/

/* rcPortalOpr.h - header file for rcPortalOpr.c
 */

/// \file
/// \brief Declares client-side portal transfer helpers.



#ifndef RC_PORTAL_OPR_H__
#define RC_PORTAL_OPR_H__

#include "irods/rods.h"
#include "irods/objInfo.h"
#include "irods/dataObjInpOut.h"

/// \brief Maximum number of progress updates tracked for a transfer.
#define MAX_PROGRESS_CNT	8

/// \brief Contains inputs for a portal transfer worker.
typedef struct RcPortalTransferInp {
    rcComm_t *conn;                       ///< Client connection.
    int destFd;                           ///< Destination file descriptor.
    int srcFd;                            ///< Source file descriptor.
    int threadNum;                        ///< Number of threads participating in the transfer.
    int status;                           ///< Worker status.
    rodsLong_t	bytesWritten;              ///< Number of bytes transferred.
    unsigned char shared_secret[ NAME_LEN ]; ///< Shared secret for the transfer.
} rcPortalTransferInp_t;

#ifdef __cplusplus
extern "C" {
#endif

/// \brief Populates a portal transfer input structure.
int
fillRcPortalTransferInp( rcComm_t *conn, rcPortalTransferInp_t *myInput,
                         int destFd, int srcFd, int threadNum );
/// \brief Uploads a local file using a portal transfer.
int
putFileToPortal( rcComm_t *conn, portalOprOut_t *portalOprOut,
                 char *locFilePath, char *objPath, rodsLong_t dataSize );
/// \brief Downloads a file using a portal transfer.
int
getFileFromPortal( rcComm_t *conn, portalOprOut_t *portalOprOut,
                   char *locFilePath, char *objPath, rodsLong_t dataSize );
/// \brief Writes a portal transfer segment to the destination.
void
rcPartialDataPut( rcPortalTransferInp_t *myInput );
/// \brief Reads a portal transfer segment from the source.
void
rcPartialDataGet( rcPortalTransferInp_t *myInput );
/// \brief Receives a transfer header from a socket.
int
rcvTranHeader( int sock, transferHeader_t *myHeader );

/// \brief Sends a transfer header over a socket.
int
sendTranHeader( int sock, int oprType, int flags, rodsLong_t offset,
                 rodsLong_t length );
/// \brief Loads a local file into a bytes buffer.
int
fillBBufWithFile( rcComm_t *conn, bytesBuf_t *myBBuf, char *locFilePath,
                   rodsLong_t dataSize );
/// \brief Uploads a local file to an opened data object.
int
putFile( rcComm_t *conn, int l1descInx, char *locFilePath, char *objPath,
          rodsLong_t dataSize );
/// \brief Fetches include file contents into a buffer.
int
getIncludeFile( rcComm_t *conn, bytesBuf_t *dataObjOutBBuf, char *locFilePath );
/// \brief Downloads an opened data object to a local file.
int
getFile( rcComm_t *conn, int l1descInx, char *locFilePath, char *objPath,
          rodsLong_t dataSize );
/// \brief Initializes restart tracking for a file transfer.
int initFileRestart(rcComm_t* conn, char* fileName, char* objPath, rodsLong_t fileSize, int numThr);
/// \brief Writes restart information to disk.
int
writeLfRestartFile( char *infoFile, fileRestartInfo_t *info );
/// \brief Reads restart information from disk.
int
readLfRestartFile( char *infoFile, fileRestartInfo_t **info );
/// \brief Clears restart state for a file transfer.
int
clearLfRestartFile( fileRestart_t *fileRestart );
/// \brief Resumes an upload using saved restart information.
int
lfRestartPutWithInfo( rcComm_t *conn, fileRestartInfo_t *info );
/// \brief Resumes a download using saved restart information.
int
lfRestartGetWithInfo( rcComm_t *conn, fileRestartInfo_t *info );
/// \brief Uploads a transfer segment.
int
putSeg( rcComm_t *conn, rodsLong_t segSize, int localFd,
         openedDataObjInp_t *dataObjWriteInp, bytesBuf_t *dataObjWriteInpBBuf,
         int bufLen, int *writtenSinceUpdated, fileRestartInfo_t *info,
         rodsLong_t *dataSegLen );
/// \brief Downloads a transfer segment.
int
getSeg( rcComm_t *conn, rodsLong_t segSize, int localFd,
         openedDataObjInp_t *dataObjReadInp, bytesBuf_t *dataObjReadInpBBuf,
         int bufLen, int *writtenSinceUpdated, fileRestartInfo_t *info,
         rodsLong_t *dataSegLen );
/// \brief Writes a data object to standard output.
int
catDataObj( rcComm_t *conn, char *objPath );
#ifdef __cplusplus
}
#endif
#endif	// RC_PORTAL_OPR_H__
