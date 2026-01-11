/**
 * @file test_union_constraint_solving_stubs.cpp
 * @brief Minimal stubs for union constraint solving test dependencies
 *
 * Provides stub implementations of symbols needed by the rule engine
 * type system that are not required by the union constraint solving test.
 */

#include "irods/private/re/restructs.hpp"
#include "irods/private/re/typing.hpp"
#include "irods/private/re/configuration.hpp"
#include "irods/region.h"
#include <cstring>

// Stub for ruleEngineConfig global
Cache ruleEngineConfig = {};

// Stub for typeName_ExprType
char *typeName_ExprType(node *exprType) {
    if (exprType == NULL) return (char*)"null";
    switch (getNodeType(exprType)) {
        case T_INT: return (char*)"int";
        case T_DOUBLE: return (char*)"double";
        case T_STRING: return (char*)"string";
        case T_BOOL: return (char*)"bool";
        case T_DYNAMIC: return (char*)"dynamic";
        case T_TUPLE: return (char*)"tuple";
        case T_CONS: return (char*)"collection";
        case T_VAR: return (char*)"union";
        default: return (char*)"unknown";
    }
}

// Stub for printHashtable
void printHashtable(hashtable *ht, char *prefix) {
    // Empty stub - not needed for test
}

// Stub for parseFuncTypeFromString
node *parseFuncTypeFromString(char *str, Region *r) {
    // Return a dummy function type
    return newSimpType(T_DYNAMIC, r);
}

// Stub for generateErrMsg
char *generateErrMsg(char *msg, long errloc, char *ruleBaseName, char errbuf[ERR_MSG_LEN]) {
    if (errbuf) {
        snprintf(errbuf, ERR_MSG_LEN, "Error: %s", msg);
    }
    return errbuf;
}

// Stub for isLegacySystemFunction
int isLegacySystemFunction(const char *funcName) {
    return 0;  // Not a legacy function
}

// Stub for getLegacyFunctionDeprecationInfo
const char *getLegacyFunctionDeprecationInfo(const char *funcName) {
    return NULL;  // No deprecation info
}

// Stub for readRuleSetFromFile
int readRuleSetFromFile(const char *filename, ruleSet *rset, env *e, int *errorCode,
                        ErrorStack *errs, Region *r) {
    return 0;
}

// Stub for convertResToString
char *convertResToString(node *resNode) {
    return (char*)"";
}

// Stub for getRuleBasePath
int getRuleBasePath(const char *name, char *buf) {
    strcpy(buf, "/etc/irods");
    return 0;
}
