/* For copyright information please refer to files in the COPYRIGHT directory
 */

#ifndef FUNCTIONS_HPP
#define	FUNCTIONS_HPP

#include "irods/irods_hashtable.h"
#include "irods/region.h"
#include "irods/private/re/parser.hpp"
#include "irods/private/re/debug.hpp"

/// Carries state for iterating over a rule language value.
typedef struct reIterableData {
    char *varName; ///< Name of the loop variable.
    Res *res; ///< Iterable value being traversed.
    void *itrSpecData; ///< Iterator-specific implementation state.
    Res *errorRes; ///< Error result to report on iteration failure.
    Node **subtrees; ///< AST subtrees associated with the iteration body.
    Node *node; ///< AST node being evaluated.
    ruleExecInfo_t *rei; ///< Active rule execution context.
    int reiSaveFlag; ///< Indicates whether REI state must be preserved.
    Env *env; ///< Evaluation environment.
    rError_t* errmsg; ///< Error stack for evaluation diagnostics.
} ReIterableData;

/// Identifies the supported iterable backing types.
typedef enum reIterableType {
    RE_ITERABLE_COMMA_STRING, ///< Comma-delimited string values.
    RE_ITERABLE_STRING_ARRAY, ///< String array values.
    RE_ITERABLE_INT_ARRAY, ///< Integer array values.
    RE_ITERABLE_LIST, ///< Rule language list values.
    RE_ITERABLE_COLLECTION, ///< iRODS collection values.
    RE_ITERABLE_GEN_QUERY, ///< General query input values.
    RE_ITERABLE_GEN_QUERY_OUT, ///< General query output values.
    RE_ITERABLE_KEY_VALUE_PAIRS, ///< Key-value pair values.
    RE_NOT_ITERABLE, ///< Value is not iterable.
} ReIterableType;

/// Defines the iterator operations for one iterable type.
typedef struct reIterable {
    void ( *init )( ReIterableData *itrSpecData, Region *r ); ///< Initializes iterator state.
    int ( *hasNext )( ReIterableData *itrSpecData, Region *r ); ///< Returns non-zero if another value exists.
    Res *( *next )( ReIterableData *itrSpecData, Region *r ); ///< Returns the next value.
    void ( *finalize )( ReIterableData *itrSpecData, Region *r ); ///< Releases iterator state.
} ReIterable;

/// Associates an iterable type with its iterator implementation.
typedef struct reIterableTableRow {
    ReIterableType nodeType; ///< Iterable type key.
    ReIterable reIterable; ///< Iterator operations for the type.
} ReIterableTableRow;

/// Allocates and initializes iteration state for a rule language loop.
ReIterableData *newReIterableData(
    char *varName,
    Res *res,
    Node **subtrees,
    Node *node,
    ruleExecInfo_t *rei,
    int reiSaveFlag,
    Env *env,
    rError_t* errmsg
);
/// Releases a `ReIterableData` instance.
void deleteReIterableData( ReIterableData *itrData );

/// Registers built-in functions in a function table.
void getSystemFunctions( Hashtable *ft, Region *r );

/// Evaluates a rule language expression.
Res* eval( char *expr, Env *env, ruleExecInfo_t *rei, int saveREI, rError_t *errmsg, Region *r );

/// Constructs a node representing a constructor application.
Node *construct( char *fn, Node **args, int argc, Node* constype, Region *r );
/// Deconstructs a node using the requested projection.
Node *deconstruct( Node **args, int proj );
/// Converts a wildcard pattern into an anchored match string.
char* matchWholeString( char *buf );
/// Converts a wildcard pattern into a regular expression.
char *wildCardToRegex( char *buf );

#endif	/* FUNCTIONS_H */
