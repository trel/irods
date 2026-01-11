/* For copyright information please refer to files in the COPYRIGHT directory
 */

#include "irods/private/re/parser_helpers.hpp"
#include "irods/private/re/parser.hpp"
#include "irods/private/re/restructs.hpp"
#include "irods/region.h"

/* ============================================================================
 * Node Construction
 * ============================================================================ */

Node *buildNode(NodeType type, const char *text, Label *label, int degree, ParserContext *context) {
    if (context->error != 0) {
        return NULL;
    }
    
    Node *node = newNode(type, text, label, context->region);
    if (node == NULL || node->nodeType == N_ERROR) {
        context->error = 1;
        updateErrorLocation(context, label);
        return NULL;
    }
    
    if (degree > 0) {
        Node **subs = setDegree(node, degree, context->region);
        if (subs == NULL) {
            context->error = 1;
            updateErrorLocation(context, label);
            return NULL;
        }
        /* Subtrees will be assigned separately or via buildNodeWithChildren */
    }
    
    return node;
}

Node *buildNodeWithChildren(NodeType type, const char *text, Label *label, int degree, ParserContext *context) {
    if (context->error != 0) {
        return NULL;
    }
    
    Node *node = buildNode(type, text, label, degree, context);
    if (node == NULL) {
        return NULL;
    }
    
    /* Pop children from stack in reverse order */
    if (degree > 0) {
        Node **subs = node->subtrees;
        for (int i = 1; i <= degree; i++) {
            if (context->nodeStackTop >= i) {
                subs[degree - i] = context->nodeStack[context->nodeStackTop - i];
            } else {
                /* Stack underflow - this should not happen in correct code */
                context->error = 1;
                updateErrorLocation(context, label);
                return NULL;
            }
        }
        context->nodeStackTop -= degree;
    }
    
    pushNode(context, node);
    return node;
}

Node *buildTupleNode(Label *label, int degree, ParserContext *context) {
    if (context->error != 0) {
        return NULL;
    }
    
    Node *tupleNode = buildNodeWithChildren(N_TUPLE, "TUPLE", label, degree, context);
    return tupleNode;
}

Node *buildApplicationNode(const char *funcName, Label *label, int arity, ParserContext *context) {
    if (context->error != 0) {
        return NULL;
    }
    
    /* Build tuple of arguments (arity items from stack) */
    Node *argTuple = buildTupleNode(label, arity, context);
    if (argTuple == NULL) {
        return NULL;
    }
    
    /* Build text node for function name */
    Node *funcNode = buildNode(TK_TEXT, funcName, label, 0, context);
    if (funcNode == NULL) {
        popNode(context); /* Remove argTuple */
        return NULL;
    }
    pushNode(context, funcNode);
    
    /* Swap to get correct order: funcNode below argTuple */
    swapTopNodes(context);
    
    /* Build application node with 2 children: function and arguments */
    Node *appNode = buildNodeWithChildren(N_APPLICATION, "APPLICATION", label, 2, context);
    return appNode;
}

Node *buildErrorNode(const char *errorMsg, Label *label, ParserContext *context) {
    Node *errNode = newNode(N_ERROR, errorMsg, label, context->region);
    return errNode;
}

/* ============================================================================
 * Error Handling
 * ============================================================================ */

void updateErrorLocation(ParserContext *context, Label *label) {
    if (label != NULL && label->exprloc > context->errloc.exprloc) {
        context->errloc = *label;
    }
}

void setParserError(ParserContext *context, Label *label) {
    context->error = 1;
    updateErrorLocation(context, label);
}

int hasParserError(ParserContext *context) {
    return context->error != 0;
}

void clearParserError(ParserContext *context) {
    context->error = 0;
}

/* ============================================================================
 * Choice Point Management
 * ============================================================================ */

void saveChoicePoint(ParserContext *context, ChoicePoint *choicePoint, Label *label) {
    choicePoint->nodeStackSavepoint = context->nodeStackTop;
    choicePoint->tokenQueuePosition = context->tqp;
    choicePoint->choiceStart = *label;
}

void restoreChoicePoint(ParserContext *context, const ChoicePoint *choicePoint) {
    context->tqp = choicePoint->tokenQueuePosition;
    context->nodeStackTop = choicePoint->nodeStackSavepoint;
    context->error = 0;
}

int commitBranch(ParserContext *context) {
    return context->error == 0;
}

/* ============================================================================
 * Explicit Error Propagation
 * ============================================================================ */

ParseResult makeParseResult(Node *node) {
    ParseResult result;
    result.node = node;
    result.error = 0;
    result.errorLocation.exprloc = -1;
    result.errorLocation.base = NULL;
    return result;
}

ParseResult makeParseError(ParserContext *context) {
    ParseResult result;
    result.node = NULL;
    result.error = 1;
    result.errorLocation = context->errloc;
    return result;
}

int isParseSuccess(ParseResult result) {
    return result.error == 0 && result.node != NULL;
}

int isParseError(ParseResult result) {
    return result.error != 0 || result.node == NULL;
}

Node *unwrapParseResult(ParseResult result, ParserContext *context) {
    if (isParseError(result)) {
        context->error = 1;
        if (result.errorLocation.exprloc > context->errloc.exprloc) {
            context->errloc = result.errorLocation;
        }
        return NULL;
    }
    return result.node;
}

void reportParserError(ParserContext *context, int errorCode, const char *message, Label *label) {
    char errbuf[ERR_MSG_LEN];
    
    if (message != NULL) {
        strncpy(errbuf, message, ERR_MSG_LEN - 1);
        errbuf[ERR_MSG_LEN - 1] = '\0';
    } else {
        strcpy(errbuf, "Unknown parser error");
    }
    
    if (context->errmsg != NULL) {
        addRErrorMsg(context->errmsg, errorCode, errbuf);
    }
    
    context->error = 1;
    updateErrorLocation(context, label);
}

void reportParseExpectationError(ParserContext *context, const char *expected, const char *got, Label *label) {
    char errbuf[ERR_MSG_LEN];
    
    if (expected != NULL && got != NULL) {
        snprintf(errbuf, ERR_MSG_LEN, "expected %s, but found %s", expected, got);
    } else if (expected != NULL) {
        snprintf(errbuf, ERR_MSG_LEN, "expected %s", expected);
    } else {
        strcpy(errbuf, "parse error");
    }
    
    reportParserError(context, RE_PARSER_ERROR, errbuf, label);
}

/* ============================================================================
 * Parser Combinators
 * ============================================================================ */

int parseDelimitedSequence(Pointer *expr, ParserContext *context, ItemParserFunc itemParser, 
                          const char *delimiter, int allowTrailing) {
    int count = 0;
    
    if (hasParserError(context)) {
        return -1;
    }
    
    while (1) {
        /* Try to parse an item */
        ChoicePoint choice;
        saveChoicePoint(context, &choice, NULL);
        
        if (itemParser(expr, context) == 0) {
            /* No more items */
            restoreChoicePoint(context, &choice);
            break;
        }
        
        if (hasParserError(context)) {
            return -1;
        }
        
        count++;
        
        /* Look for delimiter */
        Token *token = nextTokenRuleGen(expr, context, 0, 0);
        if (token == NULL || token->type == N_ERROR) {
            break;  /* No delimiter, end of sequence */
        }
        
        if (strcmp(token->text, delimiter) != 0) {
            pushback(token, context);
            break;  /* Not a delimiter, end of sequence */
        }
        
        /* We have a delimiter, continue looping */
    }
    
    return count;
}

int parseOptional(Pointer *expr, ParserContext *context, ParserFunc parser) {
    if (hasParserError(context)) {
        return 0;
    }
    
    ChoicePoint choice;
    saveChoicePoint(context, &choice, NULL);
    
    if (parser(expr, context) != 0 && !hasParserError(context)) {
        return 1;  /* Successfully parsed */
    }
    
    /* Parsing failed, restore state and return 0 (not an error) */
    restoreChoicePoint(context, &choice);
    return 0;
}

int parseRepeat(Pointer *expr, ParserContext *context, ParserFunc parser) {
    int count = 0;
    
    if (hasParserError(context)) {
        return -1;
    }
    
    while (1) {
        ChoicePoint choice;
        saveChoicePoint(context, &choice, NULL);
        
        if (parser(expr, context) == 0 || hasParserError(context)) {
            restoreChoicePoint(context, &choice);
            break;
        }
        
        count++;
    }
    
    return count;
}

int parseRepeatMin(Pointer *expr, ParserContext *context, ParserFunc parser, int minCount) {
    int count = parseRepeat(expr, context, parser);
    
    if (count < 0) {
        return -1;  /* Error occurred */
    }
    
    if (count < minCount) {
        context->error = 1;
        return -1;  /* Too few items parsed */
    }
    
    return count;
}

int parseExactly(Pointer *expr, ParserContext *context, ParserFunc parser, int count) {
    for (int i = 0; i < count; i++) {
        if (parser(expr, context) == 0 || hasParserError(context)) {
            context->error = 1;
            return -1;
        }
    }
    return count;
}

int parseChoice(Pointer *expr, ParserContext *context, ParserFunc *parsers, int numChoices) {
    if (hasParserError(context)) {
        return -1;
    }
    
    for (int i = 0; i < numChoices; i++) {
        ChoicePoint choice;
        saveChoicePoint(context, &choice, NULL);
        
        if (parsers[i](expr, context) != 0 && !hasParserError(context)) {
            return i + 1;  /* Return 1-based index of successful parser */
        }
        
        /* Try next alternative */
        restoreChoicePoint(context, &choice);
    }
    
    /* All alternatives failed */
    return 0;
}

/* ============================================================================
 * Debug/Tracing Support
 * ============================================================================ */

/* Trace control flags (stored in context->ival or a separate flags field) */
#define PARSER_TRACE_FLAG_ENABLED  0x01
#define PARSER_TRACE_FLAG_VERBOSE  0x02
#define PARSER_TRACE_FLAG_STACK    0x04

static int parser_trace_flags = 0;
static int parser_trace_depth = 0;

void initParserTracing(ParserContext *context) {
    parser_trace_flags = 0;
    parser_trace_depth = 0;
    
    /* Check environment variables */
    const char *trace_env = getenv("IRODS_PARSER_TRACE");
    if (trace_env != NULL && strcmp(trace_env, "1") == 0) {
        parser_trace_flags |= PARSER_TRACE_FLAG_ENABLED;
    }
    
    const char *verbose_env = getenv("IRODS_PARSER_TRACE_VERBOSE");
    if (verbose_env != NULL && strcmp(verbose_env, "1") == 0) {
        parser_trace_flags |= PARSER_TRACE_FLAG_VERBOSE;
    }
    
    const char *stack_env = getenv("IRODS_PARSER_TRACE_STACK");
    if (stack_env != NULL && strcmp(stack_env, "1") == 0) {
        parser_trace_flags |= PARSER_TRACE_FLAG_STACK;
    }
}

int isParserTracingEnabled(ParserContext *context) {
    return (parser_trace_flags & PARSER_TRACE_FLAG_ENABLED) != 0;
}

void traceParserEvent(ParserTraceEvent event, const char *funcName, const char *details, int depth) {
    if ((parser_trace_flags & PARSER_TRACE_FLAG_ENABLED) == 0) {
        return;
    }
    
    /* Print indentation based on depth */
    for (int i = 0; i < depth; i++) {
        fprintf(stderr, "  ");
    }
    
    /* Print event */
    switch (event) {
        case PARSER_TRACE_ENTER:
            fprintf(stderr, ">> %s\n", funcName);
            break;
        case PARSER_TRACE_EXIT:
            fprintf(stderr, "<< %s\n", funcName);
            break;
        case PARSER_TRACE_SUCCESS:
            fprintf(stderr, "OK %s", funcName);
            if (details != NULL) fprintf(stderr, " (%s)", details);
            fprintf(stderr, "\n");
            break;
        case PARSER_TRACE_FAILURE:
            fprintf(stderr, "FAIL %s\n", funcName);
            break;
        case PARSER_TRACE_ERROR:
            fprintf(stderr, "ERR %s: %s\n", funcName, details != NULL ? details : "unknown");
            break;
        case PARSER_TRACE_BACKTRACK:
            fprintf(stderr, "BK %s\n", funcName);
            break;
        case PARSER_TRACE_TOKEN:
            fprintf(stderr, "TK %s\n", details != NULL ? details : "?");
            break;
        case PARSER_TRACE_NODE:
            fprintf(stderr, "ND %s\n", details != NULL ? details : "?");
            break;
        case PARSER_TRACE_STACK:
            fprintf(stderr, "ST %s\n", details != NULL ? details : "?");
            break;
    }
    
    fflush(stderr);
}

void dumpParserState(ParserContext *context, const char *label) {
    if ((parser_trace_flags & PARSER_TRACE_FLAG_VERBOSE) == 0) {
        return;
    }
    
    fprintf(stderr, "\n=== Parser State %s ===\n", label != NULL ? label : "");
    fprintf(stderr, "Error: %s (at %ld)\n", context->error ? "yes" : "no", context->errloc.exprloc);
    fprintf(stderr, "Node Stack: %d items\n", context->nodeStackTop);
    fprintf(stderr, "Token Queue: p=%d top=%d bot=%d\n", context->tqp, context->tqtop, context->tqbot);
    
    if ((parser_trace_flags & PARSER_TRACE_FLAG_STACK) != 0) {
        dumpNodeStack(context, 5);
    }
    fprintf(stderr, "\n");
}

void dumpNodeStack(ParserContext *context, int maxDepth) {
    if ((parser_trace_flags & PARSER_TRACE_FLAG_VERBOSE) == 0) {
        return;
    }
    
    fprintf(stderr, "--- Node Stack ---\n");
    int limit = maxDepth > 0 ? maxDepth : context->nodeStackTop;
    if (limit > context->nodeStackTop) {
        limit = context->nodeStackTop;
    }
    
    for (int i = context->nodeStackTop - 1; i >= context->nodeStackTop - limit && i >= 0; i--) {
        Node *node = context->nodeStack[i];
        if (node != NULL) {
            fprintf(stderr, "[%d] type=%d text=%s degree=%d\n", i, node->nodeType, 
                   node->text != NULL ? node->text : "NULL", node->degree);
        }
    }
}

void debugPrintNode(Node *node, int maxDepth, const char *label) {
    if ((parser_trace_flags & PARSER_TRACE_FLAG_VERBOSE) == 0) {
        return;
    }
    
    if (label != NULL) {
        fprintf(stderr, "=== %s ===\n", label);
    }
    
    /* Use existing printTree function if available, or simple implementation */
    if (node != NULL) {
        fprintf(stderr, "Node: type=%d text=%s degree=%d\n", 
               node->nodeType, node->text != NULL ? node->text : "NULL", node->degree);
    } else {
        fprintf(stderr, "Node: NULL\n");
    }
}
