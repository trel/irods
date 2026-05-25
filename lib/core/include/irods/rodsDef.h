#ifndef RODS_DEF_H__
#define RODS_DEF_H__

#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include <string.h>
#include <signal.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/types.h>

#ifdef solaris_platform
    #include <fcntl.h>
    #include <arpa/inet.h>
#endif

#ifndef _WIN32
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <sys/file.h>
    #include <unistd.h>
    #include <dirent.h>
    #include <time.h>
    #include <sys/param.h>
    #include <pwd.h>
    #include <grp.h>
    #include <netdb.h>
    #include <netinet/in.h>
    #include <netinet/tcp.h>
#endif

// clang-format off

/** Maximum length of a message header type string. */
#define HEADER_TYPE_LEN                 128
/** Length of a formatted time string. */
#define TIME_LEN                        32
/** Standard short name buffer length. */
#define NAME_LEN                        64
/** Length of a legacy checksum string. */
#define CHKSUM_LEN                      64
/** Length of an extended checksum string. */
#define LONG_CHKSUM_LEN                 128
/** Length of a long general-purpose name buffer. */
#define LONG_NAME_LEN                   256
/** Maximum path length accepted by iRODS. */
#define MAX_PATH_ALLOWED                1024
/* MAX_NAME_LEN actually has space for a few extra characters to match
   the way it is often used in the code. */
#define MAX_NAME_LEN                    (MAX_PATH_ALLOWED+64) /**< Path-sized buffer with extra headroom. */
/** Very large temporary name buffer length. */
#define HUGE_NAME_LEN                   100000

/* increase to 20K because the French Lib rules are very long */
#define META_STR_LEN                    (1024*20) /**< Maximum metadata string length. */

/** Length of short string buffers such as mode fields. */
#define SHORT_STR_LEN                   32

#ifndef NULL
#define NULL                            0 /**< Null pointer constant for C builds lacking one. */
#endif

/** Growth increment used when extending pointer arrays. */
#define PTR_ARRAY_MALLOC_LEN            10

/* definition for the global variable int ProcessType */

/** Process type identifier for clients. */
#define CLIENT_PT                       0
/** Process type identifier for servers. */
#define SERVER_PT                       1
/** Process type identifier for agents. */
#define AGENT_PT                        2
/** Process type identifier for delay servers. */
#define DELAY_SERVER_PT                 3

/* definition for rcat type */

/** Catalog role identifier for the primary catalog provider. */
#define PRIMARY_RCAT                    0
/** Catalog role identifier for a secondary catalog consumer. */
#define SECONDARY_RCAT                  1
/** Initial size of the execute-command buffer. */
#define INIT_SZ_FOR_EXECMD_BUF          (16*1024)
/** Maximum size of the execute-command buffer. */
#define MAX_SZ_FOR_EXECMD_BUF           (1*1024*1024)
/** Minimum data size for parallel transfer. */
#define MIN_SZ_FOR_PARA_TRAN            (1*1024*1024)
/** Default transfer chunk size. */
#define TRANS_SZ                        (40*1024*1024)
/** Large placeholder value used for free-space calculations. */
#define LARGE_SPACE                     1000000000
/** Sentinel indicating that the file size is unknown. */
#define UNKNOWN_FILE_SZ                 -99
/** Smallest restartable transfer size. */
#define MIN_RESTART_SIZE                (64*1024*1024)
/** Size threshold for updating restart files. */
#define RESTART_FILE_UPDATE_SIZE        (32*1024*1024)

/** Maximum configured number of transfer threads. */
#define MAX_NUM_CONFIG_TRAN_THR         64

/* definition for numThreads input */

/** Disables threaded transfer behavior. */
#define NO_THREADING                    -1
/** Requests automatic thread selection. */
#define AUTO_THREADING                  0

/** Indicates that the rule execution information should not be saved. */
#define NO_SAVE_REI                     0
/** Indicates that the rule execution information should be saved. */
#define SAVE_REI                        1

/** Timeout in seconds when waiting on a connection. */
#define SELECT_TIMEOUT_FOR_CONN         60
/* this is the return value for the rcExecMyRule call indicating the
 * server is requesting the client to client to perform certain task */
/** Server requests a client-side microservice action. */
#define SYS_SVR_TO_CLI_MSI_REQUEST      99999995
/** Server requests collection stat information from the client. */
#define SYS_SVR_TO_CLI_COLL_STAT        99999996
/** Client reply carrying collection stat information. */
#define SYS_CLI_TO_SVR_COLL_STAT_REPLY  99999997
/** Server requests a client-side put action. */
#define SYS_SVR_TO_CLI_PUT_ACTION       99999990
/** Server requests a client-side get action. */
#define SYS_SVR_TO_CLI_GET_ACTION       99999991
/** Indicates that the rsync target changed during processing. */
#define SYS_RSYNC_TARGET_MODIFIED       99999992

/* definition for iRODS server to client action request from a microservice.
 * these definitions are put in the "label" field of MsParam */

/** Label identifying a client put action request. */
#define CL_PUT_ACTION           "CL_PUT_ACTION"
/** Label identifying a client get action request. */
#define CL_GET_ACTION           "CL_GET_ACTION"
/** Label carrying a cross-zone operation index. */
#define CL_ZONE_OPR_INX         "CL_ZONE_OPR_INX"

/* the following defines the RSYNC_MODE_KW */
/** Rsync mode for local-to-iRODS transfers. */
#define LOCAL_TO_IRODS          "LOCAL_TO_IRODS"
/** Rsync mode for iRODS-to-local transfers. */
#define IRODS_TO_LOCAL          "IRODS_TO_LOCAL"
/** Rsync mode for iRODS-to-iRODS transfers. */
#define IRODS_TO_IRODS          "IRODS_TO_IRODS"
/** Rsync mode for copying into a target collection. */
#define IRODS_TO_COLLECTION     "IRODS_TO_COLLECTION"

/* definition for public user. Deprecated in 5.1.0. */
#define PUBLIC_USER_NAME        "public" /**< Name of the deprecated public user. */

/* definition for anonymous user */
#define ANONYMOUS_USER          "anonymous" /**< Name of the anonymous user. */

/* definition for bulk operation */
/** Maximum number of files grouped into a bulk operation. */
#define MAX_NUM_BULK_OPR_FILES  50
/** Maximum size of a single file in a bulk operation. */
#define MAX_BULK_OPR_FILE_SIZE  (4*1024*1024)
/** Buffer size reserved for bulk operations. */
#define BULK_OPR_BUF_SIZE       (8*MAX_BULK_OPR_FILE_SIZE)
/** Estimated tar-format overhead for bulk operations. */
#define TAR_OVERHEAD            (MAX_NUM_BULK_OPR_FILES * MAX_NAME_LEN * 2)

/* generic return value for policy rules */
/** Generic disabled value for policy evaluation. */
#define POLICY_OFF              0
/** Generic enabled value for policy evaluation. */
#define POLICY_ON               1

/** Supported client-server wire protocols. */
typedef enum iRODSProtocol {
    NATIVE_PROT, ///< Native binary protocol.
    XML_PROT     ///< XML-based protocol.
} irodsProt_t;

/** Descriptor kinds handled by low-level I/O helpers. */
typedef enum iRODSDescType {
    SOCK_TYPE,      ///< Socket descriptor.
    FILE_DESC_TYPE  ///< File descriptor.
} irodsDescType_t;

/** Queue insertion positions. */
typedef enum iRODSPosition {
    BOTTOM_POS, ///< Insert at the bottom.
    TOP_POS     ///< Insert at the top.
} irodsPosition_t;

/** Generic three-state flag values. */
typedef enum iRODSStateFlag {
    UNINIT_STATE, ///< Value has not been initialized.
    OFF_STATE,    ///< Feature is disabled.
    ON_STATE      ///< Feature is enabled.
} irodsStateFlag_t;

/** Classification of orphaned path types. */
typedef enum OrphanPathType {
    NOT_ORPHAN_PATH, ///< Path is not orphaned.
    IS_ORPHAN_PATH,  ///< Path is orphaned.
    is_ORPHAN_HOME   ///< Path is inside an orphan home.
} orphanPathType_t;

/** Default wire protocol used by iRODS. */
#define DEF_IRODS_PROT  NATIVE_PROT

/**
 * \var bytesBuf_t
 * \brief  general struct to store a buffer of bytes
 * \since 1.0
 *
 * \ingroup capi_input_data_structures
 *
 * \remark none
 *
 * \note
 * Elements of bytesBuf_t:
 * \li int len - the length of the allocated buffer in buf or the length of
 *        the data stored in buf depending on the application.
 * \li void *buf - pointer to the buffer.
 *
 * \sa none
 */

typedef struct BytesBuf {   /* have to add BytesBuf to get Doxygen working */
    int len;    ///< Length of the buffer in bytes.
    void *buf;  ///< Pointer to the buffer contents.
} bytesBuf_t;

/** Generic typed array wrapper. */
typedef struct DataArray {   /* have to add BytesBuf to get Doxygen working */
    int type;   ///< Type identifier for the array contents.
    int len;    ///< Number of array elements stored in `buf`.
    void *buf;  ///< Pointer to the array data.
} dataArray_t;

/* The msg header for all communication between client and server */

/** Message header exchanged before each client-server payload. */
typedef struct msgHeader {
    char type[HEADER_TYPE_LEN]; ///< Message type string.
    int msgLen;                 ///< Length of the main message payload.
    int errorLen;               ///< Length of the serialized error payload.
    int bsLen;                  ///< Length of the optional byte-stream payload.
    int intInfo;                ///< Additional integer information such as an API number.
} msgHeader_t;

/* header length XML tag */
#define MSG_HEADER_LEN_TAG      "MsgHeaderLen" /**< XML tag storing the header length. */

/* msg type */
/** Message type for startup connection requests. */
#define RODS_CONNECT_T          "RODS_CONNECT"
/** Message type for heartbeat traffic. */
#define RODS_HEARTBEAT_T        "HEARTBEAT"
/** Message type for version exchange. */
#define RODS_VERSION_T          "RODS_VERSION"
/** Message type for API requests. */
#define RODS_API_REQ_T          "RODS_API_REQ"
/** Message type for disconnect requests. */
#define RODS_DISCONNECT_T       "RODS_DISCONNECT"
/** Message type for reconnect requests. */
#define RODS_RECONNECT_T        "RODS_RECONNECT"
/** Message type for reauthentication requests. */
#define RODS_REAUTH_T           "RODS_REAUTH"
/** Message type for API replies. */
#define RODS_API_REPLY_T        "RODS_API_REPLY"
/** Message type for client-server negotiation. */
#define RODS_CS_NEG_T           "RODS_CS_NEG_T"

/* The strct sent with RODS_CONNECT type by client */
/** Startup information sent by the client at connection time. */
typedef struct startupPack {
    irodsProt_t irodsProt;          ///< Requested communication protocol.
    int reconnFlag;                 ///< Reconnection mode requested by the client.
    int connectCnt;                 ///< Connection attempt count.
    char proxyUser[NAME_LEN];       ///< Proxy user name.
    char proxyRodsZone[NAME_LEN];   ///< Proxy user zone.
    char clientUser[NAME_LEN];      ///< Client user name.
    char clientRodsZone[NAME_LEN];  ///< Client user zone.
    char relVersion[NAME_LEN];      ///< Client release version.
    char apiVersion[NAME_LEN];      ///< Client API version.
    char option[LONG_NAME_LEN];     ///< Additional startup options.
} startupPack_t;

/* env variable for the client protocol */
#define IRODS_PROT              "irodsProt" /**< Environment key for the client protocol. */

/* env variable for the startup pack */

/** Startup pack key storing a newly opened socket. */
#define SP_NEW_SOCK             "spNewSock"
/** Startup pack key storing the client address. */
#define SP_CLIENT_ADDR          "spClientAddr"
/** Startup pack key storing the connection count. */
#define SP_CONNECT_CNT          "spConnectCnt"
/** Startup pack key storing the selected protocol. */
#define SP_PROTOCOL             "spProtocol"
/** Startup pack key storing the reconnect flag. */
#define SP_RECONN_FLAG          "spReconnFlag"
/** Startup pack key storing the proxy user name. */
#define SP_PROXY_USER           "spProxyUser"
/** Startup pack key storing the proxy zone. */
#define SP_PROXY_RODS_ZONE      "spProxyRodsZone"
/** Startup pack key storing the client user name. */
#define SP_CLIENT_USER          "spClientUser"
/** Startup pack key storing the client zone. */
#define SP_CLIENT_RODS_ZONE     "spClientRodsZone"
/** Startup pack key storing the release version. */
#define SP_REL_VERSION          "spRelVersion"
/** Startup pack key storing the API version. */
#define SP_API_VERSION          "spApiVersion"
/** Startup pack key storing extra options. */
#define SP_OPTION               "spOption"
/** Startup pack key storing the log level. */
#define SP_LOG_LEVEL            "spLogLevel"
/** Startup pack key storing the rule-engine cache salt. */
#define SP_RE_CACHE_SALT        "reCacheSalt"
/** Startup pack key storing the server boot time. */
#define SERVER_BOOT_TIME        "serverBootTime"

// =-=-=-=-=-=-=-
// magic token to assign to startup pack option variable
// in order to request a client-server negotiation
#define REQ_SVR_NEG             "request_server_negotiation" /**< Startup option requesting negotiation. */

/* Definition for resource status. If it is empty (strlen == 0), it is
 * assumed to be up */
/** Resource status string for a down resource. */
#define RESC_DOWN               "down"
/** Resource status string for an up resource. */
#define RESC_UP                 "up"
/** Resource status string for an automatically-enabled resource. */
#define RESC_AUTO_UP            "auto-up"
/** Resource status string for an automatically-disabled resource. */
#define RESC_AUTO_DOWN          "auto-down"

/** Integer resource status value representing up. */
#define  INT_RESC_STATUS_UP     0
/** Integer resource status value representing down. */
#define  INT_RESC_STATUS_DOWN   1

/* The strct sent with RODS_VERSION type by server */

/** Version information returned by the server. */
typedef struct Version {
    int status;                    ///< Status code or reconnect port when positive.
    char relVersion[NAME_LEN];     ///< Server release version.
    char apiVersion[NAME_LEN];     ///< Server API version.
    int reconnPort;                ///< Reconnect port number.
    char reconnAddr[LONG_NAME_LEN];///< Reconnect host address.
    int cookie;                    ///< Session cookie.
} version_t;
/* struct that defines a Host Addr */

/** Network address of an iRODS host. */
typedef struct RodsHostAddress {
    char hostAddr[LONG_NAME_LEN]; ///< Host address string.
    char zoneName[NAME_LEN];      ///< Zone served by the host.
    int portNum;                  ///< Listening port number.
    int dummyInt;                 ///< Padding to preserve alignment.
} rodsHostAddr_t;

/* definition for restartState */

/** Restart state flag indicating path matching is active. */
#define PATH_MATCHING           0x1
/** Restart state flag indicating the last path matched. */
#define LAST_PATH_MATCHED       0x2
/** Restart state flag indicating the restart collection matched. */
#define MATCHED_RESTART_COLL    0x4
/** Restart state flag indicating the operation resumed. */
#define OPR_RESUMED             0x8

/* definition for DNN WOS connection env */

/** Environment key storing the WOS host name. */
#define WOS_HOST_ENV            "wosHost"
/** Environment key storing the WOS policy. */
#define WOS_POLICY_ENV          "wosPolicy"

/* struct that defines restart operation */

/** Persistent state for restartable operations. */
typedef struct RodsRestart {
    char restartFile[MAX_NAME_LEN];   ///< Path to the restart file.
    int fd;                           ///< File descriptor for the restart file.
    int doneCnt;                      ///< Number of completed entries.
    char collection[MAX_NAME_LEN];    ///< Collection or directory being processed.
    char lastDonePath[MAX_NAME_LEN];  ///< Last completed path.
    char oprType[NAME_LEN];           ///< Bulk or non-bulk operation type string.
    int curCnt;                       ///< Current processed entry count.
    int restartState;                 ///< Bitmask of restart state flags.
} rodsRestart_t;

/* definition for handler function */
#ifdef __cplusplus
/** Generic callback function pointer used throughout legacy APIs. */
typedef int( ( *funcPtr )( ... ) );
#else
/** Generic callback function pointer used throughout legacy APIs. */
typedef int( ( *funcPtr )( ) );
#endif

/* some platform does not support vfork */
#if defined(sgi_platform) || defined(aix_platform)
#define RODS_FORK() fork()
#else
#define RODS_FORK() vfork()
#endif

#define VAULT_PATH_POLICY       "VAULT_PATH_POLICY" /**< `msParam` label for vault path policy. */
/* definition for vault filePath scheme */
/** Strategies for constructing vault paths. */
typedef enum VaultPathScheme {
    GRAFT_PATH_S, ///< Build the path by grafting path components.
    RANDOM_S      ///< Build the path using randomized components.
} vaultPathScheme_t;

/** Default vault path construction scheme. */
#define DEF_VAULT_PATH_SCHEME   GRAFT_PATH_S
/** Default value for including the user name in grafted paths. */
#define DEF_ADD_USER_FLAG       1
/** Default number of directory levels to trim for grafted paths. */
#define DEF_TRIM_DIR_CNT        1

/** Policy describing how vault paths should be generated. */
typedef struct VaultPathPolicy {
    vaultPathScheme_t scheme; ///< Selected vault path scheme.
    int addUserName;          ///< Whether to include the user name in the path.
    int trimDirCnt;           ///< Number of directories to trim for grafted paths.
} vaultPathPolicy_t;

/** Per-process information written to agent logs. */
typedef struct ProcessLog {
    int pid;                      ///< Process identifier.
    unsigned int startTime;       ///< Process start time.
    char clientName[NAME_LEN];    ///< Client user name.
    char clientZone[NAME_LEN];    ///< Client zone name.
    char proxyName[NAME_LEN];     ///< Proxy user name.
    char proxyZone[NAME_LEN];     ///< Proxy zone name.
    char remoteAddr[NAME_LEN];    ///< Remote client address.
    char serverAddr[NAME_LEN];    ///< Local server address.
    char progName[NAME_LEN];      ///< Program name.
} procLog_t;

// clang-format on

#endif // RODS_DEF_H__
