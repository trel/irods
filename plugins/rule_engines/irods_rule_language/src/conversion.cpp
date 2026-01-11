/* For copyright information please refer to files in the COPYRIGHT directory
 */
#include "irods/private/re/arithmetics.hpp"
#include "irods/private/re/index.hpp"
#include "irods/private/re/datetime.hpp"
#include "irods/rcMisc.h"
#ifndef DEBUG
#include "irods/rodsType.h"
#endif


int addKeyVal( keyValPair_t *k, const char * key, const char *val );
char * getAttrNameFromAttrId( int id );


/************************ Microservice parameter to Res ***********************/
/**
 * convert string value to Res
 */
void convertStrValue( Res *res, char *val, Region *r ) {
     if ( val == NULL ) {
         res->text = NULL;
     }
     else {
         int len = ( strlen( val ) + 1 ) * sizeof( char );
         res->text = ( char* )region_alloc( r, len );
         if ( res->text == NULL ) {
             rodsLog( LOG_ERROR, "Cannot allocate string buffer for value conversion" );
             res->text = NULL;
             RES_STRING_STR_LEN( res ) = 0;
             res->exprType = newSimpType( T_STRING, r );
             return;
         }
         memcpy( res->text, val, len );
         RES_STRING_STR_LEN( res ) = strlen( val );
     }
     res->exprType = newSimpType( T_STRING, r );
 }
/**
 * convert int value to Res
 */
void convertIntValue( Res *res, int inval, Region *r ) {
    RES_INT_VAL_LVAL( res ) = inval;
    res->exprType = newSimpType( T_INT, r );
}
/**
 * convert double value to Res
 */
void convertDoubleValue( Res *res, double inval, Region *r ) {
    RES_DOUBLE_VAL_LVAL( res ) = inval;
    res->exprType = newSimpType( T_DOUBLE, r );
}
/**
 * adapted from reHelpers2.c
 * input
 *     typ: collection type
 *     inPtr: collection value
 *     inx: index of the element
 * output
 *     value: element value (new on heap)
 *     inx: index of the next element
 *     outtyp: the type of the element (new on heap)
 * return
 *     0
 * 	   NO_VALUES_FOUND
 *     USER_PARAM_TYPE_ERR
 */
Res* getValueFromCollection( char *typ, void *inPtr, int inx, Region *r ) {
    Res *res;
    int i, j;

    if ( !strcmp( typ, StrArray_MS_T ) ) {
        strArray_t *strA;
        /* ->size size of an element */
        /* ->len  length of the array */
        strA = ( strArray_t  * ) inPtr;
        if ( inx >= strA->len ) {
            return NULL;
        }
        res = newStringRes( r, strA->value + inx * strA->size );
        return res;
    }
    else if ( !strcmp( typ, IntArray_MS_T ) ) {
        res = newRes( r );
        res->exprType = newSimpType( T_INT, r );
        intArray_t *intA;
        intA = ( intArray_t * ) inPtr;
        if ( inx >= intA->len ) {
            return NULL;
        }
        RES_INT_VAL_LVAL( res ) = intA->value[inx];
        return res;
    }
    else if ( !strcmp( typ, GenQueryOut_MS_T ) ) {
        keyValPair_t *k; /* element value */
        genQueryOut_t *g = ( genQueryOut_t * ) inPtr; /* the result set */
        char *cname, *aval; /* key and value */
        sqlResult_t *v; /* a result row */
        if ( g->rowCnt == 0 || inx >= g->rowCnt ) {
            return NULL;
        }
        k = ( keyValPair_t * )region_alloc( r, sizeof( keyValPair_t ) );
        if ( k == NULL ) {
            rodsLog( LOG_ERROR, "Cannot allocate keyValPair_t" );
            return NULL;
        }
        k->len = 0;
        k->keyWord = NULL;
        k->value = NULL;
        for ( i = 0; i < g->attriCnt; i++ ) {
            v = g->sqlResult + i;
            cname = ( char * ) getAttrNameFromAttrId( v->attriInx );
            aval = v->value + v->len * inx;
            j  = addKeyVal( k, cname, aval ); /* addKeyVal duplicates the strings */
            if ( j < 0 ) {
                return NULL;
            }
        }
        res = newUninterpretedRes( r, KeyValPair_MS_T, k, NULL );
        return res;
    }
    else if ( strcmp( typ, KeyValPair_MS_T ) == 0 ) {
        return newStringRes( r, ( ( keyValPair_t * ) inPtr )->keyWord[inx] );
    }
    else {
        return NULL;
    }
}
int getCollectionSize( char *typ, void *inPtr ) {
    if ( !strcmp( typ, StrArray_MS_T ) ) {
        strArray_t *strA;
        /* ->size size of an element */
        /* ->len  length of the array */
        strA = ( strArray_t  * ) inPtr;
        return strA->len;
    }
    else if ( !strcmp( typ, IntArray_MS_T ) ) {
        intArray_t *intA;
        intA = ( intArray_t * ) inPtr;
        return intA->len;
    }
    else if ( !strcmp( typ, GenQueryOut_MS_T ) ) {
        genQueryOut_t *g = ( genQueryOut_t * ) inPtr; /* the result set */
        return g->rowCnt;
    }
    else if ( strcmp( typ, KeyValPair_MS_T ) == 0 ) {
        return ( ( keyValPair_t * ) inPtr ) -> len;
    }
    else {
        return USER_PARAM_TYPE_ERR;
    }
}
int convertMsParamToRes( msParam_t *mP, Res *res, Region *r ) {
#ifdef DEBUG
    writeToTmp( "relog.txt", "type: " );
    writeToTmp( "relog.txt", mP->type );
    writeToTmp( "relog.txt", "\n" );
#endif
    if ( mP->type == NULL ) {
        res->exprType = newSimpType( T_UNSPECED, r );
        return 0;

    }
    else if ( strcmp( mP->type, DOUBLE_MS_T ) == 0 ) { /* if the parameter is an integer */
        convertDoubleValue( res, *( double * )mP->inOutStruct, r );
        return 0;
    }
    else if ( strcmp( mP->type, INT_MS_T ) == 0 ) { /* if the parameter is an integer */
        /* this could be int, bool, or datatime */
        if ( res->exprType == NULL ) { /* output parameter */
            RES_INT_VAL_LVAL( res ) = *( int * )mP->inOutStruct;
            res->exprType = newSimpType( T_INT, r );
        }
        else
            switch ( TYPE( res ) ) {
            case T_INT:
                RES_INT_VAL_LVAL( res ) = *( int * )mP->inOutStruct;
                break;
            case T_BOOL:
                RES_BOOL_VAL_LVAL( res ) = *( int * )mP->inOutStruct;
                break;
            case T_DATETIME:
                RES_TIME_VAL( res ) = *( rodsLong_t * )mP->inOutStruct;
                break;
            default:
                convertIntValue( res, *( int * )mP->inOutStruct, r );
            }
        return 0;
    }
    else if ( strcmp( mP->type, STR_MS_T ) == 0 ) { /* if the parameter is a string */
        convertStrValue( res, ( char * )mP->inOutStruct, r );
        return 0;
    }
    else if ( strcmp( mP->type, DATETIME_MS_T ) == 0 ) {
        RES_TIME_VAL( res ) = *( rodsLong_t * )mP->inOutStruct;
        TYPE( res ) = T_DATETIME;
        return 0;
        /*
        	} else if(strcmp(mP->type, StrArray_MS_T) == 0) {
        		convertCollectionToRes(mP, res);
        		return 1;
        	} else if(strcmp(mP->type, IntArray_MS_T) == 0) {
        		convertCollectionToRes(mP, res);
        		return 1;
        	} else if(strcmp(mP->type, GenQueryOut_MS_T) == 0) {
        		convertCollectionToRes(mP, res);
        		return 1;
        */
    }
    else {
        if ( res->param == NULL ) {
            res->param = newMsParam( mP->type, mP->inOutStruct, mP->inpOutBuf, r );
        }
        else {
            res->param->type = mP->type ? strdup( mP->type ) : NULL;
            RES_UNINTER_STRUCT( res ) = mP->inOutStruct;
            RES_UNINTER_BUFFER( res ) = mP->inpOutBuf;
        }
        res->exprType = newIRODSType( mP->type, r );
        return 0;
    }


}
Res* convertMsParamToRes( msParam_t* myArgv, Region* r ) {
    Res* res = newRes( r ); /* we need to create a new res here to make keep all res'es immutable */
    int ret = convertMsParamToResAndFreeNonIRODSType( myArgv, res, r );
    if ( ret != 0 ) {
        res = newErrorRes( r, ret );
    }
    return res;
}
int convertMsParamToResAndFreeNonIRODSType( msParam_t *mP, Res *res, Region *r ) {
#ifdef DEBUG
    writeToTmp( "relog.txt", "type: " );
    writeToTmp( "relog.txt", mP->type );
    writeToTmp( "relog.txt", "\n" );
#endif
    if ( mP->type == NULL ) {
        res->exprType = newSimpType( T_UNSPECED, r );
        return 0;

    }
    else if ( strcmp( mP->type, DOUBLE_MS_T ) == 0 ) { /* if the parameter is an integer */
        convertDoubleValue( res, *( double * )mP->inOutStruct, r );
        free( mP->inOutStruct );
        mP->inOutStruct = NULL;
        return 0;
    }
    else if ( strcmp( mP->type, INT_MS_T ) == 0 ) { /* if the parameter is an integer */
        /* this could be int, bool, or datatime */
        if ( res->exprType == NULL ) { /* output parameter */
            RES_INT_VAL_LVAL( res ) = *( int * )mP->inOutStruct;
            res->exprType = newSimpType( T_INT, r );
        }
        else
            switch ( TYPE( res ) ) {
            case T_INT:
                RES_INT_VAL_LVAL( res ) = *( int * )mP->inOutStruct;
                break;
            case T_BOOL:
                RES_BOOL_VAL_LVAL( res ) = *( int * )mP->inOutStruct;
                break;
            case T_DATETIME:
                RES_TIME_VAL( res ) = *( rodsLong_t * )mP->inOutStruct;
                break;
            default:
                convertIntValue( res, *( int * )mP->inOutStruct, r );
            }
        free( mP->inOutStruct );
        mP->inOutStruct = NULL;
        return 0;
    }
    else if ( strcmp( mP->type, STR_MS_T ) == 0 ) { /* if the parameter is a string */
        convertStrValue( res, ( char * )mP->inOutStruct, r );
        free( mP->inOutStruct );
        mP->inOutStruct = NULL;
        return 0;
    }
    else if ( strcmp( mP->type, DATETIME_MS_T ) == 0 ) {
        RES_TIME_VAL( res ) = *( rodsLong_t * )mP->inOutStruct;
        TYPE( res ) = T_DATETIME;
        free( mP->inOutStruct );
        mP->inOutStruct = NULL;
        return 0;
        /*
        	} else if(strcmp(mP->type, StrArray_MS_T) == 0) {
        		convertCollectionToRes(mP, res);
        		return 1;
        	} else if(strcmp(mP->type, IntArray_MS_T) == 0) {
        		convertCollectionToRes(mP, res);
        		return 1;
        	} else if(strcmp(mP->type, GenQueryOut_MS_T) == 0) {
        		convertCollectionToRes(mP, res);
        		return 1;
        */
    }
    else {
        if ( res->param == NULL ) {
            res->param = newMsParam( mP->type, mP->inOutStruct, mP->inpOutBuf, r );
        }
        else {
            res->param->type = mP->type ? strdup( mP->type ) : NULL;
            RES_UNINTER_STRUCT( res ) = mP->inOutStruct;
            RES_UNINTER_BUFFER( res ) = mP->inpOutBuf;
        }
        res->exprType = newIRODSType( mP->type, r );
        return 0;
    }


}
/************************ Microservice parameter type to ExprType ***********************/
ExprType *convertToExprType( char *type, Region *r ) {
    if ( strcmp( type, DOUBLE_MS_T ) == 0 ) { /* if the parameter is an integer */
        return newSimpType( T_DOUBLE, r );
    }
    else if ( strcmp( type, INT_MS_T ) == 0 ) { /* if the parameter is an integer */
        return newSimpType( T_INT, r );
    }
    else if ( strcmp( type, STR_MS_T ) == 0 ) { /* if the parameter is a string */
        return newSimpType( T_STRING, r );
        /*
        	} else if(strcmp(type, DATETIME_MS_T) == 0) {
        		return newSimp(STRING);
        */
    }
    else if ( strcmp( type, StrArray_MS_T ) == 0 ) {
        return newCollType( newSimpType( T_STRING, r ), r );
    }
    else if ( strcmp( type, IntArray_MS_T ) == 0 ) {
        return newCollType( newSimpType( T_INT, r ), r );
    }
    else if ( strcmp( type, GenQueryOut_MS_T ) == 0 ) {
        return newCollType( newIRODSType( KeyValPair_MS_T, r ), r );
    }
    else {
        return newIRODSType( type, r );
    }
}

/*
void convertCollectionToRes(msParam_t *mP, Res* res) {
	int i =0;
	char *typ = mP->type;
	void *inPtr = mP->inOutStruct;
	int len = getCollectionSize(typ, inPtr);
        char buf[1024];
        sprintf(buf, "%d\n", len);
        writeToTmp("relog.txt", buf);
	ExprType *elemType = convertToExprType(typ);
	setCollRes(res, len, elemType);
	freeType(elemType);
	for(i=0;i<len;i++) {
		res->value.c.elems[i]=getValueFromCollection(typ, inPtr, i);
	}
}
*/

/************************ Res to Microservice parameter ***********************/
int convertResToMsParam( msParam_t *var, Res *res, rError_t *errmsg, Region *r ) {
    strArray_t *arr = NULL;
    intArray_t *arr2 = NULL;
    int i = 0;
    int maxlen = 0;
    var->inpOutBuf = NULL;
    var->label = NULL;
    switch ( TYPE( res ) ) {
    case T_ERROR: /* error message */
        var->inOutStruct = ( int * )region_alloc( r, sizeof( int ) );
        if ( var->inOutStruct == NULL ) {
            rodsLog( LOG_ERROR, "Cannot allocate int" );
            addRErrorMsg( errmsg, SYS_MALLOC_ERR, "Cannot allocate int" );
            return SYS_MALLOC_ERR;
        }
        *( ( int * )var->inOutStruct ) = RES_ERR_CODE( res );
        var->type = strdup( INT_MS_T );
        break;
    case T_DOUBLE: /* number */
        var->inOutStruct = ( double * )region_alloc( r, sizeof( double ) );
        if ( var->inOutStruct == NULL ) {
            rodsLog( LOG_ERROR, "Cannot allocate double" );
            addRErrorMsg( errmsg, SYS_MALLOC_ERR, "Cannot allocate double" );
            return SYS_MALLOC_ERR;
        }
        *( ( double * )var->inOutStruct ) = RES_DOUBLE_VAL( res );
        var->type = strdup( DOUBLE_MS_T );
        break;
    case T_INT: /* number */
        var->inOutStruct = ( int * )region_alloc( r, sizeof( int ) );
        if ( var->inOutStruct == NULL ) {
            rodsLog( LOG_ERROR, "Cannot allocate int" );
            addRErrorMsg( errmsg, SYS_MALLOC_ERR, "Cannot allocate int" );
            return SYS_MALLOC_ERR;
        }
        *( ( int * )var->inOutStruct ) = RES_INT_VAL( res );
        var->type = strdup( INT_MS_T );
        break;
    case T_STRING: /* string */
        var->inOutStruct = res->text == NULL ? NULL : strdup( res->text );
        var->type = strdup( STR_MS_T );
        break;
    case T_PATH: /* path */
        var->inOutStruct = res->text == NULL ? NULL : strdup( res->text );
        var->type = strdup( STR_MS_T );
        break;
    case T_DATETIME: /* date time */
        var->inOutStruct = ( int* )region_alloc( r, sizeof( int ) );
        if ( var->inOutStruct == NULL ) {
            rodsLog( LOG_ERROR, "Cannot allocate int" );
            addRErrorMsg( errmsg, SYS_MALLOC_ERR, "Cannot allocate int" );
            return SYS_MALLOC_ERR;
        }
        *( ( int* )var->inOutStruct ) = ( int )RES_TIME_VAL( res );
        var->type = strdup( INT_MS_T );
        break;
    case T_CONS:
        if ( strcmp( T_CONS_TYPE_NAME( res->exprType ), LIST ) == 0 ) {
            switch ( getNodeType( T_CONS_TYPE_ARG( res->exprType, 0 ) ) ) {
            case T_STRING:
                arr = ( strArray_t * )region_alloc( r, sizeof( strArray_t ) );
                if ( arr == NULL ) {
                    rodsLog( LOG_ERROR, "Cannot allocate strArray_t" );
                    addRErrorMsg( errmsg, SYS_MALLOC_ERR, "Cannot allocate strArray_t" );
                    return SYS_MALLOC_ERR;
                }
                arr->len = res->degree;
                maxlen = 0;
                for ( i = 0; i < res->degree; i++ ) {
                    int slen = RES_STRING_STR_LEN( res->subtrees[i] );
                    maxlen = maxlen < slen ? slen : maxlen;
                }
                arr->size = maxlen + 1;
                arr->value = ( char * )region_alloc( r, sizeof( char ) * (arr->size) * ( arr->len ) );
                if ( arr->value == NULL ) {
                    rodsLog( LOG_ERROR, "Cannot allocate string array values" );
                    addRErrorMsg( errmsg, SYS_MALLOC_ERR, "Cannot allocate string array values" );
                    free( arr );
                    return SYS_MALLOC_ERR;
                }
                for ( i = 0; i < res->degree; i++ ) {
                    /* Validate array bounds before dereferencing */
                    if ( res->subtrees == NULL || res->subtrees[i] == NULL || res->subtrees[i]->text == NULL ) {
                        rodsLog( LOG_ERROR, "convertResToMsParam: array bounds violation at index %d", i );
                        addRErrorMsg( errmsg, SYS_INTERNAL_ERR, "error: array bounds violation during type conversion." );
                        free( arr->value );
                        free( arr );
                        return SYS_INTERNAL_ERR;
                    }
                    strcpy( arr->value + arr->size * i, res->subtrees[i]->text );
                }
                var->inOutStruct = arr;
                var->type = strdup( StrArray_MS_T );
                break;
            case T_INT:
                arr2 = ( intArray_t * )region_alloc( r, sizeof( intArray_t ) );
                if ( arr2 == NULL ) {
                    rodsLog( LOG_ERROR, "Cannot allocate intArray_t" );
                    addRErrorMsg( errmsg, SYS_MALLOC_ERR, "Cannot allocate intArray_t" );
                    return SYS_MALLOC_ERR;
                }
                arr2->len = res->degree;
                arr2->value = ( int * )region_alloc( r, sizeof( int ) * ( arr2->len ) );
                if ( arr2->value == NULL ) {
                    rodsLog( LOG_ERROR, "Cannot allocate int array values" );
                    addRErrorMsg( errmsg, SYS_MALLOC_ERR, "Cannot allocate int array values" );
                    return SYS_MALLOC_ERR;
                }
                for ( i = 0; i < res->degree; i++ ) {
                    /* Validate array bounds before dereferencing */
                    if ( res->subtrees == NULL || res->subtrees[i] == NULL ) {
                        rodsLog( LOG_ERROR, "convertResToMsParam: array bounds violation at index %d", i );
                        addRErrorMsg( errmsg, SYS_INTERNAL_ERR, "error: array bounds violation during type conversion." );
                        free( arr2->value );
                        free( arr2 );
                        return SYS_INTERNAL_ERR;
                    }
                    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
                    arr2->value[i] = RES_INT_VAL(res->subtrees[i]);
                }
                var->inOutStruct = arr2;
                var->type = strdup( IntArray_MS_T );
                break;
            default:
                /* current there is no existing packing instructions for arbitrary collection */
                /* report error */
                addRErrorMsg( errmsg, RE_PACKING_ERROR, "no packing instruction for arbitrary collection type" );
                return RE_PACKING_ERROR;
            }
        }
        else {
            addRErrorMsg( errmsg, RE_PACKING_ERROR, "no packing instruction for arbitrary constructed type" );
            return RE_PACKING_ERROR;
        }
        break;
    case T_IRODS:
        var->inOutStruct = RES_UNINTER_STRUCT( res );
        var->inpOutBuf = RES_UNINTER_BUFFER( res );
        var->type = res->exprType->text ? strdup( res->exprType->text ) : NULL;
        break;
    case T_UNSPECED:
        var->inOutStruct = NULL;
        var->type = NULL;
        break;
    default:
        /*error */
        addRErrorMsg( errmsg, RE_PACKING_ERROR, "no packing instruction for arbitrary type" );
        return RE_PACKING_ERROR;
    }
    return 0;
}
int updateResToMsParam( msParam_t *var, Res *res, rError_t *errmsg, Region *r ) {
    if ( var->type != NULL && ( strcmp( var->type, INT_MS_T ) == 0 ||
                                strcmp( var->type, DOUBLE_MS_T ) == 0 ||
                                strcmp( var->type, STR_MS_T ) == 0 ) ) {
         /* do not free msParam_t if its inOutStruct and inOutBuf are shared */
         if ( var->inOutStruct != NULL ) {
             free( var->inOutStruct );
             var->inOutStruct = nullptr;
         }
         if ( var->inpOutBuf != NULL ) {
             free( var->inpOutBuf );
             var->inpOutBuf = nullptr;
         }
     }
     if ( var->label != NULL ) {
         free( var->label );
         var->label = nullptr;
     }
     return convertResToMsParam( var, res, errmsg, r );
}
int convertEnvToMsParamArray( msParamArray_t *var, Env *env, rError_t *errmsg, Region *r ) {
    int ret;
    if ( env->previous != NULL ) {
        if ( ( ret = convertEnvToMsParamArray( var, env->previous, errmsg, r ) ) != 0 ) {
            return ret;
        }
    }
    return convertHashtableToMsParamArray( var, env->current, errmsg, r );
}

int convertHashtableToMsParamArray( msParamArray_t *var, Hashtable *env, rError_t *errmsg, Region *r ) {
    int i;
    for ( i = 0; i < env->size; i++ ) {
        struct bucket *b = env->buckets[i];
        while ( b != NULL && !IS_TVAR_NAME( b->key ) ) {
            Res *res = ( Res * )b->value;
            msParam_t *v = NULL;
            int varindex;
            int ret;
            for ( varindex = 0; varindex < var->len; varindex++ ) {
                if ( var->msParam[varindex]->label != NULL && strcmp( var->msParam[varindex]->label, b->key ) == 0 ) {
                    v = var->msParam[varindex];
                    ret = updateResToMsParam( v, res, errmsg, r );
                    break;
                }
            }
            if ( v == NULL ) {
                v = ( msParam_t * ) region_alloc( r, sizeof( msParam_t ) );
                ret = convertResToMsParam( v, res, errmsg, r );
                if ( var->msParam == NULL ) {
                    var->len = 0;
                    var->msParam = ( msParam_t ** ) region_alloc( r, sizeof( msParam_t * ) * ( PTR_ARRAY_MALLOC_LEN ) );
                }
                else if ( var->len % PTR_ARRAY_MALLOC_LEN == 0 ) {
                    var->msParam = ( msParam_t ** ) region_alloc( r, sizeof( msParam_t * ) * ( PTR_ARRAY_MALLOC_LEN + var->len ) );
                }

                var->msParam[var->len++] = v;
            }
            v->label = strdup( b->key );

            if ( ret != 0 ) {
                /* error during conversion; clean up allocated parameter */
                /* Note: msParamArray 'var' is caller-owned and freed by caller on error */
                /* Region-allocated memory will be freed with region cleanup */
                return ret;
            }

            b = b->next;
        }
    }
    return 0;
}
int convertMsParamArrayToEnv( msParamArray_t *var, Env *env, Region *r ) {
    return updateMsParamArrayToEnv( var, env, r );
}

int convertMsParamArrayToEnvAndFreeNonIRODSType( msParamArray_t *var, Env *env, Region *r ) {
    return updateMsParamArrayToEnvAndFreeNonIRODSType( var, env, r );
}
int updateMsParamArrayToEnv( msParamArray_t *var, Env *env, Region *r ) {
    int i;
    for ( i = 0; i < var->len; i++ ) {
        Res *res = newRes( r );
        int ret = convertMsParamToRes( var->msParam[i], res, r );
        if ( ret != 0 ) {
            return ret;
        }
        char *varName = var->msParam[i]->label;
        if ( varName != NULL ) {
            updateInEnv( env, varName, res );
        }
    }
    return 0;
}
int updateMsParamArrayToEnvAndFreeNonIRODSType( msParamArray_t *var, Env *env, Region *r ) {
    int i;
    for ( i = 0; i < var->len; i++ ) {
        Res *res = newRes( r );
        int ret = convertMsParamToResAndFreeNonIRODSType( var->msParam[i], res, r );
        if ( ret != 0 ) {
            return ret;
        }
        char *varName = var->msParam[i]->label;
        if ( varName != NULL ) {
            updateInEnv( env, varName, res );
        }
    }
    return 0;
}

/************* Res to String *********************/
char* convertResToString( Res *res0 ) {
    char *res;
    char *type;
    int j;
    /* Use stack buffer for temporary string conversion */
    char tmpbuf[1024];
    
    switch ( getNodeType( res0 ) ) {

    case N_ERROR:
        snprintf( tmpbuf, 1024, "error %d", RES_ERR_CODE( res0 ) );
        return strdup( tmpbuf );
    case N_VAL:
        switch ( TYPE( res0 ) ) {
        case T_INT:
        case T_DOUBLE:
            if ( res0->dval == ( int )res0->dval ) {
                snprintf( tmpbuf, 1024, "%d", ( int )res0->dval );
            }
            else {
                snprintf( tmpbuf, 1024, "%f", res0->dval );
            }
            return strdup( tmpbuf );
        case T_STRING:
            if ( res0->text == NULL ) {
                res = strdup( "<null>" );
            }
            else {
                res = strdup( res0->text );
            }
            return res;
        case T_PATH:
            if ( res0->text == NULL ) {
                res = strdup( "<null>" );
            }
            else {
                res = strdup( res0->text );
            }
            return res;
        case T_IRODS:
            tmpbuf[0] = 0;
            type = res0->exprType->text;
            if ( strcmp( type, KeyValPair_MS_T ) == 0 ) {
                int num_pairs = 0;
                keyValPair_t *kvp = ( keyValPair_t * ) RES_UNINTER_STRUCT( res0 );
                if ( kvp != NULL ) {
                    num_pairs = kvp->len;
                }
                snprintf( tmpbuf, 1024, "KeyValue[%d]:", num_pairs );
                int i;
                for ( i = 0; i < num_pairs; i++ ) {
                    snprintf( tmpbuf + strlen( tmpbuf ), 1024 - strlen( tmpbuf ), "%s=%s;", kvp->keyWord[i], kvp->value[i] );
                }

            }
            else if ( strcmp( type, BUF_LEN_MS_T ) == 0 ) {
                snprintf( tmpbuf + strlen( tmpbuf ), 1024 - strlen( tmpbuf ), "%d", *( int* )res0->param->inOutStruct );
            }
            else if ( strcmp( type, DataObjInp_MS_T ) == 0 ) {
                dataObjInp_t dataObjInp, *myDataObjInp;
                j = parseMspForDataObjInp( res0->param, &dataObjInp, &myDataObjInp, 0 );
                if ( j < 0 ) {
                    snprintf( tmpbuf + strlen( tmpbuf ), 1024 - strlen( tmpbuf ), "<error>" );
                }
                else {
                    snprintf( tmpbuf + strlen( tmpbuf ), 1024 - strlen( tmpbuf ), "%s", ( char * ) myDataObjInp->objPath );
                }
            }
            else if ( strcmp( type, CollInp_MS_T ) == 0 ) {
                collInp_t collCreateInp, *myCollCreateInp;
                j = parseMspForCollInp( res0->param, &collCreateInp, &myCollCreateInp, 0 );
                if ( j < 0 ) {
                    snprintf( tmpbuf + strlen( tmpbuf ), 1024 - strlen( tmpbuf ), "<error>" );
                }
                else {
                    snprintf( tmpbuf + strlen( tmpbuf ), 1024 - strlen( tmpbuf ), "%s", myCollCreateInp->collName );
                }
            }
            else if ( strcmp( type,  DataObjCopyInp_MS_T ) == 0 ) {
                dataObjCopyInp_t dataObjCopyInp, *myDataObjCopyInp;
                j = parseMspForDataObjCopyInp( res0->param, &dataObjCopyInp, &myDataObjCopyInp );
                if ( j < 0 ) {
                    snprintf( tmpbuf + strlen( tmpbuf ), 1024 - strlen( tmpbuf ), "<error>" );
                }
                else
                    snprintf( tmpbuf + strlen( tmpbuf ), 1024 - strlen( tmpbuf ), "COPY(%s,%s)",
                              myDataObjCopyInp->srcDataObjInp.objPath, myDataObjCopyInp->destDataObjInp.objPath );
            }
            else if ( strcmp( type,  DataObjReadInp_MS_T ) == 0
                      || strcmp( type, DataObjCloseInp_MS_T ) == 0
                      || strcmp( type, DataObjWriteInp_MS_T ) == 0 ) {
                openedDataObjInp_t  *myDataObjReadInp;
                myDataObjReadInp = ( openedDataObjInp_t  * ) res0->param->inOutStruct;
                snprintf( tmpbuf + strlen( tmpbuf ), 1024 - strlen( tmpbuf ), "OPEN(%d)", myDataObjReadInp->len );
            }
            else if ( strcmp( type, ExecCmd_MS_T ) == 0 ) {
                execCmd_t *execCmd;
                execCmd = ( execCmd_t * )  res0->param->inOutStruct;
                snprintf( tmpbuf + strlen( tmpbuf ), 1024 - strlen( tmpbuf ), "%s", execCmd->cmd );
            }
            else {
                snprintf( tmpbuf, 1024, "<value>" );
            }
            return strdup( tmpbuf );
        case T_BOOL:
            res = strdup( RES_BOOL_VAL( res0 ) ? "true" : "false" );
            return res;
        case T_CONS: {
             sprintf( tmpbuf, "[" );
             int i;
             for ( i = 0; i < res0->degree; i++ ) {
                 /* Validate array bounds before dereferencing */
                 if ( res0->subtrees == NULL || res0->subtrees[i] == NULL ) {
                     rodsLog( LOG_ERROR, "convertResToString: array bounds violation at index %d", i );
                     return NULL;
                 }
                 char *resElem = convertResToString( res0->subtrees[i] );
                 if ( resElem == NULL ) {
                     return NULL;
                 }
                 else {
                     snprintf( tmpbuf + strlen( tmpbuf ), 1024 - strlen( tmpbuf ), "%s%s", i == 0 ? "" : ",", resElem );
                     free( resElem );
                 }
             }
             snprintf( tmpbuf + strlen( tmpbuf ), 1024 - strlen( tmpbuf ), "]" );
             return strdup( tmpbuf );
         }
         case T_DATETIME:
             ttimestr( tmpbuf, 1024 - 1, "", &RES_TIME_VAL( res0 ) );
             return strdup( tmpbuf );
        case T_UNSPECED:
            res = strdup( "<undefined>" );
            return res;
        default:
            /*sprintf(res, "error: unsupported type %d", TYPE(res0)); */
            return NULL;
        }
        break;
        default:
        char defbuf[128];
        char *res = typeToString( res0, NULL, defbuf, 128 );
        return res == NULL ? NULL : strdup( res );
        }
        }

/******************* print functions **********************/
void printMsParamArray( msParamArray_t *msParamArray, char *buf2 ) {
    char buf3[MAX_NAME_LEN];
    sprintf( buf2, "len: %d\n", msParamArray->len );
    int i;
    for ( i = 0; i < msParamArray->len; i++ ) {
        msParam_t *mP = msParamArray->msParam[i];
        if ( i != 0 ) {
            strncat( buf2, ",", MAX_NAME_LEN - strlen( buf2 ) );
        }
        strncat( buf2, mP->label, MAX_NAME_LEN - strlen( buf2 ) );
        strncat( buf2, "=", MAX_NAME_LEN - strlen( buf2 ) );
        if ( mP->inOutStruct == NULL ) {
            strncat( buf2, "<null>", MAX_NAME_LEN - strlen( buf2 ) );
        }
        else {
            if ( strcmp( mP->type, DOUBLE_MS_T ) == 0 ) { /* if the parameter is an integer */
                snprintf( buf3, MAX_NAME_LEN, "%f:", *( double * )mP->inOutStruct );
            }
            else if ( strcmp( mP->type, INT_MS_T ) == 0 ) { /* if the parameter is an integer */
                snprintf( buf3, MAX_NAME_LEN, "%d:", *( int * )mP->inOutStruct );
            }
            else if ( strcmp( mP->type, STR_MS_T ) == 0 ) { /* if the parameter is a string */
                snprintf( buf3, MAX_NAME_LEN, "%s:", ( char * )mP->inOutStruct );
            }
            else if ( strcmp( mP->type, DATETIME_MS_T ) == 0 ) {
                snprintf( buf3, MAX_NAME_LEN, "%ld:", ( long ) * ( time_t * )mP->inOutStruct );
            }
            else {
                snprintf( buf3, MAX_NAME_LEN, "<value>:" );
            }
            strncat( buf2, buf3, MAX_NAME_LEN - strlen( buf2 ) );
            strncat( buf2, mP->type, MAX_NAME_LEN - strlen( buf2 ) );
        }
    }
    return;
}

void printHashtable( Hashtable *env, char* buf2 ) {
    int i;
    char typeNameBuf[128];
    sprintf( buf2, "len: %d\n", env->len );
    int k = 0;
    for ( i = 0; i < env->size; i++ ) {
        struct bucket *b = env->buckets[i];
        while ( b != NULL ) {
            Res *res = ( Res * ) b->value;
            if ( k != 0 ) {
                strncat( buf2, "\n", MAX_NAME_LEN - strlen( buf2 ) );
            }
            strncat( buf2, b->key, MAX_NAME_LEN - strlen( buf2 ) );
            strncat( buf2, "=", MAX_NAME_LEN - strlen( buf2 ) );
            if ( res == NULL ) {
                strncat( buf2, "<null>", MAX_NAME_LEN - strlen( buf2 ) );
            }
            else {
                char *buf4 = convertResToString( res );
                strncat( buf2, buf4, MAX_NAME_LEN - strlen( buf2 ) );
                strncat( buf2, ":", MAX_NAME_LEN - strlen( buf2 ) );
                strncat( buf2, res->exprType == NULL ? "<null>" : typeToString( res->exprType, NULL, typeNameBuf, 128 ), MAX_NAME_LEN - strlen( buf2 ) );
                free( buf4 );
            }
            k++;
            b = b->next;
        }
    }
}

int convertResToIntReturnValue( Res *res ) {
    int retVal;
    if ( getNodeType( res ) == N_ERROR ) {
        retVal = RES_ERR_CODE( res );
    }
    else {
        retVal = RES_INT_VAL( res );
    }
    return retVal;

}
