/**
 * @file test_attr_typing_stubs.cpp
 * @brief Stub implementations for attr_typing test external dependencies
 */

#include "irods/private/re/restructs.hpp"
#include "irods/private/re/parser.hpp"
#include "irods/private/re/typing.hpp"
#include "irods/rodsErrorTable.h"
#include <cstring>

// ============================================================================
// Configuration stubs needed for linking
// ============================================================================

// Global rule engine config (used by utils.cpp)
struct RuleEngineConfig {
    // Minimal dummy config
} ruleEngineConfig;

// ============================================================================
// Hashtable function stub
// ============================================================================

void printHashtable(Hashtable *, char *) {
    // No-op stub
}

// ============================================================================
// Parser stubs - needed because typing.cpp includes parser functionality
// ============================================================================

int isLegacySystemFunction(const char *name) {
    return 0;  // Not a legacy function
}

void getLegacyFunctionDeprecationInfo(const char *name) {
    // No-op stub - called by typing.cpp
}

char *convertResToString(Node *res) {
    static char buf[256] = "";
    return buf;
}

char *getRuleBasePath(const char *ruleBaseName, char *path) {
    if (path) {
        snprintf(path, MAX_NAME_LEN, "/tmp");
    }
    return path;
}

// ============================================================================
// Rule reading stub
// ============================================================================

int readRuleSetFromFile(const char *ruleSetPath, RuleSet *ruleSet, Env *env, int *errloc, ErrorStack *errloc_stack, Region *r) {
    // No-op stub - return success
    return 0;
}
