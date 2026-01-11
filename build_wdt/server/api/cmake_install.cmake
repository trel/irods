# Install script for directory: /src/irods/server/api

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "RelWithDebInfo")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/opt/irods-externals/clang16.0.6-0/bin/llvm-objdump")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-dev" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/usr/include/irods" TYPE FILE FILES
    "/src/irods/server/api/include/irods/rs_atomic_apply_acl_operations.hpp"
    "/src/irods/server/api/include/irods/rs_atomic_apply_metadata_operations.hpp"
    "/src/irods/server/api/include/irods/rs_check_auth_credentials.hpp"
    "/src/irods/server/api/include/irods/rs_data_object_finalize.hpp"
    "/src/irods/server/api/include/irods/rs_delay_rule_lock.hpp"
    "/src/irods/server/api/include/irods/rs_delay_rule_unlock.hpp"
    "/src/irods/server/api/include/irods/rs_genquery2.hpp"
    "/src/irods/server/api/include/irods/rs_get_delay_rule_info.hpp"
    "/src/irods/server/api/include/irods/rs_get_file_descriptor_info.hpp"
    "/src/irods/server/api/include/irods/rs_get_grid_configuration_value.hpp"
    "/src/irods/server/api/include/irods/rs_get_library_features.hpp"
    "/src/irods/server/api/include/irods/rs_get_resource_info_for_operation.hpp"
    "/src/irods/server/api/include/irods/rs_register_physical_path.hpp"
    "/src/irods/server/api/include/irods/rs_replica_close.hpp"
    "/src/irods/server/api/include/irods/rs_replica_open.hpp"
    "/src/irods/server/api/include/irods/rs_replica_truncate.hpp"
    "/src/irods/server/api/include/irods/rs_set_delay_server_migration_info.hpp"
    "/src/irods/server/api/include/irods/rs_set_grid_configuration_value.hpp"
    "/src/irods/server/api/include/irods/rs_touch.hpp"
    "/src/irods/server/api/include/irods/rs_update_replica_access_time.hpp"
    "/src/irods/server/api/include/irods/rsAuthCheck.hpp"
    "/src/irods/server/api/include/irods/rsAuthPluginRequest.hpp"
    "/src/irods/server/api/include/irods/rsAuthRequest.hpp"
    "/src/irods/server/api/include/irods/rsAuthResponse.hpp"
    "/src/irods/server/api/include/irods/rsBulkDataObjPut.hpp"
    "/src/irods/server/api/include/irods/rsBulkDataObjReg.hpp"
    "/src/irods/server/api/include/irods/rsChkNVPathPerm.hpp"
    "/src/irods/server/api/include/irods/rsChkObjPermAndStat.hpp"
    "/src/irods/server/api/include/irods/rsClientHints.hpp"
    "/src/irods/server/api/include/irods/rsCloseCollection.hpp"
    "/src/irods/server/api/include/irods/rsCollCreate.hpp"
    "/src/irods/server/api/include/irods/rsCollRepl.hpp"
    "/src/irods/server/api/include/irods/rsDataCopy.hpp"
    "/src/irods/server/api/include/irods/rsDataGet.hpp"
    "/src/irods/server/api/include/irods/rsDataObjChksum.hpp"
    "/src/irods/server/api/include/irods/rsDataObjClose.hpp"
    "/src/irods/server/api/include/irods/rsDataObjCopy.hpp"
    "/src/irods/server/api/include/irods/rsDataObjCreate.hpp"
    "/src/irods/server/api/include/irods/rsDataObjCreateAndStat.hpp"
    "/src/irods/server/api/include/irods/rsDataObjGet.hpp"
    "/src/irods/server/api/include/irods/rsDataObjLseek.hpp"
    "/src/irods/server/api/include/irods/rsDataObjOpen.hpp"
    "/src/irods/server/api/include/irods/rsDataObjOpenAndStat.hpp"
    "/src/irods/server/api/include/irods/rsDataObjPhymv.hpp"
    "/src/irods/server/api/include/irods/rsDataObjPut.hpp"
    "/src/irods/server/api/include/irods/rsDataObjRead.hpp"
    "/src/irods/server/api/include/irods/rsDataObjRename.hpp"
    "/src/irods/server/api/include/irods/rsDataObjRepl.hpp"
    "/src/irods/server/api/include/irods/rsDataObjRsync.hpp"
    "/src/irods/server/api/include/irods/rsDataObjTrim.hpp"
    "/src/irods/server/api/include/irods/rsDataObjTruncate.hpp"
    "/src/irods/server/api/include/irods/rsDataObjUnlink.hpp"
    "/src/irods/server/api/include/irods/rsDataObjWrite.hpp"
    "/src/irods/server/api/include/irods/rsDataPut.hpp"
    "/src/irods/server/api/include/irods/rsEndTransaction.hpp"
    "/src/irods/server/api/include/irods/rsExecCmd.hpp"
    "/src/irods/server/api/include/irods/rsExecMyRule.hpp"
    "/src/irods/server/api/include/irods/rsExecRuleExpression.hpp"
    "/src/irods/server/api/include/irods/rsFileChksum.hpp"
    "/src/irods/server/api/include/irods/rsFileChmod.hpp"
    "/src/irods/server/api/include/irods/rsFileClose.hpp"
    "/src/irods/server/api/include/irods/rsFileClosedir.hpp"
    "/src/irods/server/api/include/irods/rsFileCreate.hpp"
    "/src/irods/server/api/include/irods/rsFileGet.hpp"
    "/src/irods/server/api/include/irods/rsFileGetFsFreeSpace.hpp"
    "/src/irods/server/api/include/irods/rsFileLseek.hpp"
    "/src/irods/server/api/include/irods/rsFileMkdir.hpp"
    "/src/irods/server/api/include/irods/rsFileOpen.hpp"
    "/src/irods/server/api/include/irods/rsFileOpendir.hpp"
    "/src/irods/server/api/include/irods/rsFilePut.hpp"
    "/src/irods/server/api/include/irods/rsFileRead.hpp"
    "/src/irods/server/api/include/irods/rsFileReaddir.hpp"
    "/src/irods/server/api/include/irods/rsFileRename.hpp"
    "/src/irods/server/api/include/irods/rsFileRmdir.hpp"
    "/src/irods/server/api/include/irods/rsFileStageToCache.hpp"
    "/src/irods/server/api/include/irods/rsFileStat.hpp"
    "/src/irods/server/api/include/irods/rsFileSyncToArch.hpp"
    "/src/irods/server/api/include/irods/rsFileTruncate.hpp"
    "/src/irods/server/api/include/irods/rsFileUnlink.hpp"
    "/src/irods/server/api/include/irods/rsFileWrite.hpp"
    "/src/irods/server/api/include/irods/rsGenQuery.hpp"
    "/src/irods/server/api/include/irods/rsGeneralAdmin.hpp"
    "/src/irods/server/api/include/irods/rsGetHierFromLeafId.hpp"
    "/src/irods/server/api/include/irods/rsGetHierarchyForResc.hpp"
    "/src/irods/server/api/include/irods/rsGetHostForGet.hpp"
    "/src/irods/server/api/include/irods/rsGetHostForPut.hpp"
    "/src/irods/server/api/include/irods/rsGetLimitedPassword.hpp"
    "/src/irods/server/api/include/irods/rsGetMiscSvrInfo.hpp"
    "/src/irods/server/api/include/irods/rsGetRemoteZoneResc.hpp"
    "/src/irods/server/api/include/irods/rsGetRescQuota.hpp"
    "/src/irods/server/api/include/irods/rsGetTempPassword.hpp"
    "/src/irods/server/api/include/irods/rsGetTempPasswordForOther.hpp"
    "/src/irods/server/api/include/irods/rsIESClientHints.hpp"
    "/src/irods/server/api/include/irods/rsL3FileGetSingleBuf.hpp"
    "/src/irods/server/api/include/irods/rsL3FilePutSingleBuf.hpp"
    "/src/irods/server/api/include/irods/rsModAVUMetadata.hpp"
    "/src/irods/server/api/include/irods/rsModAccessControl.hpp"
    "/src/irods/server/api/include/irods/rsModColl.hpp"
    "/src/irods/server/api/include/irods/rsModDataObjMeta.hpp"
    "/src/irods/server/api/include/irods/rsObjStat.hpp"
    "/src/irods/server/api/include/irods/rsOpenCollection.hpp"
    "/src/irods/server/api/include/irods/rsOprComplete.hpp"
    "/src/irods/server/api/include/irods/rsPamAuthRequest.hpp"
    "/src/irods/server/api/include/irods/rsPhyPathReg.hpp"
    "/src/irods/server/api/include/irods/rsProcStat.hpp"
    "/src/irods/server/api/include/irods/rsQuerySpecColl.hpp"
    "/src/irods/server/api/include/irods/rsReadCollection.hpp"
    "/src/irods/server/api/include/irods/rsRegColl.hpp"
    "/src/irods/server/api/include/irods/rsRegDataObj.hpp"
    "/src/irods/server/api/include/irods/rsRegReplica.hpp"
    "/src/irods/server/api/include/irods/rsRmColl.hpp"
    "/src/irods/server/api/include/irods/rsRuleExecDel.hpp"
    "/src/irods/server/api/include/irods/rsRuleExecMod.hpp"
    "/src/irods/server/api/include/irods/rsRuleExecSubmit.hpp"
    "/src/irods/server/api/include/irods/rsServerReport.hpp"
    "/src/irods/server/api/include/irods/rsSpecificQuery.hpp"
    "/src/irods/server/api/include/irods/rsSslEnd.hpp"
    "/src/irods/server/api/include/irods/rsSslStart.hpp"
    "/src/irods/server/api/include/irods/rsStreamClose.hpp"
    "/src/irods/server/api/include/irods/rsStreamRead.hpp"
    "/src/irods/server/api/include/irods/rsStructFileBundle.hpp"
    "/src/irods/server/api/include/irods/rsStructFileExtAndReg.hpp"
    "/src/irods/server/api/include/irods/rsStructFileExtract.hpp"
    "/src/irods/server/api/include/irods/rsStructFileSync.hpp"
    "/src/irods/server/api/include/irods/rsSubStructFileClose.hpp"
    "/src/irods/server/api/include/irods/rsSubStructFileClosedir.hpp"
    "/src/irods/server/api/include/irods/rsSubStructFileCreate.hpp"
    "/src/irods/server/api/include/irods/rsSubStructFileGet.hpp"
    "/src/irods/server/api/include/irods/rsSubStructFileLseek.hpp"
    "/src/irods/server/api/include/irods/rsSubStructFileMkdir.hpp"
    "/src/irods/server/api/include/irods/rsSubStructFileOpen.hpp"
    "/src/irods/server/api/include/irods/rsSubStructFileOpendir.hpp"
    "/src/irods/server/api/include/irods/rsSubStructFilePut.hpp"
    "/src/irods/server/api/include/irods/rsSubStructFileRead.hpp"
    "/src/irods/server/api/include/irods/rsSubStructFileReaddir.hpp"
    "/src/irods/server/api/include/irods/rsSubStructFileRename.hpp"
    "/src/irods/server/api/include/irods/rsSubStructFileRmdir.hpp"
    "/src/irods/server/api/include/irods/rsSubStructFileStat.hpp"
    "/src/irods/server/api/include/irods/rsSubStructFileTruncate.hpp"
    "/src/irods/server/api/include/irods/rsSubStructFileUnlink.hpp"
    "/src/irods/server/api/include/irods/rsSubStructFileWrite.hpp"
    "/src/irods/server/api/include/irods/rsSyncMountedColl.hpp"
    "/src/irods/server/api/include/irods/rsTicketAdmin.hpp"
    "/src/irods/server/api/include/irods/rsUnbunAndRegPhyBunfile.hpp"
    "/src/irods/server/api/include/irods/rsUnregDataObj.hpp"
    "/src/irods/server/api/include/irods/rsUserAdmin.hpp"
    "/src/irods/server/api/include/irods/rsZoneReport.hpp"
    )
endif()

