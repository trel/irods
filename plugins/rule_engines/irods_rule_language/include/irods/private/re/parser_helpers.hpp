/* For copyright information please refer to files in the COPYRIGHT directory
 */

#ifndef PARSER_HELPERS_HPP
#define PARSER_HELPERS_HPP

#include "irods/private/re/restructs.hpp"
#include "irods/region.h"

/**
 * Parser Helper Functions: Refactored extraction of parser macros
 * 
 * This module provides explicit functions to replace macro-based parsing operations.
 * Goals:
 * - Improve debuggability (stepping through functions vs macros)
 * - Enable clearer error handling
 * - Support future enhancements like multi-error collection
 * - Maintain backward compatibility with existing parser code
 */

/** Forward declarations */
typedef struct {
    Node *nodeStack[1024];
    int nodeStackTop;
    int stackTopStack[1024];
    int stackTopStackTop;
    int error;
    int prec;
    Node *errnode;
    Label errloc;
    char errmsgbuf[ERR_MSG_LEN];
    Hashtable *symtable;
    rError_t *errmsg;
    Region *region;
    Token tokenQueue[1024];
    int tqp;
    int tqtop;
    int tqbot;
} ParserContext;

/* ============================================================================
 * Stack Operations
 * ============================================================================ */

/**
 * Push a node onto the parser's node stack
 * @param context The parser context
 * @param node The node to push
 */
inline void pushNode(ParserContext *context, Node *node) {
    if (context->nodeStackTop < 1024) {
        context->nodeStack[context->nodeStackTop++] = node;
    }
    /* Stack overflow handled by CASCADE check in caller */
}

/**
 * Pop a node from the parser's node stack
 * @param context The parser context
 * @return The popped node, or NULL if stack is empty
 */
inline Node *popNode(ParserContext *context) {
    if (context->nodeStackTop > 0) {
        return context->nodeStack[--context->nodeStackTop];
    }
    return NULL;
}

/**
 * Peek at the top node without removing it
 * @param context The parser context
 * @return The top node, or NULL if stack is empty
 */
inline Node *peekNode(ParserContext *context) {
    if (context->nodeStackTop > 0) {
        return context->nodeStack[context->nodeStackTop - 1];
    }
    return NULL;
}

/**
 * Peek at node n positions from top (0 = top, 1 = second, etc.)
 * @param context The parser context
 * @param offset Number of positions from top
 * @return The node at offset, or NULL if out of bounds
 */
inline Node *peekNodeAt(ParserContext *context, int offset) {
    if (offset >= 0 && context->nodeStackTop > offset) {
        return context->nodeStack[context->nodeStackTop - 1 - offset];
    }
    return NULL;
}

/**
 * Swap the top two nodes on the stack
 * Equivalent to the old SWAP macro
 * @param context The parser context
 */
inline void swapTopNodes(ParserContext *context) {
    if (context->nodeStackTop >= 2) {
        Node *node1 = context->nodeStack[context->nodeStackTop - 1];
        Node *node2 = context->nodeStack[context->nodeStackTop - 2];
        context->nodeStack[context->nodeStackTop - 1] = node2;
        context->nodeStack[context->nodeStackTop - 2] = node1;
    }
}

/**
 * Pop n items from the stack (cleanup operation)
 * @param context The parser context
 * @param count Number of items to pop
 */
inline void popNodes(ParserContext *context, int count) {
    if (count > 0 && context->nodeStackTop >= count) {
        context->nodeStackTop -= count;
    }
}

/**
 * Get current stack depth (number of items on stack)
 * @param context The parser context
 * @return Number of items currently on stack
 */
inline int stackDepth(ParserContext *context) {
    return context->nodeStackTop;
}

/* ============================================================================
 * Node Construction
 * ============================================================================ */

/**
 * Build a new AST node of the given type
 * This is the core extraction from BUILD_NODE macro
 * 
 * @param type The node type to create
 * @param text The node text content (e.g., operator name, variable name)
 * @param label The source location for error reporting
 * @param degree The number of child nodes
 * @param context The parser context (for memory allocation)
 * @return The newly constructed node, or NULL on allocation failure
 */
Node *buildNode(NodeType type, const char *text, Label *label, int degree, ParserContext *context);

/**
 * Build a node and set its children from the stack
 * This is the combined BUILD_NODE + child assignment operation
 * 
 * @param type The node type to create
 * @param text The node text content
 * @param label The source location
 * @param degree The number of children
 * @param context The parser context
 * @return The constructed node with children set, or NULL on failure
 * 
 * Side effects:
 * - Pops degree items from stack (children are assigned in reverse order)
 * - Pushes the new node onto the stack
 * - Sets context->error if allocation fails
 */
Node *buildNodeWithChildren(NodeType type, const char *text, Label *label, int degree, ParserContext *context);

/**
 * Convenience function: Build a tuple node
 * Equivalent to: BUILD_NODE(N_TUPLE, TUPLE, loc, deg, deg)
 * 
 * @param label The source location
 * @param degree The number of elements in the tuple
 * @param context The parser context
 * @return The new tuple node, or NULL on failure
 * 
 * Side effects: Pops degree items from stack, pushes tuple
 */
Node *buildTupleNode(Label *label, int degree, ParserContext *context);

/**
 * Convenience function: Build an application node
 * Typical pattern: BUILD_APP_NODE(funcName, loc, arity)
 * Builds: (funcName args...) where args are popped from stack
 * 
 * @param funcName The name of the function being applied
 * @param label The source location
 * @param arity The number of arguments
 * @param context The parser context
 * @return The application node, or NULL on failure
 * 
 * Side effects:
 * - Creates a tuple of arguments (arity items from stack)
 * - Creates a text node for funcName
 * - Creates an application node
 * - Pops arity + 1 items from stack, pushes application
 */
Node *buildApplicationNode(const char *funcName, Label *label, int arity, ParserContext *context);

/**
 * Convenience function: Build an error node
 * @param errorMsg The error message
 * @param label The source location
 * @param context The parser context
 * @return An error node (nodeType == N_ERROR)
 */
Node *buildErrorNode(const char *errorMsg, Label *label, ParserContext *context);

/* ============================================================================
 * Error Handling
 * ============================================================================ */

/**
 * Update error location if current location is further along in source
 * @param context The parser context
 * @param label The current source location
 */
void updateErrorLocation(ParserContext *context, Label *label);

/**
 * Mark parser as having encountered an error
 * @param context The parser context
 * @param label The location where error occurred (if NULL, uses current errloc)
 */
void setParserError(ParserContext *context, Label *label);

/**
 * Check if parser is in error state
 * @param context The parser context
 * @return Non-zero if error, zero if no error
 */
int hasParserError(ParserContext *context);

/**
 * Clear parser error state (for error recovery)
 * @param context The parser context
 */
void clearParserError(ParserContext *context);

/* ============================================================================
 * Choice Point Management (for TRY/OR/END_TRY macros)
 * ============================================================================ */

/**
 * Choice point: a parser state savepoint for backtracking
 * Used internally to implement TRY/OR alternatives
 */
typedef struct {
    int nodeStackSavepoint;  /* Saved nodeStackTop */
    int tokenQueuePosition;  /* Saved token queue position */
    Label choiceStart;       /* Source location of choice start */
} ChoicePoint;

/**
 * Save current parser state for a choice point
 * Call this before trying alternatives (equivalent to CHOICE_BEGIN)
 * 
 * @param context The parser context
 * @param choicePoint Output parameter for saved state
 * @param label The source location to save
 */
void saveChoicePoint(ParserContext *context, ChoicePoint *choicePoint, Label *label);

/**
 * Restore parser to a previously saved choice point
 * Call this when starting a new alternative (equivalent to BRANCH_BEGIN)
 * Resets: nodeStackTop, token queue position, error flag
 * 
 * @param context The parser context
 * @param choicePoint The previously saved choice point
 */
void restoreChoicePoint(ParserContext *context, const ChoicePoint *choicePoint);

/**
 * Commit current branch as successful
 * Call this after successfully parsing a branch (equivalent to BRANCH_END when successful)
 * This prevents backtracking to other alternatives
 * 
 * @param context The parser context (error flag should be 0 for success)
 * @return Non-zero if branch was successful, zero if it failed
 */
int commitBranch(ParserContext *context);

/* ============================================================================
 * Explicit Error Propagation
 * 
 * These functions replace the CASCADE macro pattern of implicit error handling.
 * They enable explicit error checking and better error recovery.
 * ============================================================================ */

/**
 * Error result type: tracks both node result and error state
 * Used for functions that may return NULL (error) or a valid node
 */
typedef struct {
    Node *node;           /* The parsed node, or NULL if error */
    int error;            /* Non-zero if error occurred */
    Label errorLocation;  /* Source location where error occurred */
} ParseResult;

/**
 * Create a successful parse result
 * @param node The successfully parsed node (must not be NULL)
 * @return A ParseResult with error=0
 */
ParseResult makeParseResult(Node *node);

/**
 * Create an error parse result
 * @param context The parser context (for error location)
 * @return A ParseResult with error=1 and node=NULL
 */
ParseResult makeParseError(ParserContext *context);

/**
 * Check if a parse result indicates success
 * @param result The parse result to check
 * @return Non-zero if successful, zero if error
 */
int isParseSuccess(ParseResult result);

/**
 * Check if a parse result indicates error
 * @param result The parse result to check
 * @return Non-zero if error, zero if successful
 */
int isParseError(ParseResult result);

/**
 * Unwrap parse result, updating context on error
 * @param result The parse result
 * @param context The parser context (updated if result is error)
 * @return The node if successful, NULL if error
 * 
 * Side effect: If result.error, sets context->error and context->errloc
 */
Node *unwrapParseResult(ParseResult result, ParserContext *context);

/**
 * Report a parser error with context information
 * Adds message to error stack and updates error location
 * 
 * @param context The parser context
 * @param errorCode The error code (e.g., RE_PARSER_ERROR)
 * @param message The error message
 * @param label The source location of the error
 */
void reportParserError(ParserContext *context, int errorCode, const char *message, Label *label);

/**
 * Report a parsing failure for a specific construct
 * Example: "expected '+' but found '-'"
 * 
 * @param context The parser context
 * @param expected Description of what was expected
 * @param got Description of what was found (NULL = unknown)
 * @param label The source location
 */
void reportParseExpectationError(ParserContext *context, const char *expected, const char *got, Label *label);

/* ============================================================================
 * Parser Combinators
 * 
 * Combinator functions compose parser operations into larger parsing strategies.
 * These replace common patterns found in REPEAT, LIST, OPTIONAL, and other macros.
 * ============================================================================ */

/**
 * Sequence: Parse a sequence of items with a delimiter
 * Equivalent to: item (delimiter item)* with optional leading/trailing delimiter
 * 
 * @param context The parser context
 * @param itemParser Function that parses a single item (should leave result on stack)
 * @param delimiter The token text that separates items (e.g., "," or ";")
 * @param allowTrailing If non-zero, allows trailing delimiter before closing bracket
 * @return Number of items parsed, or -1 if error
 * 
 * Usage: Parse "a, b, c" into 3 stack items
 */
typedef int (*ItemParserFunc)(Pointer *expr, ParserContext *context);
int parseDelimitedSequence(Pointer *expr, ParserContext *context, ItemParserFunc itemParser, 
                          const char *delimiter, int allowTrailing);

/**
 * Optional: Try to parse something, but don't fail if it's missing
 * Equivalent to TRY/OR with empty branch
 * 
 * @param context The parser context
 * @param parser The parser function to try
 * @return 1 if parsed successfully, 0 if not present (no error set)
 * 
 * Side effect: Pushes result to stack if successful, does nothing if not present
 */
typedef int (*ParserFunc)(Pointer *expr, ParserContext *context);
int parseOptional(Pointer *expr, ParserContext *context, ParserFunc parser);

/**
 * Repeat: Parse zero or more occurrences of something
 * Equivalent to REPEAT_BEGIN/REPEAT_END macro pattern
 * 
 * @param context The parser context
 * @param parser The parser function to repeat
 * @return Number of items parsed (0 or more), or -1 if error
 * 
 * Side effect: Each successful parse leaves item on stack
 */
int parseRepeat(Pointer *expr, ParserContext *context, ParserFunc parser);

/**
 * Repeat with minimum: Parse at least N occurrences
 * Fails if fewer than minCount items are parsed
 * 
 * @param context The parser context
 * @param parser The parser function to repeat
 * @param minCount Minimum number of items required
 * @return Total number of items parsed, or -1 if error or too few
 */
int parseRepeatMin(Pointer *expr, ParserContext *context, ParserFunc parser, int minCount);

/**
 * Sequence with fixed count: Parse exactly N items
 * Fails if not exactly N items available
 * 
 * @param context The parser context
 * @param parser The parser function for each item
 * @param count Number of items required
 * @return count if successful, -1 if error
 */
int parseExactly(Pointer *expr, ParserContext *context, ParserFunc parser, int count);

/**
 * Choice: Try multiple alternatives in order
 * Uses backtracking to try each alternative
 * 
 * Example:
 *   ParserFunc parsers[] = { parseA, parseB, parseC };
 *   if (parseChoice(expr, context, parsers, 3) > 0) { ... }
 * 
 * @param context The parser context
 * @param parsers Array of parser functions to try
 * @param numChoices Number of alternatives
 * @return Index of successful parser (1-based), 0 if none succeed, -1 if error
 */
int parseChoice(Pointer *expr, ParserContext *context, ParserFunc *parsers, int numChoices);

/* ============================================================================
 * Debug/Tracing Support
 * 
 * These functions enable parser instrumentation for debugging and analysis.
 * Controlled by environment variables or parser context flags.
 * ============================================================================ */

/**
 * Parser trace event type
 */
typedef enum {
    PARSER_TRACE_ENTER,      /* Entering a parser function */
    PARSER_TRACE_EXIT,       /* Exiting a parser function */
    PARSER_TRACE_SUCCESS,    /* Parser succeeded */
    PARSER_TRACE_FAILURE,    /* Parser failed */
    PARSER_TRACE_ERROR,      /* Parser error occurred */
    PARSER_TRACE_BACKTRACK,  /* Backtracking to choice point */
    PARSER_TRACE_TOKEN,      /* Token consumed */
    PARSER_TRACE_NODE,       /* Node created */
    PARSER_TRACE_STACK,      /* Stack operation */
} ParserTraceEvent;

/**
 * Initialize parser tracing
 * Call this once at parser startup to enable tracing
 * Checks environment variables: IRODS_PARSER_TRACE, IRODS_PARSER_TRACE_LEVEL
 * 
 * @param context The parser context
 */
void initParserTracing(ParserContext *context);

/**
 * Check if tracing is enabled
 * @param context The parser context
 * @return Non-zero if tracing is enabled
 */
int isParserTracingEnabled(ParserContext *context);

/**
 * Emit a parser trace event
 * 
 * @param event The event type
 * @param funcName The parser function name (e.g., "parseValue")
 * @param details Additional details (e.g., token text, node type)
 * @param depth Current recursion depth (for indentation)
 */
void traceParserEvent(ParserTraceEvent event, const char *funcName, const char *details, int depth);

/**
 * Print parser state for debugging
 * Shows: current stack, token queue, error state
 * 
 * @param context The parser context
 * @param label Optional label for output
 */
void dumpParserState(ParserContext *context, const char *label);

/**
 * Print the node stack contents
 * 
 * @param context The parser context
 * @param maxDepth Maximum number of stack items to show (0 = all)
 */
void dumpNodeStack(ParserContext *context, int maxDepth);

/**
 * Print an AST node for debugging
 * Recursively prints tree structure with indentation
 * 
 * @param node The node to print
 * @param maxDepth Maximum recursion depth (0 = unlimited)
 * @param label Optional label for output
 */
void debugPrintNode(Node *node, int maxDepth, const char *label);

/* ============================================================================
 * Backward Compatibility: Macro Definitions
 * 
 * These macros provide a bridge to gradually migrate from the old macro-based
 * code to the new function-based approach. Eventually these should be removed
 * as each parser function is refactored.
 * ============================================================================ */

/**
 * OLD: BUILD_NODE - to be replaced with buildNodeWithChildren()
 * Kept here temporarily for comparison
 * See REFACTORING_STATUS.md for migration status
 */
#define OLD_BUILD_NODE(type,cons,loc,deg,consume) \
    if(context->error==0){Node *var = newNode((type), (cons), (loc), context->region); \
        if(deg!=0) { \
            Node **subs = setDegree(var, (deg), context->region); \
            int counter; \
            for(counter = 1;counter <= (deg);counter ++) { \
                subs[(deg)-counter] = context->nodeStack[context->nodeStackTop-counter]; \
            } \
        } \
        context->nodeStackTop -= (consume); \
        if(var == NULL || var->nodeType == N_ERROR) { \
            context->error = 1; \
            break; \
        } else { \
            context->nodeStack[context->nodeStackTop++] = var; \
        } \
    }

#endif
