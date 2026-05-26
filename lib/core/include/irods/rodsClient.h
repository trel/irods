#ifndef RODS_CLIENT_H__
#define RODS_CLIENT_H__

#include "irods/rods.h"
#include "irods/apiHeaderAll.h"

#ifdef __cplusplus
extern "C" {
#endif

/// \brief Transfer progress information for client-side data movement utilities.
typedef struct TransferProgress {
    rodsLong_t bytesReceived;              ///< Number of bytes transferred so far.
    rodsLong_t bytesExpected;              ///< Total number of bytes expected to transfer.
    char currentFilePath[MAX_NAME_LEN];    ///< Path of the file currently being transferred.
} xferProgress_t;

/// \brief Initializes the legacy client API table.
/// \deprecated Use \ref load_client_api_plugins instead.
void init_client_api_table(void) __attribute__((deprecated("Use load_client_api_plugins instead")));

/// \brief Loads client API plugins.
void load_client_api_plugins();

#ifdef __cplusplus
} // extern "C"
#endif

#endif  /* RODS_CLIENT_H__ */
