/*** Copyright (c), The Regents of the University of California            ***
 *** For more information please refer to files in the COPYRIGHT directory ***/

/* fileOpr.h - header file for fileOpr.c
 */



#ifndef FILE_OPR_HPP
#define FILE_OPR_HPP

#include "irods/rodsConnect.h"
#include "irods/fileDriver.hpp"
#include "irods/chkNVPathPerm.h"

/// Number of entries in the global file descriptor table.
#define NUM_FILE_DESC   1026    /* number of FileDesc */

/* definition for inuseFlag */

/// Indicates that a file descriptor entry is free.
#define FD_FREE         0

/// Indicates that a file descriptor entry is in use.
#define FD_INUSE        1

/// Placeholder file name used for stream descriptors.
#define STREAM_FILE_NAME        "stream"   /* a fake file name for stream */

/// Describes an entry in the server's file descriptor table.
typedef struct {
    /// Indicates whether the file descriptor entry is in use.
    int inuseFlag;      /* whether the fileDesc is in use, 0=no */
    /// Points to the server host managing the file.
    rodsServerHost_t *rodsServerHost;
    /// Stores the physical path to the file.
    char *fileName;
    /// Stores the resource hierarchy for the file.
    char *rescHier;             // The hierarchy of resources in which this file resides
    /// Stores the logical path to the object.
    char *objPath;              // The logical path
    /// Stores the open mode for the file.
    int mode;
    /// Controls whether vault path permissions are checked.
    int chkPerm;        /* check for permission in the file vault */
    /// Stores the driver-level file descriptor.
    int fd;             /* the file descriptor from driver */
    /// Indicates whether data has been written through this descriptor.
    int writtenFlag;    /* indicated whether the file has been written to */
    /// Stores driver-specific state associated with the descriptor.
    void *driverDep;    /* driver dependent stuff */
} fileDesc_t;

/// Initializes the global file descriptor table.
int
initFileDesc();

/// Allocates a free slot in the global file descriptor table.
int
allocFileDesc();

/// Allocates and initializes a file descriptor table entry.
/// 
/// \param[in] rodsServerHost The server host managing the file.
/// \param[in] objPath        The logical path to the data object.
/// \param[in] fileName       The physical path to the file.
/// \param[in] rescHier       The resource hierarchy for the file.
/// \param[in] fd             The driver-level file descriptor.
/// \param[in] mode           The open mode.
int
allocAndFillFileDesc( rodsServerHost_t *rodsServerHost, const std::string& objPath,
                      const std::string& fileName, const std::string& rescHier, int fd, int mode );

/// Releases a file descriptor table entry.
///
/// \param[in] fileInx The index of the file descriptor entry to free.
int
freeFileDesc( int fileInx );

/// Returns the server host associated with a file descriptor entry.
///
/// \param[in]  fileInx        The index of the file descriptor entry.
/// \param[out] rodsServerHost Receives the server host pointer.
int
getServerHostByFileInx( int fileInx, rodsServerHost_t **rodsServerHost );

/// Creates the parent directory for a file path if needed.
///
/// \param[in] rsComm      The server communication context.
/// \param[in] startDirLen The prefix length that must already exist.
/// \param[in] filePath    The file path whose parent directory may be created.
/// \param[in] hier        The resource hierarchy to use.
/// \param[in] mode        The mode applied to created directories.
int
mkDirForFilePath( rsComm_t *rsComm, size_t startDirLen, const std::string& filePath, const std::string& hier, int mode );

/// Recursively creates a directory path.
///
/// \param[in] rsComm      The server communication context.
/// \param[in] startDirLen The prefix length that must already exist.
/// \param[in] destDir     The directory path to create.
/// \param[in] hier        The resource hierarchy to use.
/// \param[in] mode        The mode applied to created directories.
int
mkFileDirR( rsComm_t *rsComm, size_t startDirLen, const std::string& destDir, const std::string& hier, int mode );

/// Checks whether the requested file path satisfies vault path permissions.
///
/// \param[in] rsComm         The server communication context.
/// \param[in] fileOpenInp    The file open input structure.
/// \param[in] rodsServerHost The server host managing the request.
/// \param[in] chkType        The permission check type.
int
chkFilePathPerm( rsComm_t *rsComm, fileOpenInp_t *fileOpenInp,
                 rodsServerHost_t *rodsServerHost, int chkType ); // JMC - backport 4774

/// Returns whether a physical path is allowed for server-side use.
///
/// \param[in] path The path to validate.
int // JMC - backport 4766
isValidFilePath( const std::string& path );

/// Checks whether a client path matches a vault path.
int
matchCliVaultPath( rsComm_t*,
                   const std::string&,
                   rodsServerHost_t* );

/// Checks whether a cache directory is empty.
///
/// \param[in] rsComm   The server communication context.
/// \param[in] cacheDir The cache directory to inspect.
/// \param[in] hier     The resource hierarchy to use.
int
chkEmptyDir( rsComm_t *rsComm, const std::string& cacheDir, const std::string& hier );

/// Determines the type of file path for a resource.
///
/// \param[in] rsComm   The server communication context.
/// \param[in] objPath  The logical path to the data object.
/// \param[in] fileName The physical path to the file.
/// \param[in] rescHier The resource hierarchy to use.
int
filePathTypeInResc( rsComm_t *rsComm, const std::string& objPath, const std::string& fileName, const std::string& rescHier );

/// Associates a stream file descriptor with iRODS state.
///
/// \param[in] rodsServerHost The server host managing the descriptor.
/// \param[in] fd             The file descriptor to bind.
int
bindStreamToIRods( rodsServerHost_t *rodsServerHost, int fd );
#endif  /* FILE_OPR_H */
