#ifndef RODS_CONNECT_H
#define RODS_CONNECT_H

/// \file

#include "irods/rodsDef.h"
#include "irods/rcConnect.h"

/// The default base name of the rule engine configuration file.
#define RE_RULES_FILE                   "reRules"

/// Keyword selecting a user allowlist in the connect control file.
#define USER_ALLOWLIST_KW               "allowlist"
/// Keyword selecting a user denylist in the connect control file.
#define USER_DENYLIST_KW                "denylist"
/// Indicates that no maximum connection limit is enforced.
#define NO_MAX_CONNECTION_LIMIT         -1
/// The default maximum connection setting.
#define DEF_MAX_CONNECTION              NO_MAX_CONNECTION_LIMIT

/// Indicates that initialization has not completed.
#define INITIAL_NOT_DONE                0
/// Indicates that initialization has completed.
#define INITIAL_DONE                    1

/// Stores a host name in a linked list of host aliases.
typedef struct hostName {
    char *name;              /**< The host name string. */
    struct hostName *next;   /**< The next host name in the list. */
} hostName_t;

/// Indicates that host locality is unknown.
#define UNKNOWN_HOST_LOC                -1
/// Identifies the local host.
#define LOCAL_HOST                      0
/// Identifies a remote host in the local zone.
#define REMOTE_HOST                     1
/// Identifies a remote gateway host.
#define REMOTE_GW_HOST                  2
/// Identifies a host in a remote zone.
#define REMOTE_ZONE_HOST                3

/// Indicates that no catalog is enabled for the host.
#define NOT_RCAT_ENABLED                0
/// Indicates that the local host provides the catalog.
#define LOCAL_ICAT                      1
/// Indicates that the local host provides the secondary catalog.
#define LOCAL_SECONDARY_ICAT            2
/// Indicates that a remote host provides the catalog.
#define REMOTE_ICAT                     3

/// Runs a single-pass operation.
#define SINGLE_PASS                     0
/// Runs in server mode.
#define SERVER                          1
/// Runs in standalone server mode.
#define STANDALONE_SERVER               2

/// Describes an iRODS server host and its connection state.
typedef struct rodsServerHost {
    hostName_t *hostName;             /**< Linked list of known names for the host. */
    rcComm_t *conn;                   /**< The cached connection to the host, if any. */
    int rcatEnabled;                  /**< Indicates the host's catalog provider role. */
    int reHostFlag;                   /**< Marks whether the host is a resource server. */
    int localFlag;                    /**< Describes the host's locality relative to this server. */
    int status;                       /**< The current status of the host entry. */
    void *zoneInfo;                   /**< Back-pointer to the owning zone information. */
    struct rodsServerHost *next;      /**< The next host entry in the list. */
} rodsServerHost_t;

/// Describes a zone and its associated catalog provider hosts.
typedef struct zoneInfo {
    char zoneName[NAME_LEN];                    /**< The zone name. */
    int portNum;                                /**< The catalog service port for the zone. */
    rodsServerHost_t* primaryServerHost;        /**< The primary catalog provider host. */
    rodsServerHost_t* secondaryServerHost;      /**< The secondary catalog provider host. */
    struct zoneInfo *next;                      /**< The next zone entry in the list. */
} zoneInfo_t;

/// The maximum number of federated server IDs tracked.
#define MAX_FED_RSIDS                   5
/// The keyword storing the local zone server ID.
#define LOCAL_ZONE_SID_KW               "LocalZoneSID"
/// The keyword storing the remote zone server ID.
#define REMOTE_ZONE_SID_KW              "RemoteZoneSID"
/// The keyword storing the server ID shared secret key.
#define SID_KEY_KW                      "SIDKey"

#ifdef __cplusplus
extern "C" {
#endif

/// Adds a host address to the host-name queue for a server entry.
int queueAddr(rodsServerHost_t* rodsServerHost, const char* myHostName);

/// Adds a host name to a server entry, optionally at the front of the list.
int queueHostName(rodsServerHost_t *rodsServerHost,
                  const char *myHostName,
                  int topFlag);

/// Appends a server host entry to the global host list.
int queueRodsServerHost(rodsServerHost_t **rodsServerHostHead,
                         rodsServerHost_t *myRodsServerHost);

/// Allocates and initializes a server host entry for the provided address and zone.
rodsServerHost_t* mkServerHost(char *myHostAddr, char *zoneName);

/// Adds a zone entry to the global zone list.
int queueZone(const char* zoneName,
              int portNum,
              rodsServerHost_t* primaryServerHost,
              rodsServerHost_t* secondaryServerHost);

/// Matches a server host entry against the configured host definitions.
int matchHostConfig(rodsServerHost_t *myRodsServerHost);

/// Queues configured host names onto a server host entry.
int queueConfigName(rodsServerHost_t *configServerHost,
                     rodsServerHost_t *myRodsServerHost);

/// Retrieves and connects to a catalog host, authenticating if needed.
int getAndConnRcatHost(rsComm_t *rsComm,
                        int rcatType,
                        const char *rcatZoneHint,
                        rodsServerHost_t **rodsServerHost);

/// Retrieves and connects to a catalog host without logging in.
int getAndConnRcatHostNoLogin(rsComm_t *rsComm,
                               int rcatType,
                               char *rcatZoneHint,
                               rodsServerHost_t **rodsServerHost);

/// \brief Gets the Catalog Service Provider host information from `ZoneInfoHead`.
///
/// \parblock
/// The information populated in `*rodsServerHost` will be a pointer to `primaryServerHost` or
/// `secondaryServerHost` of the global `ZoneInfoHead` list. These linked lists are identical to the
/// pointers in the global linked list of server information called `ServerHostHead`.
///
/// This function should be used for getting this pointer in particular because any connections
/// made to the retrieved server will be reused from previous redirections, if applicable, and
/// cleaned up automatically on agent teardown.
/// \endparblock
int getRcatHost(int rcatType,
                const char *rcatZoneHint,
                rodsServerHost_t **rodsServerHost);

/// Disconnects the cached catalog host connection for the requested zone and type.
int disconnRcatHost(int rcatType, const char *rcatZoneHint);

/// \brief Gets the Catalog Service Provider host from `ZoneInfoHead` and connects to it.
///
/// \parblock
/// The information populated in `*rodsServerHost` will be a pointer to `primaryServerHost` or
/// `secondaryServerHost` of the global `ZoneInfoHead` list. These linked lists are identical to the
/// pointers in the global linked list of server information called `ServerHostHead`.
///
/// This function should be used for making any redirect connections to the Catalog Service
/// Provider. Connections will be reused from previous redirections, if applicable, and cleaned
/// up automatically on agent teardown.
/// \endparblock
/// Retrieves a catalog host, connects to it, and disconnects on completion.
int getAndDisconnRcatHost(int rcatType,
                           char *rcatZoneHint,
                           rodsServerHost_t **rodsServerHost);

/// \brief Disconnects and cleans up all server-to-server connections made by this agent.
///
/// \parblock
/// This function walks the list of connected iRODS servers in `ServerHostHead` and disconnects
/// them. This is convenient at agent teardown to ensure that the agent is not leaving any open
/// connections or leaking memory from the connection structures.
/// \endparblock
int disconnectAllSvrToSvrConn();

/// Retrieves and connects to the remote zone host for a data object operation.
int getAndConnRemoteZone(rsComm_t *rsComm,
                          dataObjInp_t *dataObjInp,
                          rodsServerHost_t **rodsServerHost,
                          char *remotZoneOpr);

/// Returns the local resource host entry.
int getReHost(rodsServerHost_t **rodsServerHost);

/// Retrieves and connects to the local resource host.
int getAndConnReHost(rsComm_t *rsComm, rodsServerHost_t **rodsServerHost);

/// Clears cached catalog host connection state for the requested zone and type.
int resetRcatHost(int rcatType, const char *rcatZoneHint);

/// Returns whether the provided host address refers to the local host.
int isLocalHost(const char *hostAddr);

/// Returns the local zone information record.
int getLocalZoneInfo(zoneInfo_t **outZoneInfo);

/// Returns the name of the local zone.
char* getLocalZoneName();

/// Looks up zone information using the provided zone hint.
int getZoneInfo(const char *rcatZoneHint, zoneInfo_t **myZoneInfo);

/// Resolves the remote zone host for a data object operation without connecting to it.
int getRemoteZoneHost(rsComm_t *rsComm,
                       dataObjInp_t *dataObjInp,
                       rodsServerHost_t **rodsServerHost,
                       char *remotZoneOpr);

/// Returns whether the provided zone hint names the local zone.
int isLocalZone(const char *zoneHint);

/// Returns whether two zone hints refer to the same zone.
int isSameZone(char *zoneHint1, char *zoneHint2);

/// Converts socket-layer failures into zone-resolution error codes.
int convZoneSockError(int inStatus);

/// Resolves a host address into a server host entry.
int resolveHost(rodsHostAddr_t *addr, rodsServerHost_t **rodsServerHost);

/// Resolves and connects to the host identified by a data object information record.
int resoAndConnHostByDataObjInfo(rsComm_t *rsComm,
                                  dataObjInfo_t *dataObjInfo,
                                  rodsServerHost_t **rodsServerHost);

/// Prints the configured zone information.
int printZoneInfo();

/// Prints a server host entry.
int printServerHost(rodsServerHost_t *myServerHost);

#ifdef __cplusplus
} //extern C
#endif

#endif // RODS_CONNECT_H
