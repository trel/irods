#ifndef RULE_EXEC_SUBMIT_H__
#define RULE_EXEC_SUBMIT_H__

/// \file
/// \brief Declares the rule execution submission API.

#include "irods/objInfo.h"

struct RcComm;
struct BytesBuf;

/// Execution status indicating that a rule is currently running.
#define RE_RUNNING              "RE_RUNNING"
/// Execution status indicating that a rule is queued for execution.
#define RE_IN_QUEUE             "RE_IN_QUEUE"
/// Execution status indicating that a rule execution failed.
#define RE_FAILED               "RE_FAILED"

/// Includes failed rule executions when querying queued executions.
#define RE_FAILED_STATUS        0x1     // run the RE_FAILED too

/// Default buffer size used for packed rule execution state.
#define REI_BUF_LEN             (100 * 1024)

/// Base file name used for serialized rule execution input.
#define REI_FILE_NAME           "rei"
/// Default user name recorded for packed rule execution input.
#define DEF_REI_USER_NAME       "systemUser"
/// Directory name used for packed rule execution input.
#define PACKED_REI_DIR          "packedRei"

/// Input for submitting a delayed rule execution request.
typedef struct RuleExecSubmitInput {
    char ruleName[META_STR_LEN];              ///< Rule or policy name to execute.
    char reiFilePath[MAX_NAME_LEN];           ///< Path to the serialized rule execution input.
    char userName[NAME_LEN];                  ///< User associated with the execution request.
    char exeAddress[NAME_LEN];                ///< Host address where the rule should run.
    char exeTime[TIME_LEN];                   ///< Scheduled execution time.
    char exeFrequency[NAME_LEN];              ///< Recurrence interval for repeated execution.
    char priority[NAME_LEN];                  ///< Queue priority for the execution request.
    char lastExecTime[NAME_LEN];              ///< Most recent execution time for recurring rules.
    char exeStatus[NAME_LEN];                 ///< Current execution status string.
    char estimateExeTime[NAME_LEN];           ///< Estimated execution duration.
    char notificationAddr[NAME_LEN];          ///< Notification target for execution updates.
    keyValPair_t condInput;                   ///< Additional keyword options.
    struct BytesBuf* packedReiAndArgBBuf;     ///< Packed rule execution input and arguments.
    char ruleExecId[NAME_LEN];                ///< Output identifier assigned to the submitted execution.
} ruleExecSubmitInp_t;

/// Packing instruction for `ruleExecSubmitInp_t`.
#define RULE_EXEC_SUBMIT_INP_PI "str ruleName[META_STR_LEN]; str reiFilePath[MAX_NAME_LEN]; str userName[NAME_LEN]; str exeAddress[NAME_LEN]; str exeTime[TIME_LEN]; str exeFrequency[NAME_LEN]; str priority[NAME_LEN]; str lastExecTime[NAME_LEN]; str exeStatus[NAME_LEN]; str estimateExeTime[NAME_LEN]; str notificationAddr[NAME_LEN]; struct KeyValPair_PI; struct *BytesBuf_PI; str ruleExecId[NAME_LEN];"

#ifdef __cplusplus
extern "C" {
#endif

/// Submits a delayed rule execution request and returns its identifier.
int rcRuleExecSubmit(struct RcComm* conn, ruleExecSubmitInp_t* ruleExecSubmitInp, char** ruleExecId);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // RULE_EXEC_SUBMIT_H_
