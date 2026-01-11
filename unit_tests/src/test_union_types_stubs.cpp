/**
 * @file test_union_types_stubs.cpp
 * @brief Stub implementations for test_union_types
 */

#include "irods/private/re/restructs.hpp"
#include "irods/private/re/typing.hpp"
#include "irods/private/re/utils.hpp"
#include "irods/private/re/configuration.hpp"
#include "irods/rcMisc.h"
#include <cstring>
#include <cstdlib>

/**
 * Global rule engine configuration (stub)
 * This is an external variable from configuration.hpp
 */
Cache ruleEngineConfig;

/**
 * Stub: Get type name from ExprType
 */
char *typeName_ExprType(ExprType *n)
{
    static char buf[256];
    
    if (!n) {
        return (char*)"null";
    }

    switch (getNodeType(n)) {
        case T_INT:
            return (char*)"int";
        case T_DOUBLE:
            return (char*)"double";
        case T_STRING:
            return (char*)"string";
        case T_BOOL:
            return (char*)"bool";
        case T_DYNAMIC:
            return (char*)"dynamic";
        case T_VAR:
            snprintf(buf, sizeof(buf), "?%u", T_VAR_ID(n));
            return buf;
        case T_IRODS:
            return (char*)"irods";
        default:
            return (char*)"unknown";
    }
}

/**
 * Stub: Print hashtable
 */
void printHashtable(hashtable *h, char *prefix)
{
    // Stub - no-op
}

/**
 * Stub: Parse function type from string
 */
ExprType *parseFuncTypeFromString(char *string, Region *r)
{
    // Return a simple dynamic type for stubbed parsing
    if (!string || !r) {
        return NULL;
    }
    return newSimpType(T_DYNAMIC, r);
}

/**
 * Stub: Check if function is legacy
 */
int isLegacySystemFunction(const char *funcName)
{
    return 0; // Not legacy
}

// Forward declaration for DeprecationInfo
struct DeprecationInfo;

/**
 * Stub: Get legacy function deprecation info
 */
const struct DeprecationInfo *getLegacyFunctionDeprecationInfo(const char *funcName)
{
    return NULL; // No deprecation info
}

/**
 * Stub: Generate error message
 */
char *generateErrMsg(char *msg, long errloc, char* ruleBaseName, char errbuf[ERR_MSG_LEN])
{
    if (errbuf && msg) {
        snprintf(errbuf, ERR_MSG_LEN, "%s", msg);
    }
    return errbuf;
}
