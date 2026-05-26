#ifndef IRODS_RE_IN2P3_SYS_RULE_HPP
#define IRODS_RE_IN2P3_SYS_RULE_HPP

#include "irods/msParam.h"
#include "irods/rods.h"
#include "irods/rsGlobalExtern.hpp"   // server global
#include "irods/rcGlobalExtern.h"     // client global
#include "irods/rodsLog.h"
#include "irods/sockComm.h"
#include "irods/getRodsEnv.h"
#include "irods/rcConnect.h"
#include "irods/generalAdmin.h"

#include <string>

#define NFIELDS                 4       // number of fields in HostControlAccess file: <user> <group> <IP address> <subnet mask>
#define MAXLEN                  100
#define MAXSTR                  30
#define MAXLIST                 40      // max number of entries in the access list tab.

#define MON_PERF_SCRIPT         "irodsServerMonPerf"
#define NRESULT                 7                                   // number of metrics returned by MON_PERF_SCRIPT.
#define MAX_VALUE               512                                 // for array definition.
#define MAX_MESSAGE_SIZE        2000
#define MAX_NSERVERS            512                                 // max number of servers that can be monitored (load balancing).
#define TIMEOUT                 20                                  // number of seconds after which the request (the thread taking care of it) for server load is canceled.
#define MON_OUTPUT_NO_ANSWER    "#-1#-1#-1#-1#-1#-1#-1#-1#"         // used if no monitoring output from remote server.
#define LEN_SECONDS             4                                   // length in bytes for the encoding of number of seconds.

/// Describes the inputs used by a monitoring worker thread.
struct ThreadInput
{
    char cmd[LONG_NAME_LEN]; ///< Command to execute.
    char cmdArgv[HUGE_NAME_LEN]; ///< Command arguments buffer.
    char execAddr[LONG_NAME_LEN]; ///< Host address used for execution.
    char hintPath[MAX_NAME_LEN]; ///< Optional path hint for the command.
    int threadId; ///< Identifier of the worker thread.
    int addPathToArgv; ///< Non-zero if `hintPath` should be appended to the arguments.
    char rescName[MAX_NAME_LEN]; ///< Resource name associated with the request.
    ruleExecInfo_t rei; ///< Rule execution context copied into the worker.
};

/// Alias for `ThreadInput`.
using thrInp_t  = ThreadInput;

/// Stores resource metadata returned by monitoring helpers.
struct ResourceInfo
{
    char serverName[LONG_NAME_LEN]; ///< Host serving the resource.
    char rescName[MAX_NAME_LEN]; ///< Resource name.
    char rescType[LONG_NAME_LEN]; ///< Resource type.
    char vaultPath[LONG_NAME_LEN]; ///< Vault path for the resource.
};

/// Alias for `ResourceInfo`.
using monInfo_t = ResourceInfo;

/// Checks whether a user is allowed to access a host.
int checkHostAccessControl(const std::string& _user_name,
                           const std::string& _client_host,
                           const std::string& _groups_name);

/// Microservice wrapper for host access control checks.
int msiCheckHostAccessControl(ruleExecInfo_t* rei);

#endif // IRODS_RE_IN2P3_SYS_RULE_HPP
