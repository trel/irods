#ifndef IRODS_NREP_CONFIGURATION_HPP
#define IRODS_NREP_CONFIGURATION_HPP

#include "irods/private/re/rules.hpp"
#include "irods/irods_hashtable.h"
#include "irods/private/re/parser.hpp"
#include "irods/private/re/datetime.hpp"
#include "irods/checksum.h"

/// Resource flag for the core ruleset.
#define RESC_CORE_RULE_SET 0x1
/// Resource flag for the application ruleset.
#define RESC_APP_RULE_SET 0x2
/// Resource flag for the external ruleset.
#define RESC_EXT_RULE_SET 0x4
/// Resource flag for the system function description index.
#define RESC_SYS_FUNC_DESC_INDEX 0x10
/// Resource flag for the core function description index.
#define RESC_CORE_FUNC_DESC_INDEX 0x20
/// Resource flag for the application function description index.
#define RESC_APP_FUNC_DESC_INDEX 0x40
/// Resource flag for the external function description index.
#define RESC_EXT_FUNC_DESC_INDEX 0x80
/// Resource flag for the system region.
#define RESC_REGION_SYS 0x100
/// Resource flag for the core region.
#define RESC_REGION_CORE 0x200
/// Resource flag for the application region.
#define RESC_REGION_APP 0x400
/// Resource flag for the external region.
#define RESC_REGION_EXT 0x800
/// Resource flag for the serialized cache image.
#define RESC_CACHE 0x1000

/// Tracks initialization state for rule engine resources.
typedef enum ruleEngineStatus {
    UNINITIALIZED, ///< Resource has not been created.
    INITIALIZED, ///< Resource is allocated and ready for use.
    COMPRESSED, ///< Resource exists only in compressed cache form.
    /*SHARED,
    LOCAL,
    DISABLED*/
} RuleEngineStatus;

/// Aggregates rule engine resources and cache state.
struct Cache
{
    unsigned char *address; ///< Serialized cache image base address.
    unsigned char *pointers; ///< Pointer fixup table for the cache image.
    size_t dataSize; ///< Size of the serialized data section.
    size_t cacheSize; ///< Total size of the cache image.
    RuleEngineStatus coreRuleSetStatus; ///< Status of the core ruleset.
    RuleEngineStatus appRuleSetStatus; ///< Status of the application ruleset.
    RuleEngineStatus extRuleSetStatus; ///< Status of the external ruleset.
    RuleEngineStatus sysFuncDescIndexStatus; ///< Status of the system function index.
    RuleEngineStatus coreFuncDescIndexStatus; ///< Status of the core function index.
    RuleEngineStatus appFuncDescIndexStatus; ///< Status of the application function index.
    RuleEngineStatus extFuncDescIndexStatus; ///< Status of the external function index.
    RuleEngineStatus ruleEngineStatus; ///< Overall rule engine state.
    RuleEngineStatus cacheStatus; ///< Status of the serialized cache.
    RuleEngineStatus sysRegionStatus; ///< Status of the system region.
    RuleEngineStatus coreRegionStatus; ///< Status of the core region.
    RuleEngineStatus appRegionStatus; ///< Status of the application region.
    RuleEngineStatus extRegionStatus; ///< Status of the external region.
    RuleSet *coreRuleSet; ///< Core ruleset pointer.
    RuleSet *appRuleSet; ///< Application ruleset pointer.
    RuleSet *extRuleSet; ///< External ruleset pointer.
    Env *sysFuncDescIndex; ///< System function description index.
    Env *coreFuncDescIndex; ///< Core function description index.
    Env *appFuncDescIndex; ///< Application function description index.
    Env *extFuncDescIndex; ///< External function description index.
    Region *sysRegion; ///< System allocation region.
    Region *coreRegion; ///< Core allocation region.
    Region *appRegion; ///< Application allocation region.
    Region *extRegion; ///< External allocation region.
    int tvarNumber; ///< Counter for generated type variables.
    int clearDelayed; ///< Indicates whether deferred cleanup is pending.
    time_type timestamp; ///< Timestamp of the loaded configuration.
    int logging; ///< User logging preference.
    char ruleBase[RULE_SET_DEF_LENGTH]; ///< Rule base source text or identifier.
    char hash[CHKSUM_LEN]; ///< Checksum of the active rule base.
};

/// Returns true if a component is initialized or cached in compressed form.
#define isComponentInitialized(x) ((x)==INITIALIZED || (x)==COMPRESSED)
/// Returns true if a component is currently allocated in memory.
#define isComponentAllocated(x) ((x)==INITIALIZED)
/// Frees an initialized region selected by the resource flags.
#define clearRegion(u, l) \
		if((resources & RESC_REGION_##u) && isComponentAllocated(ruleEngineConfig.l##Region##Status)) { \
			region_free(ruleEngineConfig.l##Region); \
			ruleEngineConfig.l##Region = NULL; \
			ruleEngineConfig.l##Region##Status = UNINITIALIZED; \
		} \

/// Schedules an initialized region for deferred cleanup.
#define delayClearRegion(u, l) \
		if((resources & RESC_REGION_##u) && isComponentAllocated(ruleEngineConfig.l##Region##Status)) { \
			listAppendNoRegion(&regionsToClear, ruleEngineConfig.l##Region); \
			ruleEngineConfig.l##Region = NULL; \
			ruleEngineConfig.l##Region##Status = UNINITIALIZED; \
		} \

/// Creates a region if it has not been initialized yet.
#define createRegion(u, l) \
		if(ruleEngineConfig.l##Region##Status != INITIALIZED) { \
			ruleEngineConfig.l##Region = make_region(0, NULL); \
			ruleEngineConfig.l##Region##Status = INITIALIZED; \
		} \

/// Clears a ruleset selected by the resource flags.
#define clearRuleSet(u, l) \
		if((resources & RESC_##u##_RULE_SET) && isComponentInitialized(ruleEngineConfig.l##RuleSetStatus)) { \
			ruleEngineConfig.l##RuleSet = NULL; \
			ruleEngineConfig.l##RuleSetStatus = UNINITIALIZED; \
		} \

/// Marks a ruleset for deferred cleanup.
#define delayClearRuleSet(u, l) \
		if((resources & RESC_##u##_RULE_SET) && isComponentInitialized(ruleEngineConfig.l##RuleSetStatus)) { \
			ruleEngineConfig.l##RuleSet = NULL; \
			ruleEngineConfig.l##RuleSetStatus = UNINITIALIZED; \
		} \

/// Marks a function description index for deferred cleanup.
#define delayClearFuncDescIndex(u, l) \
		if((resources & RESC_##u##_FUNC_DESC_INDEX) && isComponentInitialized(ruleEngineConfig.l##FuncDescIndexStatus)) { \
			listAppendNoRegion(&envToClear, ruleEngineConfig.l##FuncDescIndex); \
			ruleEngineConfig.l##FuncDescIndex = NULL; \
			ruleEngineConfig.l##FuncDescIndexStatus = UNINITIALIZED; \
		} else if((resources & RESC_##u##_FUNC_DESC_INDEX) && ruleEngineConfig.l##FuncDescIndexStatus == COMPRESSED) { \
			ruleEngineConfig.l##FuncDescIndexStatus = UNINITIALIZED; \
		} \

/// Creates a ruleset object if it is not initialized.
#define createRuleSet(u, l) \
		if(!isComponentInitialized(ruleEngineConfig.l##RuleSetStatus)) { \
			ruleEngineConfig.l##RuleSet = (RuleSet *)region_alloc(ruleEngineConfig.l##Region, sizeof(RuleSet)); \
			ruleEngineConfig.l##RuleSet->len = 0; \
			ruleEngineConfig.l##RuleSetStatus = INITIALIZED; \
		} \

/// Clears a function description index selected by the resource flags.
#define clearFuncDescIndex(u, l) \
	if((resources & RESC_##u##_FUNC_DESC_INDEX) && isComponentInitialized(ruleEngineConfig.l##FuncDescIndexStatus)) { \
		/* deleteEnv(ruleEngineConfig.l##FuncDescIndex, 1); */\
		ruleEngineConfig.l##FuncDescIndex = NULL; \
		ruleEngineConfig.l##FuncDescIndexStatus = UNINITIALIZED; \
	} \

/// Creates a function description index if it is not initialized.
#define createFuncDescIndex(u, l) \
	if(!isComponentInitialized(ruleEngineConfig.l##FuncDescIndexStatus)) { \
		ruleEngineConfig.l##FuncDescIndex = newEnv(NULL, NULL, NULL, ruleEngineConfig.l##Region); \
		ruleEngineConfig.l##FuncDescIndex->current = newHashTable2(1000, ruleEngineConfig.l##Region); \
		ruleEngineConfig.l##FuncDescIndexStatus = INITIALIZED; \
	} \

/// Global rule engine status.
extern RuleEngineStatus _ruleEngineStatus;
/// Non-zero when running in the server process.
extern int isServer;
/// Global rule engine configuration cache.
extern Cache ruleEngineConfig;

/// Returns the current global rule engine status.
RuleEngineStatus getRuleEngineStatus();
/// Unlinks all function description indexes from the active configuration.
int unlinkFuncDescIndex();
/// Clears the resources identified by the resource bitmask.
int clearResources( int resources );
/// Rebuilds the core rule index.
int clearCoreRuleIndex( );
/// Rebuilds the application rule index.
int clearAppRuleIndex( );
/// Reads rule structures and populates a ruleset from a file.
int readRuleStructAndRuleSetFromFile( const char *ruleBaseName, const char *rulesFileName );
/// Reads rule structures and populates a ruleset from an in-memory buffer.
int readRuleStructAndRuleSetFromBuffer(const char* ruleBaseName, char* ruleBase);
/// Loads rules either from cache or from a rule base file.
int loadRuleFromCacheOrFile( const char*, const char *irbSet );
/// Creates the core rule index.
int createCoreRuleIndex( );
/// Creates the application rule index.
int createAppRuleIndex( );
/// Returns the number of available rules.
int availableRules();
/// Removes one rule from the external rule index.
void removeRuleFromExtIndex( char *ruleName, int i );
/// Appends one rule to the external rule index.
void appendRuleIntoExtIndex( RuleDesc *rule, int i, Region *r );
/// Prepends one rule to the application rule index.
void prependRuleIntoAppIndex( RuleDesc *rule, int i, Region *r );
/// Creates a checkpoint for the external ruleset.
int checkPointExtRuleSet( Region *r );
/*void appendAppRule( RuleDesc *rd, Region *r );*/
/// Prepends one rule to the application ruleset.
void prependAppRule( RuleDesc *rd, Region *r );
/// Restores the external ruleset to a previous checkpoint.
void popExtRuleSet( int checkPoint );
/// Performs deferred cleanup of delayed resources.
void clearDelayed();
/// Generates function description tables for the rule engine.
int generateFunctionDescriptionTables();
/// Reads a user information attribute from the catalog.
int readICatUserInfo( char *userName, char *attr, char userInfo[MAX_NAME_LEN], rsComm_t *rsComm );
/// Writes a user information attribute to the catalog.
int writeICatUserInfo( char *userName, char *attr, char *userInfo, rsComm_t *rsComm );
/// Reads the rule logging preference for a user.
int readICatUserLogging( char *userName, int *logging, rsComm_t *rsComm );
/// Writes the rule logging preference for a user.
int writeICatUserLogging( char *userName, int logging, rsComm_t *rsComm );

/// Catalog attribute storing the per-user rule logging preference.
#define RE_LOGGING_ATTR "rulelogging"

#endif // IRODS_NREP_CONFIGURATION_HPP
