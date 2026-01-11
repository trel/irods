/* For copyright information please refer to files in the COPYRIGHT directory
 */

#include "irods/private/re/deprecation.hpp"
#include "irods/rodsErrorTable.h"
#include <cstdio>
#include <cstring>

/**
 * Legacy system functions that are deprecated
 */
static const DeprecationInfo legacyFunctions[] = {
    {
        .name = "forExec",
        .since_version = "1.0",
        .removal_version = "3.0",
        .alternative = "Use modern 'for' loop: for (init; cond; update) { ... }",
        .reason = "Old-style imperative loop replaced with modern syntax",
        .level = DEPRECATION_WARNING
    },
    {
        .name = "whileExec",
        .since_version = "1.0",
        .removal_version = "3.0",
        .alternative = "Use modern 'while' loop: while (condition) { ... }",
        .reason = "Old-style imperative loop replaced with modern syntax",
        .level = DEPRECATION_WARNING
    },
    {
        .name = "forEachExec",
        .since_version = "1.0",
        .removal_version = "3.0",
        .alternative = "Use modern 'foreach' loop: foreach (var in collection) { ... }",
        .reason = "Old-style iterative loop replaced with modern syntax",
        .level = DEPRECATION_WARNING
    },
    {
        .name = "delayExec",
        .since_version = "1.0",
        .removal_version = "3.0",
        .alternative = "Use modern 'delay' construct: delay(...) { ... }",
        .reason = "Legacy function-based syntax replaced with structured construct",
        .level = DEPRECATION_WARNING
    },
    {
        .name = "remoteExec",
        .since_version = "1.0",
        .removal_version = "3.0",
        .alternative = "Use modern 'remote' construct: remote(host, user) { ... }",
        .reason = "Legacy function-based syntax replaced with structured construct",
        .level = DEPRECATION_WARNING
    },
    {
        .name = "assign",
        .since_version = "1.0",
        .removal_version = "3.0",
        .alternative = "Use direct assignment: variable = value",
        .reason = "Legacy explicit assignment function replaced with operators",
        .level = DEPRECATION_WARNING
    },
    {
        .name = "ifExec",
        .since_version = "1.0",
        .removal_version = "3.0",
        .alternative = "Use modern 'if' construct: if (condition) then { ... } else { ... }",
        .reason = "Old-style conditional replaced with modern syntax",
        .level = DEPRECATION_WARNING
    },
};

static const int numLegacyFunctions = sizeof(legacyFunctions) / sizeof(legacyFunctions[0]);

/**
 * Check if a function name is a legacy deprecated system function
 */
int isLegacySystemFunction(const char *func_name)
{
    if (!func_name) {
        return 0;
    }

    for (int i = 0; i < numLegacyFunctions; i++) {
        if (strcmp(legacyFunctions[i].name, func_name) == 0) {
            return 1;
        }
    }

    return 0;
}

/**
 * Get deprecation info for a legacy system function
 */
const DeprecationInfo *getLegacyFunctionDeprecationInfo(const char *func_name)
{
    if (!func_name) {
        return NULL;
    }

    for (int i = 0; i < numLegacyFunctions; i++) {
        if (strcmp(legacyFunctions[i].name, func_name) == 0) {
            return &legacyFunctions[i];
        }
    }

    return NULL;
}
