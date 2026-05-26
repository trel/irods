#ifndef EXEC_MY_RULE_H__
#define EXEC_MY_RULE_H__

#include "irods/rodsDef.h"
#include "irods/objInfo.h"
#include "irods/msParam.h"
#include "irods/rcConnect.h"

/** Input for executing a user-supplied rule. */
typedef struct ExecMyRuleInp {
    char myRule[META_STR_LEN]; ///< Rule text to execute.
    rodsHostAddr_t addr; ///< Host address of the target rule engine.
    keyValPair_t condInput; ///< Conditional input keywords for the request.
    char outParamDesc[LONG_NAME_LEN];  ///< Output labels separated by "%".
    msParamArray_t *inpParamArray; ///< Input parameter array passed to the rule.
} execMyRuleInp_t;
#define ExecMyRuleInp_PI "str myRule[META_STR_LEN]; struct RHostAddr_PI; struct KeyValPair_PI; str outParamDesc[LONG_NAME_LEN]; struct *MsParamArray_PI;"


#ifdef __cplusplus
extern "C"
#endif
/**
 * Execute a user-supplied rule.
 *
 * @param[in] conn Client connection handle.
 * @param[in] execMyRuleInp Rule execution request.
 * @param[out] outParamArray Output parameter array produced by the rule.
 *
 * @return Operation status.
 */
int rcExecMyRule( rcComm_t *conn, execMyRuleInp_t *execMyRuleInp, msParamArray_t **outParamArray );

#endif
