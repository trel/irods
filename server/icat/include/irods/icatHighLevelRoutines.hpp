#ifndef IRODS_ICAT_HIGHLEVEL_ROUTINES_HPP
#define IRODS_ICAT_HIGHLEVEL_ROUTINES_HPP

/// \file

#include "irods/objInfo.h"
#include "irods/ruleExecSubmit.h"
#include "irods/rcConnect.h"
#include "irods/icatStructs.hpp"
#include "irods/specificQuery.h"
#include "irods/irods_resource_manager.hpp"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <string>
#include <vector>
#include <map>

#include <boost/tuple/tuple.hpp>

#include "irods/irods_error.hpp"

using leaf_bundle_t = irods::resource_manager::leaf_bundle_t;

extern icatSessionStruct icss;

/// \brief Opens the catalog connection.
/// \return Status code.
int chlOpen();

/// \brief Closes the catalog connection.
/// \return Status code.
int chlClose();

/// \brief Reports whether the catalog connection is active.
/// \return Connection state as a status code.
int chlIsConnected();

/// \brief Updates metadata for a data object.
/// \param[in] rsComm Server communication context.
/// \param[in] dataObjInfo Data object information to update.
/// \param[in] regParam Additional update options.
/// \return Status code.
int chlModDataObjMeta( rsComm_t *rsComm, dataObjInfo_t *dataObjInfo,
                       keyValPair_t *regParam );

/// \brief Adjusts the stored object count for a resource.
/// \param[in] _resc Resource name.
/// \param[in] _delta Count delta to apply.
/// \return Status code.
int chlUpdateRescObjCount( const std::string& _resc, int _delta );

/// \brief Registers a data object in the catalog.
/// \param[in] rsComm Server communication context.
/// \param[in] dataObjInfo Data object information to register.
/// \return Status code.
int chlRegDataObj( rsComm_t *rsComm, dataObjInfo_t *dataObjInfo );

/// \brief Registers a rule execution object.
/// \param[in] rsComm Server communication context.
/// \param[in] ruleExecSubmitInp Rule execution input to register.
/// \return Status code.
int chlRegRuleExecObj( rsComm_t *rsComm,
                       ruleExecSubmitInp_t *ruleExecSubmitInp );

/// \brief Registers a replica in the catalog.
/// \param[in] rsComm Server communication context.
/// \param[in] srcDataObjInfo Source replica information.
/// \param[in] dstDataObjInfo Destination replica information.
/// \param[in] condInput Conditional options.
/// \return Status code.
int chlRegReplica( rsComm_t *rsComm, dataObjInfo_t *srcDataObjInfo,
                   dataObjInfo_t *dstDataObjInfo, keyValPair_t *condInput );

/// \brief Unregisters a data object from the catalog.
/// \param[in] rsComm Server communication context.
/// \param[in] dataObjInfo Data object information to unregister.
/// \param[in] condInput Conditional options.
/// \return Status code.
int chlUnregDataObj( rsComm_t *rsComm, dataObjInfo_t *dataObjInfo,
                     keyValPair_t *condInput );

/// \brief Registers a resource.
/// \param[in] rsComm Server communication context.
/// \param[in] _resc_input Resource properties.
/// \return Status code.
int chlRegResc( rsComm_t *rsComm, std::map<std::string, std::string>& _resc_input );

/// \brief Adds a child resource relationship.
/// \param[in] rsComm Server communication context.
/// \param[in] _resc_input Parent and child resource properties.
/// \return Status code.
int chlAddChildResc( rsComm_t* rsComm, std::map<std::string, std::string>& _resc_input );

/// \brief Deletes a resource.
/// \param[in] rsComm Server communication context.
/// \param[in] _resc_name Resource name.
/// \param[in] _dryrun Non-zero to validate without applying changes.
/// \return Status code.
int chlDelResc( rsComm_t *rsComm, const std::string& _resc_name, int _dryrun = 0 ); // JMC

/// \brief Removes a child resource relationship.
/// \param[in] rsComm Server communication context.
/// \param[in] _resc_input Parent and child resource properties.
/// \return Status code.
int chlDelChildResc( rsComm_t* rsComm, std::map<std::string, std::string>& _resc_input );

/// \brief Rolls back the current catalog transaction.
/// \param[in] rsComm Server communication context.
/// \return Status code.
int chlRollback( rsComm_t *rsComm );

/// \brief Commits the current catalog transaction.
/// \param[in] rsComm Server communication context.
/// \return Status code.
int chlCommit( rsComm_t *rsComm );

/// \brief Deletes a user rule execution entry.
/// \param[in] rsComm Server communication context.
/// \param[in] userInfo User information identifying the entry.
/// \return Status code.
int chlDelUserRE( rsComm_t *rsComm, userInfo_t *userInfo );

/// \brief Registers a collection as an administrator.
/// \param[in] rsComm Server communication context.
/// \param[in] collInfo Collection information to register.
/// \return Status code.
int chlRegCollByAdmin( rsComm_t *rsComm, collInfo_t *collInfo );

/// \brief Registers a collection.
/// \param[in] rsComm Server communication context.
/// \param[in] collInfo Collection information to register.
/// \return Status code.
int chlRegColl( rsComm_t *rsComm, collInfo_t *collInfo );

/// \brief Modifies collection metadata in the catalog.
/// \param[in] rsComm Server communication context.
/// \param[in] collInfo Collection information containing updates.
/// \return Status code.
int chlModColl( rsComm_t *rsComm, collInfo_t *collInfo );

/// \brief Executes a general query.
/// \param[in] genQueryInp Query input structure.
/// \param[out] result Query result structure.
/// \return Status code.
int chlGenQuery( genQueryInp_t genQueryInp, genQueryOut_t *result );

/// \brief Sets access-control context for subsequent general queries.
/// \param[in] user User name.
/// \param[in] zone User zone.
/// \param[in] host Client host.
/// \param[in] priv Privilege level.
/// \param[in] controlFlag Access-control setup flag.
/// \return Status code.
int chlGenQueryAccessControlSetup( const char *user, const char *zone, const char *host,
                                   int priv, int controlFlag );

/// \brief Sets ticket context for subsequent general queries.
/// \param[in] ticket Ticket string.
/// \param[in] clientAddr Client address.
/// \return Status code.
int chlGenQueryTicketSetup( const char *ticket, const char *clientAddr );

/// \brief Executes a specific query.
/// \param[in] specificQueryInp Specific query input structure.
/// \param[out] genQueryOut Query result structure.
/// \return Status code.
int chlSpecificQuery( specificQueryInp_t specificQueryInp,
                      genQueryOut_t *genQueryOut );

/// \brief Deletes a collection as an administrator.
/// \param[in] rsComm Server communication context.
/// \param[in] collInfo Collection information to delete.
/// \return Status code.
int chlDelCollByAdmin( rsComm_t *rsComm, collInfo_t *collInfo );

/// \brief Deletes a collection.
/// \param[in] rsComm Server communication context.
/// \param[in] collInfo Collection information to delete.
/// \return Status code.
int chlDelColl( rsComm_t *rsComm, collInfo_t *collInfo );

/// \brief Checks authentication credentials.
/// \param[in] rsComm Server communication context.
/// \param[in] scheme Authentication scheme.
/// \param[in] challenge Authentication challenge.
/// \param[in] response Authentication response.
/// \param[in] username User name being authenticated.
/// \param[out] userPrivLevel User privilege level.
/// \param[out] clientPrivLevel Client privilege level.
/// \return Status code.
int chlCheckAuth( rsComm_t *rsComm, const char* scheme, const char *challenge, const char *response,
                  const char *username, int *userPrivLevel, int *clientPrivLevel );

/// \brief Generates a temporary password value.
/// \param[in] rsComm Server communication context.
/// \param[out] pwValueToHash Buffer receiving the password value.
/// \param[in] otherUser Optional target user name.
/// \return Status code.
int chlMakeTempPw( rsComm_t *rsComm, char *pwValueToHash, const char *otherUser );

/// \brief Generates a limited-lifetime password value.
/// \param[in] rsComm Server communication context.
/// \param[in] ttl Password lifetime.
/// \param[out] pwValueToHash Buffer receiving the password value.
/// \return Status code.
int chlMakeLimitedPw(rsComm_t* rsComm, int ttl, char* pwValueToHash);

/// \brief Modifies a user property.
/// \param[in] rsComm Server communication context.
/// \param[in] userName User name.
/// \param[in] option Property to modify.
/// \param[in] newValue New property value.
/// \return Status code.
int chlModUser( rsComm_t *rsComm, const char *userName, const char *option,
                const char *newValue );

/// \brief Modifies group membership or properties.
/// \param[in] rsComm Server communication context.
/// \param[in] groupName Group name.
/// \param[in] option Requested modification.
/// \param[in] userName User name involved in the change.
/// \param[in] userZone User zone involved in the change.
/// \return Status code.
int chlModGroup( rsComm_t *rsComm, const char *groupName, const char *option,
                 const char *userName, const char *userZone );

/// \brief Modifies a resource property.
/// \param[in] rsComm Server communication context.
/// \param[in] rescName Resource name.
/// \param[in] option Property to modify.
/// \param[in] optionValue New property value.
/// \return Status code.
int chlModResc( rsComm_t *rsComm, const char *rescName, const char *option,
                const char *optionValue );

/// \brief Updates resource data paths.
/// \param[in] rsComm Server communication context.
/// \param[in] rescName Resource name.
/// \param[in] oldPath Existing data path prefix.
/// \param[in] newPath Replacement data path prefix.
/// \param[in] userName User requesting the change.
/// \return Status code.
int chlModRescDataPaths( rsComm_t *rsComm, const char *rescName, const char *oldPath,
                         const char *newPath, const char *userName );

/// \brief Updates the recorded free space for a resource.
/// \param[in] rsComm Server communication context.
/// \param[in] rescName Resource name.
/// \param[in] updateValue Free-space update value.
/// \return Status code.
int chlModRescFreeSpace( rsComm_t *rsComm, const char *rescName,
                         int updateValue );

/// \brief Registers a user rule execution entry.
/// \param[in] rsComm Server communication context.
/// \param[in] userInfo User information to register.
/// \return Status code.
int chlRegUserRE( rsComm_t *rsComm, userInfo_t *userInfo );

/// \brief Adds AVU metadata.
/// \param[in] rsComm Server communication context.
/// \param[in] type Target object type.
/// \param[in] name Target object name.
/// \param[in] attribute AVU attribute.
/// \param[in] value AVU value.
/// \param[in] units AVU units.
/// \param[in] condInput Conditional options.
/// \return Status code.
int chlAddAVUMetadata( rsComm_t *rsComm, const char *type,
                       const char *name, const char *attribute, const char *value, const char *units,
                       const KeyValPair* condInput);

/// \brief Deletes AVU metadata.
/// \param[in] rsComm Server communication context.
/// \param[in] option Delete mode.
/// \param[in] type Target object type.
/// \param[in] name Target object name.
/// \param[in] attribute AVU attribute.
/// \param[in] value AVU value.
/// \param[in] units AVU units.
/// \param[in] noCommit Non-zero to leave the transaction uncommitted.
/// \param[in] condInput Conditional options.
/// \return Status code.
int chlDeleteAVUMetadata( rsComm_t *rsComm, int option, const char *type,
                          const char *name, const char *attribute, const char *value, const char *units, int noCommit,
                          const KeyValPair* condInput);

/// \brief Replaces AVU metadata values.
/// \param[in] rsComm Server communication context.
/// \param[in] type Target object type.
/// \param[in] name Target object name.
/// \param[in] attribute AVU attribute.
/// \param[in] newValue Replacement AVU value.
/// \param[in] newUnit Replacement AVU units.
/// \param[in] condInput Conditional options.
/// \return Status code.
int chlSetAVUMetadata( rsComm_t *rsComm, const char *type, // JMC - backport 4836
                       const char *name, const char *attribute, const char *newValue, const char *newUnit,
                       const KeyValPair* condInput);

/// \brief Copies AVU metadata between catalog entries.
/// \param[in] rsComm Server communication context.
/// \param[in] type1 Source object type.
/// \param[in] type2 Destination object type.
/// \param[in] name1 Source object name.
/// \param[in] name2 Destination object name.
/// \param[in] condInput Conditional options.
/// \return Status code.
int chlCopyAVUMetadata( rsComm_t *rsComm, const char *type1,  const char *type2,
                        const char *name1, const char *name2,
                        const KeyValPair* condInput);

/// \brief Applies a general AVU metadata modification.
/// \param[in] rsComm Server communication context.
/// \param[in] type Target object type.
/// \param[in] name Target object name.
/// \param[in] attribute AVU attribute.
/// \param[in] value AVU value.
/// \param[in] unitsOrChange0 Units or first change argument.
/// \param[in] change1 Additional change argument.
/// \param[in] change2 Additional change argument.
/// \param[in] change3 Additional change argument.
/// \param[in] condInput Conditional options.
/// \return Status code.
int chlModAVUMetadata( rsComm_t *rsComm, const char *type, const char *name,
                       const char *attribute, const char *value, const char *unitsOrChange0,
                       const char *change1, const char *change2, const char *change3,
                       const KeyValPair *condInput );

/// \brief Modifies access control on a catalog path.
/// \param[in] rsComm Server communication context.
/// \param[in] recursiveFlag Non-zero to apply recursively.
/// \param[in] accessLevel Access level to apply.
/// \param[in] userName User receiving the access level.
/// \param[in] zone User zone.
/// \param[in] pathName Target path.
/// \return Status code.
int chlModAccessControl( rsComm_t *rsComm, int recursiveFlag,
                         const char* accessLevel, const char *userName, const char *zone,
                         const char* pathName );

/// \brief Registers a delayed rule execution entry.
/// \param[in] rsComm Server communication context.
/// \param[in] ruleExecSubmitInp Rule execution input to register.
/// \return Status code.
int chlRegRuleExec( rsComm_t *rsComm, ruleExecSubmitInp_t *ruleExecSubmitInp );

/// \brief Modifies a delayed rule execution entry.
/// \param[in] rsComm Server communication context.
/// \param[in] ruleExecId Rule execution identifier.
/// \param[in] regParam Update parameters.
/// \return Status code.
int chlModRuleExec( rsComm_t *rsComm, const char *ruleExecId, keyValPair_t *regParam );

/// \brief Deletes a delayed rule execution entry.
/// \param[in] rsComm Server communication context.
/// \param[in] ruleExecId Rule execution identifier.
/// \return Status code.
int chlDelRuleExec( rsComm_t *rsComm, const char *ruleExecId );

/// \brief Renames a catalog object.
/// \param[in] rsComm Server communication context.
/// \param[in] objId Object identifier.
/// \param[in] newName Replacement name.
/// \return Status code.
int chlRenameObject( rsComm_t *rsComm, rodsLong_t objId, const char *newName );

/// \brief Moves a catalog object to another collection.
/// \param[in] rsComm Server communication context.
/// \param[in] objId Object identifier.
/// \param[in] targetCollId Target collection identifier.
/// \return Status code.
int chlMoveObject( rsComm_t *rsComm, rodsLong_t objId, rodsLong_t targetCollId );

/// \brief Registers a token value.
/// \param[in] rsComm Server communication context.
/// \param[in] nameSpace Token namespace.
/// \param[in] name Token name.
/// \param[in] value Primary token value.
/// \param[in] value2 Secondary token value.
/// \param[in] value3 Tertiary token value.
/// \param[in] comment Token comment.
/// \return Status code.
int chlRegToken( rsComm_t *rsComm, const char *nameSpace, const char *name, const char *value,
                 const char *value2, const char *value3, const char *comment );

/// \brief Deletes a token value.
/// \param[in] rsComm Server communication context.
/// \param[in] nameSpace Token namespace.
/// \param[in] Name Token name.
/// \return Status code.
int chlDelToken( rsComm_t *rsComm, const char *nameSpace, const char *Name );

/// \brief Registers a zone.
/// \param[in] rsComm Server communication context.
/// \param[in] zoneName Zone name.
/// \param[in] zoneType Zone type.
/// \param[in] zoneConnInfo Zone connection information.
/// \param[in] zoneComment Zone comment.
/// \return Status code.
int chlRegZone( rsComm_t *rsComm, const char *zoneName, const char *zoneType,
                const char *zoneConnInfo, const char *zoneComment );

/// \brief Modifies a zone property.
/// \param[in] rsComm Server communication context.
/// \param[in] zoneName Zone name.
/// \param[in] option Property to modify.
/// \param[in] optionValue New property value.
/// \return Status code.
int chlModZone( rsComm_t *rsComm, const char *zoneName, const char *option,
                const char *optionValue );

/// \brief Modifies access control on a zone collection.
/// \param[in] rsComm Server communication context.
/// \param[in] accessLevel Access level to apply.
/// \param[in] userName User receiving the access level.
/// \param[in] pathName Target collection path.
/// \return Status code.
int chlModZoneCollAcl( rsComm_t *rsComm, const char* accessLevel, const char *userName,
                       const char* pathName );

/// \brief Deletes a zone.
/// \param[in] rsComm Server communication context.
/// \param[in] zoneName Zone name.
/// \return Status code.
int chlDelZone( rsComm_t *rsComm, const char *zoneName );

/// \brief Renames the local zone.
/// \param[in] rsComm Server communication context.
/// \param[in] oldZoneName Current zone name.
/// \param[in] newZoneName Replacement zone name.
/// \return Status code.
int chlRenameLocalZone( rsComm_t *rsComm, const char *oldZoneName, const char *newZoneName );

/// \brief Renames a collection.
/// \param[in] rsComm Server communication context.
/// \param[in] oldName Current collection name.
/// \param[in] newName Replacement collection name.
/// \return Status code.
int chlRenameColl( rsComm_t *rsComm, const char *oldName, const char *newName );

/// \brief Registers a server load sample.
/// \param[in] rsComm Server communication context.
/// \param[in] hostName Host name.
/// \param[in] rescName Resource name.
/// \param[in] cpuUsed CPU usage value.
/// \param[in] memUsed Memory usage value.
/// \param[in] swapUsed Swap usage value.
/// \param[in] runqLoad Run queue load value.
/// \param[in] diskSpace Disk space value.
/// \param[in] netInput Network input value.
/// \param[in] netOutput Network output value.
/// \return Status code.
int chlRegServerLoad( rsComm_t *rsComm,
                      const char *hostName, const char *rescName,
                      const char *cpuUsed, const char *memUsed, const char *swapUsed, const char *runqLoad,
                      const char *diskSpace, const char *netInput, const char *netOutput );

/// \brief Deletes server load samples older than a threshold.
/// \param[in] rsComm Server communication context.
/// \param[in] secondsAgo Age threshold in seconds.
/// \return Status code.
int chlPurgeServerLoad( rsComm_t *rsComm, const char *secondsAgo );

/// \brief Registers a server load digest entry.
/// \param[in] rsComm Server communication context.
/// \param[in] rescName Resource name.
/// \param[in] loadFactor Aggregated load value.
/// \return Status code.
int chlRegServerLoadDigest( rsComm_t *rsComm, const char *rescName, const char *loadFactor );

/// \brief Deletes server load digest entries older than a threshold.
/// \param[in] rsComm Server communication context.
/// \param[in] secondsAgo Age threshold in seconds.
/// \return Status code.
int chlPurgeServerLoadDigest( rsComm_t *rsComm, const char *secondsAgo );

/// \brief Recalculates usage and quota information.
/// \param[in] rsComm Server communication context.
/// \return Status code.
int chlCalcUsageAndQuota( rsComm_t *rsComm );

/// \brief Fetches a grid configuration value.
/// \param[in] _rsComm Server communication context.
/// \param[in] _namespace Configuration namespace.
/// \param[in] _optionName Configuration option name.
/// \param[out] _optionValue Buffer receiving the option value.
/// \param[in] _optionValueBufferSize Size of \p _optionValue in bytes.
/// \return Status code.
int chlGetGridConfigurationValue(rsComm_t*   _rsComm,
                                 const char* _namespace,
                                 const char* _optionName,
                                 char*       _optionValue,
                                 std::size_t _optionValueBufferSize);

/// \brief Sets a grid configuration value.
/// \param[in] _rsComm Server communication context.
/// \param[in] _namespace Configuration namespace.
/// \param[in] _optionName Configuration option name.
/// \param[in] _optionValue Configuration option value.
/// \return Status code.
int chlSetGridConfigurationValue(rsComm_t*   _rsComm,
                                 const char* _namespace,
                                 const char* _optionName,
                                 const char* _optionValue);

/// \brief Sets a quota entry.
/// \param[in] rsComm Server communication context.
/// \param[in] type Quota target type.
/// \param[in] name Target user or group name.
/// \param[in] rescName Resource name.
/// \param[in] limit Quota limit value.
/// \return Status code.
int chlSetQuota( rsComm_t *rsComm, const char *type, const char *name, const char *rescName,
                 const char *limit );

/// \brief Checks quota information for a user on a resource.
/// \param[in] rsComm Server communication context.
/// \param[in] userName User name.
/// \param[in] rescName Resource name.
/// \param[out] userQuota Quota value for the user.
/// \param[out] quotaStatus Quota status indicator.
/// \return Status code.
int chlCheckQuota( rsComm_t *rsComm, const char *userName, const char *rescName,
                   rodsLong_t *userQuota, int *quotaStatus );

/// \brief Deletes AVUs no longer attached to catalog entries.
/// \param[in] rsComm Server communication context.
/// \return Status code.
int chlDelUnusedAVUs( rsComm_t *rsComm ); // TODO Does this need a condInput too?

/// \brief Registers a specific query.
/// \param[in] rsComm Server communication context.
/// \param[in] alias Query alias.
/// \param[in] sql Query SQL text.
/// \return Status code.
int chlAddSpecificQuery( rsComm_t *rsComm, const char *alias, const char *sql );

/// \brief Deletes a specific query.
/// \param[in] rsComm Server communication context.
/// \param[in] sqlOrAlias Query SQL text or alias.
/// \return Status code.
int chlDelSpecificQuery( rsComm_t *rsComm, const char *sqlOrAlias );

/// \brief Retrieves the local zone name.
/// Writes the zone name to the provided string.
/// \return Status code.
int chlGetLocalZone( std::string& );

/// \brief Initializes query table metadata.
/// \return Status code.
int sTableInit();

/// \brief Registers a foreign-key style table link for query setup.
/// \param[in] table1 First table name.
/// \param[in] table2 Second table name.
/// \param[in] connectingSQL SQL fragment connecting the tables.
/// \return Status code.
int sFklink( const char *table1, const char *table2, const char *connectingSQL );

/// \brief Registers a table for query setup.
/// \param[in] tableName Table name.
/// \param[in] tableAlias Table alias.
/// \param[in] cycler Table cycle identifier.
/// \return Status code.
int sTable( const char *tableName, const char *tableAlias, int cycler );

/// \brief Registers a column for query setup.
/// \param[in] defineVal Column definition value.
/// \param[in] tableName Table name.
/// \param[in] columnName Column name.
/// \return Status code.
int sColumn( int defineVal, const char *tableName, const char *columnName );

/// \brief Sets catalog debug mode.
/// \param[in] debugMode Debug mode string.
/// \return Status code.
int chlDebug( const char *debugMode );

/// \brief Inserts a rule base entry.
/// \param[in] rsComm Server communication context.
/// \param[in] baseName Rule base name.
/// \param[in] priorityStr Rule priority.
/// \param[in] ruleName Rule name.
/// \param[in] ruleHead Rule head.
/// \param[in] ruleCondition Rule condition.
/// \param[in] ruleAction Rule action.
/// \param[in] ruleRecovery Rule recovery action.
/// \param[in] ruleIdStr Rule identifier.
/// \param[in] myTime Timestamp string.
/// \return Status code.
int chlInsRuleTable( rsComm_t *rsComm,
                     const char *baseName, const char *priorityStr, const char *ruleName,
                     const char *ruleHead, const char *ruleCondition, const char *ruleAction,
                     const char *ruleRecovery, const char *ruleIdStr, const char *myTime );

/// \brief Versions a rule base.
/// \param[in] rsComm Server communication context.
/// \param[in] baseName Rule base name.
/// \param[in] myTime Timestamp string.
/// \return Status code.
int chlVersionRuleBase( rsComm_t *rsComm,
                        const char *baseName, const char *myTime );

/// \brief Versions a DVM base.
/// \param[in] rsComm Server communication context.
/// \param[in] baseName DVM base name.
/// \param[in] myTime Timestamp string.
/// \return Status code.
int chlVersionDvmBase( rsComm_t *rsComm,
                       const char *baseName, const char *myTime );
/*int chlDatabaseObjectAdmin(rsComm_t *rsComm,
  databaseObjectAdminInp_t *databaseObjectAdminInp,
  databaseObjectAdminOut_t *databaseObjectAdminOut);*/
/// \brief Inserts a DVM table entry.
/// \param[in] rsComm Server communication context.
/// \param[in] baseName DVM base name.
/// \param[in] varName Variable name.
/// \param[in] action Requested action.
/// \param[in] var2CMap Variable-to-C mapping.
/// \param[in] myTime Timestamp string.
/// \return Status code.
int chlInsDvmTable( rsComm_t *rsComm,
                    const char *baseName, const char *varName, const char *action,
                    const char *var2CMap, const char *myTime );

/// \brief Inserts an FNM table entry.
/// \param[in] rsComm Server communication context.
/// \param[in] baseName FNM base name.
/// \param[in] funcName Function name.
/// \param[in] func2CMap Function-to-C mapping.
/// \param[in] myTime Timestamp string.
/// \return Status code.
int chlInsFnmTable( rsComm_t *rsComm,
                    const char *baseName, const char *funcName,
                    const char *func2CMap, const char *myTime );

/// \brief Inserts a microservice table entry.
/// \param[in] rsComm Server communication context.
/// \param[in] moduleName Module name.
/// \param[in] msrvcName Microservice name.
/// \param[in] msrvcSignature Microservice signature.
/// \param[in] msrvcVersion Microservice version.
/// \param[in] msrvcHost Microservice host.
/// \param[in] msrvcLocation Microservice location.
/// \param[in] msrvcLanguage Microservice language.
/// \param[in] msrvcTypeName Microservice type name.
/// \param[in] msrvcStatus Microservice status.
/// \param[in] myTime Timestamp string.
/// \return Status code.
int chlInsMsrvcTable( rsComm_t *rsComm,
                      const char *moduleName,
                      const char *msrvcName,
                      const char *msrvcSignature,
                      const char *msrvcVersion,
                      const char *msrvcHost,
                      const char *msrvcLocation,
                      const char *msrvcLanguage,
                      const char *msrvcTypeName,
                      const char *msrvcStatus,
                      const char *myTime );

/// \brief Versions an FNM base.
/// \param[in] rsComm Server communication context.
/// \param[in] baseName FNM base name.
/// \param[in] myTime Timestamp string.
/// \return Status code.
int chlVersionFnmBase( rsComm_t *rsComm,
                       const char *baseName, const char *myTime );

/// \brief Modifies ticket state or properties.
/// \param[in] rsComm Server communication context.
/// \param[in] opName Ticket operation name.
/// \param[in] ticket Ticket string.
/// \param[in] arg1 First operation argument.
/// \param[in] arg2 Second operation argument.
/// \param[in] arg3 Third operation argument.
/// \param[in] condInput Conditional options.
/// \return Status code.
int chlModTicket( rsComm_t *rsComm, const char *opName, const char *ticket,
                  const char *arg1, const char *arg2, const char *arg3,
                  const KeyValPair *condInput);

/// \brief Updates an iRODS PAM password.
/// \param[in] _comm Server communication context.
/// \param[in] _user_name User name.
/// \param[in] _ttl Password lifetime.
/// \param[in] _test_time Optional test timestamp.
/// \param[out] _password_buffer Buffer pointer receiving the generated password.
/// \param[in] _password_buffer_size Size of the password buffer.
/// \return Status code.
auto chlUpdateIrodsPamPassword(rsComm_t* _comm,
                               const char* _user_name,
                               int _ttl,
                               const char* _test_time,
                               char** _password_buffer,
                               std::size_t _password_buffer_size) -> int;

/// =-=-=-=-=-=-=-
/// @brief typedefs and prototype for query used for rebalancing operation
typedef std::vector< rodsLong_t > dist_child_result_t;

/// =-=-=-=-=-=-=-
/// @brief query which distinct data objects do not existin on a
///        given child resource which do exist on the parent
int chlGetDistinctDataObjsMissingFromChildGivenParent(
    const std::string&   _parent,
    const std::string&   _child,
    int                  _limit,
    const std::string&   _invocation_timestamp,
    dist_child_result_t& _results );

/// =-=-=-=-=-=-=-
/// @brief the the distinct data object count for a resource
int chlGetDistinctDataObjCountOnResource(
    const std::string&   _resc_name,
    long long&           _count );

/// \brief Retrieves the hierarchy string for a resource.
/// \param[in] resc_name Resource name.
/// \param[in] zone_name Zone name.
/// \param[out] hierarchy Resource hierarchy string.
/// \return Status code.
int chlGetHierarchyForResc(
    const std::string&	resc_name,
    const std::string&	zone_name,
    std::string& hierarchy );

/// \brief Checks an object request using the provided communication handle, type, name, and access strings.
/// \return Status code.
int chlCheckAndGetObjectID(
    rsComm_t*, // comm
    char*,     // type
    char*,     // name
    char* );   // access

/// \brief Retrieves the active catalog session structure through the provided pointer.
/// \return Status code.
int chlGetRcs( icatSessionStruct** );

/// \brief Retrieves replica candidates for leaf-bundle rebalancing.
/// \param[in] _count Number of results to return.
/// \param[in] _child_idx Index of the child resource.
/// \param[in] _bundles Leaf resource bundles.
/// \param[in] _invocation_timestamp Rebalance invocation timestamp.
/// \param[out] _results Result vector receiving data IDs.
/// \return Status code.
int chlGetReplListForLeafBundles(
    rodsLong_t                  _count,
    size_t                      _child_idx,
    const std::vector<leaf_bundle_t>* _bundles,
    const std::string*          _invocation_timestamp,
    dist_child_result_t*        _results );

/// \brief Retrieves data ids that require rebalancing. The offset may be used to skip previous results.
///
/// \parblock
/// In contrast to chlGetReplListForLeafBundles, this function has the additional
/// ability to offset the data ids returned from the generated query.
///
/// An example use for this function is to process the results in a partial
/// manner, leaving some results behind that need to be accounted for via
/// an offset.
/// \endparblock
///
/// \param[in] _count Number of results to return at once
/// \param[in] _child_idx Index to leaf resource of interest
/// \param[in] _bundles Vector of all leaf resources
/// \param[in] _invocation_timestamp Timestamp of when rebalance was called
/// \param[in,out] _results Vector of items needing rebalancing
/// \param[in] _offset Number of results to skip before returning
///
/// \returns Error code based on whether results are generated successfully.
///
/// \since 5.1.0
int chlGetReplListForLeafBundlesOffset(rodsLong_t _count,
                                       size_t _child_idx,
                                       const std::vector<leaf_bundle_t>* _bundles,
                                       const std::string* _invocation_timestamp,
                                       dist_child_result_t* _results,
                                       int _offset);

/// \brief High-level wrapper for database operation which calls cmlCheckDataObjId
///
/// \parblock
/// Checks to see whether the specified data_id shows up when searching for the
/// objects for which the given user has permissions to modify.
///
/// If a ticket is in use when this is called (that is, if mySessionTicket is set
/// in the database plugin), the ticket information will be checked. If the ticket
/// is expired, the bytes written with the ticket exceeds the write byte limit, or
/// the ticket is invalid for any other reason, the appropriate error code will be
/// returned. The usage count and the write file count will be updated as appropriate
/// as well.
/// \endparblock
///
/// \param[in,out] _comm iRODS comm structure
/// \param[in] _data_id Data ID for the object to check
///
/// \returns Error code based on whether authenticated user has permission\p
///          to modify the data object.
/// \retval 0 User has sufficient permission to modify the data object
/// \retval CAT_NO_ACCESS_PERMISSION User does not have sufficient permission\p
///                                  to modify the data object
///
/// \since 4.2.9
auto chl_check_permission_to_modify_data_object(RsComm& _comm, const rodsLong_t _data_id) -> int;

/// \brief High-level wrapper for database operation which calls cmlTicketUpdateWriteBytes
///
/// \parblock
/// Updates the write byte count for the ticket by the amount specified for
/// the given data object. Historically, this is usually the full size of a
/// replica for the data object being updated after some change (e.g. write).
/// \endparblock
///
/// \param[in,out] _comm iRODS comm structure
/// \param[in] _data_id Data ID for the object to check
/// \param[in] _bytes_written Number of bytes to add to write byte count
///
/// \returns Error code based on whether updating the catalog was successful
/// \retval 0 Success
///
/// \since 4.2.9
auto chl_update_ticket_write_byte_count(RsComm& _comm, const rodsLong_t _data_id, const rodsLong_t _bytes_written) -> int;

/// \brief High-level wrapper for fetching all the information about a delay rule.
///
/// Triggers policy associated with database operations.
///
/// \param[in]     _comm    The communication object.
/// \param[in]     _rule_id The ID of the delay rule.
/// \param[in,out] _info    A pointer to a vector of strings that will hold the row information.
///
/// \returns An error code representing whether the operation was successful.
/// \retval  0 On success.
/// \retval <0 On failure.
///
/// \since 4.2.12
auto chl_get_delay_rule_info(RsComm& _comm, const char* _rule_id, std::vector<std::string>* _info) -> int;

/// \brief High-level wrapper for atomically updating rows in R_DATA_MAIN for all replicas of a particular data object.
//
/// \parblock
/// \p json_input must have the following JSON structure:
/// \code{.js}
/// {
///     "replicas": [
///         {
///             "before": {
///                 "data_id": <string>,
///                 "coll_id": <string>,
///                 "data_repl_num": <string>,
///                 "data_version": <string>,
///                 "data_type_name": <string>,
///                 "data_size": <string>,
///                 "data_path": <string>,
///                 "data_owner_name": <string>,
///                 "data_owner_zone": <string>,
///                 "data_is_dirty": <string>,
///                 "data_status": <string>,
///                 "data_checksum": <string>,
///                 "data_expiry_ts": <string>,
///                 "data_map_id": <string>,
///                 "data_mode": <string>,
///                 "r_comment": <string>,
///                 "create_ts": <string>,
///                 "modify_ts": <string>,
///                 "resc_id": <string>
///             },
///             "after": {
///                 "data_id": <string>,
///                 "coll_id": <string>,
///                 "data_repl_num": <string>,
///                 "data_version": <string>,
///                 "data_type_name": <string>,
///                 "data_size": <string>,
///                 "data_path": <string>,
///                 "data_owner_name": <string>,
///                 "data_owner_zone": <string>,
///                 "data_is_dirty": <string>,
///                 "data_status": <string>,
///                 "data_checksum": <string>,
///                 "data_expiry_ts": <string>,
///                 "data_map_id": <string>,
///                 "data_mode": <string>,
///                 "r_comment": <string>,
///                 "create_ts": <string>,
///                 "modify_ts": <string>,
///                 "resc_id": <string>
///             }
///         },
///         ...
///     ]
/// }
/// \endcode
/// \endparblock
///
/// \param[in] _comm iRODS comm structure
/// \param[in] _json_input String holding a JSON object with an array of replicas at key "replicas"
///
/// \returns Error code based on whether updating the catalog was successful
/// \retval 0 Success
///
/// \since 4.2.12
auto chl_data_object_finalize(RsComm& _comm, const char* _json_input) -> int;

/// \brief High-level wrapper for verifying if the native authentication credentials for a specific
///        user are correct.
///
/// Triggers policy associated with database operations.
///
/// \param[in]     _comm     The communication object.
/// \param[in]     _username The part of a fully-qualified iRODS username preceding the pound sign.
/// \param[in]     _zone     The part of a fully-qualified iRODS username following the pound sign.
/// \param[in]     _password The expected obfuscated password.
/// \param[in,out] _correct  \parblock The integer that will hold whether the credentials are correct.
///
/// If the credentials are correct, this parameter is set to 1.
/// If the credentials are incorrect, this parameter is set to 0.
/// If correctness cannot be determined, this parameter is set to -1.
/// \endparblock
///
/// \return An integer.
/// \retval  0 On success.
/// \retval <0 On failure.
///
/// \since 4.3.1
auto chl_check_auth_credentials(RsComm& _comm,
                                const char* _username,
                                const char* _zone,
                                const char* _password,
                                int* _correct) -> int;

/// \brief High-level wrapper for executing SQL generated by GenQuery2.
///
/// Triggers policy associated with database operations.
///
/// \param[in]  _comm   The communication object.
/// \param[in]  _sql    The SQL, generated by the GenQuery2 parser, to execute.
/// \param[in]  _values The list of values to bind to the query.
/// \param[out] _output The pointer that will hold the results of the query.
///
/// \return An integer.
/// \retval  0 On success.
/// \retval <0 On failure.
///
/// \since 4.3.2
auto chl_execute_genquery2_sql(RsComm& _comm, const char* _sql, const std::vector<std::string>* _values, char** _output)
    -> int;

/// \brief High-level wrapper for locking delay rules.
///
/// Triggers policy associated with database operations.
///
/// \param[in] _comm          The communication object.
/// \param[in] _rule_id       The ID of the delay rule to lock.
/// \param[in] _delay_rule_host The FQDN, hostname, or IP of the delay server.
/// \param[in] _delay_rule_pid  The PID of the delay server process.
///
/// \return An integer.
/// \retval  0 On success.
/// \retval <0 On failure.
///
/// \see #rs_delay_rule_lock
///
/// \since 5.0.0
auto chl_delay_rule_lock(RsComm& _comm, const char* _rule_id, const char* _delay_rule_host, int _delay_rule_pid) -> int;

/// \brief High-level wrapper for unlocking delay rules.
///
/// Triggers policy associated with database operations.
///
/// \param[in] _comm     The communication object.
/// \param[in] _rule_ids A JSON string containing a list of delay rule IDs.
///
/// \return An integer.
/// \retval  0 On success.
/// \retval <0 On failure.
///
/// \see #rs_delay_rule_unlock
///
/// \since 5.0.0
auto chl_delay_rule_unlock(RsComm& _comm, const char* _rule_ids) -> int;

/// \brief High-level wrapper for updating the access time of one or more replicas.
///
/// Triggers policy associated with database operations.
///
/// \param[in]  _comm       The communication object.
/// \param[in]  _json_input A JSON string containing access time update information.
/// \param[out] _output     A pointer which will hold error details about the operation, if available.
///
/// \return An integer.
/// \retval >=0 The number of replicas that were affected.
/// \retval  <0 On failure.
///
/// \see #rs_update_replica_access_time
///
/// \since 5.0.0
auto chl_update_replica_access_time(RsComm& _comm, const char* _json_input, char** _output) -> int;

/// \brief High-level wrapper for checking user passwords.
///
/// \param[in] _comm The communication object.
/// \param[in] _json_input \parblock
/// A JSON string of the following form containing information about the user whose password is being checked:
/// \code{.js}
/// {
///     "user_name": "<string>",
///     "zone_name": "<string>",
///     "password": "<string>
/// }
/// \endcode
///
/// \p user_name and \p zone_name are the user name and zone name of the user whose password is being checked.
///
/// \p password is the value of the password to check.
/// \endparblock
/// \param[out] _valid A pointer which will hold a value of 0 if the password check fails, or 1 if it succeeds.
///
/// \retval 0 On success.
/// \retval <0 \parblock
/// If an error occurs while querying for the provided user's password(s) in the catalog. If the search yields no
/// results for the given inputs, this is not an error. In that case, the \p _valid out pointer will hold a value of 0.
/// \endparblock
///
/// \since 5.1.0
auto chl_check_password(RsComm* _comm, const char* _json_input, int* _valid) -> int;

/// \brief High-level wrapper for checking session tokens.
///
/// \param[in] _comm The communication object.
/// \param[in] _json_input \parblock
/// A JSON string of the following form containing information about the session token being checked:
/// \code{.js}
/// {
///     "user_name": "<string>",
///     "zone_name": "<string>",
///     "auth_scheme": "<string>",
///     "session_token": "<string>
/// }
/// \endcode
///
/// \p user_name and \p zone_name are the user name and zone name of the user with whom the session token to check is
/// associated.
///
/// \p auth_scheme indicates the name of the authentication scheme with which this session token is being used.
///
/// \p session_token is the value of the session token to check. This should be a 36-character UUID.
/// \endparblock
/// \param[out] _valid A pointer which will hold a value of 0 if the session token check fails, or 1 if it succeeds.
///
/// \retval 0 On success.
/// \retval <0 \parblock
/// If an error occurs while querying for the provided session token in the catalog. If the search yields no results
/// for the given inputs, this is not an error. In that case, the \p _valid out pointer will hold a value of 0.
/// \endparblock
///
/// \since 5.1.0
auto chl_check_session_token(RsComm* _comm, const char* _json_input, int* _valid) -> int;

/// \brief High-level wrapper for creating session tokens.
///
/// \param[in] _comm The communication object.
/// \param[in] _json_input \parblock
/// A JSON string of the following form containing information about the session token being made:
/// \code{.js}
/// {
///     "user_name": "<string>",
///     "zone_name": "<string>",
///     "auth_scheme": "<string>",
///     "expires": <bool>
/// }
/// \endcode
///
/// \p user_name and \p zone_name are the user name and zone name of the user with whom the session token to be created
/// should be associated.
///
/// \p auth_scheme indicates the name of the authentication scheme with which this session token should be used.
///
/// \p expires is an optional parameter. If specified and set to false, the new session token will never expire. If
/// the option is not specified or set to true, the session token will expire. Note: Only rodsadmins are allowed to
/// request session tokens which do not expire.
/// \endparblock
/// \param[out] _token A pointer which will hold the value of the newly created session token.
///
/// \retval 0 On success.
/// \retval <0 On failure.
///
/// \since 5.1.0
auto chl_make_session_token(RsComm* _comm, const char* _json_input, char** _token) -> int;

/// \brief High-level wrapper for removing session tokens.
///
/// \param[in] _comm The communication object.
/// \param[in] _json_input \parblock
/// A JSON string of the following form containing information about the session tokens to delete:
/// \code{.js}
/// {
///     "user_name": "<string>",
///     "zone_name": "<string>",
///     "expired_only": <bool>
/// }
/// \endcode
///
/// If \p expired_only is true, only expired session tokens associated with the specified user will be deleted. Else,
/// all session tokens associated with the specified user will be deleted.
///
/// \p user_name and \p zone_name are the user name and zone name of the user with whom the session tokens to be deleted
/// are associated. If no \p user_name or \p zone_name are specified, session tokens belonging to any user will be
/// deleted. Not specifying a user should be done deliberately and with caution. If either \p user_name or \p zone_name
/// are empty strings, the parameters are considered specified and will result in no session tokens being deleted
/// because no user ID will be found with which to select session tokens to delete.
/// \endparblock
///
/// \retval 0 On success. Note: Does not necessarily mean that any session tokens were deleted.
/// \retval <0 \parblock
/// If an error occurs. If no session tokens are selected for deletion as a result of the request, an error does not
/// occur. An error only occurs if there are session tokens selected for deletion and the deletion fails for some
/// reason. For example, if the specified user has no session tokens, no session tokens will be selected for deletion.
/// \endparblock
///
/// \since 5.1.0
auto chl_remove_session_tokens(RsComm* _comm, const char* _json_input) -> int;

/// \brief High-level wrapper for removing passwords.
///
/// \param[in] _comm The communication object.
/// \param[in] _json_input \parblock
/// A JSON string of the following form containing information about the user whose password is to be deleted:
/// \code{.js}
/// {
///     "user_name": "<string>",
///     "zone_name": "<string>"
/// }
/// \endcode
/// \endparblock
///
/// \retval 0 On success. Note: Does not necessarily mean that any passwords were deleted.
/// \retval <0 \parblock
/// If an error occurs. If no password is found for the specified user, an error does not occur. An error only occurs if
/// there is a password for the specified user and the deletion fails for some reason.
/// \endparblock
///
/// \since 5.1.0
auto chl_remove_password(RsComm* _comm, const char* _json_input) -> int;

/// \brief Trigger logical quota recalculation based on total object size/count.
///
/// \param[in] _comm The communication object.
/// \param[in] _coll_name A placeholder string parameter. Currently unused.
///
/// \return An integer.
/// \retval  0 On success.
/// \retval <0 On failure.
///
/// \since 5.1.0
auto chl_calc_logical_usage_and_quota(RsComm* _comm, const char* _coll_name) -> int;

/// \brief Update or create a new logical quota.
///
/// \param[in] _comm The communication object.
/// \param[in] _coll_name The collection name to which the logical quota will apply.
/// \param[in] _byte_limit The byte limit for the collection.
/// Negative values will be a no-op for existing quotas and set to 0 (unenforced) for new quotas.
/// \param[in] _object_limit The object limit for the collection.
/// Negative values will be a no-op for existing quotas and set to 0 (unenforced) for new quotas.
///
/// \return An integer.
/// \retval  0 On success.
/// \retval <0 On failure.
///
/// \since 5.1.0
auto chl_set_logical_quota(RsComm* _comm, const char* _coll_name, const char* _byte_limit, const char* _object_limit)
    -> int;

/// \brief Fetch a list of logical quotas.
///
/// \param[in] _comm The communication object.
/// \param[in] _coll_name \parblock
/// The collection name to fetch applicable logical quotas for.
/// This will fetch any quotas applied to a particular collection as well as any quotas set on ancestors.
/// e.g. Passing in /tempZone/home/rods will fetch quotas for
/// /tempZone, /tempZone/home, and /tempZone/home/rods, if any exist.
///
/// Pass in an empty string or nullptr to fetch all quotas.
/// \endparblock
/// \param[out] _quota_values \parblock
/// A vector of tuples, each of which contains the following fields, in order:
/// Collection name, byte limit, object limit, bytes over, objects over.
/// "Bytes over" and "objects over" is the difference between
/// the total bytes/objects stored in the collection and the set byte/object limit.
/// i.e. bytes_over = total_bytes_in_collection - byte_limit
/// \endparblock
///
/// \return An integer.
/// \retval  0 On success.
/// \retval <0 On failure.
///
/// \since 5.1.0
auto chl_check_logical_quota(
    RsComm* _comm,
    const char* _coll_name,
    std::vector<std::tuple<std::string, std::int64_t, std::int64_t, std::int64_t, std::int64_t>>* _quota_values) -> int;

#endif // IRODS_ICAT_HIGHLEVEL_ROUTINES_HPP
