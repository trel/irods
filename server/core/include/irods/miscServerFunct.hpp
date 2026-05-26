/*** Copyright (c), The Regents of the University of California            ***
 *** For more information please refer to files in the COPYRIGHT directory ***/

/* miscServerFunct.h - header file for miscServerFunct.c
 */



#ifndef MISC_SERVER_FUNCT_HPP
#define MISC_SERVER_FUNCT_HPP

/// \file

#include <sys/types.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#include "irods/rods.h"
#include "irods/rcConnect.h"
#include "irods/fileOpen.h"
#include "irods/dataObjInpOut.h"
#include "irods/dataCopy.h"
#include "irods/rodsConnect.h"

#include "irods/structFileSync.h" /* JMC */

/// \brief Maximum number of reconnection errors tolerated before aborting.
#define MAX_RECON_ERROR_CNT	10

/// \brief Input describing a portal-based data transfer operation.
typedef struct PortalTransferInp {
    rsComm_t *rsComm; ///< Server communication object for the transfer.
    int destFd; ///< Destination file descriptor.
    int srcFd; ///< Source file descriptor.
    int destRescTypeInx; ///< Destination resource type index.
    int srcRescTypeInx; ///< Source resource type index.
    int threadNum; ///< Transfer thread count.
    rodsLong_t size; ///< Number of bytes to transfer.
    rodsLong_t offset; ///< Starting offset for the transfer.
    rodsLong_t bytesWritten; ///< Number of bytes written so far.
    int flags; ///< Transfer option flags.
    int status; ///< Transfer status code.
    dataOprInp_t *dataOprInp; ///< Data operation parameters for the transfer.

    int  key_size; ///< Encryption key size in bytes.
    int  salt_size; ///< Salt size in bytes.
    int  num_hash_rounds; ///< Number of hash rounds used for key derivation.
    char encryption_algorithm[ NAME_LEN ]; ///< Name of the encryption algorithm in use.
    char shared_secret[ NAME_LEN ]; ///< Shared secret for the portal thread.

} portalTransferInp_t;

/// \brief Connects to another iRODS server and performs login if needed.
int
svrToSvrConnect( rsComm_t *rsComm, rodsServerHost_t *rodsServerHost );
int
svrToSvrConnect( rsComm_t *rsComm, rodsServerHost_t *rodsServerHost );
/// \brief Connects to another iRODS server without performing login.
int
svrToSvrConnectNoLogin( rsComm_t *rsComm, rodsServerHost_t *rodsServerHost );
/// \brief Creates a server portal for parallel transfer operations.
int
createSrvPortal( rsComm_t *rsComm, portList_t *thisPortList, int proto );
/// \brief Accepts an incoming server portal connection.
int
acceptSrvPortal( rsComm_t *rsComm, portList_t *thisPortList );
/// \brief Performs a server-to-server portal put or get operation.
int
svrPortalPutGet( rsComm_t *rsComm );
/// \brief Transfers a portion of data to the destination portal.
void
partialDataPut( portalTransferInp_t *myInput );
/// \brief Transfers a portion of data from the source portal.
void
partialDataGet( portalTransferInp_t *myInput );
/// \brief Fills a portal transfer input structure.
int
fillPortalTransferInp( portalTransferInp_t *myInput, rsComm_t *rsComm,
                       int srcFd, int destFd, int destRescTypeInx, int srcRescTypeInx,
                       int threadNum, rodsLong_t size, rodsLong_t offset, int flags );
/// \brief Copies data between resources on the same host.
int
sameHostCopy( rsComm_t *rsComm, dataCopyInp_t *dataCopyInp );
/// \brief Copies part of a same-host transfer.
void sameHostPartialCopy(portalTransferInp_t* myInput);
/// \brief Copies data between a remote and local resource.
int
remLocCopy( rsComm_t *rsComm, dataCopyInp_t *dataCopyInp );
/// \brief Copies a partial transfer from a remote source to a local destination.
void
remToLocPartialCopy( portalTransferInp_t *myInput );
/// \brief Copies a partial transfer from a local source to a remote destination.
void
locToRemPartialCopy( portalTransferInp_t *myInput );
/// \brief Performs a single-threaded remote-local copy.
int
singleRemLocCopy( rsComm_t *rsComm, dataCopyInp_t *dataCopyInp );
/// \brief Performs a single-threaded remote-to-local copy.
int
singleRemToLocCopy( rsComm_t *rsComm, dataCopyInp_t *dataCopyInp );
/// \brief Performs a single-threaded local-to-remote copy.
int
singleLocToRemCopy( rsComm_t *rsComm, dataCopyInp_t *dataCopyInp );
/// \brief Returns whether the connected user has elevated privileges.
int
isUserPrivileged( rsComm_t *rsComm );
/// \brief Placeholder returning an integer for unsupported operations.
int intNoSupport( ... );
/// \brief Placeholder returning a long integer for unsupported operations.
rodsLong_t longNoSupport( ... );
/// \brief Looks up the server identifier for the named zone.
void getZoneServerId(char* zoneName, char* zoneSID);
/// \brief Manages reconnection state for a server connection.
void
reconnManager( rsComm_t *rsComm );
/// \brief Checks reconnection state before a read begins.
int
svrChkReconnAtReadStart( rsComm_t *rsComm );
/// \brief Checks reconnection state after a read completes.
int
svrChkReconnAtReadEnd( rsComm_t *rsComm );
/// \brief Checks reconnection state before sending data.
int
svrChkReconnAtSendStart( rsComm_t *rsComm );
/// \brief Checks reconnection state after sending data.
int
svrChkReconnAtSendEnd( rsComm_t *rsComm );
/// \brief Opens a socket for an incoming server connection.
int
svrSockOpenForInConn( rsComm_t *rsComm, int *portNum, char **addr, int proto );
/// \brief Returns the address of the local server.
char *
getLocalSvrAddr();
/// \brief Returns the address for the provided server host.
char *
_getSvrAddr( rodsServerHost_t *rodsServerHost );
/// \brief Returns the address for the provided server host.
char *
getSvrAddr( rodsServerHost_t *rodsServerHost );
/// \brief Stores the local server address string.
int
setLocalSrvAddr( char *outLocalAddr );
/// \brief Prepares a server portal for a parallel operation.
int setupSrvPortalForParaOpr(rsComm_t* rsComm, dataOprInp_t* dataOprInp, int oprType, portalOprOut_t** portalOprOut);
/// \brief Initializes service user information.
int
initServiceUser();
/// \brief Returns whether service user information has been initialized.
int
isServiceUserSet();
/// \brief Switches process privileges to the root user.
int
changeToRootUser();
/// \brief Switches process privileges to the configured service user.
int
changeToServiceUser();
/// \brief Switches process privileges to the specified user.
int
changeToUser( uid_t uid );
/// \brief Drops root privileges for the current process.
int
dropRootPrivilege();
/// \brief Validates the type of a modifier argument.
int
checkModArgType( const char *arg );

#ifdef __cplusplus
#include "irods/irods_error.hpp"
#include "irods/irods_plugin_base.hpp"
#include "irods/irods_network_object.hpp"

/// \brief Reads the startup pack from a network object.
irods::error readStartupPack(
    irods::network_object_ptr,
    startupPack_t **startupPack,
    struct timeval *tv );

/// \brief Loads the rule engine cache salt from the environment.
irods::error setRECacheSaltFromEnv();

/// \brief Adds global rule engine parameters to a key-value pair for dynamic PEPs.
irods::error add_global_re_params_to_kvp_for_dynpep(
    keyValPair_t& _kvp );

/// \brief Returns the configured catalog service role.
irods::error get_catalog_service_role( std::string& );
/// \brief Returns the default rule plugin instance name.
irods::error get_default_rule_plugin_instance(std::string&);
/// \brief Lists all configured rule plugin instance names.
irods::error list_rule_plugin_instances( std::vector< std::string >& );

#endif // __cplusplus

#endif	/* MISC_SERVER_FUNCT_H */
