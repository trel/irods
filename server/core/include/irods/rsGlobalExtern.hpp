#ifndef RS_GLOBAL_EXTERN_HPP
#define RS_GLOBAL_EXTERN_HPP

/// \file

#include "irods/rods.h"
#include "irods/apiHandler.hpp"
#include "irods/fileOpr.hpp"
#include "irods/objDesc.hpp"
#include "irods/querySpecColl.h"
#include "irods/miscUtil.h"
#include "irods/authenticate.h"
#include "irods/openCollection.h"

#include "irods/irods_resource_manager.hpp"

// externs to singleton plugin managers
/// \brief Global resource manager instance.
extern irods::resource_manager resc_mgr;

/// \brief File descriptor for the active server log file.
extern int LogFd;         		/* the log file descriptor */
/// \brief Path to the active server log file.
extern char *CurLogfileName;         	/* the path of the current logfile */
/// \brief Directory containing process log files.
extern char ProcLogDir[MAX_NAME_LEN];
/// \brief Global table of registered server APIs.
extern irods::api_entry_table RsApiTable;
/// \brief Indicates whether resource groups have been initialized.
extern int RescGrpInit;
/// \brief Global table of special collection descriptors.
extern specCollDesc_t SpecCollDesc[NUM_SPEC_COLL_DESC];
/// \brief Global collection handle table.
extern std::vector<collHandle_t> CollHandle;;

/// \brief Global table of L1 descriptors which contains information about open replicas.
extern l1desc_t L1desc[NUM_L1_DESC];

/// \brief Global table of file descriptors which contains information about open files.
extern fileDesc_t FileDesc[NUM_FILE_DESC];

/// \brief Global linked list of iRODS servers to which the agent has connected.
///
/// \parblock
/// `LocalServerHost` is added as the head of this list in `initLocalServerHost()`.
///
/// Whenever a new connection is made to an iRODS server, its `rodsServerHost` information
/// should be added to this list.
///
/// After the connection is no longer needed, it should be left in the list for reuse within
/// the agent should a later redirection to the same server be needed later. All of the
/// connections held open in this linked list will be connected on agent teardown via
/// `cleanup()`.
/// \endparblock
extern rodsServerHost_t *ServerHostHead;

/// \brief Global head of a linked list of connected iRODS servers, `ServerHostHead`.
///
/// \parblock
/// This `rodsServerHost*` represents the local server and is always the head of the linked list
/// of connected servers.
/// \endparblock
extern rodsServerHost_t *LocalServerHost;

/// \brief Global linked list of zones. The head is always the local zone.
///
/// \p `queZone` is used to add zones to this list.
extern zoneInfo_t *ZoneInfoHead;

/// \brief Head of the host configuration list.
extern rodsServerHost_t *HostConfigHead;

/* global Rule Engine File Initialization String */

/// \brief Rule engine initialization string for rule definitions.
extern char reRuleStr[LONG_NAME_LEN];
/// \brief Rule engine initialization string for function mappings.
extern char reFuncMapStr[LONG_NAME_LEN];
/// \brief Rule engine initialization string for variable mappings.
extern char reVariableMapStr[LONG_NAME_LEN];

/* Kerberos server name */
/// \brief Kerberos service principal name.
extern char KerberosName[MAX_NAME_LEN];

/// \brief Initial agent state.
extern int InitialState;
/// \brief Pointer to the current server communication object.
extern rsComm_t *ThisComm;

/// \brief Current catalog connection state.
extern int IcatConnState;

/// \brief Head of the special collection cache list.
extern specCollCache_t *SpecCollCacheHead;

/// \brief Local zone shared identifier.
extern char localSID[MAX_PASSWORD_LEN];

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
/// \brief Mapping of remote zone shared identifiers to negotiation keys.
extern irods::lookup_table <std::pair <std::string, std::string> > remote_SID_key_map; // remote zone SIDs and negotiation keys
#pragma GCC diagnostic pop

/* quota for all resources for this user in bytes */
/// \brief Quota limit across all resources for the current user.
extern rodsLong_t GlobalQuotaLimit; /* quota for all resources for this user */
/// \brief Amount by which the current user exceeds the global quota.
extern rodsLong_t GlobalQuotaOverrun;  /* quota overrun for this user */
/// \brief Active resource quota policy.
extern int RescQuotaPolicy;
/// \brief Timestamp of the last resource information refresh.
extern time_t LastRescUpdateTime;

/* manage server process permissions */
/// \brief User ID of the service account.
extern uid_t ServiceUid;
/// \brief Group ID of the service account.
extern gid_t ServiceGid;

/// \brief Rule engine read-write state flag.
extern irodsStateFlag_t ReadWriteRuleState;

#endif // RS_GLOBAL_EXTERN_HPP
