#ifndef SPECIFIC_QUERY_H__
#define SPECIFIC_QUERY_H__

#include "irods/rcConnect.h"
#include "irods/objInfo.h"
#include "irods/rodsGenQuery.h"

/** Input for executing a registered specific query. */
typedef struct SpecificQueryInp {
    char *sql; ///< Registered SQL alias or statement to execute.
    char *args[10]; ///< Optional bind variable arguments for the query.

    int maxRows; ///< Maximum number of rows to return for this request.
    int continueInx; ///< Continuation index used to fetch additional rows.
    int rowOffset; ///< Row offset for the query request.
    int options; ///< Bitmask of query options.
    keyValPair_t condInput; ///< Conditional input keywords for the specific query.
} specificQueryInp_t;
/** Packing instruction string for ::specificQueryInp_t. */
#define specificQueryInp_PI "str *sql; str *arg1; str *arg2; str *arg3; str *arg4; str *arg5; str *arg6; str *arg7; str *arg8; str *arg9; str *arg10; int maxRows; int continueInx; int rowOffset; int options; struct KeyValPair_PI;"

#ifdef __cplusplus
extern "C"
#endif
/**
 * Execute a registered specific query.
 *
 * @param[in] conn Client connection handle.
 * @param[in] specificQueryInp Query request input.
 * @param[out] genQueryOut Query result rows.
 *
 * @return Operation status.
 */
int rcSpecificQuery( rcComm_t *conn, specificQueryInp_t *specificQueryInp, genQueryOut_t **genQueryOut );

#endif
