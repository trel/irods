#ifndef IRODS_RC_CONNECT_H
#define IRODS_RC_CONNECT_H

/// \file

#include "irods/rodsDef.h"
#include "irods/rodsError.h"
#include "irods/rodsLog.h"
#include "irods/stringOpr.h"
#include "irods/rodsType.h"
#include "irods/rodsUser.h"
#include "irods/getRodsEnv.h"
#include "irods/objInfo.h"
#include "irods/dataObjInpOut.h"
#include "irods/guiProgressCallback.h"

#include <openssl/ssl.h>
#include <openssl/x509v3.h>
#include <openssl/err.h>

/// Indicates that reconnection support is disabled.
#define NO_RECONN       0
/// Deprecated reconnection mode retained for compatibility.
#define RECONN_NOTUSED  1
/// Enables timeout-based reconnection handling.
#define RECONN_TIMEOUT  200

/// Defines the reconnection timeout period in seconds.
#define RECONN_TIMEOUT_TIME  600

/// Forward declaration of per-thread connection state.
struct thread_context;

/// Identifies the current communication state of a process.
typedef enum ProcState {
    PROCESSING_STATE, /**< The process is idle with respect to I/O. */
    RECEIVING_STATE,  /**< The process is receiving data. */
    SENDING_STATE,    /**< The process is sending data. */
    CONN_WAIT_STATE   /**< The process is waiting on a connection event. */
} procState_t;

/// Carries state exchanged during reconnection handling.
typedef struct reconnMsg {
    int status;             /**< The status associated with the reconnection event. */
    int cookie;             /**< The reconnection cookie used to match peers. */
    procState_t procState;  /**< The process state observed during reconnection. */
    int flag;               /**< Additional reconnection control flags. */
} reconnMsg_t;

/// Describes a byte range assigned to a transfer thread.
typedef struct dataSeg {
    rodsLong_t len;     /**< The length of the segment in bytes. */
    rodsLong_t offset;  /**< The starting byte offset of the segment. */
} dataSeg_t;

/// Controls whether file restart support is enabled.
typedef enum FileRestartFlag {
    FILE_RESTART_OFF, /**< File restart support is disabled. */
    FILE_RESTART_ON   /**< File restart support is enabled. */
} fileRestartFlag_t;

/// Reports whether a file transfer was restarted.
typedef enum FileRestartStatus {
    FILE_NOT_RESTART, /**< The transfer has not been restarted. */
    FILE_RESTARTED    /**< The transfer resumed from restart information. */
} fileRestartStatus_t;

/// Captures restart metadata for a single file transfer.
typedef struct FileRestartInfo {
    char fileName[MAX_NAME_LEN];                 /**< The local file path being restarted. */
    char objPath[MAX_NAME_LEN];                  /**< The iRODS object path for the transfer. */
    int numSeg;                                  /**< The number of transfer segments recorded. */
    fileRestartStatus_t status;                  /**< The restart status for the transfer. */
    rodsLong_t fileSize;                         /**< The total size of the file in bytes. */
    dataSeg_t dataSeg[MAX_NUM_CONFIG_TRAN_THR];  /**< The saved segment layout per transfer thread. */
} fileRestartInfo_t;

/// Tracks restart settings and persisted state for a transfer.
typedef struct FileRestart {
    fileRestartFlag_t flags;             /**< Indicates whether restart handling is enabled. */
    rodsLong_t writtenSinceUpdated;      /**< Bytes transferred since the restart file was updated. */
    char infoFile[MAX_NAME_LEN];         /**< The path to the restart information file. */
    fileRestartInfo_t info;              /**< Restart details. This must remain the final member. */
} fileRestart_t;

/// Indicates whether process logging has been completed.
typedef enum ProcLogFlag {
    PROC_LOG_NOT_DONE, /**< Process logging has not completed yet. */
    PROC_LOG_DONE      /**< Process logging has completed. */
} procLogFlag_t;

/// Describes a client-side connection to an iRODS server.
typedef struct RcComm {
    irodsProt_t                irodsProt;             /**< The negotiated communication protocol. */
    char                       host[NAME_LEN];        /**< The target host name. */
    int                        sock;                  /**< The primary socket descriptor. */
    int                        portNum;               /**< The target server port number. */
    int                        loggedIn;              /**< Indicates whether authentication completed. */
    struct sockaddr_in         localAddr;             /**< The local socket address. */
    struct sockaddr_in         remoteAddr;            /**< The remote socket address. */
    userInfo_t                 proxyUser;             /**< The proxy user identity. */
    userInfo_t                 clientUser;            /**< The client user identity. */
    version_t*                 svrVersion;            /**< The server version information. */
    rError_t*                  rError;                /**< Collected remote error information. */
    int                        flag;                  /**< General connection flags. */
    transferStat_t             transStat;             /**< Transfer statistics for the connection. */
    int                        apiInx;                /**< The current API number in use. */
    int                        status;                /**< The last connection-related status. */
    int                        windowSize;            /**< The negotiated transfer window size. */
    int                        reconnectedSock;       /**< The socket descriptor used after reconnection. */
    time_t                     reconnTime;            /**< The time reconnection handling started. */
    volatile int               exit_flg;              /**< Signals shutdown to helper threads. */
    struct thread_context*     thread_ctx;            /**< Per-thread connection context. */
    procState_t                agentState;            /**< The observed state of the agent. */
    procState_t                clientState;           /**< The observed state of the client. */
    procState_t                reconnThrState;        /**< The state of the reconnection thread. */
    operProgress_t             operProgress;          /**< Callback state for progress reporting. */

    int                        key_size;              /**< The negotiated encryption key size. */
    int                        salt_size;             /**< The negotiated salt size. */
    int                        num_hash_rounds;       /**< The negotiated hash round count. */
    char                       encryption_algorithm[ NAME_LEN ]; /**< The negotiated encryption algorithm. */
    char                       negotiation_results[ MAX_NAME_LEN ]; /**< Negotiation status details. */
    unsigned char              shared_secret[ NAME_LEN ]; /**< The negotiated shared secret material. */

    int                        ssl_on;                /**< Indicates whether SSL is enabled. */
    SSL_CTX*                   ssl_ctx;               /**< The OpenSSL context for the connection. */
    SSL*                       ssl;                   /**< The OpenSSL session object. */

    fileRestart_t              fileRestart;           /**< Restart metadata. This member must remain last. */

    // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    char session_signature[33]; /**< Hex-encoded 16-byte session signature plus terminator. */ // NOLINT(cppcoreguidelines-avoid-c-arrays, modernize-avoid-c-arrays)
} rcComm_t;

/// Collects counts for orphaned and non-orphaned operations.
typedef struct PerfStat {
    int orphanCnt;     /**< The number of orphaned operations observed. */
    int nonOrphanCnt;  /**< The number of non-orphaned operations observed. */
} perfStat_t;

/// Describes a server-side connection and request context.
typedef struct RsComm {
    irodsProt_t irodsProt;                  /**< The negotiated communication protocol. */
    int sock;                               /**< The primary socket descriptor. */
    int connectCnt;                         /**< The number of connection attempts made. */
    struct sockaddr_in localAddr;           /**< The local socket address. */
    struct sockaddr_in remoteAddr;          /**< The remote socket address. */
    char clientAddr[NAME_LEN];              /**< The string form of the remote client address. */
    userInfo_t proxyUser;                   /**< The proxy user identity. */
    userInfo_t clientUser;                  /**< The client user identity. */
    rodsEnv myEnv;                          /**< The local server environment. */
    version_t cliVersion;                   /**< The client version information. */
    char option[LONG_NAME_LEN];             /**< Option text associated with the connection. */
    procLogFlag_t procLogFlag;              /**< Indicates whether process logging is complete. */
    rError_t rError;                        /**< Error stack for the request. */
    portalOpr_t *portalOpr;                 /**< Portal operation state for parallel transfer. */
    int apiInx;                             /**< The current API number in use. */
    int status;                             /**< The current request status. */
    perfStat_t perfStat;                    /**< Performance counters for the request. */
    int windowSize;                         /**< The transfer window size. */
    int reconnFlag;                         /**< Reconnection mode for the server-side socket. */
    int reconnSock;                         /**< The socket descriptor used for reconnection. */
    int reconnPort;                         /**< The port used for reconnection. */
    int reconnectedSock;                    /**< The socket descriptor accepted after reconnection. */
    char *reconnAddr;                       /**< The address used for reconnection. */
    int cookie;                             /**< The reconnection cookie. */

    struct thread_context* thread_ctx;      /**< Per-thread connection context. */

    procState_t agentState;                 /**< The observed state of the agent. */
    procState_t clientState;                /**< The observed state of the client. */
    procState_t reconnThrState;             /**< The state of the reconnection thread. */
    int gsiRequest;                         /**< Indicates whether GSI authentication was requested. */
    char* auth_scheme;                      /**< The active authentication scheme name. */

    int ssl_on;                             /**< Indicates whether SSL is enabled. */
    SSL_CTX *ssl_ctx;                       /**< The OpenSSL context for the connection. */
    SSL *ssl;                               /**< The OpenSSL session object. */
    int ssl_do_accept;                      /**< Indicates whether the SSL accept step is pending. */
    int ssl_do_shutdown;                    /**< Indicates whether SSL shutdown is required. */

    char negotiation_results[MAX_NAME_LEN]; /**< Negotiation status details. */
    unsigned char shared_secret[NAME_LEN];  /**< The negotiated shared secret material. */

    int  key_size;                          /**< The negotiated encryption key size. */
    int  salt_size;                         /**< The negotiated salt size. */
    int  num_hash_rounds;                   /**< The negotiated hash round count. */
    char encryption_algorithm[NAME_LEN];    /**< The negotiated encryption algorithm. */

    keyValPair_t session_props;             /**< General-purpose session-scoped properties. */
} rsComm_t;

#ifdef __cplusplus
#include <fmt/format.h>
#include <type_traits>

/// Formats `ProcState` values using the underlying integer representation.
template <>
struct fmt::formatter<ProcState> : fmt::formatter<std::underlying_type_t<ProcState>>
{
    /// Writes the formatted enum value to the provided formatting context.
    constexpr auto format(const ProcState& e, format_context& ctx) const
    {
        return fmt::formatter<std::underlying_type_t<ProcState>>::format(
            static_cast<std::underlying_type_t<ProcState>>(e), ctx);
    }
};

extern "C" {
#endif

/// Connects to a server using the same proxy and client user identity.
rcComm_t *
rcConnect( const char *rodsHost, int rodsPort, const char *userName, const char *rodsZone,
           int reconnFlag, rErrMsg_t *errMsg );

/// Connects to a server using explicit proxy and client user identities.
rcComm_t *
_rcConnect( const char *rodsHost, int rodsPort,
            const char *proxyUserName, const char *proxyRodsZone,
            const char *clientUserName, const char *clientRodsZone, rErrMsg_t *errMsg, int connectCnt,
            int reconnFlag );

/// Populates proxy and client user structures from the provided names and zones.
int
setUserInfo(
    const char *proxyUserName, const char *proxyRodsZone,
    const char *clientUserName, const char *clientRodsZone,
    userInfo_t *clientUser, userInfo_t *proxyUser );

/// Resolves and stores the remote host information for a client connection.
int
setRhostInfo( rcComm_t *conn, const char *rodsHost, int rodsPort );

/// Populates a socket address structure for the provided host and port.
int
setSockAddr( struct sockaddr_in *remoteAddr, const char *rodsHost, int rodsPort );

/// Sets authentication-related fields for the proxy and client user information.
int setAuthInfo( char *rodsAuthScheme,
                  char *authStr, char *rodsServerDn,
                  userInfo_t *clientUser, userInfo_t *proxyUser, int flag );

/// Disconnects a client connection and releases associated network resources.
int
rcDisconnect( rcComm_t *conn );

/// Frees a client connection structure and owned allocations.
int
freeRcComm( rcComm_t *conn );

/// Resets the dynamic state held by a client connection structure.
int
cleanRcComm( rcComm_t *conn );

// clang-format off
#ifdef __cplusplus
/// Authenticates a client connection using the configured authentication scheme.
///
/// Parameters: `conn` is the client connection handle, `_context` is the authentication context
/// string, and `_scheme_override` optionally selects a specific authentication scheme.
[[deprecated("Use irods::authentication::authenticate_client instead.")]]
int clientLogin(rcComm_t* conn, const char* _context = nullptr, const char* _scheme_override = nullptr);
#else
/// \cond IRODS_DOXYGEN_INTERNAL
/* Authenticates a client connection using the configured authentication scheme. */
///
/// Parameters: `conn` is the client connection handle, `_context` is the authentication context
/// string, and `_scheme_override` optionally selects a specific authentication scheme.
__attribute__((deprecated("Use rc_authenticate_client instead.")))
int clientLogin(rcComm_t* conn, const char* _context, const char* _scheme_override);
/// \endcond
#endif

/// \cond IRODS_DOXYGEN_INTERNAL
/* Authenticates using PAM with a password and TTL for the connected client. */
///
/// Parameters: `conn` is the client connection handle, `password` is the PAM password, and `ttl`
/// is the requested lifetime for the generated iRODS password.
__attribute__((deprecated("Use rc_authenticate_client with pam_password scheme, and AUTH_PASSWORD_KEY and TTL_KEY in context.")))
int clientLoginPam(rcComm_t* conn, char* password, int ttl);
/// \endcond

/// \cond IRODS_DOXYGEN_INTERNAL
/// Requests a limited password and stores it in the client's obfuscated password file.
///
/// This function does not authenticate the connection. It is used by `iinit` for native
/// authentication with a TTL-limited password.
__attribute__((deprecated("Native authentication plugin now handles limited passwords (i.e. TTL).")))
int clientLoginTTL(rcComm_t* conn, int ttl);

/// Authenticates using the legacy native password flow.
__attribute__((deprecated("Use rc_authenticate_client with AUTH_PASSWORD_KEY in context.")))
int clientLoginWithPassword(rcComm_t* conn, char* password);
/// \endcond

/// Returns the legacy client-side session signature buffer.
__attribute__((deprecated("Use session_signature member variable in RcComm instance.")))
char* getSessionSignatureClientside();
// clang-format on

/// Stores the first 16 bytes of a buffer as hexadecimal in the \p session_signature member
/// variable of an RcComm.
///
/// If either pointer is NULL or the size of the buffer is too small, an error is returned.
///
/// \param[in] _comm        A pointer to a RcComm.
/// \param[in] _buffer      A buffer holding at least 16 bytes.
/// \param[in] _buffer_size The size of \p _buffer.
///
/// \return An integer.
/// \retval  0 On success.
/// \retval <0 On failure.
///
/// \b Example
/// \code{.cpp}
/// RcComm* comm = // Our iRODS connection.
///
/// char buf[256];
/// memset(buf, 0, sizeof(buf));
/// fill_with_random_bytes(buf, sizeof(buf));
///
/// assert(set_session_signature_client_side(comm, buf, sizeof(buf)) == 0);
/// assert(strncmp(comm->session_signature, buf, 16) == 0);
/// \endcode
///
/// \since 4.3.1
int set_session_signature_client_side(rcComm_t* _comm, const char* _buffer, size_t _buffer_size);

/// Manages client-side reconnection for the provided connection.
void
cliReconnManager( rcComm_t *conn );

/// Performs reconnection checks before sending data.
int
cliChkReconnAtSendStart( rcComm_t *conn );

/// Performs reconnection checks after sending data.
int
cliChkReconnAtSendEnd( rcComm_t *conn );

/// Performs reconnection checks before reading data.
int
cliChkReconnAtReadStart( rcComm_t *conn );

/// Performs reconnection checks after reading data.
int
cliChkReconnAtReadEnd( rcComm_t *conn );

/// Returns whether the provided IP address refers to a loopback interface.
int
isLoopbackAddress( const char* ip_address );

#ifdef __cplusplus
} // extern "C"
#endif

#endif // IRODS_RC_CONNECT_H
