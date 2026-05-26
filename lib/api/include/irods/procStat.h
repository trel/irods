#ifndef PROC_STAT_H__
#define PROC_STAT_H__

#include "irods/rcConnect.h"
#include "irods/objInfo.h"
#include "irods/rodsGenQuery.h"

/** Attribute index for the process identifier column in process status output. */
#define PID_INX                 1000001
/** Attribute index for the process start time column in process status output. */
#define STARTTIME_INX           1000002
/** Attribute index for the client user name column in process status output. */
#define CLIENT_NAME_INX         1000003
/** Attribute index for the client zone column in process status output. */
#define CLIENT_ZONE_INX         1000004
/** Attribute index for the proxy user name column in process status output. */
#define PROXY_NAME_INX          1000005
/** Attribute index for the proxy zone column in process status output. */
#define PROXY_ZONE_INX          1000006
/** Attribute index for the remote address column in process status output. */
#define REMOTE_ADDR_INX         1000007
/** Attribute index for the program name column in process status output. */
#define PROG_NAME_INX           1000008
/** Attribute index for the server address column in process status output. */
#define SERVER_ADDR_INX         1000009

/** Input for querying server process status information. */
typedef struct {
    char addr[LONG_NAME_LEN]; ///< Server address to query when targeting a specific host.
    char rodsZone[NAME_LEN]; ///< Zone containing the target server.
    keyValPair_t condInput; ///< Conditional input keywords for the process status query.
} procStatInp_t;

/** Packing instruction string for ::procStatInp_t. */
#define ProcStatInp_PI "str addr[LONG_NAME_LEN];str rodsZone[NAME_LEN];struct KeyValPair_PI;"

/** Maximum number of process status rows returned by the API. */
#define MAX_PROC_STAT_CNT      2000

#ifdef __cplusplus
extern "C"
#endif
/**
 * Query server process status information.
 *
 * @param[in] conn Client connection handle.
 * @param[in] procStatInp Query input describing the target server and options.
 * @param[out] procStatOut Query output rows describing matching processes.
 *
 * @return Operation status.
 */
int rcProcStat( rcComm_t *conn, procStatInp_t *procStatInp, genQueryOut_t **procStatOut );

#endif
