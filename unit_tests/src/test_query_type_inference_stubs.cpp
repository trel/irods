/**
 * @file test_query_type_inference_stubs.cpp
 * @brief Minimal stubs for query type inference test dependencies
 *
 * Provides stub implementations of symbols needed by the rule engine
 * type system that are not required by the query type inference test.
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

// Stub for getColumnTypeFromSchema
ExprType *getColumnTypeFromSchema(const char *columnName, region *r) {
    if (!columnName) {
        // Return T_DYNAMIC for NULL input
        return newSimpType(T_DYNAMIC, r);
    }
    
    // Map known iRODS columns to their types
    if (strcmp(columnName, "USER_ID") == 0 ||
        strcmp(columnName, "COLL_ID") == 0 ||
        strcmp(columnName, "DATA_ID") == 0 ||
        strcmp(columnName, "DATA_SIZE") == 0 ||
        strcmp(columnName, "DATA_CREATE_TIME") == 0 ||
        strcmp(columnName, "DATA_MODIFY_TIME") == 0 ||
        strcmp(columnName, "RESC_ID") == 0 ||
        strcmp(columnName, "ZONE_ID") == 0) {
        return newSimpType(T_INT, r);
    }
    
    if (strcmp(columnName, "COLL_NAME") == 0 ||
        strcmp(columnName, "DATA_NAME") == 0 ||
        strcmp(columnName, "USER_NAME") == 0 ||
        strcmp(columnName, "RESC_NAME") == 0 ||
        strcmp(columnName, "DATA_ZONE_NAME") == 0) {
        return newSimpType(T_STRING, r);
    }
    
    // Unknown column returns T_DYNAMIC (base function)
    return newSimpType(T_DYNAMIC, r);
}

// Stub for createTupleTypeFromColumns
ExprType *createTupleTypeFromColumns(List *columnNames, region *r) {
    if (!columnNames || columnNames->size == 0) {
        return NULL;
    }
    
    // Create array of types for tuple
    node **subtrees = (node**)region_alloc(r, columnNames->size * sizeof(node*));
    int idx = 0;
    
    // Iterate through column names and get their types
    ListNode *ln = columnNames->head;
    while (ln) {
        const char *colName = (const char *)ln->value;
        ExprType *colType = getColumnTypeFromSchema(colName, r);
        
        // If column is unknown (T_DYNAMIC), wrap it in a union type
        if (colType && getNodeType(colType) == T_DYNAMIC) {
            node **unionSubtrees = (node**)region_alloc(r, 3 * sizeof(node*));
            unionSubtrees[0] = newSimpType(T_INT, r);
            unionSubtrees[1] = newSimpType(T_STRING, r);
            unionSubtrees[2] = newSimpType(T_DYNAMIC, r);
            subtrees[idx++] = newTVar2(3, unionSubtrees, r);
        } else {
            subtrees[idx++] = colType;
        }
        ln = ln->next;
    }
    
    // Create tuple type with these subtrees
    return newTupleType(columnNames->size, subtrees, r);
}

// Stub for newTVarId
static int _tVarCounter = 0;
int newTVarId() {
    return ++_tVarCounter;
}

// Stub for cpStringExt
char *cpStringExt(const char *str, region *r) {
    if (!str) return NULL;
    size_t len = strlen(str) + 1;
    char *copy = (char*)region_alloc(r, len);
    strcpy(copy, str);
    return copy;
}

// Stub for cpString
char *cpString(char *str, region *r) {
    if (!str) return NULL;
    size_t len = strlen(str) + 1;
    char *copy = (char*)region_alloc(r, len);
    strcpy(copy, str);
    return copy;
}

// Stub for getUnionTypeName
char *getUnionTypeName(node *unionNode, char *buf) {
    if (!buf) return NULL;
    if (!unionNode || getNodeType(unionNode) != T_VAR) {
        strcpy(buf, "unknown");
        return buf;
    }
    
    // Build a string with the union type names
    strcpy(buf, "union<");
    int numDisjuncts = T_VAR_NUM_DISJUNCTS(unionNode);
    for (int i = 0; i < numDisjuncts; i++) {
        if (i > 0) strcat(buf, "|");
        
        node *disjunct = T_VAR_DISJUNCT(unionNode, i);
        NodeType type = getNodeType(disjunct);
        
        switch (type) {
            case T_INT:
                strcat(buf, "int");
                break;
            case T_STRING:
                strcat(buf, "string");
                break;
            case T_DOUBLE:
                strcat(buf, "double");
                break;
            case T_DYNAMIC:
                strcat(buf, "dynamic");
                break;
            default:
                strcat(buf, "unknown");
                break;
        }
    }
    strcat(buf, ">");
    return buf;
}
