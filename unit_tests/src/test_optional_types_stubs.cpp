/**
 * @file test_optional_types_stubs.cpp
 * @brief Stub implementations for optional_types test external dependencies
 */

#include "irods/private/re/restructs.hpp"
#include "irods/rodsErrorTable.h"
#include <cstring>
#include <cstdlib>

// ============================================================================
// String utility stubs (used by restructs.cpp)
// ============================================================================

char *cpString(char *str, region *r) {
    if (!str) return NULL;
    size_t len = strlen(str) + 1;
    char *copy = (char *)malloc(len);
    if (copy) strcpy(copy, str);
    return copy;
}

char *cpStringExt(const char *str, region *r) {
    if (!str) return NULL;
    size_t len = strlen(str) + 1;
    char *copy = (char *)malloc(len);
    if (copy) strcpy(copy, str);
    return copy;
}

// ============================================================================
// Type variable stub (used by restructs.cpp)
// ============================================================================

static int nextTVarId = 0;

int newTVarId() {
    return nextTVarId++;
}

// ============================================================================
// Parser stubs
// ============================================================================

node *parseFuncTypeFromString(char *str, region *r) {
    // Return NULL - not used in this test
    return NULL;
}

// ============================================================================
// Deprecation stubs
// ============================================================================

int isLegacySystemFunction(const char *name) {
    return 0;  // Not a legacy function
}

void getLegacyFunctionDeprecationInfo(const char *name) {
    // No-op stub
}
