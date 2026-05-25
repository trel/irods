/* For copyright information please refer to files in the COPYRIGHT directory
 */


#ifndef RESTRUCTS_HPP
#define RESTRUCTS_HPP
#include <ctype.h>
#include <stdio.h>
#include <string.h>
//#include "irods/private/reGlobalsExtern.hpp"
#include "irods/irods_re_structs.hpp"

#ifndef DEBUG
#include "irods/rodsType.h"
#include "irods/msParam.h"
#endif

//#include "irods/reconstants.hpp"
#include "irods/region.h"
#include "irods/irods_hashtable.h"
#include "irods/irods_list.h"

/// Returns the node type of an expression type node.
#define TYPE(x) ((x)->exprType->nodeType)

/// Returns the constructor type argument list.
#define T_CONS_TYPE_ARGS(x) ((x)->subtrees)
/// Returns constructor type argument `n`.
#define T_CONS_TYPE_ARG(x, n) ((x)->subtrees[n])
/// Returns the constructor type name.
#define T_CONS_TYPE_NAME(x) ((x)->text)
/// Returns the constructor arity.
#define T_CONS_ARITY(x) ((x)->degree)
/// Returns function parameter type `n`.
#define T_FUNC_PARAM_TYPE(x, n) (T_CONS_TYPE_ARG((x)->subtrees[0], n))
/// Returns function parameter type `n`, applying vararg rules.
#define T_FUNC_PARAM_TYPE_VARARG(x, n) (n<T_FUNC_ARITY(x)?T_FUNC_PARAM_TYPE(x,n):T_FUNC_PARAM_TYPE(x,T_FUNC_ARITY(x)-1))
/// Returns the function return type.
#define T_FUNC_RET_TYPE(x) ((x)->subtrees[1])
/// Returns the function arity.
#define T_FUNC_ARITY(x) ((x)->subtrees[0]->degree)
/// Returns the function vararg mode.
#define T_FUNC_VARARG(x) ((x)->vararg)
/// Returns the variable identifier.
#define T_VAR_ID(x) ((x)->ival)
/// Returns disjunct `n` of a type variable.
#define T_VAR_DISJUNCT(x, n) ((x)->subtrees[n])
/// Returns the disjunct list of a type variable.
#define T_VAR_DISJUNCTS(x) ((x)->subtrees)
/// Returns the number of disjuncts for a type variable.
#define T_VAR_NUM_DISJUNCTS(x) ((x)->degree)
/// Returns the left side of a typing constraint.
#define TC_A(tc) ((tc)->subtrees[0])
/// Returns the right side of a typing constraint.
#define TC_B(tc) ((tc)->subtrees[1])
/// Returns the node associated with a typing constraint.
#define TC_NODE(tc) ((tc)->subtrees[2])
/// Returns the next typing constraint in a list.
#define TC_NEXT(tc) ((tc)->subtrees[3])
/// Returns application argument `n`.
#define N_APP_ARG(x, n) ((x)->subtrees[1]->subtrees[n])
/// Returns the function node of an application.
#define N_APP_FUNC(x) ((x)->subtrees[0])
/// Returns the number of application arguments.
#define N_APP_ARITY(x) ((x)->subtrees[1]->degree)
/// Returns the number of parameters for a rule node.
#define RULE_NODE_NUM_PARAMS(r) ((r)->subtrees[0]->subtrees[0]->degree)
/// Returns the name of a rule node.
#define RULE_NAME(r) ((r)->subtrees[0]->text)

/// Returns the projection index of a deconstructor.
#define FD_PROJ(x) (x)->ival
/// Returns the function arity stored in a result node.
#define RES_FUNC_N_ARGS(x) (x)->ival
/// Returns the error code stored in a result node.
#define RES_ERR_CODE(x) (x)->ival
/// Returns the function arity stored in a result node.
#define RES_FUNC_N_ARGS(x) (x)->ival
/// Returns whether tuple construction is requested.
#define N_TUPLE_CONSTRUCT_TUPLE(x) (x)->ival
/// Returns the string length stored in a result node.
#define RES_STRING_STR_LEN(x) (x)->ival
/// Returns the lvalue view of a double result.
#define RES_DOUBLE_VAL_LVAL(x) ((x)->dval)
/// Returns the lvalue view of an integer result.
#define RES_INT_VAL_LVAL(x) ((x)->dval)
/// Returns the lvalue view of a boolean result.
#define RES_BOOL_VAL_LVAL(x) ((x)->dval)
/// Returns the double value stored in a result node.
#define RES_DOUBLE_VAL(x) ((x)->dval)
/// Returns the integer value stored in a result node.
#define RES_INT_VAL(x) ((int)(x)->dval)
/// Returns the boolean value stored in a result node.
#define RES_BOOL_VAL(x) ((int)(x)->dval)
/// Returns the time value stored in a result node.
#define RES_TIME_VAL(x) ((x)->lval)
/// Returns the rule index list referenced by a function descriptor.
#define FD_RULE_INDEX_LIST(x) ((x)->ruleIndexList)
/// Returns the lvalue view of the rule index list reference.
#define FD_RULE_INDEX_LIST_LVAL(x) FD_RULE_INDEX_LIST(x)
/// Returns the SMSI function pointer stored in a function descriptor.
#define FD_SMSI_FUNC_PTR(x) ((x)->func)
/// Returns the lvalue view of the SMSI function pointer.
#define FD_SMSI_FUNC_PTR_LVAL(x) FD_SMSI_FUNC_PTR(x)
/// Returns the uninterpreted input/output structure.
#define RES_UNINTER_STRUCT(x) ((x)->param->inOutStruct)
/// Returns the uninterpreted input/output buffer.
#define RES_UNINTER_BUFFER(x) ((x)->param->inpOutBuf)
/// Returns the iRODS type name for an uninterpreted result.
#define RES_IRODS_TYPE(x) ((x)->param->type)
/// Returns the source expression position stored in a node.
#define NODE_EXPR_POS(x) ((x)->expr)
/// Converts a raw node type integer to `NodeType`.
#define getNodeType(x) ((NodeType)((x)->nodeType))
/// Sets the node type of a node.
#define setNodeType(x, t) ((x)->nodeType = (int) t)

/// Vararg mode for exactly one argument.
#define OPTION_VARARG_ONCE 0x0
/// Vararg mode for zero or more arguments.
#define OPTION_VARARG_STAR 0x1
/// Vararg mode for one or more arguments.
#define OPTION_VARARG_PLUS 0x2
/// Vararg mode for an optional argument.
#define OPTION_VARARG_OPTIONAL 0x3
/// Bitmask selecting the vararg mode bits.
#define OPTION_VARARG_MASK 0xf
/// Enables runtime coercion for a node.
#define OPTION_COERCE 0x10
/// Marks a node as typed.
#define OPTION_TYPED 0x20

/// Bitmask selecting the IO type bits.
#define OPTION_IO_TYPE_MASK 0xff00
/// Marks an input parameter.
#define IO_TYPE_INPUT 0x100
/// Marks an output parameter.
#define IO_TYPE_OUTPUT 0x200
/// Marks a dynamic parameter.
#define IO_TYPE_DYNAMIC 0x400
/// Marks an expression parameter.
#define IO_TYPE_EXPRESSION 0x800
/// Marks an actions parameter.
#define IO_TYPE_ACTIONS 0x1000

/// Returns the vararg mode encoded in a node option field.
#define getVararg(n) ((n)->option & OPTION_VARARG_MASK)
/// Sets the vararg mode encoded in a node option field.
#define setVararg(n, v) (n)->option ^= ((n)->option & OPTION_VARARG_MASK) ^ (v);
/// Returns the IO type encoded in a node option field.
#define getIOType(n) ((n)->option & OPTION_IO_TYPE_MASK)
/// Sets the IO type encoded in a node option field.
#define setIOType(n, v) (n)->option ^= ((n)->option & OPTION_IO_TYPE_MASK) ^ (v);

/// Marks a system-space rule.
#define SYSTEM_SPACE_RULE 0x100
/// Discards the value of an expression after evaluation.
#define DISCARD_EXPRESSION_RESULT 0x200

/// Forward declaration for the main AST node type.
typedef struct node Node;
/// Alias for type expression nodes.
typedef struct node ExprType;
/// Alias for runtime result nodes.
typedef struct node Res;
/// Alias for function descriptor nodes.
typedef struct node FunctionDesc;
/// Alias for typing constraint nodes.
typedef struct node TypingConstraint;
/// Pointer alias for `Node`.
typedef struct node *NodePtr;

/// Pointer alias for mutable character buffers.
typedef char *charPtr;
/// Pointer alias for `ExprType`.
typedef ExprType *ExprTypePtr;
/// Forward declaration for hash table buckets.
typedef struct bucket Bucket;
/// Pointer alias for `Bucket`.
typedef Bucket *BucketPtr;
/// Pointer alias for `msParam_t`.
typedef msParam_t *msParam_tPtr;


/// Enumerates parser tokens, AST node kinds, and type kinds.
typedef enum node_type {
    TK_EOS = -1, ///< End of source marker.
    N_ERROR = 0, ///< Error node.
    TK_INT = 1, ///< Integer token.
    TK_DOUBLE = 2, ///< Floating-point token.
    TK_TEXT = 3, ///< Text token.
    TK_STRING = 4, ///< String token.
    TK_BOOL = 5, ///< Boolean token.
    TK_BACKQUOTED = 6, ///< Backquoted token.
    TK_DATETIME = 7, /* unused */ ///< Datetime token.
    TK_VAR = 8, ///< Variable token.
    TK_LOCAL_VAR = 10, ///< Local variable token.
    TK_SESSION_VAR = 11, ///< Session variable token.
    TK_OP = 12, ///< Operator token.
    TK_MISC_OP = 14, ///< Miscellaneous operator token.
    TK_COL = 16, ///< Collection token.
    TK_PATH = 18, ///< Path token.
    N_VAL = 20, ///< Value node.
    N_TUPLE = 21, ///< Tuple node.
    N_APPLICATION = 22, ///< Function application node.
    N_PARTIAL_APPLICATION = 23, ///< Partial application node.
    N_ATTR = 25, ///< Attribute node.
    N_QUERY_COND = 26, ///< Query condition node.
    N_QUERY_COND_JUNCTION = 27, ///< Query condition junction node.
    N_QUERY = 28, ///< Query node.
    N_ACTIONS = 30, ///< Actions node.
    N_ACTIONS_RECOVERY = 31, ///< Recovery actions node.
    N_RULE_NAME = 32, ///< Rule name node.
    N_PARAM_LIST = 33, ///< Parameter list node.
    N_PARAM_TYPE_LIST = 34, ///< Parameter type list node.
    N_AVU = 35, ///< AVU node.
    N_META_DATA = 36, ///< Metadata node.
    N_RULE_PACK = 37, ///< Rule pack node.
    N_RULESET = 38, ///< Ruleset node.
    N_FD_FUNCTION = 41, ///< Function descriptor node.
    N_FD_CONSTRUCTOR = 42, ///< Constructor descriptor node.
    N_FD_DECONSTRUCTOR = 43, ///< Deconstructor descriptor node.
    N_FD_EXTERNAL = 44, ///< External descriptor node.
    N_FD_RULE_INDEX_LIST = 45, ///< Rule index list descriptor node.
    N_SYM_LINK = 46, ///< Symbolic link node.
    N_RULE_CODE = 50, ///< Rule body node.
    N_RULE = 60, ///< Rule node.
    N_CONSTRUCTOR_DEF = 61, ///< Constructor definition node.
    N_EXTERN_DEF = 62, ///< External definition node.
    N_DATA_DEF = 63, ///< Data definition node.
    N_UNPARSED = 64, ///< Unparsed node.
    /* K_FLEX = 90, */
    T_UNSPECED = 100, /* indicates a variable which is not assigned a value is passed in to a microservice */ ///< Unspecified type.
    T_ERROR = 101, ///< Error type.
    T_DYNAMIC = 200, ///< Dynamic type.
    T_DOUBLE = 201, ///< Double type.
    T_INT = 202, ///< Integer type.
    T_STRING = 203, ///< String type.
    T_DATETIME = 204, ///< Datetime type.
    T_BOOL = 205, ///< Boolean type.
    T_FLEX = 206, ///< Flexible type.
    T_FIXD = 207, ///< Fixed type.
    T_TUPLE = 208, ///< Tuple type.
    T_CONS = 209, ///< Constructor type.
    T_PATH = 220, ///< Path type.
    T_BREAK = 230, ///< Break pseudo-type.
    T_SUCCESS = 231, ///< Success pseudo-type.
    T_VAR = 300, ///< Type variable.
    T_IRODS = 400, ///< iRODS-specific type.
    T_TYPE = 500, ///< Meta-type.
    TC_LT = 600, ///< Less-than typing constraint.
    TC_SET = 660, ///< Typing constraint set.
    PI_BIN = 1002, ///< Binary primitive info node.
    PI_CHAR = 1008, ///< Character primitive info node.
    PI_STR = 1009, ///< String primitive info node.
    PI_PISTR = 1010, ///< Pointer-to-string primitive info node.
    PI_INT16 = 1016, ///< 16-bit integer primitive info node.
    PI_INT = 1032, ///< Integer primitive info node.
    PI_DOUBLE = 1064, ///< Double primitive info node.
    PI_POINTER = 2001, ///< Pointer info node.
    PI_STRUCT = 2002, ///< Struct info node.
    PI_DEPENDENT = 2003, ///< Dependent info node.
    PI_INT_DEPENDENT = 2004, ///< Integer-dependent info node.
    PI_DIM = 2006, ///< Dimension info node.
    PI_MEMBER = 2007, ///< Member info node.
    PI_INDEX = 2008, ///< Index info node.
    PI_CASE = 2009, ///< Case info node.
    PI_DEFAULT = 2010, ///< Default info node.
    PI_TYPE = 2011, ///< Type info node.
    PI_ARRAY_MEMBER = 2012, ///< Array member info node.
    C_BASE_TYPE = 2020, ///< Base C type node.
    C_POINTER_TYPE = 2021, ///< Pointer C type node.
    C_ARRAY_TYPE = 2022, ///< Array C type node.
    C_ARRAY_TYPE_DIM = 2023, ///< Array dimension node.
    C_STRUCT_TYPE = 2024, ///< Struct C type node.
    C_STRUCT_MEMBER = 2025, ///< Struct member node.
    C_STRUCT_DEF = 2026, ///< Struct definition node.
    C_DEF_SET = 2030, ///< Definition set node.
    C_DEF_SET_SET = 2031, ///< Definition set collection node.
    CG_ANNOTATION = 2032, ///< Code generation annotation node.
    CG_ANNOTATIONS = 2033, ///< Code generation annotations node.
} NodeType;

/// Associates condition indexes with one rule.
typedef struct condIndexVal {
    Node *params; ///< Rule parameter list.
    Node *condExp; ///< Condition expression.
    Hashtable *valIndex; /* char * -> int * */ ///< Value index table.
} CondIndexVal;

/// Links one rule index entry into a doubly linked list.
typedef struct ruleIndexListNode {
    struct ruleIndexListNode *next, *prev; ///< Adjacent list nodes.
    int secondaryIndex; ///< Secondary sort key.
    int ruleIndex; ///< Index into the owning ruleset.
    CondIndexVal *condIndex; ///< Condition index data.
} RuleIndexListNode;

/// Holds all indexed entries for one rule name.
typedef struct ruleIndexList {
    char *ruleName; ///< Indexed rule name.
    RuleIndexListNode *head, *tail; ///< Bounds of the rule entry list.
} RuleIndexList;

/// Forward declaration for nested environments.
typedef struct env Env;
/// Represents one lexical environment frame.
struct env {
    Hashtable *current; ///< Bindings defined in this frame.
    Env *previous; ///< Previous frame at the same level.
    Env *lower; ///< Enclosing lower-priority frame.
};

/// Signature of a microservice adapter callable from the rule engine.
typedef Res *( SmsiFuncType )( Node **, int, Node *, ruleExecInfo_t *, int, Env *, rError_t *, Region * );
/// Pointer type for `SmsiFuncType`.
typedef SmsiFuncType *SmsiFuncTypePtr;



/// Linked list of strings.
typedef struct str_list {
    char *str; ///< Stored string value.
    struct str_list *next; ///< Next list node.
} StringList;



/// Core AST, type, and value node representation.
struct node {
    int nodeType; /* node type */ ///< Raw `NodeType` value.
    int degree; ///< Number of subtrees.
    int option; /* weather runtime coercion is needed */ ///< Option flags for typing and evaluation.
    int ival; ///< Integer payload.
    /* when this node represents a type or a pattern, this field indicates whether the trailing subtree represents varargs */
    ExprType *exprType; /* expression type */ ///< Inferred or declared expression type.
    ExprType *coercionType; /* coercion type */ ///< Runtime coercion target type.
    char *text; ///< Text payload.
    rodsLong_t expr; ///< Source position or integer payload.
    struct node **subtrees; ///< Child node array.
    char *base; ///< Source base identifier.
    double dval; ///< Floating-point payload.
    rodsLong_t lval; ///< Long integer payload.
    RuleIndexList *ruleIndexList; ///< Rule index list payload.
    SmsiFuncTypePtr func; ///< Microservice function pointer payload.
    msParam_t *param; ///< `msParam_t` payload.
};

/// Classifies rule descriptors.
typedef enum ruleType {
    RK_REL, ///< Relational rule.
    RK_FUNC, ///< Function rule.
    RK_DATA, ///< Data definition.
    RK_CONSTRUCTOR, ///< Constructor definition.
    RK_EXTERN, ///< External definition.
    /* RK_UNPARSED, */
} RuleType;

/// Describes one rule entry in a ruleset.
typedef struct {
    int id; ///< Rule identifier.
    Node *type; ///< Declared type information.
    Node *node; ///< Rule AST node.
    RuleType ruleType; ///< Rule category.
    int dynamictyping; ///< Non-zero if dynamic typing is enabled.
} RuleDesc;

/// Pointer alias for `RuleDesc`.
typedef RuleDesc *RuleDescPtr;

/// Collection of rules loaded into one ruleset.
typedef struct ruleSet {
    int len; ///< Number of populated rule entries.
    RuleDesc* rules[MAX_NUM_RULES]; ///< Rule descriptor table.
    /* Region *region; */
} RuleSet;

/// Identifies a source location.
typedef struct label {
    long exprloc; ///< Byte offset in the source.
    char *base; ///< Source base identifier.
} Label;

/// Represents one token produced by the lexer.
typedef struct token {
    NodeType type; ///< Token type.
    char text[MAX_TOKEN_TEXT_LEN + 1]; ///< Token text.
    int vars[100]; ///< Variable references discovered during lexing.
    long exprloc; ///< Source position of the token.
} Token;

/* rule engine events */
/// Enumerates rule engine debug events.
typedef enum ruleEngineEvent {
    EXEC_RULE_BEGIN, /* execute a rule */ ///< Beginning of rule execution.
    EXEC_ACTION_BEGIN, /* execute an action */ ///< Beginning of action execution.
    EXEC_MICRO_SERVICE_BEGIN, /* execute a microservice */ ///< Beginning of microservice execution.
    EXEC_RULE_END, /* execute a rule */ ///< End of rule execution.
    EXEC_ACTION_END, /* execute an action */ ///< End of action execution.
    EXEC_MICRO_SERVICE_END, /* execute a microservice */ ///< End of microservice execution.
    GOT_RULE, /* got a rule from rule index */ ///< Rule selected from an index.
    APPLY_RULE_BEGIN, /* apply a rule */ ///< Beginning of apply-rule processing.
    APPLY_RULE_END, /* apply a rule */ ///< End of apply-rule processing.
    APPLY_ALL_RULES_BEGIN, /* apply all rules */ ///< Beginning of apply-all-rules processing.
    APPLY_ALL_RULES_END, /* apply all rules */ ///< End of apply-all-rules processing.
    EXEC_MY_RULE_BEGIN, /* execute user submitted rule */ ///< Beginning of user-submitted rule execution.
    EXEC_MY_RULE_END /* execute user submitted rule */ ///< End of user-submitted rule execution.
} RuleEngineEvent;

/// Carries metadata for one rule engine event.
typedef struct ruleEngineEventParam {
    int ruleIndex; ///< Index of the rule involved in the event.
    char *actionName; ///< Action name involved in the event.
} RuleEngineEventParam;

/// Allocates a new generic node.
Node *newNode( NodeType type, const char* text, Label * exprloc, Region *r );
/// Allocates a new expression type node.
Node *newExprType( NodeType t, int degree, Node **subtrees, Region *r );
/// Allocates a fresh type variable.
ExprType *newTVar( Region *r );
/// Allocates a type variable with explicit disjuncts.
ExprType *newTVar2( int numDisjuncts, Node **disjuncts, Region *r );
/// Allocates a collection type.
ExprType *newCollType( ExprType *elemType, Region *r );
/// Allocates a tuple type.
ExprType *newTupleType( int arity, ExprType **typeArgs, Region *r );
/// Allocates a unary type node.
ExprType *newUnaryType( NodeType nodeType, ExprType *typeArg, Region *r );
/// Allocates a function type.
ExprType *newFuncType( ExprType *paramType, ExprType *retType, Region *r );
/// Allocates a vararg function type.
ExprType *newFuncTypeVarArg( int arity, int vararg, ExprType **paramTypes, ExprType *elemType, Region *r );
/// Allocates a constructor type.
ExprType *newConsType( int arity, char *cons, ExprType **paramTypes, Region *r );
/// Allocates a vararg tuple type.
ExprType *newTupleTypeVarArg( int arity, int vararg, ExprType **paramTypes, Region *r );
/// Allocates a simple built-in type.
ExprType *newSimpType( NodeType t, Region *r );
/// Allocates an error type.
ExprType *newErrorType( int errcode, Region *r );
/// Allocates an iRODS-specific type.
ExprType *newIRODSType( const char *name, Region *r );
/*ExprType *newFlexKind( int arity, ExprType **typeArgs, Region *r );*/
/// Allocates a function descriptor symbolic link.
FunctionDesc *newFuncSymLink( char *fn , int nArgs, Region *r );
/// Allocates a partial application node.
Node *newPartialApplication( Node *func, Node *arg, int nArgsLeft, Region *r );

/** Res functions */
/// Allocates an empty result value.
Res* newRes( Region *r );
/// Allocates an integer result value.
Res* newIntRes( Region *r, int n );
/// Allocates a floating-point result value.
Res* newDoubleRes( Region *r, double a );
/// Allocates a boolean result value.
Res* newBoolRes( Region *r, int n );
/// Allocates an error result value.
Res* newErrorRes( Region *r, int errcode );
/// Allocates an unspecified result value.
Res* newUnspecifiedRes( Region *r );
/// Allocates a string result value.
Res* newStringRes( Region *r, const char *s );
/// Allocates a path result value.
Res* newPathRes( Region *r, const char *s );
/// Allocates a datetime result value.
Res* newDatetimeRes( Region *r, long dt );
/// Allocates a collection result value.
Res* newCollRes( int size, ExprType *elemType, Region *r );
/// Allocates an uninterpreted result value.
Res* newUninterpretedRes( Region *r, const char *typeName, void *ioStruct, bytesBuf_t *ioBuf );
/// Allocates a tuple result value.
Res* newTupleRes( int arity, Res **compTypes, Region *r );
/// Allocates an `msParam_t` in the supplied region.
msParam_t *newMsParam( const char *typeName, void *ioStruct, bytesBuf_t *ioBuf, Region *r );

/// Allocates a new environment frame.
Env *newEnv( Hashtable *current, Env *previous, Env *lower, Region *r );
/* void deleteEnv(Env *env, int deleteCurrent); */
/// Allocates an empty `msParamArray_t`.
msParamArray_t *newMsParamArray();
/// Deallocates an `msParamArray_t`.
void deleteMsParamArray( msParamArray_t *msParamArray );

/// Allocates a typing constraint node.
TypingConstraint *newTypingConstraint( ExprType *a, ExprType *b, NodeType type, Node *node, Region *r );

/// Allocates a function descriptor for a microservice.
FunctionDesc *newFunctionFD( char* type, SmsiFuncTypePtr func, Region *r );
/// Allocates a constructor descriptor from a type name.
FunctionDesc *newConstructorFD( char* type, Region *r );
/// Allocates an external descriptor.
FunctionDesc *newExternalFD( Node* type, Region *r );
/// Allocates a constructor descriptor from a type node.
FunctionDesc *newConstructorFD2( Node* type, Region *r );
/// Allocates a deconstructor descriptor.
FunctionDesc *newDeconstructorFD( char *type, int proj, Region *r );
/// Allocates a rule-index-list descriptor.
///
/// The descriptor references a rule index list and its associated type information.
FunctionDesc *newRuleIndexListFD( RuleIndexList *ruleIndexList, ExprType *, Region *r );

/// Sets the source base for a node.
void setBase( Node *node, char *base, Region *r );
/// Allocates storage for a node's subtree array and sets its degree.
Node **setDegree( Node *node, int d, Region *r );
/// Creates a unary function application node.
Node *createUnaryFunctionNode( char *fn, Node *a, Label * exprloc, Region *r );
/// Creates a binary function application node.
Node *createBinaryFunctionNode( char *fn, Node *a, Node *b, Label * exprloc, Region *r );
/// Creates a function application node.
Node *createFunctionNode( const char *fn, Node **params, int paramsLen, Label * exprloc, Region *r );
/// Creates an actions node.
Node *createActionsNode( Node **params, int paramsLen, Label * exprloc, Region *r );
/// Creates a text literal node.
Node *createTextNode( char *t, Label * exprloc, Region *r );
/// Creates a string literal node.
Node *createStringNode( char *t, Label * exprloc, Region *r );
/// Creates an error node.
Node *createErrorNode( char *error, Label * exprloc, Region *r );

/// Allocates an empty ruleset.
RuleSet *newRuleSet( Region *r );
/// Allocates a rule descriptor.
RuleDesc *newRuleDesc( RuleType rk, Node *n, int dynamictyping, Region *r );

#endif
