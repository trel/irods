#ifndef RULE_EXEC_MOD_H__
#define RULE_EXEC_MOD_H__

#include "irods/rodsDef.h"
#include "irods/objInfo.h"

struct RcComm;

/** Input for modifying a delayed rule execution entry. */
typedef struct RuleExecModifyInput {
    /** Identifier of the rule execution entry to modify. */
    char ruleId[NAME_LEN];
    /** Conditional input keywords describing the requested modifications. */
    keyValPair_t condInput;
} ruleExecModInp_t;

/** Packing instruction string for ::ruleExecModInp_t. */
#define RULE_EXEC_MOD_INP_PI "str ruleId[NAME_LEN];struct KeyValPair_PI;"

#ifdef __cplusplus
extern "C"
#endif
/**
 * Modify a delayed rule execution entry.
 *
 * @param[in] _comm Client connection handle.
 * @param[in] _ruleExecModInp Modify request containing the target rule id and updates.
 *
 * @return Operation status.
 */
int rcRuleExecMod(struct RcComm* _comm, struct RuleExecModifyInput* _ruleExecModInp);

#endif // RULE_EXEC_MOD_H__
