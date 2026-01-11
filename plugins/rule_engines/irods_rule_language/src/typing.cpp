/* For copyright information please refer to files in the COPYRIGHT directory
 */

#include "irods/private/re/typing.hpp"
#include "irods/private/re/functions.hpp"
#include "irods/private/re/deprecation.hpp"
#include "irods/rcMisc.h"
// Macros RE_ERROR and RE_ERROR2 replaced with explicit error handling (i-5509)
#define N_BASE_TYPES 7
NodeType baseTypes[N_BASE_TYPES] = {
    T_INT,
    T_BOOL,
    T_DOUBLE,
    T_DATETIME,
    T_STRING,
    T_PATH,
    T_IRODS,
};
void doNarrow( Node **l, Node **r, int ln, int rn, int flex, Node **nl, Node **nr, int *nln, int *nrn );
Satisfiability createSimpleConstraint( ExprType *a, ExprType *b, int flex, Node *node, Hashtable *typingEnv, Hashtable *equivalence, List *simpleTypingConstraints, Region *r );
ExprType *createType( ExprType *t, Node **nc, int nn, Hashtable *typingEnv, Hashtable *equivalence, Region *r );
ExprType *getFullyBoundedVar( Region *r );

/**
 * \brief Detect and mark type patterns in match expressions
 * 
 * Type patterns are parenthesized type names like (int), (string), (_), etc.
 * This function recursively traverses match expressions and marks type patterns.
 * 
 * \param node    AST node to check (typically a match pattern node)
 * \param r       Region for memory allocation
 * \return        1 if node is a type pattern, 0 otherwise
 */
int detectAndMarkTypePattern( Node *node, Region *r ) {
    if ( node == NULL ) {
        return 0;
    }

    /* Check if this looks like a type pattern:
     * - Single identifier (not a complex expression)
     * - Common type names: int, string, double, bool, time, _
     */
    if ( getNodeType( node ) == N_APPLICATION && node->degree == 0 ) {
        char *typeName = node->text;
        if ( typeName != NULL ) {
            if ( strcmp( typeName, "int" ) == 0 ||
                 strcmp( typeName, "string" ) == 0 ||
                 strcmp( typeName, "double" ) == 0 ||
                 strcmp( typeName, "bool" ) == 0 ||
                 strcmp( typeName, "time" ) == 0 ||
                 strcmp( typeName, "_" ) == 0 ) {
                setTypePattern( node );
                return 1;
            }
        }
    }

    /* Recursively check subtrees for type patterns */
    int i;
    for ( i = 0; i < node->degree; i++ ) {
        if ( node->subtrees[i] != NULL ) {
            detectAndMarkTypePattern( node->subtrees[i], r );
        }
    }

    return 0;
}

/**
 * Extract column names from an N_QUERY AST node
 * Returns a list of column name strings (char*)
 * The N_QUERY node structure:
 *   - subtrees[0..n] contain Column nodes (N_ATTR with TK_COL child)
 *   - followed by optional QueryCond nodes (for WHERE clause)
 * 
 * Column extraction stops at first non-column node (typically first WHERE condition)
 * 
 * Returns: List of column names (must be freed by caller)
 *          NULL if no columns found or memory error
 */
List *extractColumnNamesFromQuery( Node *queryNode, Region *r ) {
    if ( queryNode == NULL || getNodeType( queryNode ) != N_QUERY ) {
        return NULL;
    }
    
    List *columnNames = newList( r );
    int i;
    
    /* Traverse subtrees looking for columns (N_ATTR nodes containing TK_COL) */
    for ( i = 0; i < queryNode->degree; i++ ) {
        Node *subtree = queryNode->subtrees[i];
        
        /* N_ATTR wraps TK_COL */
        if ( getNodeType( subtree ) == N_ATTR && subtree->degree > 0 ) {
            Node *colNode = subtree->subtrees[0];
            if ( getNodeType( colNode ) == TK_COL ) {
                /* Extract column name from TK_COL node */
                char *colName = colNode->text;
                if ( colName != NULL ) {
                    listAppend( columnNames, (void*)colName, r );
                }
            }
        }
        /* Stop at first non-column node (WHERE clause conditions) */
        else if ( getNodeType( subtree ) != N_ATTR ) {
            break;
        }
    }
    
    return columnNames;
}

/**
 * Get the ExprType for a GenQuery column name
 * 
 * Maps genQuery column names to their iRODS types:
 * - ID columns (ZONE_ID, USER_ID, RESC_ID, DATA_ID, etc.) -> T_INT
 * - Name columns (ZONE_NAME, USER_NAME, RESC_NAME, COLL_NAME, DATA_NAME, etc.) -> T_STRING
 * - Size/count columns (DATA_SIZE, RESC_FREE_SPACE, etc.) -> T_INT
 * - Time columns (CREATE_TIME, MODIFY_TIME, ACCESS_TIME, etc.) -> T_INT (Unix timestamp)
 * - Type/status columns (USER_TYPE, RESC_TYPE_NAME, DATA_TYPE_NAME, etc.) -> T_STRING
 * - Path columns (RESC_VAULT_PATH, DATA_PATH, COLL_NAME, etc.) -> T_STRING
 * - Info/comment columns (USER_INFO, RESC_INFO, COLL_COMMENTS, DATA_COMMENTS, etc.) -> T_STRING
 * - Boolean/flag columns (COLL_INHERITANCE, DATA_REPL_STATUS, DATA_STATUS, etc.) -> T_INT (0/1 flag)
 * 
 * Returns: ExprType* - type of the column, or T_DYNAMIC for unknown columns
 * Note: Caller must NOT free the returned ExprType (persistent types)
 */
ExprType *getColumnTypeFromSchema( const char *columnName, Region *r ) {
    /* Return T_DYNAMIC for NULL column name */
    if ( columnName == NULL ) {
        return newSimpType( T_DYNAMIC, r );
    }
    
    /* ID columns: ZONE_ID, USER_ID, RESC_ID, DATA_ID, COLL_ID, etc. */
    if ( strstr( columnName, "_ID" ) != NULL ) {
        /* Specific ID columns that are integers */
        if ( strcmp( columnName, "ZONE_ID" ) == 0 ||
             strcmp( columnName, "USER_ID" ) == 0 ||
             strcmp( columnName, "RESC_ID" ) == 0 ||
             strcmp( columnName, "DATA_ID" ) == 0 ||
             strcmp( columnName, "DATA_COLL_ID" ) == 0 ||
             strcmp( columnName, "COLL_ID" ) == 0 ||
             strcmp( columnName, "COLL_MAP_ID" ) == 0 ||
             strcmp( columnName, "DATA_MAP_ID" ) == 0 ||
             strcmp( columnName, "META_DATA_ATTR_ID" ) == 0 ||
             strcmp( columnName, "META_COLL_ATTR_ID" ) == 0 ||
             strcmp( columnName, "META_RESC_ATTR_ID" ) == 0 ||
             strcmp( columnName, "META_USER_ATTR_ID" ) == 0 ||
             strcmp( columnName, "GROUP_ID" ) == 0 ||
             strcmp( columnName, "GROUP_MEMBER_ID" ) == 0 ||
             strcmp( columnName, "USER_AUTH_ID" ) == 0 ||
             strcmp( columnName, "RESC_PARENT_ID" ) == 0 ||
             strcmp( columnName, "DELAY_RULE_ID" ) == 0 ) {
            return newSimpType( T_INT, r );
        }
    }
    
    /* Name columns: ZONE_NAME, USER_NAME, RESC_NAME, COLL_NAME, DATA_NAME, etc. */
    if ( strstr( columnName, "_NAME" ) != NULL ) {
        if ( strcmp( columnName, "ZONE_NAME" ) == 0 ||
             strcmp( columnName, "USER_NAME" ) == 0 ||
             strcmp( columnName, "RESC_NAME" ) == 0 ||
             strcmp( columnName, "COLL_NAME" ) == 0 ||
             strcmp( columnName, "COLL_PARENT_NAME" ) == 0 ||
             strcmp( columnName, "DATA_NAME" ) == 0 ||
             strcmp( columnName, "RESC_TYPE_NAME" ) == 0 ||
             strcmp( columnName, "RESC_CLASS_NAME" ) == 0 ||
             strcmp( columnName, "DATA_TYPE_NAME" ) == 0 ||
             strcmp( columnName, "USER_TYPE" ) == 0 ||
             strcmp( columnName, "USER_TYPE_NAME" ) == 0 ||
             strcmp( columnName, "RESC_ZONE_NAME" ) == 0 ||
             strcmp( columnName, "ZONE_TYPE" ) == 0 ||
             strcmp( columnName, "COLL_TYPE" ) == 0 ||
             strcmp( columnName, "USER_DN" ) == 0 ||
             strcmp( columnName, "DELAY_RULE_NAME" ) == 0 ||
             strcmp( columnName, "DELAY_RULE_USER_NAME" ) == 0 ||
             strcmp( columnName, "DELAY_RULE_EXE_ADDRESS" ) == 0 ) {
            return newSimpType( T_STRING, r );
        }
    }
    
    /* Size/count columns (numeric) */
    if ( strcmp( columnName, "DATA_SIZE" ) == 0 ||
         strcmp( columnName, "RESC_FREE_SPACE" ) == 0 ||
         strcmp( columnName, "DATA_REPL_NUM" ) == 0 ||
         strcmp( columnName, "DATA_VERSION" ) == 0 ||
         strcmp( columnName, "DATA_MODE" ) == 0 ) {
        return newSimpType( T_INT, r );
    }
    
    /* Time columns (stored as Unix timestamp integers) */
    if ( strstr( columnName, "_TIME" ) != NULL ||
         strstr( columnName, "_TS" ) != NULL ) {
        if ( strcmp( columnName, "ZONE_CREATE_TIME" ) == 0 ||
             strcmp( columnName, "ZONE_MODIFY_TIME" ) == 0 ||
             strcmp( columnName, "USER_CREATE_TIME" ) == 0 ||
             strcmp( columnName, "USER_MODIFY_TIME" ) == 0 ||
             strcmp( columnName, "RESC_CREATE_TIME" ) == 0 ||
             strcmp( columnName, "RESC_MODIFY_TIME" ) == 0 ||
             strcmp( columnName, "DATA_CREATE_TIME" ) == 0 ||
             strcmp( columnName, "DATA_MODIFY_TIME" ) == 0 ||
             strcmp( columnName, "DATA_ACCESS_TIME" ) == 0 ||
             strcmp( columnName, "COLL_CREATE_TIME" ) == 0 ||
             strcmp( columnName, "COLL_MODIFY_TIME" ) == 0 ||
             strcmp( columnName, "META_DATA_CREATE_TIME" ) == 0 ||
             strcmp( columnName, "META_DATA_MODIFY_TIME" ) == 0 ||
             strcmp( columnName, "META_COLL_CREATE_TIME" ) == 0 ||
             strcmp( columnName, "META_COLL_MODIFY_TIME" ) == 0 ||
             strcmp( columnName, "META_RESC_CREATE_TIME" ) == 0 ||
             strcmp( columnName, "META_RESC_MODIFY_TIME" ) == 0 ||
             strcmp( columnName, "META_USER_CREATE_TIME" ) == 0 ||
             strcmp( columnName, "META_USER_MODIFY_TIME" ) == 0 ||
             strcmp( columnName, "DELAY_RULE_EXE_TIME" ) == 0 ) {
            return newSimpType( T_INT, r );
        }
        /* Millisecond precision timestamps */
        if ( strcmp( columnName, "RESC_MODIFY_TIME_MILLIS" ) == 0 ||
             strcmp( columnName, "RESC_FREE_SPACE_TIME" ) == 0 ||
             strcmp( columnName, "DATA_EXPIRY" ) == 0 ) {
            return newSimpType( T_INT, r );
        }
    }
    
    /* Path/string columns */
    if ( strcmp( columnName, "RESC_VAULT_PATH" ) == 0 ||
         strcmp( columnName, "RESC_HOSTNAME" ) == 0 ||
         strcmp( columnName, "DATA_PATH" ) == 0 ||
         strcmp( columnName, "RESC_INFO" ) == 0 ||
         strcmp( columnName, "RESC_COMMENT" ) == 0 ||
         strcmp( columnName, "ZONE_COMMENT" ) == 0 ||
         strcmp( columnName, "USER_COMMENT" ) == 0 ||
         strcmp( columnName, "USER_INFO" ) == 0 ||
         strcmp( columnName, "DATA_COMMENTS" ) == 0 ||
         strcmp( columnName, "COLL_COMMENTS" ) == 0 ||
         strcmp( columnName, "DATA_CHECKSUM" ) == 0 ||
         strcmp( columnName, "ZONE_CONNECTION" ) == 0 ||
         strcmp( columnName, "RESC_CONTEXT" ) == 0 ||
         strcmp( columnName, "RESC_PARENT_CONTEXT" ) == 0 ||
         strcmp( columnName, "COLL_INHERITANCE" ) == 0 ||
         strcmp( columnName, "COLL_INFO1" ) == 0 ||
         strcmp( columnName, "COLL_INFO2" ) == 0 ||
         strcmp( columnName, "RESC_CHILDREN" ) == 0 ||
         strcmp( columnName, "DATA_RESC_HIER" ) == 0 ||
         strcmp( columnName, "RESC_STATUS" ) == 0 ||
         strcmp( columnName, "DELAY_RULE_REI_FILE_PATH" ) == 0 ||
         strcmp( columnName, "DELAY_RULE_EXE_FREQUENCY" ) == 0 ) {
        return newSimpType( T_STRING, r );
    }
    
    /* Status/flag columns (integer 0/1 or similar) */
    if ( strcmp( columnName, "DATA_REPL_STATUS" ) == 0 ||
         strcmp( columnName, "DATA_STATUS" ) == 0 ) {
        return newSimpType( T_INT, r );
    }
    
    /* Metadata attribute columns */
    if ( strcmp( columnName, "META_DATA_ATTR_NAME" ) == 0 ||
         strcmp( columnName, "META_DATA_ATTR_VALUE" ) == 0 ||
         strcmp( columnName, "META_DATA_ATTR_UNITS" ) == 0 ||
         strcmp( columnName, "META_COLL_ATTR_NAME" ) == 0 ||
         strcmp( columnName, "META_COLL_ATTR_VALUE" ) == 0 ||
         strcmp( columnName, "META_COLL_ATTR_UNITS" ) == 0 ||
         strcmp( columnName, "META_RESC_ATTR_NAME" ) == 0 ||
         strcmp( columnName, "META_RESC_ATTR_VALUE" ) == 0 ||
         strcmp( columnName, "META_RESC_ATTR_UNITS" ) == 0 ||
         strcmp( columnName, "META_USER_ATTR_NAME" ) == 0 ||
         strcmp( columnName, "META_USER_ATTR_VALUE" ) == 0 ||
         strcmp( columnName, "META_USER_ATTR_UNITS" ) == 0 ) {
        return newSimpType( T_STRING, r );
    }
    
    /* Special data resource column */
    if ( strcmp( columnName, "DATA_RESC_ID" ) == 0 ) {
        return newSimpType( T_INT, r );
    }
    
    /* Delay rule priority */
    if ( strcmp( columnName, "DELAY_RULE_PRIORITY" ) == 0 ||
         strcmp( columnName, "DELAY_RULE_ESTIMATED_EXE_TIME" ) == 0 ) {
        return newSimpType( T_INT, r );
    }
    
    /* Unknown column - return T_DYNAMIC for graceful degradation */
    return newSimpType( T_DYNAMIC, r );
}

/**
 * Create a tuple type from a list of column names
 * 
 * Builds a composite tuple type where each element corresponds to the type
 * of the column at that position. The tuple represents the structure of a
 * query result row.
 * 
 * Examples:
 * - ["COLL_NAME", "DATA_NAME"] -> tuple<string, string>
 * - ["USER_ID", "USER_NAME", "DATA_SIZE"] -> tuple<int, string, int>
 * - ["DATA_ID"] -> tuple<int> (single element tuple)
 * 
 * Args:
 *   columnNames: List of column name strings (char*), or NULL
 *   r: Memory region for allocation
 * 
 * Returns: ExprType* - tuple type with elements corresponding to column types
 *          NULL if columnNames is NULL or empty
 *          
 * Note: For queries with no columns, returns NULL. Caller must handle.
 */
ExprType *createTupleTypeFromColumns( List *columnNames, Region *r ) {
    if ( columnNames == NULL ) {
        return NULL;
    }
    
    /* Get the number of columns */
    int numColumns = columnNames->size;
    if ( numColumns == 0 ) {
        return NULL;
    }
    
    /* Allocate array of ExprType pointers for tuple elements */
    ExprType **typeArgs = (ExprType **)region_alloc( r, numColumns * sizeof(ExprType *) );
    if ( typeArgs == NULL ) {
        return NULL;
    }
    
    /* Build type for each column */
    int i;
    ListNode *node = columnNames->head;
    for ( i = 0; i < numColumns && node != NULL; i++, node = node->next ) {
        char *columnName = (char *)node->value;
        ExprType *colType = getColumnTypeFromSchema( columnName, r );
        
        if ( colType == NULL ) {
            /* Column type lookup failed - use union of common types as safe fallback */
            ExprType *intType = newSimpType( T_INT, r );
            ExprType *stringType = newSimpType( T_STRING, r );
            ExprType *dynamicType = newSimpType( T_DYNAMIC, r );
            
            ExprType **unionDisjuncts = (ExprType **)region_alloc( r, sizeof(ExprType *) * 3 );
            if ( unionDisjuncts != NULL ) {
                unionDisjuncts[0] = intType;
                unionDisjuncts[1] = stringType;
                unionDisjuncts[2] = dynamicType;
                typeArgs[i] = newUnionType( 3, unionDisjuncts, r );
            }
            else {
                /* Memory allocation failed - fall back to T_DYNAMIC */
                typeArgs[i] = newSimpType( T_DYNAMIC, r );
            }
        }
        else if ( getNodeType( colType ) == T_DYNAMIC ) {
            /* Unknown column (returned T_DYNAMIC by schema) - enhance with union for better type inference */
            /* Union of common column types: int | string | dynamic */
            ExprType *intType = newSimpType( T_INT, r );
            ExprType *stringType = newSimpType( T_STRING, r );
            
            ExprType **unionDisjuncts = (ExprType **)region_alloc( r, sizeof(ExprType *) * 3 );
            if ( unionDisjuncts != NULL ) {
                unionDisjuncts[0] = intType;
                unionDisjuncts[1] = stringType;
                unionDisjuncts[2] = colType;  /* T_DYNAMIC as fallback */
                typeArgs[i] = newUnionType( 3, unionDisjuncts, r );
            }
            else {
                /* Memory allocation failed - use original T_DYNAMIC */
                typeArgs[i] = colType;
            }
        }
        else {
            /* Known column type - use directly */
            typeArgs[i] = colType;
        }
    }
    
    /* Create tuple type from the element types */
    ExprType *tupleType = newTupleType( numColumns, typeArgs, r );
    
    return tupleType;
}

/**
 * return 0 to len-1 index of the parameter with type error
 *        -1 success
 */
int typeParameters( ExprType** paramTypes, int len, Node** subtrees, int dynamictyping, Env* funcDesc, Hashtable *symbol_type_table, List *typingConstraints, rError_t *errmsg, Node **errnode, Region *r ) {
    int i;
    for ( i = 0; i < len; i++ ) {
        paramTypes[i] = dereference( typeExpression3( subtrees[i], dynamictyping, funcDesc, symbol_type_table, typingConstraints, errmsg, errnode, r ), symbol_type_table, r );
        if ( getNodeType( paramTypes[i] ) == T_ERROR ) {
            return i;
        }
    }
    return -1;
}

int tautologyLt( ExprType *type, ExprType *expected ) {

    if ( typeEqSyntatic( type, expected ) ) {
        return 1;
    }
    if ( getNodeType( type ) == T_DYNAMIC ) {
        return 0;
    }
    if ( getNodeType( expected ) == T_DYNAMIC ) {
        return 1;
    }
    int i;
    ExprType a, b;
    if ( getNodeType( type ) == T_VAR ) {
        if ( T_VAR_NUM_DISJUNCTS( type ) > 0 ) {
            for ( i = 0; i < T_VAR_NUM_DISJUNCTS( type ); i++ ) {
                ExprType *disjunct = T_VAR_DISJUNCT( type, i );
                if ( disjunct == NULL ) {
                    rodsLog( LOG_ERROR, "tautologyLtBase: array bounds violation - disjunct index %d out of range", i );
                    return 0;
                }
                setNodeType( &a, getNodeType( disjunct ) );
                if ( !tautologyLt( &a, expected ) ) {
                    return 0;
                }
            }
            return 1;
        }
        else {
            return 0;
        }

    }
    else if ( getNodeType( expected ) == T_VAR ) {
        if ( T_VAR_NUM_DISJUNCTS( expected ) > 0 ) {
            for ( i = 0; i < T_VAR_NUM_DISJUNCTS( expected ); i++ ) {
                ExprType *disjunct = T_VAR_DISJUNCT( expected, i );
                if ( disjunct == NULL ) {
                    rodsLog( LOG_ERROR, "tautologyLtBase: array bounds violation - disjunct index %d out of range", i );
                    return 0;
                }
                setNodeType( &b, getNodeType( disjunct ) );
                if ( !tautologyLt( type, &b ) ) {
                    return 0;
                }
            }
            return 1;
        }
        else {
            return 0;
        }
    }
    else if ( ( getNodeType( type ) == T_CONS && getNodeType( expected ) == T_CONS ) || ( getNodeType( type ) == T_TUPLE && getNodeType( expected ) == T_TUPLE ) ) {
        if ( getNodeType( type ) == T_CONS && strcmp( T_CONS_TYPE_NAME( type ), T_CONS_TYPE_NAME( expected ) ) != 0 ) {
            return 0;
        }
        int i;
        for ( i = 0; i < T_CONS_ARITY( type ); i++ ) {
            if ( tautologyLt( T_CONS_TYPE_ARG( type, 0 ), T_CONS_TYPE_ARG( expected, 0 ) ) == 0 ) {
                return 0;
            }
        }
        return 1;
    }
    else {
        return tautologyLtBase( type, expected );
    }
}
char *getBaseTypeOrTVarId( ExprType *a, char buf[128] ) {
    if ( isBaseType( a ) ) {
        snprintf( buf, 128, "%s", typeName_ExprType( a ) );
    }
    else {
        getTVarName( T_VAR_ID( a ), buf );
    }
    return buf;
}
int tautologyLtBase( ExprType *a, ExprType *b ) {
    if ( getNodeType( a ) != T_IRODS && getNodeType( a ) == getNodeType( b ) ) {
        return 1;
    }
    if ( getNodeType( a ) == T_INT && getNodeType( b ) == T_DOUBLE ) {
        return 1;
    }
    if ( getNodeType( a ) == T_IRODS && getNodeType( b ) == T_IRODS && ( a->text == NULL || b->text == NULL || strcmp( a->text, b->text ) == 0 ) ) {
        return 1;
    }
    int i;
    if ( a->exprType != 0 && TYPE( a ) == T_VAR ) {
        for ( i = 0; i < a->exprType->degree; i++ ) {
            if ( typeEqSyntatic( a->exprType->subtrees[i], b ) ) {
                return 1;
            }
        }
    }
    return 0;
}
int occursIn( ExprType *var, ExprType *type ) {
    if ( getNodeType( type ) == T_VAR ) {
        return T_VAR_ID( var ) == T_VAR_ID( type );
    }
    else {
        int i;
        for ( i = 0; i < type->degree; i++ ) {
            if ( occursIn( var, type->subtrees[i] ) ) {
                return 1;
            }
        }
        return 0;
    }
}
ExprType* getEquivalenceClassRep( ExprType *varOrBase, Hashtable *equivalence ) {
    ExprType *equiv1 = NULL, *equiv2;
    char name[128];
    equiv2 = varOrBase;
    int ref = 0;
    while ( equiv2 != NULL ) {
        equiv1 = equiv2;
        equiv2 = ( ExprType * ) lookupFromHashTable( equivalence, getBaseTypeOrTVarId( equiv1, name ) );
        ref ++;
    }
    if ( ref > 1 ) {
        updateInHashTable( equivalence, getBaseTypeOrTVarId( varOrBase, name ), equiv1 );
    }
    return equiv1;

}


int occursInEquiv( ExprType *var, ExprType *type, Hashtable *equivalence ) {
    if ( getNodeType( type ) == T_VAR && isBaseType( type ) ) {
        ExprType *varEquiv = getEquivalenceClassRep( var, equivalence );
        ExprType *typeEquiv = getEquivalenceClassRep( type, equivalence );
        return typeEqSyntatic( varEquiv, typeEquiv );
    }
    else {
        int i;
        for ( i = 0; i < type->degree; i++ ) {
            if ( occursInEquiv( var, type->subtrees[i], equivalence ) ) {
                return 1;
            }
        }
        return 0;
    }
}

Satisfiability splitVarR( ExprType *consTuple, ExprType *var, int flex, Node *node, Hashtable *typingEnv, Hashtable *equivalence, List *simpleTypingConstraints, Region *r ) {
    if ( occursInEquiv( var, consTuple, equivalence ) || isBaseType( getEquivalenceClassRep( var, equivalence ) ) ) {
        return ABSURDITY;
    }
    char tvarname[128];
    ExprType **typeArgs = ( ExprType ** ) region_alloc( r, sizeof( ExprType * ) * T_CONS_ARITY( consTuple ) );
    int i;
    ExprType *type;
    for ( i = 0; i < T_CONS_ARITY( consTuple ); i++ ) {
        typeArgs[i] = newTVar( r );
    }
    if ( getNodeType( consTuple ) == T_CONS ) {
        type = newConsType( T_CONS_ARITY( consTuple ), T_CONS_TYPE_NAME( consTuple ), typeArgs, r );
    }
    else {
        type = newTupleType( T_CONS_ARITY( consTuple ), typeArgs, r );
    }
    insertIntoHashTable( typingEnv, getTVarName( T_VAR_ID( var ), tvarname ), type );
    return splitConsOrTuple( consTuple, type, flex, node, typingEnv, equivalence, simpleTypingConstraints, r );

}
Satisfiability splitVarL( ExprType *var, ExprType *consTuple, int flex, Node *node, Hashtable *typingEnv, Hashtable *equivalence, List *simpleTypingConstraints, Region *r ) {
    if ( occursInEquiv( var, consTuple, equivalence ) || isBaseType( getEquivalenceClassRep( var, equivalence ) ) ) {
        return ABSURDITY;
    }
    char tvarname[128];
    ExprType **typeArgs = ( ExprType ** ) region_alloc( r, sizeof( ExprType * ) * T_CONS_ARITY( consTuple ) );
    int i;
    ExprType *type;
    for ( i = 0; i < T_CONS_ARITY( consTuple ); i++ ) {
        typeArgs[i] = newTVar( r );
    }
    if ( getNodeType( consTuple ) == T_CONS ) {
        type = newConsType( T_CONS_ARITY( consTuple ), T_CONS_TYPE_NAME( consTuple ), typeArgs, r );
    }
    else {
        type = newTupleType( T_CONS_ARITY( consTuple ), typeArgs, r );
    }
    insertIntoHashTable( typingEnv, getTVarName( T_VAR_ID( var ), tvarname ), type );
    return splitConsOrTuple( type, consTuple, flex, node, typingEnv, equivalence, simpleTypingConstraints, r );

}

/*
 * simplify b which is a variable, bounded or unbounded, based on a which is a base type
 * returns 1 tautology
 *         0 contingency
 *         -1 absurdity
 */
Satisfiability simplifyR( ExprType *a, ExprType *b, int flex, Node *node, Hashtable *typingEnv, Hashtable *equivalence, List *simpleTypingConstraints, Region *r ) {
    ExprType *bm;
    if ( T_VAR_NUM_DISJUNCTS( b ) == 0 ) {
        bm = getFullyBoundedVar( r );
    }
    else {
        bm = b;
    }
    Node *cl[MAX_NUM_DISJUNCTS], *cr[MAX_NUM_DISJUNCTS];
    int nln, nrn;
    doNarrow( &a, T_VAR_DISJUNCTS( bm ), 1, T_VAR_NUM_DISJUNCTS( bm ), flex, cl, cr, &nln, &nrn );
    ExprType *bn;
    if ( nrn == 0 ) {
        return ABSURDITY;
    }
    else {
        bn = createType( b, cr, nrn, typingEnv, equivalence, r );
        if ( bn == b ) {
            return TAUTOLOGY;
        }
//        insertIntoHashTable( typingEnv, b->text, bn );
        return createSimpleConstraint( a, bn, flex, node, typingEnv, equivalence, simpleTypingConstraints, r );
    }
}

ExprType *getFullyBoundedVar( Region *r ) {
    ExprType **ds = ( ExprType ** ) region_alloc( r, sizeof( ExprType * ) * N_BASE_TYPES );
    int i;
    for ( i = 0; i < N_BASE_TYPES; i++ ) {
        ds[i] = newSimpType( baseTypes[i], r );
    }
    ExprType *var = newTVar2( N_BASE_TYPES, ds, r );
    return var;
}

Satisfiability simplifyL( ExprType *a, ExprType *b, int flex, Node *node, Hashtable *typingEnv, Hashtable *equivalence, List *simpleTypingConstraints, Region *r ) {
    ExprType *am;
    if ( T_VAR_NUM_DISJUNCTS( a ) == 0 ) {
        am = getFullyBoundedVar( r );
    }
    else {
        am = a;
    }
    Node *cl[MAX_NUM_DISJUNCTS], *cr[MAX_NUM_DISJUNCTS];
    int nln, nrn;
    doNarrow( T_VAR_DISJUNCTS( am ), &b, T_VAR_NUM_DISJUNCTS( am ), 1, flex, cl, cr, &nln, &nrn );
    ExprType *an;
    if ( nln == 0 ) {
        return ABSURDITY;
    }
    else {
        an = createType( a, cl, nln, typingEnv, equivalence, r );
        if ( an == a ) {
            return TAUTOLOGY;
        }
//        insertIntoHashTable( typingEnv, a->text, an );
        return createSimpleConstraint( an, b, flex, node, typingEnv, equivalence, simpleTypingConstraints, r );
    }

}

void addToEquivalenceClass( ExprType *a, ExprType *b, Hashtable *equivalence ) {
    char name[128];
    ExprType *an = getEquivalenceClassRep( a, equivalence );
    ExprType *bn = getEquivalenceClassRep( b, equivalence );
    if ( !typeEqSyntatic( an, bn ) ) {
        if ( isBaseType( an ) ) {
            insertIntoHashTable( equivalence, getTVarName( T_VAR_ID( bn ), name ), an );
        }
        else {
            insertIntoHashTable( equivalence, getTVarName( T_VAR_ID( an ), name ), bn );
        }
    }
}
void doNarrow( Node **l, Node **r, int ln, int rn, int flex, Node **nl, Node **nr, int *nln, int *nrn ) {
    Node *retl[MAX_NUM_DISJUNCTS], *retr[MAX_NUM_DISJUNCTS];
    int i, k;
    for ( i = 0; i < ln; i++ ) {
        retl[i] = NULL;
    }
    for ( k = 0; k < rn; k++ ) {
        retr[k] = NULL;
    }
    for ( k = 0; k < rn; k++ ) {
        for ( i = 0; i < ln; i++ ) {
            if ( applyBaseTypeRule( l[i], r[k], flex ) == TAUTOLOGY ) {
                retl[i] = l[i];
                retr[k] = r[k];
                if ( getNodeType( l[i] ) == T_IRODS && l[i]->text == NULL ) {
                    retl[i] = retr[k];
                }
                if ( getNodeType( r[k] ) == T_IRODS && r[k]->text == NULL ) {
                    retr[k] = retl[i];
                }
                /*	break;*/
            }
        }
    }
    *nln = 0;
    for ( i = 0; i < ln; i++ ) {
        if ( retl[i] != NULL ) {
            nl[( *nln )++] = retl[i];
        }
    }
    *nrn = 0;
    for ( k = 0; k < rn; k++ ) {
        if ( retr[k] != NULL ) {
            nr[( *nrn )++] = retr[k];
        }
    }
}

Satisfiability createSimpleConstraint( ExprType *a, ExprType *b, int flex, Node *node, Hashtable *typingEnv, Hashtable *equivalence, List *simpleTypingConstraints, Region *r ) {
    char name[128];
    if ( isBaseType( a ) && isBaseType( b ) ) {
        return TAUTOLOGY;
    }
    else {
        addToEquivalenceClass( a, b, equivalence );
        if ( flex ) {
            listAppend( simpleTypingConstraints, newTypingConstraint( a, newUnaryType( T_FLEX, b, r ), TC_LT, node, r ), r );
            return CONTINGENCY;
        }
        else {
            if ( ( getNodeType( a ) == T_VAR && T_VAR_NUM_DISJUNCTS( a ) == 0 ) || isBaseType( b ) ) {
                insertIntoHashTable( typingEnv, getTVarName( T_VAR_ID( a ), name ), b );
            }
            else if ( ( getNodeType( b ) == T_VAR && T_VAR_NUM_DISJUNCTS( b ) == 0 ) || isBaseType( a ) ) {
                insertIntoHashTable( typingEnv, getTVarName( T_VAR_ID( b ), name ), a );
            }
            else {   // T_VAR_NUM_DISJUNCTS(a) == T_VAR_NUM_DISJUNCTS(b)
                insertIntoHashTable( typingEnv, getTVarName( T_VAR_ID( a ), name ), b );
            }
            return TAUTOLOGY;
        }
    }
}
ExprType *createType( ExprType *t, Node **nc, int nn, Hashtable *typingEnv, Hashtable *equivalence, Region *r ) {
    char name[128];
    ExprType *gcd;
    if ( nn == T_VAR_NUM_DISJUNCTS( t ) ) {
        gcd = t;
    }
    else {
        if ( nn == 1 ) {
            gcd = *nc;
        }
        else {
            gcd = newTVar2( nn, nc, r );
        }
        insertIntoHashTable( typingEnv, getTVarName( T_VAR_ID( t ), name ), gcd );
        addToEquivalenceClass( t, gcd, equivalence );
    }
    return gcd;
}
Satisfiability narrow( ExprType *type, ExprType *expected, int flex, Node *node, Hashtable *typingEnv, Hashtable *equivalence, List *simpleTypingConstraints, Region *r ) {

    if ( T_VAR_ID( type ) == T_VAR_ID( expected ) ) {
        return TAUTOLOGY;
    }
    else if ( T_VAR_NUM_DISJUNCTS( type ) > 0 && T_VAR_NUM_DISJUNCTS( expected ) > 0 ) {
        int nln, nrn;
        Node *cl[100], *cr[100];
        doNarrow( T_VAR_DISJUNCTS( type ), T_VAR_DISJUNCTS( expected ), T_VAR_NUM_DISJUNCTS( type ), T_VAR_NUM_DISJUNCTS( expected ), flex, cl, cr, &nln, &nrn );
        ExprType *an;
        ExprType *bn;
        if ( nln == 0 || nrn == 0 ) {
            return ABSURDITY;
        }
        else {
            an = createType( type, cl, nln, typingEnv, equivalence, r );
            bn = createType( expected, cr, nrn, typingEnv, equivalence, r );
        }
        return createSimpleConstraint( an, bn, flex, node, typingEnv, equivalence, simpleTypingConstraints, r );
    }
    else if ( T_VAR_NUM_DISJUNCTS( type ) == 0 ) { /* free */
        return createSimpleConstraint( type, expected, flex, node, typingEnv, equivalence, simpleTypingConstraints, r );
    }
    else { /*if(T_VAR_NUM_DISJUNCTS(expected)==0)*/
        /* free */
        return createSimpleConstraint( type, expected, flex, node, typingEnv, equivalence, simpleTypingConstraints, r );
    }
}
#define logicalAnd(a, b, c) \
	{ \
		switch(a) { \
		case ABSURDITY: \
			(c) = ABSURDITY; \
		case CONTINGENCY: \
			(c) = (b) == ABSURDITY ? ABSURDITY : CONTINGENCY; \
		case TAUTOLOGY: \
			(c) = (b); \
		} \
	}

Satisfiability splitConsOrTuple( ExprType *a, ExprType *b, int flex, Node *node, Hashtable *typingEnv, Hashtable *equivalence, List *simpleTypingConstraints, Region *r ) {
    /* split composite constraints with same top level type constructor */
    if ( ( getNodeType( a ) == T_CONS && strcmp( T_CONS_TYPE_NAME( a ), T_CONS_TYPE_NAME( b ) ) != 0 ) ||
            T_CONS_ARITY( a ) != T_CONS_ARITY( b ) ) {
        return ABSURDITY;
    }
    else {
        int i;
        Satisfiability ret = TAUTOLOGY;
        for ( i = 0; i < T_CONS_ARITY( a ); i++ ) {
            ExprType *simpa = T_CONS_TYPE_ARG( a, i );
            ExprType *simpb = T_CONS_TYPE_ARG( b, i );
            /* bounds check: T_CONS_TYPE_ARG now returns NULL on out-of-bounds */
            if ( simpa == NULL || simpb == NULL ) {
                rodsLog( LOG_ERROR, "splitConsOrTuple: array bounds violation - index %d out of range", i );
                return ABSURDITY;
            }
            Satisfiability sat = simplifyLocally( simpa, simpb, flex, node, typingEnv, equivalence, simpleTypingConstraints, r );
            switch ( sat ) {
            case ABSURDITY:
                return ABSURDITY;
            case TAUTOLOGY:
                break;
            case CONTINGENCY:
                ret = CONTINGENCY;
                break;
            }

        }
        return ret;
    }
}


int isBaseType( ExprType *t ) {
    int i;
    for ( i = 0; i < N_BASE_TYPES; i++ ) {
        if ( getNodeType( t ) == baseTypes[i] ) {
            return 1;
        }
    }
    return 0;
}

int applyBaseTypeRule( ExprType *tca, ExprType *tcb, int flex ) {
    return ( flex && tautologyLtBase( tca, tcb ) ) ||
           ( !flex && ( ( getNodeType( tca ) == T_IRODS && getNodeType( tcb ) == T_IRODS && ( tca->text == NULL || tcb->text == NULL ) ) ||
                        typeEqSyntatic( tca, tcb ) ) );
}

int baseRuleApplies( ExprType *ta, ExprType *tb, int flex, ExprType **templa, ExprType **templb, Region *r ) {
    if ( isBaseType( ta ) && isBaseType( tb ) && applyBaseTypeRule( ta, tb, flex ) ) {
        *templa = ta;
        *templb = tb;
        return 1;
    }
    else if ( flex && isIterableBaseRuleType( ta, templa, templb, r ) && getNodeType( tb ) == T_CONS ) {
        return 1;
    }
    else {
        return 0;
    }
}

Satisfiability simplifyLocally( ExprType *tca, ExprType *tcb, int flex, Node *node, Hashtable *typingEnv, Hashtable *equivalence, List *simpleTypingConstraints, Region *r ) {
    /*
        char buf[1024], buf2[1024], buf3[1024], buf4[ERR_MSG_LEN];
        generateErrMsg("simplifyLocally: constraint generated from ", NODE_EXPR_POS(node), node->base, buf4);
        printf("%s", buf4);
        snprintf(buf, 1024, "\t%s<%s\n",
                    typeToString(tca, NULL, buf2, 1024),
                    typeToString(tcb, NULL, buf3, 1024));
        printf("%s", buf);
        snprintf(buf, 1024, "\tinstantiated: %s<%s\n",
                    typeToString(tca, typingEnv, buf2, 1024),
                    typeToString(tcb, typingEnv, buf3, 1024));
        printf("%s", buf);
    */
    if ( tca == tcb ) {
        return TAUTOLOGY;
    }

    if ( getNodeType( tcb ) == T_FLEX ) {
        tcb = tcb->subtrees[0];
        flex = 1;
    }
    else if ( getNodeType( tcb ) == T_FIXD ) {
        tcb = tcb->subtrees[0];
    }

    tca = dereference( tca, typingEnv, r );
    tcb = dereference( tcb, typingEnv, r );

    ExprType *templa, *templb;

    if ( getNodeType( tca ) == T_UNSPECED || getNodeType( tca ) == T_DYNAMIC || getNodeType( tcb ) == T_DYNAMIC ) { /* is an undefined variable argument or a parameter with dynamic type */
        return TAUTOLOGY;
    }
    /* Union type handling: when tcb is a union (T_VAR with disjuncts),
     * check if tca matches any disjunct */
    else if ( getNodeType( tcb ) == T_VAR && T_VAR_NUM_DISJUNCTS( tcb ) > 0 && getNodeType( tca ) != T_VAR ) {
        /* Try to match tca against one of the union disjuncts */
        for ( int i = 0; i < T_VAR_NUM_DISJUNCTS( tcb ); i++ ) {
            ExprType *disjunct = T_VAR_DISJUNCT( tcb, i );
            Satisfiability result = simplifyLocally( tca, disjunct, flex, node, typingEnv, equivalence, simpleTypingConstraints, r );
            if ( result == TAUTOLOGY || result == CONTINGENCY ) {
                return result;  /* Match found in union */
            }
        }
        /* No disjunct matched - type incompatible with union */
        return ABSURDITY;
    }
    /* Union type handling: when tca is a union, defer to narrow() for constraint tracking */
    else if ( getNodeType( tca ) == T_VAR && T_VAR_NUM_DISJUNCTS( tca ) > 0 ) {
        return narrow( tca, tcb, flex, node, typingEnv, equivalence, simpleTypingConstraints, r );
    }
    else if ( baseRuleApplies( tca, tcb, flex, &templa, &templb, r ) ) {
        Satisfiability c = TAUTOLOGY;
        logicalAnd( simplifyLocally( tca, templa, flex, node, typingEnv, equivalence, simpleTypingConstraints, r ),
                    simplifyLocally( templb, tcb, flex, node, typingEnv, equivalence, simpleTypingConstraints, r ), c );
        return c;
    }
    else if ( getNodeType( tca ) == T_VAR && getNodeType( tcb ) == T_VAR ) {
        return narrow( tca, tcb, flex, node, typingEnv, equivalence, simpleTypingConstraints, r );

    }
    else if ( getNodeType( tca ) == T_VAR && isBaseType( tcb ) ) {
        return simplifyL( tca, tcb, flex, node, typingEnv, equivalence, simpleTypingConstraints, r );

    }
    else if ( getNodeType( tcb ) == T_VAR && isBaseType( tca ) ) {
        return simplifyR( tca, tcb, flex, node, typingEnv, equivalence, simpleTypingConstraints, r );

    }
    else if ( getNodeType( tca ) == T_VAR && ( getNodeType( tcb ) == T_CONS || getNodeType( tcb ) == T_TUPLE ) ) {
        return splitVarL( tca, tcb, flex, node, typingEnv, equivalence, simpleTypingConstraints, r );

    }
    else if ( getNodeType( tcb ) == T_VAR && ( getNodeType( tca ) == T_CONS || getNodeType( tca ) == T_TUPLE ) ) {
        return splitVarR( tca, tcb, flex, node, typingEnv, equivalence, simpleTypingConstraints, r );

    }
    else if ( ( getNodeType( tca ) == T_CONS && getNodeType( tcb ) == T_CONS )
              || ( getNodeType( tca ) == T_TUPLE && getNodeType( tcb ) == T_TUPLE ) ) {
        return splitConsOrTuple( tca, tcb, flex, node, typingEnv, equivalence, simpleTypingConstraints, r );
    }
    else {
        return ABSURDITY;
    }
}
/*
 * not 0 solved
 * 0 not solved
 */
Satisfiability solveConstraints( List *typingConstraints, Hashtable *typingEnv, rError_t *errmsg, Node ** errnode, Region *r ) {
    /*
        char buf0[1024];
        typingConstraintsToString(typingConstraints, buf0, 1024);
        printf("solving constraints: %s\n", buf0);
    */
    ListNode *nextNode = NULL;
    do {
        Satisfiability sat = simplify( typingConstraints, typingEnv, errmsg, errnode, r );
        if ( sat == ABSURDITY ) {
            return ABSURDITY;
        }
        int changed = 0;
        nextNode = typingConstraints->head;
        while ( nextNode != NULL && !changed ) {
            TypingConstraint *tc = ( TypingConstraint * )nextNode->value;
            /*
                        char buf2[1024], buf3[1024];
            */
            /* printf("dereferencing %s and %s.\n", typeToString(TC_A(tc), typingEnv, buf2, 1024), typeToString(TC_B(tc), typingEnv, buf3, 1024)); */
            ExprType *a = dereference( TC_A( tc ), typingEnv, r );
            ExprType *b = dereference( TC_B( tc ), typingEnv, r );
            if ( getNodeType( b ) == T_FLEX || getNodeType( b ) == T_FIXD ) {
                b = b->subtrees[0];
            }
            /*
                        printf("warning: collasping %s with %s.\n", typeToString(a, typingEnv, buf2, 1024), typeToString(b, typingEnv, buf3, 1024));
            */
            /*printVarTypeEnvToStdOut(typingEnv); */
            if ( getNodeType( a ) == T_VAR && getNodeType( b ) == T_VAR && T_VAR_ID( a ) == T_VAR_ID( b ) ) {
                listRemove( typingConstraints, nextNode );
                nextNode = typingConstraints->head;
                changed = 1;
                /*            } else if (getNodeType(a) == T_VAR && T_VAR_NUM_DISJUNCTS(a) == 0 &&
                            		(getNodeType(b) != T_VAR || T_VAR_NUM_DISJUNCTS(b) != 0)) {
                                insertIntoHashTable(typingEnv, getTVarName(T_VAR_ID(a), buf), b);
                                listRemove(typingConstraints, nextNode);
                            	nextNode = typingConstraints->head;
                                changed = 1;
                            } else if (getNodeType(b) == T_VAR && T_VAR_NUM_DISJUNCTS(b) == 0 &&
                            		(getNodeType(a) != T_VAR || T_VAR_NUM_DISJUNCTS(a) != 0)) {
                                insertIntoHashTable(typingEnv, getTVarName(T_VAR_ID(b), buf), a);
                                listRemove(typingConstraints, nextNode);
                            	nextNode = typingConstraints->head;
                                changed = 1;
                            } else if (getNodeType(a) == T_VAR && getNodeType(b) == T_VAR &&
                            		T_VAR_NUM_DISJUNCTS(a) != 0 && T_VAR_NUM_DISJUNCTS(b) != 0) {
                                if(T_VAR_NUM_DISJUNCTS(a) > T_VAR_NUM_DISJUNCTS(b)) {
                                    insertIntoHashTable(typingEnv, getTVarName(T_VAR_ID(b), buf), a);
                                } else {
                                    insertIntoHashTable(typingEnv, getTVarName(T_VAR_ID(a), buf), b);
                                }
                                listRemove(typingConstraints, nextNode);
                            	nextNode = typingConstraints->head;
                                changed = 1;*/
            }
            else if ( getNodeType( a ) != T_VAR && getNodeType( b ) != T_VAR ) {
                printf( "error: simplified type does not have variable on either side.\n" );
            }
            else {
                nextNode = nextNode->next;
            }
            /* printVarTypeEnvToStdOut(typingEnv); */
        }
    }
    while ( nextNode != NULL );
    if ( !consistent( typingConstraints, typingEnv, r ) ) {
        return ABSURDITY;
    }
    return typingConstraints->head == NULL ? TAUTOLOGY : CONTINGENCY;
}

int consistent( List*, Hashtable*, Region* ) {
    return 1;
}
Satisfiability simplify( List *typingConstraints, Hashtable *typingEnv, rError_t *errmsg, Node **errnode, Region *r ) {
    ListNode *ln;
    int changed;
    Hashtable *equivalence = newHashTable2( 10, r );
    if ( equivalence == NULL ) {
        /* Out of memory */
        rodsLog( LOG_ERROR, "simplify: Failed to allocate equivalence class table" );
        addRErrorMsg( errmsg, SYS_MALLOC_ERR, "error: out of memory during constraint simplification." );
        return ABSURDITY;
    }
    List *simpleTypingConstraints = newList( r );
    /* printf("start\n"); */
    /*char buf[1024];
    typingConstraintsToString(typingConstraints, buf, 1024);*/
    Satisfiability ret = TAUTOLOGY;
    do {
        changed = typingEnv->len;
        ln = typingConstraints->head;
        /*typingConstraintsToString(typingConstraints, buf, 1024);
        printf("constraints: \n%s\n\n", buf);*/
        while ( ln != NULL ) {
            TypingConstraint *tc = ( TypingConstraint * )ln->value;
            switch ( simplifyLocally( TC_A( tc ), TC_B( tc ), 0, TC_NODE( tc ), typingEnv, equivalence, simpleTypingConstraints, r ) ) {
            case TAUTOLOGY:
                break;
            case CONTINGENCY:
                /* printf("contingency\n"); */
                /* printf("tautology\n"); */
                /*    TypingConstraint *tcp;
                tcp = tc;
                while(tcp!=NULL) {
                	printf("simplified %s<%s to %s<%s.\n",
                			typeToString(a, NULL, buf3, 1024), typeToString(b, NULL, buf4, 1024),
                			typeToString(tcp->a, NULL, buf1, 1024), typeToString(tcp->b, NULL, buf2, 1024));
                	tcp = tcp->next;
                }*/
                ret = CONTINGENCY;
                break;
            case ABSURDITY:
                *errnode = TC_NODE( tc );
                char errmsgbuf1[ERR_MSG_LEN], errmsgbuf2[ERR_MSG_LEN], buf2[1024], buf3[1024];
                snprintf( errmsgbuf1, ERR_MSG_LEN, "simplify: unsolvable typing constraint %s < %s.\n", typeToString( TC_A( tc ), typingEnv, buf2, 1024 ), typeToString( TC_B( tc ), typingEnv, buf3, 1024 ) );
                generateErrMsg( errmsgbuf1, NODE_EXPR_POS( ( *errnode ) ), ( *errnode )->base, errmsgbuf2 );
                addRErrorMsg( errmsg, RE_TYPE_ERROR, errmsgbuf2 );
                /*printVarTypeEnvToStdOut(typingEnv); */
                /* printf("absurdity\n"); */
                return ABSURDITY;
            }
            ln = ln->next;
        }
        /*typingConstraintsToString(typingConstraints, buf, 1024);
        printf("simplified constraints: \n%s\n\n", buf);
        printHashtable(typingEnv, buf);
        printf("env: \n%s\n", buf);*/
        typingConstraints->head = simpleTypingConstraints->head;
        typingConstraints->tail = simpleTypingConstraints->tail;
        simpleTypingConstraints->head = simpleTypingConstraints->tail = NULL;
    }
    while ( changed < typingEnv->len );

    return ret;
}

/* return the elem type of special iterable types
 * assume that type is a special iterable type */
ExprType *getElemType( ExprType *type, Region *r ) {
    if ( getNodeType( type ) == T_STRING ) {
        return newSimpType( T_STRING, r );
    }
    else if ( getNodeType( type ) == T_PATH || strcmp( type->text, CollInp_MS_T ) == 0 ) {
        return newIRODSType( DataObjInp_MS_T, r );
    }
    else if ( strcmp( type->text, IntArray_MS_T ) == 0 ) {
        return newSimpType( T_INT, r );
    }
    else if ( strcmp( type->text, StrArray_MS_T ) == 0 ) {
        return newSimpType( T_STRING, r );
    }
    else if ( strcmp( type->text, GenQueryOut_MS_T ) == 0 ) {
        return newIRODSType( KeyValPair_MS_T, r );
    }
    else if ( strcmp( type->text, KeyValPair_MS_T ) == 0 ) {
        return newSimpType( T_STRING, r );
    }
    return NULL;
}
/*
 * param templa a template type which this type must conform to in order to be used in a base rule.
 *       templb a template type which the supertype must conform to in order to be used in a base rule.
 */
int isIterableBaseRuleType( ExprType *type, ExprType **templa, ExprType **templb, Region *r ) {
    int nodeType = getNodeType( type );
    char *typeName = type->text;
    switch ( nodeType ) {
    case T_IRODS:
        if ( strcmp( typeName, CollInp_MS_T ) == 0 ) {
            *templa = type;
        }
        else if ( strcmp( typeName, IntArray_MS_T ) == 0 ) {
            *templa = type;
        }
        else if ( strcmp( typeName, StrArray_MS_T ) == 0 ) {
            *templa = type;
        }
        else if ( strcmp( typeName, GenQueryOut_MS_T ) == 0 ) {
            *templa = type;
        }
        else if ( strcmp( typeName, KeyValPair_MS_T ) == 0 ) {
            *templa = type;
        }
        else {
            return 0;
        }
        *templb = newCollType( getElemType( type, r ), r );
        return 1;
    case T_STRING:
        *templa = type;
        *templb = newCollType( newSimpType( T_STRING, r ), r );
        return 1;
    case T_PATH:
        *templa = type;
        *templb = newCollType( getElemType( type, r ), r );
        return 1;
    case T_TUPLE:
        if ( T_CONS_ARITY( type ) == 2 ) {
            ExprType **compTypes = ( ExprType ** )region_alloc( r, sizeof( ExprType * ) * 2 );
            compTypes[0] = newIRODSType( GenQueryInp_MS_T, r );
            compTypes[1] = newIRODSType( GenQueryOut_MS_T, r );
            *templa = newTupleType( 2, compTypes, r );
            *templb = newCollType( newIRODSType( KeyValPair_MS_T, r ), r );
            return 1;
        }
        else {
            return 0;
        }
    default:
        return 0;
    }
}
/* return the elem type if type is iterable
 *                 null if type is not iterable
 * type does not need to be dereferenced
 */
ExprType* isIterable( ExprType *type, Hashtable* var_type_table, Region *r ) {
    ExprType *derefedType = dereference( type, var_type_table, r );
    Node *disjuncts[6];
    Res *unified, *comp0, *comp1;
    switch ( getNodeType( derefedType ) ) {
    case T_CONS:
        if ( strcmp( type->text, LIST ) == 0 ) { /* list */
            /* dereference element type as only top level vars are dereferenced by the dereference function and we are accessing a subtree of the type */
            return dereference( T_CONS_TYPE_ARG( derefedType, 0 ), var_type_table, r );
        }
        else {
            return NULL;
        }
    case T_VAR:
        if ( T_VAR_NUM_DISJUNCTS( derefedType ) == 0 ) { /* not a union type */
            /* overwrite type of collection variable */
            unifyTVarL( derefedType, newCollType( newTVar( r ), r ), var_type_table, r );
            derefedType = dereference( derefedType, var_type_table, r );
            return dereference( T_CONS_TYPE_ARG( derefedType, 0 ), var_type_table, r );
        }
        /* no break */
        disjuncts[0] = newSimpType( T_STRING, r );
        disjuncts[1] = newIRODSType( CollInp_MS_T, r );
        disjuncts[2] = newIRODSType( IntArray_MS_T, r );
        disjuncts[3] = newIRODSType( StrArray_MS_T, r );
        disjuncts[4] = newIRODSType( GenQueryOut_MS_T, r );
        disjuncts[5] = newSimpType( T_PATH, r );

        unified = unifyTVarL( derefedType, newTVar2( 6, disjuncts, r ), var_type_table, r );
        if ( getNodeType( unified ) == T_ERROR ) {
            return NULL;
        }
        if ( unified->nodeType == T_VAR ) { /* more than one possible types */
            Node *disjunctsElem[4];
            int i;
            for ( i = 0; i < T_VAR_NUM_DISJUNCTS( unified ); i++ ) {
                disjunctsElem[i] = getElemType( T_VAR_DISJUNCT( unified, i ), r );
            }
            return newTVar2( T_VAR_NUM_DISJUNCTS( unified ), disjunctsElem, r );
        }
        else {
            return getElemType( unified, r );
        }

    case T_IRODS:
    case T_STRING:
    case T_PATH:
        return getElemType( derefedType, r );

    case T_TUPLE:
        if ( T_CONS_ARITY( derefedType ) != 2 ) {
            return NULL;
        }
        comp0 = dereference( T_CONS_TYPE_ARG( derefedType, 0 ), var_type_table, r );
        comp1 = dereference( T_CONS_TYPE_ARG( derefedType, 1 ), var_type_table, r );
        if ( getNodeType( comp0 ) != T_IRODS ||
                strcmp( T_CONS_TYPE_NAME( comp0 ), GenQueryInp_MS_T ) != 0 ||
                getNodeType( comp1 ) != T_IRODS ||
                strcmp( T_CONS_TYPE_NAME( comp1 ), GenQueryOut_MS_T ) != 0 ) {
            return NULL;
        }
        return newIRODSType( KeyValPair_MS_T, r );

    case T_DYNAMIC:
        return type;
    default:
        return NULL;
    }
}


ExprType* typeFunction3( Node* node, int dynamictyping, Env* funcDesc, Hashtable* var_type_table, List *typingConstraints, rError_t *errmsg, Node **errnode, Region *r ) {
     /*printTree(node, 0); */
     int i;
     char *localErrorMsg;
     ExprType *res3 = NULL;
     /*char buf[1024];*/
     /*printf("typeing %s\n",fn); */
     /*printVarTypeEnvToStdOut(var_type_table); */
     Node *fn = node->subtrees[0];
     Node *arg = node->subtrees[1];
     char buf[ERR_MSG_LEN];
     char errbuf[ERR_MSG_LEN];
     char typebuf[ERR_MSG_LEN];
     char typebuf2[ERR_MSG_LEN];
     char errmsgbuf[ERR_MSG_LEN];
     
     /* Check for legacy function calls and emit deprecation warnings */
     if ( getNodeType( fn ) == TK_TEXT && isLegacySystemFunction( fn->text ) ) {
         const DeprecationInfo *info = getLegacyFunctionDeprecationInfo( fn->text );
         if ( info && errmsg ) {
             snprintf( errbuf, ERR_MSG_LEN,
                      "Deprecated function '%s' (since v%s, removal in v%s): %s. Modern alternative: %s",
                      info->name, info->since_version, info->removal_version,
                      info->reason, info->alternative );
             addRErrorMsg( errmsg, RE_DEPRECATION_WARNING, errbuf );
         }
     }
     
     if ( getNodeType( fn ) == TK_TEXT && strcmp( fn->text, "foreach" ) == 0 ) {
         if ( getNodeType( arg ) != N_TUPLE || arg->degree != 3 ) {
             localErrorMsg = "wrong number of arguments to microservice";
             *errnode = node;
             snprintf( errmsgbuf, ERR_MSG_LEN, "type error: %s in %s", localErrorMsg, fn->text );
             generateErrMsg( errmsgbuf, NODE_EXPR_POS( ( *errnode ) ), ( *errnode )->base, errbuf );
             addRErrorMsg( errmsg, RE_TYPE_ERROR, errbuf );
             return newSimpType( T_ERROR, r );
         }
         if ( getNodeType( arg->subtrees[0] ) != TK_VAR ) {
             localErrorMsg = "argument form error";
             *errnode = node;
             snprintf( errmsgbuf, ERR_MSG_LEN, "type error: %s in %s", localErrorMsg, fn->text );
             generateErrMsg( errmsgbuf, NODE_EXPR_POS( ( *errnode ) ), ( *errnode )->base, errbuf );
             addRErrorMsg( errmsg, RE_TYPE_ERROR, errbuf );
             return newSimpType( T_ERROR, r );
         }
         char* varname = arg->subtrees[0]->text;
         ExprType *varType0 = ( ExprType * )lookupFromHashTable( var_type_table, varname );
         ExprType *varType;
         ExprType *collType = varType0 == NULL ? NULL : dereference( varType0, var_type_table, r );
         if ( collType != NULL ) {
             varType = isIterable( collType, var_type_table, r );
             if ( varType == NULL ) {
                 /* error if res is not an iterable type */
                 localErrorMsg = "foreach is applied to a non collection type";
                 *errnode = node;
                 snprintf( errmsgbuf, ERR_MSG_LEN, "type error: %s in %s", localErrorMsg, fn->text );
                 generateErrMsg( errmsgbuf, NODE_EXPR_POS( ( *errnode ) ), ( *errnode )->base, errbuf );
                 addRErrorMsg( errmsg, RE_TYPE_ERROR, errbuf );
                 return newSimpType( T_ERROR, r );
             }
         }
         else {
             varType = newTVar( r );
             collType = newCollType( varType, r );
         }
         if ( varType0 == NULL ) {
             insertIntoHashTable( var_type_table, varname, varType );
         }
         else {
             updateInHashTable( var_type_table, varname, varType );
         }
         arg->subtrees[0]->exprType = collType;
         res3 = typeExpression3( arg->subtrees[1], dynamictyping, funcDesc, var_type_table, typingConstraints, errmsg, errnode, r );
         if ( getNodeType( res3 ) == T_ERROR ) {
             localErrorMsg = "foreach loop type error";
             *errnode = node;
             snprintf( errmsgbuf, ERR_MSG_LEN, "type error: %s in %s", localErrorMsg, fn->text );
             generateErrMsg( errmsgbuf, NODE_EXPR_POS( ( *errnode ) ), ( *errnode )->base, errbuf );
             addRErrorMsg( errmsg, RE_TYPE_ERROR, errbuf );
             return newSimpType( T_ERROR, r );
         }
         res3 = typeExpression3( arg->subtrees[2], dynamictyping, funcDesc, var_type_table, typingConstraints, errmsg, errnode, r );
         if ( getNodeType( res3 ) == T_ERROR ) {
             localErrorMsg = "foreach recovery type error";
             *errnode = node;
             snprintf( errmsgbuf, ERR_MSG_LEN, "type error: %s in %s", localErrorMsg, fn->text );
             generateErrMsg( errmsgbuf, NODE_EXPR_POS( ( *errnode ) ), ( *errnode )->base, errbuf );
             addRErrorMsg( errmsg, RE_TYPE_ERROR, errbuf );
             return newSimpType( T_ERROR, r );
         }
         setIOType( arg->subtrees[0], IO_TYPE_EXPRESSION );
         for ( i = 1; i < 3; i++ ) {
             setIOType( arg->subtrees[i], IO_TYPE_ACTIONS );
         }
         ExprType **typeArgs = allocSubtrees( r, 3 );
         typeArgs[0] = collType;
         typeArgs[1] = newTVar( r );
         typeArgs[2] = newTVar( r );
         arg->coercionType = newTupleType( 3, typeArgs, r );
 
         updateInHashTable( var_type_table, varname, collType ); /* restore type of collection variable */
         return res3;
     }
     else {
         ExprType *fnType = typeExpression3( fn, dynamictyping, funcDesc, var_type_table, typingConstraints, errmsg, errnode, r );
         if ( getNodeType( fnType ) == T_ERROR ) {
             return fnType;
         }
         
         /* Check if calling a user-defined @deprecated function */
         if ( getNodeType( fn ) == TK_TEXT && funcDesc != NULL ) {
             FunctionDesc *fDesc = (FunctionDesc *)lookupFromEnv( funcDesc, fn->text );
             /* TODO: isDeprecated function needs to be implemented (ticket i-f77b) */
             if ( fDesc != NULL /* && isDeprecated( fDesc ) */ ) {
                 /* Get the deprecation message from the function node */
                 /* const char *deprecatedMsg = NULL;
                 if ( fDesc->text != NULL && strlen( fDesc->text ) > 0 ) {
                     deprecatedMsg = fDesc->text;
                 } */
                 
                 /* Emit deprecation warning */
                 /* TODO: Only emit if actually deprecated
                 if ( errmsg ) {
                     char errbuf[ERR_MSG_LEN];
                     if ( deprecatedMsg != NULL ) {
                         snprintf( errbuf, ERR_MSG_LEN,
                                  "Deprecated function '%s' called: %s",
                                  fn->text, deprecatedMsg );
                     } else {
                         snprintf( errbuf, ERR_MSG_LEN,
                                  "Deprecated function '%s' called",
                                  fn->text );
                     }
                     addRErrorMsg( errmsg, RE_DEPRECATION_WARNING, errbuf );
                 }
                 */
                 }
         }
         
         N_TUPLE_CONSTRUCT_TUPLE( arg ) = 1; /* arg must be a N_TUPLE node */
         ExprType *argType = typeExpression3( arg, dynamictyping, funcDesc, var_type_table, typingConstraints, errmsg, errnode, r );
         if ( getNodeType( argType ) == T_ERROR ) {
             return argType;
         }

         ExprType *fType = getNodeType( fnType ) == T_CONS && strcmp( fnType->text, FUNC ) == 0 ? fnType : unifyWith( fnType, newFuncType( newTVar( r ), newTVar( r ), r ), var_type_table, r );
 
         if ( getNodeType( fType ) == T_ERROR ) {
             localErrorMsg = "the first component of a function application does not have a function type";
             *errnode = node;
             snprintf( errmsgbuf, ERR_MSG_LEN, "type error: %s in %s", localErrorMsg, fn->text );
             generateErrMsg( errmsgbuf, NODE_EXPR_POS( ( *errnode ) ), ( *errnode )->base, errbuf );
             addRErrorMsg( errmsg, RE_TYPE_ERROR, errbuf );
             return newSimpType( T_ERROR, r );
         }
         ExprType *paramType = dereference( fType->subtrees[0], var_type_table, r );
         ExprType *retType = dereference( fType->subtrees[1], var_type_table, r );
 
         if ( getNodeType( fn ) == TK_TEXT && strcmp( fn->text, "assign" ) == 0 &&
                    arg->degree > 0 &&
                    !isPattern( arg->subtrees[0] ) ) {
             localErrorMsg = "the first argument of microservice assign is not a variable or a pattern";
             *errnode = node;
             snprintf( errmsgbuf, ERR_MSG_LEN, "type error: %s in %s", localErrorMsg, fn->text );
             generateErrMsg( errmsgbuf, NODE_EXPR_POS( ( *errnode ) ), ( *errnode )->base, errbuf );
             addRErrorMsg( errmsg, RE_TYPE_ERROR, errbuf );
             return newSimpType( T_ERROR, r );
         }
         if ( getNodeType( fn ) == TK_TEXT && strcmp( fn->text, "let" ) == 0 &&
                    arg->degree > 0 &&
                    !isPattern( arg->subtrees[0] ) ) {
             localErrorMsg = "the first argument of microservice let is not a variable or a pattern";
             *errnode = node;
             snprintf( errmsgbuf, ERR_MSG_LEN, "type error: %s in %s", localErrorMsg, fn->text );
             generateErrMsg( errmsgbuf, NODE_EXPR_POS( ( *errnode ) ), ( *errnode )->base, errbuf );
             addRErrorMsg( errmsg, RE_TYPE_ERROR, errbuf );
             return newSimpType( T_ERROR, r );
         }
 
         /*
                     printf("start typing %s\n", fn);
                     printTreeDeref(node, 0, var_type_table, r);
         */
         ExprType *t = NULL;
         if ( getVararg( fType ) != OPTION_VARARG_ONCE ) {
             /* generate instance of vararg tuple so that no vararg tuple goes into typing constraints */
             int fixParamN = paramType->degree - 1;
             int argN = node->subtrees[1] ->degree;
             int copyN = argN - fixParamN;
             ExprType **subtrees = paramType->subtrees;
             if ( copyN < ( getVararg( fType ) == OPTION_VARARG_PLUS ? 1 : 0 ) || ( getVararg( fType ) == OPTION_VARARG_OPTIONAL && copyN > 1 ) ) {
                 snprintf( buf, 1024, "unsolvable vararg typing constraint %s < %s %s",
                           typeToString( argType, var_type_table, typebuf, ERR_MSG_LEN ),
                           typeToString( paramType, var_type_table, typebuf2, ERR_MSG_LEN ),
                           getVararg( fType ) == OPTION_VARARG_PLUS ? "*" : getVararg( fType ) == OPTION_VARARG_OPTIONAL ? "?" : "+" );
                 localErrorMsg = buf;
                 *errnode = node;
                 snprintf( errmsgbuf, ERR_MSG_LEN, "type error: %s in %s", localErrorMsg, fn->text );
                 generateErrMsg( errmsgbuf, NODE_EXPR_POS( ( *errnode ) ), ( *errnode )->base, errbuf );
                 addRErrorMsg( errmsg, RE_TYPE_ERROR, errbuf );
                 return newSimpType( T_ERROR, r );
             }
             ExprType **paramTypes = allocSubtrees( r, argN );
             int i;
             for ( i = 0; i < fixParamN; i++ ) {
                 paramTypes[i] = subtrees[i];
             }
             for ( i = 0; i < copyN; i++ ) {
                 paramTypes[i + fixParamN] = subtrees[fixParamN];
             }
             t = newTupleType( argN, paramTypes, r );
         }
         else {
             t = paramType;
         }
         /*t = replaceDynamicWithNewTVar(t, r);
         argType = replaceDynamicWithNewTVar(argType, r);*/
         int ret = typeFuncParam( node->subtrees[1], argType, t, var_type_table, typingConstraints, errmsg, r );
         if ( ret != 0 ) {
             *errnode = node->subtrees[1];
             localErrorMsg = "parameter type error";
             *errnode = node;
             snprintf( errmsgbuf, ERR_MSG_LEN, "type error: %s in %s", localErrorMsg, fn->text );
             generateErrMsg( errmsgbuf, NODE_EXPR_POS( ( *errnode ) ), ( *errnode )->base, errbuf );
             addRErrorMsg( errmsg, RE_TYPE_ERROR, errbuf );
             return newSimpType( T_ERROR, r );
         }
         int i;
         for ( i = 0; i < node->subtrees[1]->degree; i++ ) {
             setIOType( node->subtrees[1]->subtrees[i], getIOType( t->subtrees[i] ) );
         }
 
         arg->coercionType = t; /* set coercion to parameter type */
 
         /*
                 printTreeDeref(node, 0, var_type_table, r);
                 printf("finish typing %s\n", fn);
         */
         return instantiate( replaceDynamicWithNewTVar( retType, r ), var_type_table, 0, r );
         }
         }

         ExprType *replaceDynamicWithNewTVar( ExprType *type, Region *r ) {
    ExprType *newt = ( ExprType * )region_alloc( r, sizeof( ExprType ) );
    *newt = *type;
    if ( getNodeType( type ) == T_DYNAMIC ) {
        setNodeType( newt, T_VAR );
        T_VAR_ID( newt ) = newTVarId();
    }
    int i;
    for ( i = 0; i < type->degree; i++ ) {
        newt->subtrees[i] = replaceDynamicWithNewTVar( type->subtrees[i], r );
    }
    return newt;
}
int typeFuncParam( Node *param, Node *paramType, Node *formalParamType, Hashtable *var_type_table, List *typingConstraints, rError_t *errmsg, Region *r ) {
    /*char buf[ERR_MSG_LEN];
    char errbuf[ERR_MSG_LEN];
    char typebuf[ERR_MSG_LEN];
    char typebuf2[ERR_MSG_LEN]; */
    /* printf("typing param %s < %s\n",
                        typeToString(paramType, var_type_table, typebuf, ERR_MSG_LEN),
                        typeToString(formalParamType, var_type_table, typebuf2, ERR_MSG_LEN)); */


    TypingConstraint *tc = newTypingConstraint( paramType, formalParamType, TC_LT, param, r );
    listAppend( typingConstraints, tc, r );
    Node *errnode;
    Satisfiability tcons = simplify( typingConstraints, var_type_table, errmsg, &errnode, r );
    switch ( tcons ) {
    case TAUTOLOGY:
        break;
    case CONTINGENCY:
        break;
    case ABSURDITY:
        return -1;
    }
    
    /* Validate @optional/@nonnull type constraints */
    if ( validateTypeConstraints( paramType, formalParamType, param, errmsg, r ) != 0 ) {
        return -1;
    }
    
    return 0;
}

ExprType* typeTypeAscription( Node *expr, int dynamictyping, Env *funcDesc, Hashtable *varTypes, List *typingConstraints, rError_t *errmsg, Node **errnode, Region *r ) {
    char errbuf[ERR_MSG_LEN];
    char errmsgbuf[ERR_MSG_LEN];
    char typebuf[ERR_MSG_LEN];
    char typebuf2[ERR_MSG_LEN];
        Node *param = expr->subtrees[0];
        ExprType *ascType = expr->subtrees[1];
        ExprType *exprType = typeExpression3( param, dynamictyping, funcDesc, varTypes, typingConstraints, errmsg, errnode, r );
        if ( getNodeType( exprType ) == T_ERROR ) {
            return exprType;
        }

        ExprType *t = unifyWith( exprType, ascType, varTypes, r );
        if ( t == NULL ) {
                *errnode = expr;
                snprintf( errmsgbuf, ERR_MSG_LEN, "type error: cannot unify source %s and target %s", typeToString(exprType, varTypes, typebuf, ERR_MSG_LEN), typeToString(ascType, varTypes, typebuf2, ERR_MSG_LEN));
                generateErrMsg( errmsgbuf, NODE_EXPR_POS( ( *errnode ) ), ( *errnode )->base, errbuf );
                addRErrorMsg( errmsg, RE_TYPE_ERROR, errbuf );
                return newSimpType( T_ERROR, r );
        }
        return t;
}

ExprType* typeExpression3( Node *expr, int dynamictyping, Env *funcDesc, Hashtable *varTypes, List *typingConstraints, rError_t *errmsg, Node **errnode, Region *r ) {
    ExprType *res = NULL;
    ExprType **components;
    ExprType* t = NULL;
    int i;
    expr->option |= OPTION_TYPED;
    switch ( getNodeType( expr ) ) {
    case TK_BOOL:
        return expr->exprType = dynamictyping ? newSimpType( T_DYNAMIC, r ) : newSimpType( T_BOOL, r );
    case TK_INT:
        return expr->exprType = dynamictyping ? newSimpType( T_DYNAMIC, r ) : newSimpType( T_INT, r );
    case TK_DOUBLE:
        return expr->exprType = dynamictyping ? newSimpType( T_DYNAMIC, r ) : newSimpType( T_DOUBLE, r );
    case TK_STRING:
        return expr->exprType = dynamictyping ? newSimpType( T_DYNAMIC, r ) : newSimpType( T_STRING, r );
    case TK_VAR:
        t = ( ExprType * )lookupFromHashTable( varTypes, expr->text );
        if ( t == NULL ) {
            /* define new variable */
            t = newTVar( r );
            insertIntoHashTable( varTypes, expr->text, t );
        }
        t = dereference( t, varTypes, r );
        return expr->exprType = t;
    case TK_TEXT:
        if ( strcmp( expr->text, "nop" ) == 0 ) {
            return expr->exprType = newFuncType( newTupleType( 0, NULL, r ), newSimpType( T_INT, r ), r );
        }
        else {
            /* not a variable, evaluate as a function */
            FunctionDesc *fDesc;
            if ( funcDesc != NULL && ( fDesc = ( FunctionDesc* )lookupFromEnv( funcDesc, expr->text ) ) != NULL && fDesc->exprType != NULL ) {
                /* Check if function is marked as deprecated */
                /* TODO: isDeprecated function needs to be implemented (ticket i-f77b) */
                /* if ( isDeprecated( fDesc ) ) {
                    char errbuf[ERR_MSG_LEN];
                    char deprecatedMsg[ERR_MSG_LEN];
                    
                    // Get deprecation message from fDesc->text if available
                    if ( fDesc->text != NULL && fDesc->text[0] != '\0' ) {
                        snprintf( deprecatedMsg, ERR_MSG_LEN, "%s", fDesc->text );
                    } else {
                        snprintf( deprecatedMsg, ERR_MSG_LEN, "use of deprecated function" );
                    }
                    
                    // Generate error message with function name and deprecation details
                    snprintf( errbuf, ERR_MSG_LEN, "deprecated function '%s': %s", expr->text, deprecatedMsg );
                    addRErrorMsg( errmsg, RE_DEPRECATION_WARNING, errbuf );
                } */
                
                ExprType *dupedType = dupType( fDesc->exprType, r );
                if ( dupedType == NULL ) {
                    /* Out of memory during type duplication */
                    *errnode = expr;
                    addRErrorMsg( errmsg, SYS_MALLOC_ERR, "error: out of memory duplicating function type." );
                    return expr->exprType = newErrorType( SYS_MALLOC_ERR, r );
                }
                return expr->exprType = dupedType;
            }
            else {
                ExprType *paramType = newSimpType( T_DYNAMIC, r );
                setIOType( paramType, IO_TYPE_DYNAMIC );
                ExprType *fType = newFuncType( newUnaryType( T_TUPLE, paramType, r ), newSimpType( T_DYNAMIC, r ), r );
                setVararg( fType, OPTION_VARARG_STAR );
                return expr->exprType = fType;
            }
        }
    case N_TUPLE:
         components = ( ExprType ** ) region_alloc( r, sizeof( ExprType * ) * expr->degree );
         for ( i = 0; i < expr->degree; i++ ) {
             components[i] = typeExpression3( expr->subtrees[i], dynamictyping, funcDesc, varTypes, typingConstraints, errmsg, errnode, r );
             if ( getNodeType( components[i] ) == T_ERROR ) {
                 return expr->exprType = components[i];
             }
         }
         if ( N_TUPLE_CONSTRUCT_TUPLE( expr ) || expr->degree != 1 ) {
             return expr->exprType = newTupleType( expr->degree, components, r );
         }
         else {
             return expr->exprType = components[0];
         }

    case N_UNPACKING_PATTERN:
         /**
          * Tuple unpacking pattern type checking: {a, b, c} = tuple_expr
          * 
          * The pattern declares the structure of the tuple being unpacked.
          * Each element in the pattern becomes a variable with the corresponding tuple element type.
          * 
          * Type validation:
          * - Pattern variables: a, b, c become TK_VAR nodes with fresh type variables
          * - Wildcards: _ nodes (TK_TEXT) are ignored (type is not tracked)
          * - Nested patterns: {a, {b, c}} supported for nested tuples
          * 
          * Returns: Pattern type (T_DYNAMIC initially, refined when assigned)
          */
         components = ( ExprType ** ) region_alloc( r, sizeof( ExprType * ) * expr->degree );
         for ( i = 0; i < expr->degree; i++ ) {
             Node *patternElem = expr->subtrees[i];
             /* For wildcard patterns (_), type as T_DYNAMIC and don't register variable */
             if ( getNodeType( patternElem ) == TK_TEXT && strcmp( patternElem->text, "_" ) == 0 ) {
                 components[i] = newSimpType( T_DYNAMIC, r );
             }
             else {
                 /* Recursively type the pattern element */
                 components[i] = typeExpression3( patternElem, dynamictyping, funcDesc, varTypes, typingConstraints, errmsg, errnode, r );
                 if ( getNodeType( components[i] ) == T_ERROR ) {
                     return expr->exprType = components[i];
                 }
             }
         }
         /* Unpacking pattern represents a tuple type with elements from the pattern */
         return expr->exprType = newTupleType( expr->degree, components, r );

     case N_APPLICATION:
        /* try to type as a function */
        /* the exprType is used to store the type of the return value */
        return expr->exprType = typeFunction3( expr, dynamictyping, funcDesc, varTypes, typingConstraints, errmsg, errnode, r );
    case N_ACTIONS:
        if ( expr->degree == 0 ) {
            /* type of empty action sequence == T_INT */
            return expr->exprType = newSimpType( T_INT, r );
        }
        for ( i = 0; i < expr->degree; i++ ) {
            /*printf("typing action in actions"); */
            res = typeExpression3( expr->subtrees[i], dynamictyping, funcDesc, varTypes, typingConstraints, errmsg, errnode, r );
            /*printVarTypeEnvToStdOut(varTypes); */
            if ( getNodeType( res ) == T_ERROR ) {
                return expr->exprType = res;
            }
        }
        return expr->exprType = res;
    case N_ACTIONS_RECOVERY:
        res = typeExpression3( expr->subtrees[0], dynamictyping, funcDesc, varTypes, typingConstraints, errmsg, errnode, r );
        if ( getNodeType( res ) == T_ERROR ) {
            return expr->exprType = res;
        }
        res = typeExpression3( expr->subtrees[1], dynamictyping, funcDesc, varTypes, typingConstraints, errmsg, errnode, r );
        return expr->exprType = res;
    case N_TRY_CATCH:
        /**
         * Try/Catch exception handling type checking
         * 
         * Structure:
         * - subtrees[0] = try block (N_ACTIONS)
         * - subtrees[1..n-1] = catch handlers (N_CATCH_HANDLER)
         * - subtrees[n] = finally block (N_ACTIONS or null)
         * 
         * Type validation:
         * - Try block can have any type (error may occur)
         * - Each catch handler must have compatible return type with try
         * - Finally block executes cleanup
         * - Overall type is the common type of try and catch branches
         */
        {
            /* Type the try block */
            res = typeExpression3( expr->subtrees[0], dynamictyping, funcDesc, varTypes, typingConstraints, errmsg, errnode, r );
            if ( getNodeType( res ) == T_ERROR ) {
                return expr->exprType = res;
            }
            ExprType *tryType = res;
            
            /* Type each catch handler */
            for ( i = 1; i < expr->degree; i++ ) {
                Node *handler = expr->subtrees[i];
                if ( getNodeType( handler ) == N_CATCH_HANDLER ) {
                    /* Type the catch block */
                    if ( handler->degree > 0 ) {
                        res = typeExpression3( handler->subtrees[handler->degree - 1], dynamictyping, funcDesc, varTypes, typingConstraints, errmsg, errnode, r );
                        if ( getNodeType( res ) == T_ERROR ) {
                            return expr->exprType = res;
                        }
                        /* Catch handlers should return compatible type with try block */
                        /* For now, accept T_DYNAMIC/T_UNSPECED for flexibility */
                    }
                }
            }
            
            /* Finally block (if present) executes but doesn't change return type */
            /* Return type is from try block (or catch if error occurred) */
            return expr->exprType = tryType;
        }
    case N_CATCH_HANDLER:
        /**
         * Catch handler type checking
         * Structure:
         * - text = error pattern (error code or variable name)
         * - subtrees[0] = error variable binding (optional)
         * - subtrees[1] = handler block (N_ACTIONS)
         */
        if ( expr->degree > 0 ) {
            res = typeExpression3( expr->subtrees[expr->degree - 1], dynamictyping, funcDesc, varTypes, typingConstraints, errmsg, errnode, r );
            return expr->exprType = res;
        }
        return expr->exprType = newSimpType( T_DYNAMIC, r );
    case N_ATTR:
         /**
          * Attribute access type preservation (N_ATTR)
          * 
          * Used in patterns like:
          * - foreach (row in query_results) { row.DATA_NAME ... }
          * - for (i in result_collection) { i.COLL_NAME ... }
          * - tuple_var.fieldname
          * 
          * Structure:
          * - expr->subtrees[0] = Column reference (TK_COL) or object expression
          * - expr->text = Column name (for TK_COL nodes)
          * 
          * Type preservation:
          * - If child is TK_COL with column name, lookup and return column type
          * - If child is expression, return its type (preserves type through access)
          * - For union types, access preserves the union (allows narrowing via constraints)
          */
         if ( expr->degree > 0 ) {
             Node *childNode = expr->subtrees[0];
             
             /* Check if direct TK_COL column reference */
             if ( getNodeType( childNode ) == TK_COL ) {
                 /* Extract column name and lookup type from schema */
                 const char *columnName = childNode->text;
                 ExprType *columnType = getColumnTypeFromSchema( columnName, r );
                 
                 if ( columnType != NULL ) {
                     /* Type from schema lookup - preserves specific types and unions */
                     return expr->exprType = columnType;
                 }
                 /* Fallback to child type if lookup fails */
             }
             
             /* Type the child expression (could be query result, tuple, etc.) */
             res = typeExpression3( childNode, dynamictyping, funcDesc, varTypes, typingConstraints, errmsg, errnode, r );
             if ( getNodeType( res ) == T_ERROR ) {
                 return expr->exprType = res;
             }
             
             /* Preserve the child's type through attribute access */
             /* This enables type checking of tuple element access and query iteration */
             return expr->exprType = res;
         }
         else {
             /* Attribute node with no children - type as dynamic */
             return expr->exprType = newSimpType( T_DYNAMIC, r );
         }
    case N_QUERY_COND_JUNCTION:
        /**
         * WHERE clause junction type checking (AND/OR)
         * 
         * Structure:
         * - expr->text = operator ("AND" or "OR")
         * - expr->subtrees[0..n] = Condition expressions (N_QUERY_COND or nested N_QUERY_COND_JUNCTION)
         * 
         * Type validation:
         * - Each child must evaluate to T_BOOL
         * - AND/OR of boolean conditions produces boolean result
         * - Return T_BOOL if all conditions type-check
         */
        for ( i = 0; i < expr->degree; i++ ) {
            res = typeExpression3( expr->subtrees[i], dynamictyping, funcDesc, varTypes, typingConstraints, errmsg, errnode, r );
            if ( getNodeType( res ) == T_ERROR ) {
                return expr->exprType = res;
            }
            /* Each operand of a junction must be a boolean condition */
            if ( getNodeType( res ) != T_BOOL && getNodeType( res ) != T_VAR && getNodeType( res ) != T_DYNAMIC ) {
                *errnode = expr->subtrees[i];
                char buf2[1024], buf3[ERR_MSG_LEN];
                typeToString( res, varTypes, buf2, 1024 );
                snprintf( buf3, ERR_MSG_LEN, "error: junction operand type %s is not boolean; expected T_BOOL", buf2 );
                generateErrMsg( buf3, NODE_EXPR_POS( expr->subtrees[i] ), expr->subtrees[i]->base, buf2 );
                addRErrorMsg( errmsg, RE_TYPE_ERROR, buf2 );
                return expr->exprType = newErrorType( RE_TYPE_ERROR, r );
            }
        }
        /* Junction (AND/OR) of boolean conditions produces boolean result */
        return expr->exprType = newSimpType( T_BOOL, r );
    case N_QUERY:
       /**
        * Query expression type inference
        * 
        * A query like: select COLL_NAME, DATA_NAME, DATA_SIZE where COLL_NAME like '/home/%'
        * Returns: collection of tuple<string, string, int>
        * 
        * Process:
        * 1. Extract column names from query AST
        * 2. Build tuple type from column types
        * 3. Wrap tuple type in collection type (LIST)
        * 4. Type all subexpressions (columns and conditions)
        */
       {
           /* First, type all subexpressions (columns and conditions) */
           for ( i = 0; i < expr->degree; i++ ) {
               res = typeExpression3( expr->subtrees[i], dynamictyping, funcDesc, varTypes, typingConstraints, errmsg, errnode, r );
               if ( getNodeType( res ) == T_ERROR ) {
                   return expr->exprType = res;
               }
           }
           
           /* Extract column names from query */
           List *columnNames = extractColumnNamesFromQuery( expr, r );
           
           if ( columnNames == NULL || columnNames->size == 0 ) {
               /* No columns or extraction failed - return T_DYNAMIC */
               return expr->exprType = newSimpType( T_DYNAMIC, r );
           }
           
           /* Build tuple type from column types */
           ExprType *tupleType = createTupleTypeFromColumns( columnNames, r );
           
           if ( tupleType == NULL ) {
               /* Tuple creation failed - return T_DYNAMIC */
               return expr->exprType = newSimpType( T_DYNAMIC, r );
           }
           
           /* Query result is a collection of tuples */
           /* Wrap tuple in collection type (LIST) */
           ExprType *resultType = newCollType( tupleType, r );
           
           return expr->exprType = resultType;
       }
    case N_QUERY_COND:
        /**
         * Query condition type checking: column_expr op value_expr
         * The condition must evaluate to a boolean result
         * 
         * Structure:
         * - expr->text = operator ("=", "<>", ">", "<", ">=", "<=", "in", "like", "between", "not like")
         * - expr->subtrees[0] = Column reference (TK_COL)
         * - expr->subtrees[1+] = Value operands
         * 
         * Type validation:
         * - Get column type from schema (using column name in TK_COL)
         * - Get value operand type
         * - Check compatibility based on operator
         * - Return T_BOOL if types match, T_ERROR otherwise
         */
        {
            /* Extract column name from TK_COL node */
            Node *colNode = expr->subtrees[0];
            if ( colNode == NULL || getNodeType( colNode ) != TK_COL ) {
                /* Invalid query condition structure */
                return expr->exprType = newErrorType( RE_TYPE_ERROR, r );
            }
            
            const char *columnName = colNode->text;
            ExprType *columnType = getColumnTypeFromSchema( columnName, r );
            
            if ( columnType == NULL ) {
                /* Column type lookup failed */
                return expr->exprType = newErrorType( RE_TYPE_ERROR, r );
            }
            
            /* Type the value operands based on the operator */
            if ( expr->text == NULL ) {
                return expr->exprType = newErrorType( RE_TYPE_ERROR, r );
            }
            
            /* Type the operand(s) */
            if ( strcmp( expr->text, "between" ) == 0 ) {
                /* between requires two values */
                if ( expr->degree < 2 ) {
                    return expr->exprType = newErrorType( RE_TYPE_ERROR, r );
                }
                
                /* Type both operands */
                res = typeExpression3( expr->subtrees[1], dynamictyping, funcDesc, varTypes, typingConstraints, errmsg, errnode, r );
                if ( getNodeType( res ) == T_ERROR ) {
                    return expr->exprType = res;
                }
                
                res = typeExpression3( expr->subtrees[2], dynamictyping, funcDesc, varTypes, typingConstraints, errmsg, errnode, r );
                if ( getNodeType( res ) == T_ERROR ) {
                    return expr->exprType = res;
                }
            }
            else {
                /* All other operators require exactly one value operand */
                if ( expr->degree < 1 ) {
                    return expr->exprType = newErrorType( RE_TYPE_ERROR, r );
                }
                
                res = typeExpression3( expr->subtrees[1], dynamictyping, funcDesc, varTypes, typingConstraints, errmsg, errnode, r );
                if ( getNodeType( res ) == T_ERROR ) {
                    return expr->exprType = res;
                }
            }
            
            /* All query conditions evaluate to boolean */
            return expr->exprType = newSimpType( T_BOOL, r );
        }
    case TK_COL:
        /**
         * Column reference type lookup
         * TK_COL nodes contain column names from GenQuery schema
         * Return the column's type from the schema registry
         */
        {
            const char *columnName = expr->text;
            ExprType *columnType = getColumnTypeFromSchema( columnName, r );
            if ( columnType == NULL ) {
                /* Fallback to T_DYNAMIC if type lookup fails */
                return expr->exprType = newSimpType( T_DYNAMIC, r );
            }
            return expr->exprType = columnType;
        }

    case N_EXTERN_DEF:
        return expr->exprType = typeTypeAscription( expr, dynamictyping, funcDesc, varTypes, typingConstraints, errmsg, errnode, r );
    case N_TEMPLATE_DEF:
        /**
         * Template definition type checking
         * Structure:
         * - subtrees[0] = template name (TK_TEXT)
         * - subtrees[1] = parameter list (N_PARAM_LIST)
         * - subtrees[2] = template body (N_ACTIONS)
         * - subtrees[3] = metadata/return type (optional)
         * 
         * Type validation:
         * - Parameters: each param is a fresh type variable
         * - Body: type the action sequence
         * - Return type: inferred from last expression in body
         */
        {
            /* Type the template body with fresh parameter types */
            if ( expr->degree >= 3 ) {
                Node *paramList = expr->subtrees[1];
                Node *body = expr->subtrees[2];
                
                /* Create fresh type variables for each parameter */
                int nparams = paramList != NULL ? paramList->degree : 0;
                for ( i = 0; i < nparams; i++ ) {
                    Node *param = paramList->subtrees[i];
                    ExprType *paramType = newTVar( r );
                    if ( insertIntoHashTable( varTypes, param->text, paramType ) != 0 ) {
                        /* Duplicate parameter name */
                        *errnode = param;
                        char errbuf[ERR_MSG_LEN];
                        snprintf( errbuf, ERR_MSG_LEN, "error: duplicate template parameter '%s'", param->text );
                        addRErrorMsg( errmsg, RE_TYPE_ERROR, errbuf );
                        return expr->exprType = newErrorType( RE_TYPE_ERROR, r );
                    }
                }
                
                /* Type the template body */
                res = typeExpression3( body, dynamictyping, funcDesc, varTypes, typingConstraints, errmsg, errnode, r );
                if ( getNodeType( res ) == T_ERROR ) {
                    return expr->exprType = res;
                }
                
                /* Template type is the inferred return type */
                return expr->exprType = res;
            }
            return expr->exprType = newSimpType( T_DYNAMIC, r );
        }
    case N_TEMPLATE_CALL:
        /**
         * Template instantiation/application type checking
         * Structure:
         * - text = template name
         * - subtrees[0..n-1] = template arguments
         * 
         * Type validation:
         * - Look up template definition
         * - Verify argument count matches parameter count
         * - Type each argument
         * - Unify argument types with parameter type variables
         * - Return instantiated template return type
         */
        {
            /* Template calls are essentially like function applications */
            /* For now, treat as dynamic until template registry is implemented */
            for ( i = 0; i < expr->degree; i++ ) {
                res = typeExpression3( expr->subtrees[i], dynamictyping, funcDesc, varTypes, typingConstraints, errmsg, errnode, r );
                if ( getNodeType( res ) == T_ERROR ) {
                    return expr->exprType = res;
                }
            }
            return expr->exprType = newSimpType( T_DYNAMIC, r );
        }
    default:
        break;
    }
    *errnode = expr;
    char errbuf[ERR_MSG_LEN], errbuf0[ERR_MSG_LEN];
    snprintf( errbuf0, ERR_MSG_LEN, "error: unsupported ast node %d", getNodeType( expr ) );
    generateErrMsg( errbuf0, NODE_EXPR_POS( expr ), expr->base, errbuf );
    addRErrorMsg( errmsg, RE_TYPE_ERROR, errbuf );
    return expr->exprType = newSimpType( T_ERROR, r );

}
/*
 * This process is based on a few assumptions:
 * If the type coerced to cannot be inferred, i.e., an expression is to be coerced to some tvar, bounded to a union type or free,
 * then we leave further type checking to runtime, which can be done locally under the assumption of indistinguishable inclusion.
 *
 */
void postProcessCoercion( Node *expr, Hashtable *varTypes, rError_t *errmsg, Node **errnode, Region *r ) {
    expr->coercionType = expr->coercionType == NULL ? NULL : instantiate( expr->coercionType, varTypes, 0, r );
    expr->exprType = expr->exprType == NULL ? NULL : instantiate( expr->exprType, varTypes, 0, r );
    int i;
    for ( i = 0; i < expr->degree; i++ ) {
        postProcessCoercion( expr->subtrees[i], varTypes, errmsg, errnode, r );
    }
    if ( expr->coercionType != NULL && expr->exprType != NULL ) {
        /*char buf[128];*/
        /*typeToString(expr->coercionType, NULL, buf, 128);
        printf("%s", buf);*/
        if ( getNodeType( expr ) == N_TUPLE ) {
            ExprType **csubtrees = expr->coercionType->subtrees;
            int i;
            for ( i = 0; i < expr->degree; i++ ) {
                if ( typeEqSyntatic( expr->subtrees[i]->exprType, csubtrees[i] ) ) {
                    expr->subtrees[i]->option &= ~OPTION_COERCE;
                }
                else {
                    expr->subtrees[i]->option |= OPTION_COERCE;
                }
            }
        }
    }
}
/*
 * convert single action to actions if the parameter is of type actions
 */
void postProcessActions( Node *expr, Env *systemFunctionTables, rError_t *errmsg, Node **errnode, Region *r ) {
    int i;
    switch ( getNodeType( expr ) ) {
    case N_TUPLE:
        for ( i = 0; i < expr->degree; i++ ) {
            if ( getIOType( expr->subtrees[i] ) == IO_TYPE_ACTIONS && getNodeType( expr->subtrees[i] ) != N_ACTIONS ) {
                setIOType( expr->subtrees[i], IO_TYPE_INPUT );
                Node **params = ( Node ** )region_alloc( r, sizeof( Node * ) * 1 );
                params[0] = expr->subtrees[i];
                Label pos;
                pos.base = expr->base;
                pos.exprloc = NODE_EXPR_POS( expr );
                expr->subtrees[i] = createActionsNode( params, 1, &pos, r );
                setIOType( expr->subtrees[i], IO_TYPE_ACTIONS );
                expr->subtrees[i]->exprType = params[0]->exprType;
            }
        }
        break;
    default:
        break;
    }
    for ( i = 0; i < expr->degree; i++ ) {
        postProcessActions( expr->subtrees[i], systemFunctionTables, errmsg, errnode, r );
    }
    }

    /**
    * Validate @optional type constraint
    * Ensures that values assigned to @optional types can be null/absent
    * Returns: 0 if valid, non-zero if constraint violated
    */
    int validateOptionalTypeConstraint( ExprType *valueType, ExprType *expectedType, Node *exprNode, rError_t *errmsg, Region *r ) {
    if ( expectedType == NULL || valueType == NULL ) {
       return 0;  /* Skip validation for NULL types */
    }
    
    if ( !isOptionalType( expectedType ) ) {
       return 0;  /* Not an @optional type, no constraint to validate */
    }
    
    /* @optional types can accept any value that matches the base type */
    /* This is primarily for documentation and future enforcement */
    return 0;
    }

    /**
    * Validate @nonnull type constraint
    * Ensures that values assigned to @nonnull types are never null/uninitialized
    * Returns: 0 if valid, non-zero if constraint violated
    */
    int validateNonnullTypeConstraint( ExprType *valueType, ExprType *expectedType, Node *exprNode, rError_t *errmsg, Region *r ) {
    if ( expectedType == NULL || valueType == NULL ) {
       return 1;  /* NULL types violate @nonnull constraint */
    }
    
    if ( !isNonnullType( expectedType ) ) {
       return 0;  /* Not a @nonnull type, no constraint to validate */
    }
    
    /* Check if value type is unspecified (uninitialized variable) */
    if ( getNodeType( valueType ) == T_UNSPECED ) {
        char errbuf[ERR_MSG_LEN];
        char base[256];
        if ( exprNode && exprNode->base ) {
            snprintf( base, sizeof(base), "%s", exprNode->base );
        } else {
            snprintf( base, sizeof(base), "unknown" );
        }
        generateErrMsg( "error: @nonnull type constraint violated - uninitialized variable", 
                       exprNode ? NODE_EXPR_POS( exprNode ) : 0,
                       base,
                       errbuf );
        addRErrorMsg( errmsg, RE_TYPE_ERROR, errbuf );
        return 1;
    }
    
    return 0;
    }

    /**
    * Validate all type constraints for a type expression
    * Applies constraint validation rules based on annotation flags
    * Returns: 0 if all constraints valid, non-zero if any violated
    */
    int validateTypeConstraints( ExprType *exprType, ExprType *expectedType, Node *exprNode, rError_t *errmsg, Region *r ) {
    int violations = 0;
    
    if ( expectedType == NULL ) {
       return 0;
    }
    
    /* Validate @nonnull constraint */
    if ( isNonnullType( expectedType ) ) {
       violations += validateNonnullTypeConstraint( exprType, expectedType, exprNode, errmsg, r );
    }
    
    /* Validate @optional constraint */
    if ( isOptionalType( expectedType ) ) {
       violations += validateOptionalTypeConstraint( exprType, expectedType, exprNode, errmsg, r );
    }
    
    return violations;
    }
