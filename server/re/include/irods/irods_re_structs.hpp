#ifndef IRODS_RE_STRUCTS_HPP
#define IRODS_RE_STRUCTS_HPP

#include "irods/rodsUser.h"
#include "irods/rods.h"
#include "irods/msParam.h"
#include "irods/reDefines.h"
#include "irods/ruleExecSubmit.h"
#include "irods/reconstants.hpp"
#include <boost/any.hpp>

#include <list>
#include <map>
#include <string>
#include <vector>

/// Maximum number of arguments accepted by applyRuleArg().
#define MAX_NUM_OF_ARGS_IN_ACTION 20

/// Bit flag requesting deallocation of the `msParamArray` member.
#define FREE_MS_PARAM	0x1
/// Bit flag requesting deallocation of the `doinp` member.
#define FREE_DOINP	0x2

/// Forward declaration for the client communication object.
struct RcComm;

/// Carries rule execution state across rule engine calls.
typedef struct RuleExecInfo {
    int status; ///< Status code associated with the current rule execution.
    char statusStr[MAX_NAME_LEN]; ///< Status text associated with `status`.
    char ruleName[NAME_LEN];	/* name of rule */ ///< Name of the active rule.
    RsComm *rsComm; ///< Server communication context.
    char pluginInstanceName[MAX_NAME_LEN]; ///< Rule engine plugin instance name.
    msParamArray_t *msParamArray; ///< Input parameter array.
    msParamArray_t inOutMsParamArray; ///< In-place input/output parameter array.
    int l1descInx; ///< L1 descriptor index used by the rule.
    dataObjInp_t *doinp;	/* data object type input */ ///< Data object input.
    dataObjInfo_t *doi; ///< Data object metadata.
    char rescName[NAME_LEN]; ///< Resource name.
    // uoi => user object info
    userInfo_t *uoic;  /* client XXXX should get this from rsComm->clientUser */ ///< Client user information.
    userInfo_t *uoip;  /* proxy XXXX should get this from rsComm->proxyUser */ ///< Proxy user information.
    collInfo_t *coi; ///< Collection information.
    userInfo_t *uoio;     /* other user info */ ///< Other user information.
    keyValPair_t *condInputData; ///< Additional conditional input values.
    /****        IF YOU ARE MAKING CHANGES CHECK BELOW
                 OR ABOVE FOR IMPORTANT INFORMATION  ****/
    char ruleSet[RULE_SET_DEF_LENGTH]; ///< Ruleset identifier string.
    struct RuleExecInfo *next; ///< Next entry in a linked list of execution contexts.
} ruleExecInfo_t;

/// Stores positional arguments for rule execution.
typedef struct ReArg {
    int myArgc; ///< Number of arguments.
    char **myArgv; ///< Argument vector.
} reArg_t;

/// Bundles a rule execution context with its argument list.
typedef struct RuleExecInfoAndArg {
    ruleExecInfo_t *rei; ///< Rule execution context.
    reArg_t reArg; ///< Associated arguments.
} ruleExecInfoAndArg_t;

/// Applies a named rule using an `msParamArray_t` input list.
int applyRule(
    const char *inAction,
    msParamArray_t *inMsParamArray,
    ruleExecInfo_t *rei,
    int reiSaveFlag );

/// Applies a rule using a packed `boost::any` parameter list.
int applyRuleWithInOutVars(
    const char*            _action,
    std::list<boost::any>& _params,
    ruleExecInfo_t*        _rei );

/// Deallocates an `execCmdOut_t` structure.
void freeCmdExecOut( execCmdOut_t *ruleExecOut );

/// Applies the post-processing rule for data object reads.
int applyRuleForPostProcForRead( RsComm *rsComm, bytesBuf_t *dataObjReadOutBBuf, char *objPath );
/// Applies the post-processing rule for data object writes.
int applyRuleForPostProcForWrite( RsComm *rsComm, bytesBuf_t *dataObjWriteOutBBuf, char *objPath );
/// Applies a rule using a raw argument vector.
int applyRuleArg( const char *action, const char *args[MAX_NUM_OF_ARGS_IN_ACTION], int argc,
                  ruleExecInfo_t *rei, int reiSaveFlag );

/// Initializes a rule execution context from a data object input.
int initReiWithDataObjInp( ruleExecInfo_t *rei, RsComm *rsComm,
                           dataObjInp_t *dataObjIn );
/// Initializes a rule execution context from a collection input.
int initReiWithCollInp( ruleExecInfo_t *rei, RsComm *rsComm,
                        collInp_t *collCreateInp, collInfo_t *collInfo );


/// Writes a string to the destination identified by `writeId`.
int _writeString( char *writeId, char *writeStr, ruleExecInfo_t *rei );
/// Writes a string microservice output value.
int writeString( msParam_t* where, msParam_t* inString, ruleExecInfo_t *rei );

/// Returns tagged values parsed from a delimited string.
std::map<std::string, std::vector<std::string>> getTaggedValues(const char *str);

/// Fills delayed rule execution submission conditions.
int fillSubmitConditions( const char *action, const char *inDelayCondition,
                          bytesBuf_t *packedReiAndArgBBuf, ruleExecSubmitInp_t *ruleSubmitInfo,
                          ruleExecInfo_t *rei );

/// Packs a rule execution context into a byte buffer.
int
packRei( ruleExecInfo_t *rei, bytesBuf_t **packedReiBBuf );
/// Unpacks a rule execution context from a byte buffer.
int
unpackRei( RsComm *rsComm, ruleExecInfo_t **rei,
           bytesBuf_t *packedReiBBuf );
/// Packs a rule execution context and argument list into a byte buffer.
int
packReiAndArg( ruleExecInfo_t *rei, char *myArgv[],
               int myArgc, bytesBuf_t **packedReiAndArgBBuf );
/// Unpacks a rule execution context and argument list from a byte buffer.
int
unpackReiAndArg( RsComm *rsComm, ruleExecInfoAndArg_t **reiAndArg,
                 bytesBuf_t *packedReiAndArgBBuf );

/// Copies one rule execution context into another.
int copyRuleExecInfo( ruleExecInfo_t *from, ruleExecInfo_t *to );

/// Deallocates memory pointed to by a ruleExecInfo_t*.
///
/// This function does not deallocate memory referenced by KeyValPairs.
///
/// \param[in] rs                    The pointer to a ruleExecInfo_t to deallocate.
/// \param[in] freeSpecialStructFlag \parblock A bitmask indicating whether to deallocate
/// memory owned by the DataObjInp member and/or MsParamArray data. Bitmask values include:
/// - FREE_MS_PARAM: Deallocate the \p msParamArray member variable.
/// - FREE_DOINP: Deallocate the \p doinp member variable.
/// \endparblock
///
/// \returns An integer indicating the status of the operation.
/// \retval 0        On success.
/// \retval non-zero On failure.
int freeRuleExecInfoStruct( ruleExecInfo_t *rs, int freeSpecialStructFlag );

/// Zero-initializes all members of a rule execution context.
int zeroRuleExecInfoStruct( ruleExecInfo_t *rei );

/// Deallocates memory pointed to by members of a ruleExecInfo_t.
///
/// This function does not deallocate memory referenced by KeyValPairs.
///
/// \param[in] rs                    The pointer to a ruleExecInfo_t to deallocate.
/// \param[in] freeSpecialStructFlag \parblock A bitmask indicating whether to deallocate
/// memory owned by the DataObjInp member and/or MsParamArray data. Bitmask values include:
/// - FREE_MS_PARAM: Deallocate the \p msParamArray member variable.
/// - FREE_DOINP: Deallocate the \p doinp member variable.
/// \endparblock
///
/// \returns An integer indicating the status of the operation.
/// \retval 0        On success.
/// \retval non-zero On failure.
int freeRuleExecInfoInternals( ruleExecInfo_t *rs, int freeSpecialStructFlag );

/// Deallocates all memory pointed to by a ruleExecInfo_t*.
///
/// Unlike freeRuleExecInfoStruct(ruleExecInfo_t*, int), this function deallocates memory referenced by all data
/// members, including KeyValPairs.
///
/// \since 4.3.5
void freeRuleExecInfoStructFull(ruleExecInfo_t* _rs);

/// Deallocates memory pointed to by members of a ruleExecInfo_t.
///
/// Unlike freeRuleExecInfoInternals(ruleExecInfo_t*, int), this function deallocates memory referenced by all data
/// members, including KeyValPairs.
///
/// The object pointed to by \p rs is never deallocated.
///
/// \since 4.3.5
void freeRuleExecInfoInternalsFull(ruleExecInfo_t* _rs);

/// Copies one `dataObjInfo_t` into another.
int copyDataObjInfo( dataObjInfo_t *from, dataObjInfo_t *to );

/// Copies one `collInfo_t` into another.
int copyCollInfo( collInfo_t *from, collInfo_t *to );

/// Deallocates members owned by a `collInfo_t`.
int freeCollInfo( collInfo_t *rs );

/// Copies one `userInfo_t` into another.
int copyUserInfo( userInfo_t *from, userInfo_t *to );

/// Deallocates members owned by a `userInfo_t`.
int freeUserInfo( userInfo_t *rs );

/// Copies one `keyValPair_t` into another.
int copyKeyValPairStruct( keyValPair_t *from, keyValPair_t *to );

/// Deallocates members owned by a `keyValPair_t`.
int freeKeyValPairStruct( keyValPair_t *rs );

/// Pushes a string value onto a `strArray_t` stack.
int pushStack( strArray_t *strArray, char *value );

/// Pops a string value from a `strArray_t` stack.
int popStack( strArray_t *strArray, char *value );

/// Clears msParam-related state stored in a rule execution context.
int clearMsparamInRei( ruleExecInfo_t *rei );

/// Allocates memory and zero-fills the result.
void *mallocAndZero( int s );

#endif // IRODS_RE_STRUCTS_HPP
