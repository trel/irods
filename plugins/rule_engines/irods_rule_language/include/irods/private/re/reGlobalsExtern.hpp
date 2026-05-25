/*** Copyright (c), The Regents of the University of California            ***
 *** For more information please refer to files in the COPYRIGHT directory ***/
/* reGlobalsExtern.hpp - header file for global extern declaration for the
 * rule engine modules
 */

#ifndef RE_GLOBALS_EXTERN_HPP
#define RE_GLOBALS_EXTERN_HPP

/****
#ifdef MALLOC_TESTING
/ * Include the following code to log each malloc and free.
   This can be useful in testing/debugging of memory allocation problems. * /
#define MYMALLOC 1
#define malloc(x) mymalloc(__FILE__, __LINE__ , x)
extern void* mymalloc(char* file, int line, int x);
#define free(x) myfree(__FILE__, __LINE__ , x)
extern void myfree(char* file, int line, void *x);
#endif
***/

#include "irods/rodsUser.h"
#include "irods/rods.h"
#include "irods/rcGlobalExtern.h"
#include "irods/objInfo.h"
#include "irods/fileOpen.h"
#include "irods/reDefines.h"
#include "irods/ruleExecSubmit.h"
#include "irods/ruleExecDel.h"
#include "irods/dataObjInpOut.h"
#include "irods/msParam.h"
#include "irods/modAccessControl.h"

#if 0
/***** IMPORTANT    IMPORTANT    IMPORTANT    *****/
/***** If you are changing the RuleExecInfo   *****/
/***** You need to modify the maintenance     *****/
/***** functions. Please refer to the file    *****/
/***** WhatToDoWhenYouChangeREIStructure.txt  *****/
/***** for more details.                      *****/
/***** ALSO, if any structure in RuleExecInfo *****/
/***** has its definition changed there need  *****/
/***** corresponding changes in maintenance   *****/
/***** files. Please refer to the  file       *****/
/***** WhatToDoWhenYouChangeREIStructure.txt  *****/
/***** for more details.                      *****/
/***** IMPORTANT    IMPORTANT    IMPORTANT    *****/
typedef struct RuleExecInfo {
    int status;
    char statusStr[MAX_NAME_LEN];
    char ruleName[NAME_LEN];	/* name of rule */
    rsComm_t *rsComm;
    char pluginInstanceName[MAX_NAME_LEN];
    msParamArray_t *msParamArray;
    msParamArray_t inOutMsParamArray;
    int l1descInx;
    dataObjInp_t *doinp;	/* data object type input */
    dataObjInfo_t *doi;
    char rescName[NAME_LEN]; // replaces rgi above
    userInfo_t *uoic;  /* client XXXX should get this from rsComm->clientUser */
    userInfo_t *uoip;  /* proxy XXXX should get this from rsComm->proxyUser */
    collInfo_t *coi;
    userInfo_t *uoio;     /* other user info */
    keyValPair_t *condInputData;
    /****        IF YOU ARE MAKING CHANGES CHECK BELOW
                 OR ABOVE FOR IMPORTANT INFORMATION  ****/
    char ruleSet[RULE_SET_DEF_LENGTH];
    struct RuleExecInfo *next;
} ruleExecInfo_t;
#endif
/***** IMPORTANT    IMPORTANT    IMPORTANT    *****/
/***** If you are changing the RuleExecInfo   *****/
/***** You need to modify the maintenance     *****/
/***** functions. Please refer to the file    *****/
/***** WhatToDoWhenYouChangeREIStructure.txt  *****/
/***** for more details.                      *****/
/***** ALSO, if any structure in RuleExecInfo *****/
/***** has its definition changed there need  *****/
/***** corresponding changes in maintenance   *****/
/***** files. Please refer to the  file       *****/
/***** WhatToDoWhenYouChangeREIStructure.txt  *****/
/***** for more details.                      *****/
/***** IMPORTANT    IMPORTANT    IMPORTANT    *****/

/// Stores one entry in the rule engine debug stack.
struct reDebugStack {
    char *step; ///< Text describing the debug step.
    int label; ///< Source location associated with the step.
};

/// Holds raw rule definitions loaded from a rule base.
typedef struct {
    int MaxNumOfRules; ///< Number of populated rule entries.
    char    *ruleBase[MAX_NUM_OF_RULES]; ///< Source rule base for each rule.
    char    *action[MAX_NUM_OF_RULES]; ///< Action name for each rule.
    char    *ruleHead[MAX_NUM_OF_RULES]; ///< Rule head text.
    char    *ruleCondition[MAX_NUM_OF_RULES]; ///< Rule condition text.
    char    *ruleAction[MAX_NUM_OF_RULES]; ///< Rule body text.
    char    *ruleRecovery[MAX_NUM_OF_RULES]; ///< Rule recovery text.
    long int      ruleId[MAX_NUM_OF_RULES]; ///< Persistent rule identifiers.
} ruleStruct_t;

/// Holds dynamic variable definitions.
typedef struct {
    int MaxNumOfDVars; ///< Number of populated dynamic variable entries.
    char *varName[MAX_NUM_OF_DVARS]; ///< Rule language variable names.
    char *action[MAX_NUM_OF_DVARS]; ///< Actions associated with each variable.
    char *var2CMap[MAX_NUM_OF_DVARS]; ///< Mapping from rule variable to C symbol.
    long int   varId[MAX_NUM_OF_DVARS]; ///< Variable identifiers.
} rulevardef_t;

/// Alias for the dynamic variable mapping table.
typedef rulevardef_t dvmStruct_t;

/// Holds function-to-C symbol mappings.
typedef struct {
    int MaxNumOfFMaps; ///< Number of populated function map entries.
    char *funcName[MAX_NUM_OF_FMAPS]; ///< Rule language function names.
    char *func2CMap[MAX_NUM_OF_FMAPS]; ///< Mapping from function name to C symbol.
    long int  fmapId[MAX_NUM_OF_FMAPS]; ///< Function map identifiers.
} rulefmapdef_t;

/// Alias for the function mapping table.
typedef rulefmapdef_t fnmapStruct_t;

/// Describes microservices available to the rule engine.
typedef struct {
    int MaxNumOfMsrvcs; ///< Number of populated microservice entries.
    long int   msrvcId[MAX_NUM_OF_MSRVCS]; ///< Microservice identifiers.
    char    *moduleName[MAX_NUM_OF_MSRVCS]; ///< Owning module names.
    char    *msrvcName[MAX_NUM_OF_MSRVCS]; ///< Microservice names.
    char    *msrvcSignature[MAX_NUM_OF_MSRVCS]; ///< Declared signatures.
    char    *msrvcVersion[MAX_NUM_OF_MSRVCS]; ///< Version strings.
    char    *msrvcHost[MAX_NUM_OF_MSRVCS]; ///< Host affinity information.
    char    *msrvcLocation[MAX_NUM_OF_MSRVCS]; ///< Installation locations.
    char    *msrvcLanguage[MAX_NUM_OF_MSRVCS]; ///< Implementation languages.
    char    *msrvcTypeName[MAX_NUM_OF_MSRVCS]; ///< Declared type names.
    long int   msrvcStatus[MAX_NUM_OF_MSRVCS]; ///< Per-microservice status values.
} msrvcStruct_t;

/// Core rule definitions.
extern ruleStruct_t coreRuleStrct;
/// Core dynamic variable definitions.
extern rulevardef_t coreRuleVarDef;
/// Core function map definitions.
extern rulefmapdef_t coreRuleFuncMapDef;
/// Core microservice registry.
extern  msrvcStruct_t coreMsrvcStruct;
/// Application rule definitions.
extern ruleStruct_t appRuleStrct;
/// Application dynamic variable definitions.
extern rulevardef_t appRuleVarDef;
/// Application function map definitions.
extern rulefmapdef_t appRuleFuncMapDef;
/// Application microservice registry.
extern  msrvcStruct_t appMsrvcStruct;
/// Enables rule engine test mode when non-zero.
extern int reTestFlag;
/// Enables loopback behavior when non-zero.
extern int reLoopBackFlag;
/// Enables rule engine debug tracing when non-zero.
extern int GlobalREDebugFlag;
/// Enables rule engine auditing when non-zero.
extern int GlobalREAuditFlag;
/// Full debug stack history.
extern char *reDebugStackFull[REDEBUG_STACK_SIZE_FULL];
/// Current debug stack window.
extern struct reDebugStack reDebugStackCurr[REDEBUG_STACK_SIZE_CURR];
/// Current index into `reDebugStackFull`.
extern int reDebugStackFullPtr;
/// Current index into `reDebugStackCurr`.
extern int reDebugStackCurrPtr;

//#include "irods/private/re/reFuncDefs.hpp"
#include "irods/private/re/reHelpers1.hpp"
#endif  /* RE_GLOBALS_EXTERN_H */
