/* For copyright information please refer to files in the COPYRIGHT directory
 */


#ifndef PARSER_HPP
#define PARSER_HPP

#include <ctype.h>
#include <stdio.h>
#include <string.h>


#ifndef DEBUG
#include "irods/objInfo.h"
#include "irods/private/re/reHelpers1.hpp"
#endif

#include "irods/reconstants.hpp"
#include "irods/private/re/restructs.hpp"
#include "irods/private/re/utils.hpp"
#include "irods/region.h"
#include "irods/irods_hashtable.h"


/// Describes an operator known to the parser.
typedef struct op {
    char* string; ///< Operator spelling.
    int arity; ///< Number of operands.
    int prec; ///< Precedence level.
} Op;

/// Number of built-in operators in `new_ops`.
#define num_ops 31
/// Table of built-in operators recognized by the parser.
extern Op new_ops[];

/// Buffered input source used by the parser.
typedef struct pointer {
    FILE *fp; ///< Backing file handle when parsing from a file.
    char buf[POINTER_BUF_SIZE]; ///< Character buffer.
    unsigned int len; /* len of string in buf */ ///< Number of buffered characters.
    unsigned int p; /* pointer to next char in buf */ ///< Offset of the next buffered character.
    unsigned long fpos; /* position of the beginning of the buffer in file */ ///< File offset for the current buffer.
    unsigned int strp; /* pointer to next char in strbuf */ ///< Offset of the next character in `strbuf`.
    char *strbuf; /* string buffer */ ///< Backing source buffer when parsing from memory.
    unsigned int strlen; ///< Length of `strbuf`.
    int isFile; ///< Non-zero when the source is file-backed.
    char *base; ///< Source name used in diagnostics.
} Pointer;

/// Appends a rule descriptor to a ruleset.
#define pushRule(rs, r) ((rs)->rules[(rs)->len++] = (r))


/// Holds parser stacks, token queue state, and diagnostics.
typedef struct {
    Node *nodeStack[1024]; ///< Parser node stack.
    int nodeStackTop; ///< Index of the next free slot in `nodeStack`.
    int stackTopStack[1024]; ///< Saved `nodeStackTop` values for backtracking.
    int stackTopStackTop; ///< Index of the next free slot in `stackTopStack`.
    int error; ///< Non-zero when a syntax error has been detected.
    int prec; ///< Current precedence level.
    int backwardCompatible; ///< Enables backward-compatible parsing behavior.
    Node *errnode; ///< Node associated with the current parse error.
    Label errloc; ///< Source location of the furthest parse error.
    char errmsgbuf[ERR_MSG_LEN]; ///< Formatted parser error message buffer.
    Hashtable *symtable; ///< Symbol table used during parsing.
    rError_t *errmsg; ///< Error stack for diagnostics.
    Region *region; ///< Allocation region used by the parser.
    Token tokenQueue[1024]; ///< Pushback token queue.
    int tqp; ///< Current token queue position.
    int tqtop; ///< Token queue write position.
    int tqbot; ///< Token queue read position.
} ParserContext;

/// Pushes a node onto the parser stack.
#define PUSH(n) (context->nodeStack[(context->nodeStackTop)++] = n)
/// Pops a node from the parser stack.
#define POP (context->nodeStack[--(context->nodeStackTop)])
/// Reads the next token using basic tokenization.
#define NEXT_TOKEN_BASIC NEXT_TOKEN(0)
/// Reads the next token using extended tokenization.
#define NEXT_TOKEN_EXT NEXT_TOKEN(1)
/// Reads the next token and updates parse error state on failure.
#define NEXT_TOKEN(ext) \
{ \
    FPOS; \
    token = nextTokenRuleGen(e, context, rulegen, ext); \
    if(token->type==N_ERROR) { \
        context->error=1; \
        if(pos.exprloc > context->errloc.exprloc) context->errloc = pos; \
        break;\
    } \
}
/// Returns true if the current token matches the requested type.
#define TOKEN_TYPE(t) (token->type == (t))
/// Returns true if the current token text matches the requested string.
#define TOKEN_TEXT(str) (strcmp(token->text, (str))==0)
/// Pushes the current token back into the token queue.
#define PUSHBACK pushback( token, context)
/// Returns the current source position.
#define FPOS (getFPos(&pos, e, context))
/// Updates the stored error position if the current one is farther.
#define UPDATE_ERR_LOC if(FPOS->exprloc > context->errloc.exprloc) {context->errloc = *FPOS;}
/// Propagates syntax errors from a node-producing expression.
#define CASCADE(x) \
{\
        Node *_ncascade = (x); \
        if(_ncascade == NULL || _ncascade->nodeType == N_ERROR) { \
                UPDATE_ERR_LOC; \
                context->error = 1; \
                break;\
        } else { \
                PUSH(_ncascade); \
        } \
}
/// Builds a node from the parser stack.
#define BUILD_NODE(type,cons,loc,deg, consume) \
        if(context->error==0){Node *var = newNode((type), (cons), (loc), context->region); \
            if(deg!=0) { \
                Node **subs = setDegree(var, (deg), context->region); \
                int counter; \
                for(counter = 1;counter <= (deg);counter ++) {\
                    subs[(deg)-counter] = context->nodeStack[context->nodeStackTop-counter];\
                } \
            } \
            context->nodeStackTop -= (consume); \
            CASCADE(var);}

/// Builds an application node from function and argument nodes.
#define BUILD_APP_NODE(cons,loc,deg) \
		BUILD_NODE(N_TUPLE, TUPLE, loc, deg, deg); \
		BUILD_NODE(TK_TEXT, cons, loc, 0, 0); \
		SWAP; \
		BUILD_NODE(N_APPLICATION, APPLICATION, loc, 2, 2); \

/// Declares a generated parser helper with no extra parameters.
#define PARSER_FUNC_PROTO(l) \
void CONCAT(nextRuleGen, l)(Pointer* e, ParserContext *context)
/// Declares a generated parser helper with one extra parameter.
#define PARSER_FUNC_PROTO1(l, p) \
void CONCAT(nextRuleGen, l)(Pointer* e, ParserContext *context, p)
/// Declares a generated parser helper with two extra parameters.
#define PARSER_FUNC_PROTO2(l, p, q) \
void CONCAT(nextRuleGen, l)(Pointer* e, ParserContext *context, p, q)
/// Begins a generated parser helper body.
#define PARSER_BEGIN(l) \
    Label start; \
    Label pos; \
    Token *token = NULL; (void)token; \
    skipWhitespace(e); \
    getFPos(&start, (e), context); \
    do {

/// Starts a generated parser helper with no extra parameters.
#define PARSER_FUNC_BEGIN(l) \
PARSER_FUNC_PROTO(l) { \
    PARSER_BEGIN(l)
/// Starts a generated parser helper with one extra parameter.
#define PARSER_FUNC_BEGIN1(l, p) \
PARSER_FUNC_PROTO1(l, p) { \
    PARSER_BEGIN(l)
/// Starts a generated parser helper with two extra parameters.
#define PARSER_FUNC_BEGIN2(l, p, q) \
PARSER_FUNC_PROTO2(l, p, q) { \
    PARSER_BEGIN(l)

/// Ends a generated parser helper.
#define PARSER_FUNC_END(l) \
    PARSER_END(l) \
}
/// Ends a parser production block.
#define PARSER_END(l) \
	} while(0);

/// Swaps the top two nodes on the parser stack.
#define SWAP \
{\
    Node *node = POP;\
    Node *node2 = POP;\
    PUSH(node);\
    PUSH(node2);\
}
/// Reorders tuple nodes on the parser stack.
#define UNZIP(n) \
{ \
    int i; \
    Node *node[1024]; \
    for(i=0;i<n;i++) { \
        node[i] = context->nodeStack[context->nodeStackTop - 2*(n-i) + 1]; \
        context->nodeStack[context->nodeStackTop-2*n+i] = context->nodeStack[context->nodeStackTop-2*n+2*i]; \
    } \
    for(i=0;i<n;i++) { \
        context->nodeStack[context->nodeStackTop-n+i] = node[i]; \
    } \
}

/// Consumes a text-like token with the requested spelling.
#define TTEXT(x) \
    NEXT_TOKEN(0); \
    if(!((TOKEN_TYPE(TK_TEXT)||TOKEN_TYPE(TK_OP)||TOKEN_TYPE(TK_MISC_OP)) && TOKEN_TEXT(x))) { \
        context->error = 1; \
        if(pos.exprloc > context->errloc.exprloc) context->errloc = pos; \
        break; \
    }
/// Consumes one of two expected text-like tokens.
#define TTEXT2(x,y) \
    NEXT_TOKEN(0); \
    if(!((TOKEN_TYPE(TK_TEXT)||TOKEN_TYPE(TK_OP)||TOKEN_TYPE(TK_MISC_OP)) && (TOKEN_TEXT(x)||TOKEN_TEXT(y)))) { \
        context->error = 1; \
        if(pos.exprloc > context->errloc.exprloc) context->errloc = pos; \
        break; \
    }
/// Consumes one of three expected text-like tokens.
#define TTEXT3(x,y,z) \
    NEXT_TOKEN(0); \
    if(!((TOKEN_TYPE(TK_TEXT)||TOKEN_TYPE(TK_OP)||TOKEN_TYPE(TK_MISC_OP)) && (TOKEN_TEXT(x)||TOKEN_TEXT(y)||TOKEN_TEXT(z)))) { \
        context->error = 1; \
        if(pos.exprloc > context->errloc.exprloc) context->errloc = pos; \
        break; \
    }
/// Peeks for a text-like token with the requested spelling.
#define TTEXT_LOOKAHEAD(x) \
    TTEXT(x); \
    PUSHBACK;
/// Consumes a token of the requested type.
#define TTYPE(x) \
    NEXT_TOKEN(x==TK_PATH); \
    if(!TOKEN_TYPE(x)) { \
        context->error = 1; \
        if(pos.exprloc > context->errloc.exprloc) context->errloc = pos; \
        break; \
    }
/// Peeks for a token of the requested type.
#define TTYPE_LOOKAHEAD(x) \
    TTYPE(x); \
    PUSHBACK;

/// Returns true if the parser has not recorded a syntax error.
#define NO_SYNTAX_ERROR (context->error == 0)
/// Aborts the current production if an error is set.
#define CHECK_ERROR \
if(context->error!=0) { \
    break; \
}

/// Invokes a generated parser helper with no extra parameters.
#define NT(x) \
CONCAT(nextRuleGen, x)(e, context); \
CHECK_ERROR;

/// Invokes a generated parser helper with one extra parameter.
#define NT1(x, p) \
CONCAT(nextRuleGen, x)(e, context, p); \
CHECK_ERROR;

/// Invokes a generated parser helper with two extra parameters.
#define NT2(x, p, q) \
CONCAT(nextRuleGen, x)(e, context, p, q); \
CHECK_ERROR;

/// Begins a backtracking choice block.
#define CHOICE_BEGIN(l) \
if(context->error==0) { \
    Label CONCAT(l,Start); \
    int CONCAT(l,Finish) = 0; \
    int CONCAT(l,TokenQueueP) = context->tqp; \
    getFPos(&CONCAT(l,Start), e, context); \
    context->stackTopStack[context->stackTopStackTop++] = context->nodeStackTop;

/// Ends a backtracking choice block.
#define CHOICE_END(l) \
    (context->stackTopStackTop)--; \
    if(!CONCAT(l,Finish)) { \
        UPDATE_ERR_LOC; \
        context->error = 1; \
        break;\
    } \
}

/// Begins one branch of a backtracking choice block.
#define BRANCH_BEGIN(l) \
if(!CONCAT(l,Finish)) { \
    do { \
    	context->tqp = CONCAT(l,TokenQueueP); \
        context->nodeStackTop = context->stackTopStack[context->stackTopStackTop-1]; \
        context->error = 0;

/// Ends one branch of a backtracking choice block.
#define BRANCH_END(l) \
        if(context->error == 0) \
            CONCAT(l,Finish) = 1; \
    } while(0);\
}

/// Begins a `try` branch sequence.
#define TRY(l) \
CHOICE_BEGIN(l) \
BRANCH_BEGIN(l)

/// Starts an alternate branch in a `try` sequence.
#define OR(l) \
BRANCH_END(l) \
BRANCH_BEGIN(l)

/// Starts a final branch in a `try` sequence.
#define FINALLY(l) \
BRANCH_END(l) \
{ \
    do {

/// Ends a `try` branch sequence.
#define END_TRY(l) \
BRANCH_END(l) \
CHOICE_END(l)

/// Sets a syntax error when the supplied condition is true.
#define ABORT(x) \
if(x) { \
        context->error = 1; \
        break;\
}

/// Begins an optional parser block.
#define OPTIONAL_BEGIN(l) \
CHOICE_BEGIN(l); \
BRANCH_BEGIN(l);

/// Ends an optional parser block.
#define OPTIONAL_END(l) \
BRANCH_END(l); \
BRANCH_BEGIN(l); \
BRANCH_END(l); \
CHOICE_END(l);

/// Begins a generated loop block.
#define LOOP_BEGIN(l) \
int CONCAT(done, l) = 0; \
while(!CONCAT(done, l) && NO_SYNTAX_ERROR) {

#if defined(solaris_platform)
/// Ends a generated loop block on Solaris.
#define LOOP_END(l) \
} \
if(!CONCAT(done, l)) { \
    break; \
}

/// Marks a generated loop as complete on Solaris.
#define DONE(l) \
CONCAT(done, l) = 1;

#else
/// Ends a generated loop block.
#define LOOP_END(l) \
} \
CONCAT(exit, l): \
if(!CONCAT(done, l)) { \
    break; \
}

/// Marks a generated loop as complete.
#define DONE(l) \
CONCAT(done, l) = 1; \
goto CONCAT(exit, l);
#endif

/// Names the loop counter associated with label `l`.
#define COUNTER(l) \
		CONCAT(l, Counter)

/// Begins a repetition block.
#define REPEAT_BEGIN(l) \
int COUNTER(l) = 0; \
LOOP_BEGIN(l) \
	TRY(l) \

/// Ends a repetition block.
#define REPEAT_END(l) \
		COUNTER(l)++; \
	OR(l) \
		DONE(l); \
	END_TRY(l) \
LOOP_END(l)

/// Begins a list parsing block.
#define LIST_BEGIN(l) \
	int COUNTER(l) = 0; \
	LOOP_BEGIN(l) \

/// Starts parsing the delimiter between list items.
#define LIST_DELIM(l) \
	COUNTER(l)++; \
	TRY(l) \

/// Ends a list parsing block.
#define LIST_END(l) \
	OR(l) \
		DONE(l); \
	END_TRY(l) \
LOOP_END(l)

/** utility functions */
/// Allocates and initializes a parser context.
ParserContext *newParserContext( rError_t *errmsg, Region *r );
/// Releases a parser context.
void deleteParserContext( ParserContext *t );

/// Returns the next token from the input stream.
Token *nextTokenRuleGen( Pointer* expr, ParserContext* pc, int rulegen, int ext );
/// Reads the next token string and records variable references.
int nextString( Pointer *e, char *value, int vars[] );
/// Reads the next token string using alternate tokenization rules.
int nextString2( Pointer *e, char *value, int vars[] );
/// Returns non-zero if the character ends a line.
int eol( char ch );
/// Returns non-zero if the token text names an operator.
int isOp( char *token );
/// Returns non-zero if the token is a unary operator.
int isUnaryOp( Token* token );
/// Returns the precedence of a unary operator token.
int getUnaryPrecedence( Token* token );
/// Returns non-zero if the token is a binary operator.
int isBinaryOp( Token *token );
/// Returns the precedence of a binary operator token.
int getBinaryPrecedence( Token* token );
/// Converts a source position into line and column coordinates.
void getCoor( Pointer *p, Label * errloc, int coor[2] );
/// Returns the byte range covered by the requested source line.
int getLineRange( Pointer *p, int line, rodsLong_t range[2] );

/**
 * skip a token of type TK_TEXT, TK_OP, or TK_MISC_OP and text text, token will has type N_ERROR if the token does not match
 */
int skip( Pointer *expr, char *text, Token **token, ParserContext *pc, int rulegen );
/// Skips whitespace characters in the input stream.
void skipWhitespace( Pointer *expr );
/// Finds a line-continuation marker in a source string.
char *findLineCont( char *expr );

/// Parses a complete ruleset from the input stream.
int parseRuleSet( Pointer *e, RuleSet *ruleSet, Env *funcDesc, int *errloc, rError_t *errmsg, Region *r );
/**
 * Parse a rule, create a rule pack.
 * If error, either ret==NULL or ret->type=N_ERROR.
 */
/// Parses a single rule and returns its AST node.
Node *parseRuleRuleGen( Pointer *expr, int backwardCompatible, ParserContext *pc );
/// Parses a rule term.
Node *parseTermRuleGen( Pointer *expr, int rulegn, ParserContext *pc );
/// Parses an actions block.
Node *parseActionsRuleGen( Pointer *expr, int rulegn, int backwardCompatible, ParserContext *pc );
/// Pushes a token back into the parser context.
void pushback( Token *token, ParserContext *pc );
/// Initializes a `Pointer` for file-backed input.
void initPointer( Pointer *p, FILE* fp, const char* ruleBaseName );
/// Initializes a `Pointer` for buffer-backed input.
void initPointer2( Pointer *p, char* buf );
/// Allocates a file-backed `Pointer`.
Pointer *newPointer( FILE* buf, const char *ruleBaseName );
/// Allocates a buffer-backed `Pointer`.
Pointer *newPointer2( char* buf );
/// Releases a `Pointer`.
void deletePointer( Pointer* buf );

/// Skips comments in the input stream.
void skipComments( Pointer *e );
/// Returns the next character from the input stream.
int nextChar( Pointer *p );
/// Returns the character `n` bytes ahead without consuming it.
int lookAhead( Pointer *p, unsigned int n );

/// Trims leading and trailing whitespace from a string.
char* trim( char* str );
/// Removes surrounding quote characters from a string.
void trimquotes( char *string );
/// Returns non-zero if the node represents a local variable.
int isLocalVariableNode( Node *node );
/// Returns non-zero if the node represents a session variable.
int isSessionVariableNode( Node *node );
/// Returns non-zero if the node represents any variable.
int isVariableNode( Node *node );

/// Appends a rule name to a string buffer.
void ruleNameToString( char **p, int *s, int indent, Node *rn );
/// Formats a rule descriptor into a string buffer.
void ruleToString( char *buf, int size, RuleDesc *rd );
/// Appends an actions block to a string buffer.
void actionsToString( char **p, int *s, int indent, Node *na, Node *nr );
/// Appends indentation whitespace to a string buffer.
void indentToString( char **p, int *s, int indent );
/// Appends a term representation to a string buffer.
void termToString( char **p, int *s, int indent, int prec, Node *n, int quote );
/// Appends a pattern representation to a string buffer.
void patternToString( char **p, int *s, int indent, int prec, Node *n );
/// Appends a type representation to a string buffer.
void typeToStringParser( char **p, int *s, int indent, int lifted, Node *n );
/// Formats a function application into a string buffer.
void functionApplicationToString( char *buf, int size, char *fn, Node **args, int n );

/// Returns a pointer to the next rule section in a source string.
char *nextRuleSection( char *expr, char* section );
/// Returns a pointer to the function parameter section of a source string.
char *functionParameters( char *expr, char* param );
/// Parses a function type from a string.
ExprType *parseFuncTypeFromString( char *string, Region *r );
/// Parses typing constraints from a string.
Node* parseTypingConstraintsFromString( char *string, Region *r );
/// Parses a type expression from the input stream.
ExprType *parseType( Pointer *e, int prec, Env *vtable, int lifted, Region *r );

/// Returns the current file position label.
Label *getFPos( Label *label, Pointer *p, ParserContext *context );
/// Clears the buffered input state.
void clearBuffer( Pointer *p );
/// Seeks to a byte offset in a file-backed input source.
void seekInFile( Pointer *p, unsigned long x );
/// Consumes the requested number of characters.
void nextChars( Pointer *p, int len );

/// Synchronizes the token queue with the current input position.
void syncTokenQueue( Pointer *e, ParserContext *context );

/// Copies one source line into a buffer.
int dupLine( Pointer *p, Label * start, int n, char *buf );
/// Copies a source substring into a buffer.
int dupString( Pointer *p, Label * start, int n, char *buf );

/// Collects variable names referenced by an expression node.
StringList *getVarNamesInExprNode( Node *expr, Region *r );
/// Helper for collecting variable names referenced by an expression node.
StringList *getVarNamesInExprNodeAux( Node *expr, StringList* varnames, Region *r );
/// Returns non-zero if two expression trees are syntactically equal.
int eqExprNodeSyntactic( Node *a, Node *b );
/// Returns non-zero if two expression trees are equal under a variable mapping.
int eqExprNodeSyntacticVarMapping( Node *a, Node *b, Hashtable *varMapping /* from a to b */ );

/// Reads a delimited string fragment from the input stream.
int nextStringBase( Pointer *e, char *value, int max_len, char* delim, int consumeDelim, char escape, int cntOffset, int vars[] );
/// Reads a delimited string fragment using default options.
int nextStringBase2( Pointer *e, char *value, int max_len, char* delim );
/// Reads an action argument string using backward-compatible rules.
void nextActionArgumentStringBackwardCompatible( Pointer *e, Token *token );

/// Returns the rule language type name of a `Res`.
char* typeName_Res( Res *s );
/// Returns the rule language type name of an `ExprType`.
char* typeName_ExprType( ExprType *s );
/// Returns the symbolic name of a `NodeType`.
char* typeName_NodeType( NodeType s );
/// Returns the parser-facing name of a `NodeType`.
char* typeName_Parser( NodeType s );
/// Prints an expression tree for debugging.
void printTree( Node *n, int indent );
/// Prints indentation for debugging output.
void printIndent( int indent );

/// Formats an error message using file-backed source text.
void generateErrMsgFromFile( char *msg, long errloc, char *ruleBaseName, char* ruleBasePath, char errbuf[ERR_MSG_LEN] );
/// Formats an error message using in-memory source text.
void generateErrMsgFromSource( char *msg, long errloc, char *src, char errbuf[ERR_MSG_LEN] );
/// Formats an error message using a parser pointer and label.
void generateErrMsgFromPointer( char *msg, Label *l, Pointer *e, char errbuf[ERR_MSG_LEN] );
/// Formats an error message and returns the output buffer.
char *generateErrMsg( char *msg, long errloc, char* ruleBaseName, char errbuf[ERR_MSG_LEN] );
/// Creates and appends an error message to an error stack.
void generateAndAddErrMsg( char *msg, Node *node, int errcode, rError_t *errmsg );

#endif
