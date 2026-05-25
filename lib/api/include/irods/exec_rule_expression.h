#ifndef EXEC_RULE_EXPRESSION_H
#define EXEC_RULE_EXPRESSION_H

#include "irods/rodsDef.h"

struct RcComm;
struct MsParamArray;

/// \brief Input for executing an inline rule expression.
typedef struct ExecRuleExpression {
    bytesBuf_t           rule_text_; ///< Serialized rule text to execute.
    bytesBuf_t           packed_rei_; ///< Packed rule execution information.
    struct MsParamArray* params_; ///< Input parameters passed to the rule expression.
} exec_rule_expression_t;

#define ExecRuleExpression_PI "struct BytesBuf_PI; struct BytesBuf_PI; struct *MsParamArray_PI;"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/// Execute an inline rule expression.
int rcExecRuleExpression(struct RcComm* _comm, struct ExecRuleExpression* _input);

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

#endif // EXEC_RULE_EXPRESSION_H
