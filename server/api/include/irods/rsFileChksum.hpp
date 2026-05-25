#ifndef RS_FILE_CHKSUM_HPP
#define RS_FILE_CHKSUM_HPP

/// \file

#include "irods/fileChksum.h"

struct RsComm;
struct rodsServerHost;

/// Computes or retrieves a checksum for a file operation request.
int rsFileChksum(RsComm *rsComm, fileChksumInp_t *fileChksumInp, char **chksumStr);

/// Internal implementation of `rsFileChksum()`.
int _rsFileChksum(RsComm *rsComm, fileChksumInp_t *fileChksumInp, char **chksumStr);

/// Executes a remote file checksum operation.
int remoteFileChksum(RsComm *rsComm,
                     fileChksumInp_t *fileChksumInp,
                     char **chksumStr,
                     rodsServerHost *rodsServerHost);

/// Deprecated file checksum helper.
[[deprecated("Use file_checksum.")]]
int fileChksum(RsComm *rsComm,
               char* objPath,
               char *fileName,
               char* rescHier,
               char* orig_chksum,
               char *chksumStr);

/// Computes a checksum for a data object replica at the resource layer.
int file_checksum(RsComm* _comm,
                   const char* _logical_path,
                   const char* _filename,
                  const char* _resource_hierarchy,
                  const char* _original_checksum,
                  rodsLong_t _data_size,
                  char* _calculated_checksum);

#endif // RS_FILE_CHKSUM_HPP
