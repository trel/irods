#ifndef IRODS_SOCK_COMM_H
#define IRODS_SOCK_COMM_H

/// \file

#include "irods/rodsDef.h"
#include "irods/rodsPackInstruct.h"

struct RcComm;
struct RsComm;
struct PortList;
struct DataObjInp;
struct RodsEnvironment;

/** Maximum number of pending connections on a listening socket. */
#define MAX_LISTEN_QUE                  50
/** Default number of server ports to probe. */
#define DEF_NUMBER_SVR_PORT             200
/** Connection timeout in seconds. */
#define CONNECT_TIMEOUT_TIME            100
/** Time in seconds to wait before reconnecting. */
#define RECONNECT_WAIT_TIME             100
/** Sleep interval in seconds between reconnect attempts. */
#define RECONNECT_SLEEP_TIME            300
/** Maximum number of reconnect retries. */
#define MAX_RECONN_RETRY_CNT            4
/** Maximum number of initial connect retries. */
#define MAX_CONN_RETRY_CNT              3
/** Sleep interval in microseconds between connect attempts. */
#define CONNECT_SLEEP_TIME              200000

/** Timeout in seconds when reading a startup pack. */
#define READ_STARTUP_PACK_TOUT_SEC      100
/** Timeout in seconds when reading a version message. */
#define READ_VERSION_TOUT_SEC           100

/** Environment variable enabling reconnect timeout behavior. */
#define RECONNECT_ENV                   "irodsReconnect"

/* definition for socket close function */
/** Socket close state indicating the server is still reading from the client. */
#define READING_FROM_CLI                0
/** Socket close state indicating the server is processing an API request. */
#define PROCESSING_API                  1

#ifdef _WIN32
    #define CLOSE_SOCK                  closesocket /**< Platform socket close routine. */
#else
    #define CLOSE_SOCK                  close /**< Platform socket close routine. */
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** Opens a listening socket for incoming server connections. */
int sockOpenForInConn(struct RsComm *comm, int *portNum, char **addr, int proto);

/** Applies socket options to the given file descriptor. */
int rodsSetSockOpt(int sock, int tcp_buffer_size);

/** Connects a client communication object to its remote host. */
int connectToRhost(struct RcComm *comm, int connectCnt, int reconnFlag);

/** Connects to a remote address using a specific window size and timeout mode. */
int connectToRhostWithRaddr(struct sockaddr_in *remoteAddr, int windowSize, int timeoutFlag);

/** Connects to a remote host using the default timeout behavior. */
int connectToRhostWithTout(struct sockaddr *sin);

/** Sleeps for the requested number of seconds and microseconds. */
int rodsSleep(int sec, int microSec);

/** Resolves and stores the remote address for a client communication object. */
int setConnAddr(struct RcComm *comm);

/** Retrieves the peer address for a connected socket. */
int setRemoteAddr(int sock, struct sockaddr_in *remoteAddr);

/** Retrieves the local address for a connected socket. */
int setLocalAddr(int sock, struct sockaddr_in *localAddr);

/** Sends the startup pack to the connected server. */
int sendStartupPack(struct RcComm *comm, int connectCnt, int reconnFlag);

/** Connects to a transfer portal hosted by `rodsHost`. */
int connectToRhostPortal(char *rodsHost, int rodsPort, int cookie, int windowSize);

/** Accepts an incoming connection for a server communication object. */
int rsAcceptConn(struct RsComm *comm);

/** Returns the TCP port encoded in a port list entry. */
int getTcpPortFromPortList(struct PortList *thisPortList);

/** Returns the socket descriptor encoded in a port list entry. */
int getTcpSockFromPortList(struct PortList *thisPortList);

/** Returns nonzero when `status` represents a message-read error. */
int isReadMsgError(int status);

/** Switches a server communication object to a replacement socket. */
int svrSwitchConnect(struct RsComm *comm);

/** Switches a client communication object to a replacement socket. */
int cliSwitchConnect(struct RcComm *comm);

/** Redirects a client connection to the resource server handling `dataObjInp`. */
int redirectConnToRescSvr(struct RcComm **comm, struct DataObjInp *dataObjInp, struct RodsEnvironment *myEnv, int reconnFlag);

/** Reconnects a client communication object to `newHost`. */
int rcReconnect(struct RcComm **comm, char *newHost, struct RodsEnvironment *myEnv, int reconnFlag);

/** Closes a socket using iRODS-specific shutdown handling. */
int mySockClose(int sock); // server stop fcn <==> rsAccept?

/// \brief Set the TCP_KEEPALIVE options for the specified socket.
///
/// param[in] _sfd Socket file descriptor on which options are being set.
///
/// \return An integer
/// \retval 0 on success
/// \retval <0 on failure
///
/// \since 4.3.1
int set_socket_tcp_keepalive_options(int _sfd); // NOLINT(modernize-use-trailing-return-type)

#ifdef __cplusplus
}
#endif

#endif // IRODS_SOCK_COMM_H
