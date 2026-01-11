/* For copyright information please refer to files in the COPYRIGHT directory
 */

#ifndef DEPRECATION_HPP
#define DEPRECATION_HPP

#include "irods/private/re/restructs.hpp"
#include "irods/private/re/parser.hpp"
#include "irods/rcMisc.h"

/**
 * Deprecation warning levels
 */
typedef enum {
    DEPRECATION_NOTICE = 0,    /* Just log it */
    DEPRECATION_WARNING = 1,   /* Add to error stream but don't fail parse */
    DEPRECATION_ERROR = 2      /* Treat as error if strict mode enabled */
} DeprecationLevel;

/**
 * Deprecation information for a construct
 */
typedef struct {
    const char *name;           /* Construct name (e.g., "ifExec") */
    const char *since_version;  /* Version deprecated since (e.g., "2.0") */
    const char *removal_version;/* Expected removal version (e.g., "3.0") */
    const char *alternative;    /* Suggested modern alternative */
    const char *reason;         /* Why it's deprecated */
    DeprecationLevel level;     /* How to report it */
} DeprecationInfo;

/**
 * Check if a function is a legacy system function
 * 
 * @param func_name Name of the function to check
 * @return 1 if function is a legacy deprecated system function, 0 otherwise
 */
int isLegacySystemFunction(const char *func_name);

/**
 * Get deprecation information for a legacy system function
 * 
 * @param func_name Name of the function
 * @return DeprecationInfo if function is deprecated, NULL otherwise
 */
const DeprecationInfo *getLegacyFunctionDeprecationInfo(const char *func_name);

#endif /* DEPRECATION_HPP */
