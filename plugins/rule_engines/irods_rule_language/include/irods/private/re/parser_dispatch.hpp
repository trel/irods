/* For copyright information please refer to files in the COPYRIGHT directory
 */

#ifndef PARSER_DISPATCH_HPP
#define PARSER_DISPATCH_HPP

/*
 * Parser dispatch module - routes between modern and legacy syntax
 * 
 * The iRODS rule language supports two syntax versions:
 * - Modern (v2.0+): Strict typing, quoted strings, modern operators
 * - Legacy (v1.x): Implicit types, unquoted strings, legacy functions
 * 
 * The backwardCompatible flag controls which parser is used:
 * - backwardCompatible == 1: Always use legacy parser
 * - backwardCompatible == -1: Always use modern parser  
 * - backwardCompatible == 0: Auto-detect (attempt modern, fall back to legacy)
 */

#include "irods/private/re/parser.hpp"

/**
 * Dispatch expression parsing based on backward compatibility mode
 * 
 * In legacy mode, expressions support implicit type conversions.
 * In modern mode, types must be explicit.
 */
#define DISPATCH_EXPR( level ) \
    if ( context->backwardCompatible > 0 ) { \
        nextRuleGenExprBackwardCompatible(e, context, (level)); \
    } else { \
        nextRuleGenExpr(e, context, (level)); \
    }

/**
 * Dispatch term parsing based on backward compatibility mode
 * 
 * In legacy mode, supports legacy system functions (ifExec, whileExec, etc.)
 * In modern mode, uses modern syntax (if, while, etc.)
 */
#define DISPATCH_TERM( rulegen, prec ) \
    if ( context->backwardCompatible > 0 ) { \
        nextRuleGenTermBackwardCompatible(e, context); \
    } else { \
        nextRuleGenTerm(e, context, (rulegen), (prec)); \
    }

/**
 * Dispatch system function parsing
 * 
 * Legacy: ifExec, whileExec, forEachExec, assign, forExec, delayExec, remoteExec
 * Modern: if, while, foreach, let, for, delay, remote
 */
#define DISPATCH_SYSTEM_FUNC( level ) \
    if ( context->backwardCompatible > 0 ) { \
        nextRuleGenTermSystemBackwardCompatible(e, context, (level)); \
    } else { \
        nextRuleGenTermSystem(e, context, (level)); \
    }

/**
 * Dispatch action argument parsing
 * 
 * Legacy: Allows unquoted strings
 * Modern: Requires quoted strings or variables
 */
#define DISPATCH_ACTION_ARG() \
    if ( context->backwardCompatible > 0 ) { \
        nextRuleGenActionArgumentBackwardCompatible(e, context); \
    } else { \
        nextRuleGenActionArgument(e, context); \
    }

#endif
