#ifndef IRODS_RULE_EXEC_DEL_H
#define IRODS_RULE_EXEC_DEL_H

#include "irods/rodsDef.h"

struct RcComm;

/** Input for deleting a delayed rule execution entry. */
typedef struct RuleExecDeleteInput // NOLINT(modernize-use-using)
{
    /** Identifier of the rule execution entry to delete. */
    char ruleExecId[NAME_LEN]; // NOLINT(modernize-avoid-c-arrays, cppcoreguidelines-avoid-c-arrays)
} ruleExecDelInp_t;

/** Packing instruction string for ::ruleExecDelInp_t. */
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define RULE_EXEC_DEL_INP_PI "str ruleExecId[NAME_LEN];"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Delete a delayed rule execution entry.
 *
 * @param[in] conn Client connection handle.
 * @param[in] ruleExecDelInp Delete request containing the rule execution id.
 *
 * @return Operation status.
 */
// NOLINTNEXTLINE(modernize-use-trailing-return-type)
int rcRuleExecDel(struct RcComm* conn, ruleExecDelInp_t* ruleExecDelInp);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // IRODS_RULE_EXEC_DEL_H
