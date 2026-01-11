# Install script for directory: /src/irods/lib/api

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
    "/src/irods/lib/api/include/irods/apiHeaderAll.h"
    "/src/irods/lib/api/include/irods/apiNumber.h"
    "/src/irods/lib/api/include/irods/apiNumberData.h"
    "/src/irods/lib/api/include/irods/apiNumberMap.h"
    "/src/irods/lib/api/include/irods/apiPackTable.h"
    "/src/irods/lib/api/include/irods/apiTable.hpp"
    "/src/irods/lib/api/include/irods/api_pack_table.hpp"
    "/src/irods/lib/api/include/irods/atomic_apply_acl_operations.h"
    "/src/irods/lib/api/include/irods/atomic_apply_metadata_operations.h"
    "/src/irods/lib/api/include/irods/authCheck.h"
    "/src/irods/lib/api/include/irods/authPluginRequest.h"
    "/src/irods/lib/api/include/irods/authRequest.h"
    "/src/irods/lib/api/include/irods/authResponse.h"
    "/src/irods/lib/api/include/irods/bulkDataObjPut.h"
    "/src/irods/lib/api/include/irods/bulkDataObjReg.h"
    "/src/irods/lib/api/include/irods/check_auth_credentials.h"
    "/src/irods/lib/api/include/irods/chkNVPathPerm.h"
    "/src/irods/lib/api/include/irods/chkObjPermAndStat.h"
    "/src/irods/lib/api/include/irods/client_hints.h"
    "/src/irods/lib/api/include/irods/closeCollection.h"
    "/src/irods/lib/api/include/irods/collCreate.h"
    "/src/irods/lib/api/include/irods/collRepl.h"
    "/src/irods/lib/api/include/irods/dataCopy.h"
    "/src/irods/lib/api/include/irods/dataGet.h"
    "/src/irods/lib/api/include/irods/dataObjChksum.h"
    "/src/irods/lib/api/include/irods/dataObjClose.h"
    "/src/irods/lib/api/include/irods/dataObjCopy.h"
    "/src/irods/lib/api/include/irods/dataObjCreate.h"
    "/src/irods/lib/api/include/irods/dataObjCreateAndStat.h"
    "/src/irods/lib/api/include/irods/dataObjGet.h"
    "/src/irods/lib/api/include/irods/dataObjInpOut.h"
    "/src/irods/lib/api/include/irods/dataObjLseek.h"
    "/src/irods/lib/api/include/irods/dataObjOpen.h"
    "/src/irods/lib/api/include/irods/dataObjOpenAndStat.h"
    "/src/irods/lib/api/include/irods/dataObjPhymv.h"
    "/src/irods/lib/api/include/irods/dataObjPut.h"
    "/src/irods/lib/api/include/irods/dataObjRead.h"
    "/src/irods/lib/api/include/irods/dataObjRename.h"
    "/src/irods/lib/api/include/irods/dataObjRepl.h"
    "/src/irods/lib/api/include/irods/dataObjRsync.h"
    "/src/irods/lib/api/include/irods/dataObjTrim.h"
    "/src/irods/lib/api/include/irods/dataObjTruncate.h"
    "/src/irods/lib/api/include/irods/dataObjUnlink.h"
    "/src/irods/lib/api/include/irods/dataObjWrite.h"
    "/src/irods/lib/api/include/irods/dataPut.h"
    "/src/irods/lib/api/include/irods/data_object_finalize.h"
    "/src/irods/lib/api/include/irods/data_object_modify_info.h"
    "/src/irods/lib/api/include/irods/delay_rule_lock.h"
    "/src/irods/lib/api/include/irods/delay_rule_unlock.h"
    "/src/irods/lib/api/include/irods/endTransaction.h"
    "/src/irods/lib/api/include/irods/execCmd.h"
    "/src/irods/lib/api/include/irods/execMyRule.h"
    "/src/irods/lib/api/include/irods/exec_rule_expression.h"
    "/src/irods/lib/api/include/irods/fileChksum.h"
    "/src/irods/lib/api/include/irods/fileChmod.h"
    "/src/irods/lib/api/include/irods/fileClose.h"
    "/src/irods/lib/api/include/irods/fileClosedir.h"
    "/src/irods/lib/api/include/irods/fileCreate.h"
    "/src/irods/lib/api/include/irods/fileGet.h"
    "/src/irods/lib/api/include/irods/fileGetFsFreeSpace.h"
    "/src/irods/lib/api/include/irods/fileLseek.h"
    "/src/irods/lib/api/include/irods/fileMkdir.h"
    "/src/irods/lib/api/include/irods/fileOpen.h"
    "/src/irods/lib/api/include/irods/fileOpendir.h"
    "/src/irods/lib/api/include/irods/filePut.h"
    "/src/irods/lib/api/include/irods/fileRead.h"
    "/src/irods/lib/api/include/irods/fileReaddir.h"
    "/src/irods/lib/api/include/irods/fileRename.h"
    "/src/irods/lib/api/include/irods/fileRmdir.h"
    "/src/irods/lib/api/include/irods/fileStageToCache.h"
    "/src/irods/lib/api/include/irods/fileStat.h"
    "/src/irods/lib/api/include/irods/fileSyncToArch.h"
    "/src/irods/lib/api/include/irods/fileTruncate.h"
    "/src/irods/lib/api/include/irods/fileUnlink.h"
    "/src/irods/lib/api/include/irods/fileWrite.h"
    "/src/irods/lib/api/include/irods/genQuery.h"
    "/src/irods/lib/api/include/irods/genquery2.h"
    "/src/irods/lib/api/include/irods/generalAdmin.h"
    "/src/irods/lib/api/include/irods/getHierarchyForResc.h"
    "/src/irods/lib/api/include/irods/getHostForGet.h"
    "/src/irods/lib/api/include/irods/getHostForPut.h"
    "/src/irods/lib/api/include/irods/getLimitedPassword.h"
    "/src/irods/lib/api/include/irods/getMiscSvrInfo.h"
    "/src/irods/lib/api/include/irods/getRemoteZoneResc.h"
    "/src/irods/lib/api/include/irods/getRescQuota.h"
    "/src/irods/lib/api/include/irods/getTempPassword.h"
    "/src/irods/lib/api/include/irods/getTempPasswordForOther.h"
    "/src/irods/lib/api/include/irods/get_delay_rule_info.h"
    "/src/irods/lib/api/include/irods/get_file_descriptor_info.h"
    "/src/irods/lib/api/include/irods/get_grid_configuration_value.h"
    "/src/irods/lib/api/include/irods/get_hier_from_leaf_id.h"
    "/src/irods/lib/api/include/irods/get_library_features.h"
    "/src/irods/lib/api/include/irods/get_resource_info_for_operation.h"
    "/src/irods/lib/api/include/irods/ies_client_hints.h"
    "/src/irods/lib/api/include/irods/l3FileGetSingleBuf.h"
    "/src/irods/lib/api/include/irods/l3FilePutSingleBuf.h"
    "/src/irods/lib/api/include/irods/modAVUMetadata.h"
    "/src/irods/lib/api/include/irods/modAccessControl.h"
    "/src/irods/lib/api/include/irods/modColl.h"
    "/src/irods/lib/api/include/irods/modDataObjMeta.h"
    "/src/irods/lib/api/include/irods/objStat.h"
    "/src/irods/lib/api/include/irods/openCollection.h"
    "/src/irods/lib/api/include/irods/oprComplete.h"
    "/src/irods/lib/api/include/irods/pamAuthRequest.h"
    "/src/irods/lib/api/include/irods/phyPathReg.h"
    "/src/irods/lib/api/include/irods/procStat.h"
    "/src/irods/lib/api/include/irods/querySpecColl.h"
    "/src/irods/lib/api/include/irods/readCollection.h"
    "/src/irods/lib/api/include/irods/regColl.h"
    "/src/irods/lib/api/include/irods/regDataObj.h"
    "/src/irods/lib/api/include/irods/regReplica.h"
    "/src/irods/lib/api/include/irods/register_physical_path.h"
    "/src/irods/lib/api/include/irods/replica_close.h"
    "/src/irods/lib/api/include/irods/replica_open.h"
    "/src/irods/lib/api/include/irods/replica_truncate.h"
    "/src/irods/lib/api/include/irods/rmColl.h"
    "/src/irods/lib/api/include/irods/ruleExecDel.h"
    "/src/irods/lib/api/include/irods/ruleExecMod.h"
    "/src/irods/lib/api/include/irods/ruleExecSubmit.h"
    "/src/irods/lib/api/include/irods/server_report.h"
    "/src/irods/lib/api/include/irods/set_delay_server_migration_info.h"
    "/src/irods/lib/api/include/irods/set_grid_configuration_value.h"
    "/src/irods/lib/api/include/irods/specificQuery.h"
    "/src/irods/lib/api/include/irods/sslEnd.h"
    "/src/irods/lib/api/include/irods/sslStart.h"
    "/src/irods/lib/api/include/irods/streamClose.h"
    "/src/irods/lib/api/include/irods/streamRead.h"
    "/src/irods/lib/api/include/irods/structFileBundle.h"
    "/src/irods/lib/api/include/irods/structFileExtAndReg.h"
    "/src/irods/lib/api/include/irods/structFileExtract.h"
    "/src/irods/lib/api/include/irods/structFileSync.h"
    "/src/irods/lib/api/include/irods/subStructFileClose.h"
    "/src/irods/lib/api/include/irods/subStructFileClosedir.h"
    "/src/irods/lib/api/include/irods/subStructFileCreate.h"
    "/src/irods/lib/api/include/irods/subStructFileGet.h"
    "/src/irods/lib/api/include/irods/subStructFileLseek.h"
    "/src/irods/lib/api/include/irods/subStructFileMkdir.h"
    "/src/irods/lib/api/include/irods/subStructFileOpen.h"
    "/src/irods/lib/api/include/irods/subStructFileOpendir.h"
    "/src/irods/lib/api/include/irods/subStructFilePut.h"
    "/src/irods/lib/api/include/irods/subStructFileRead.h"
    "/src/irods/lib/api/include/irods/subStructFileReaddir.h"
    "/src/irods/lib/api/include/irods/subStructFileRename.h"
    "/src/irods/lib/api/include/irods/subStructFileRmdir.h"
    "/src/irods/lib/api/include/irods/subStructFileStat.h"
    "/src/irods/lib/api/include/irods/subStructFileTruncate.h"
    "/src/irods/lib/api/include/irods/subStructFileUnlink.h"
    "/src/irods/lib/api/include/irods/subStructFileWrite.h"
    "/src/irods/lib/api/include/irods/switch_user.h"
    "/src/irods/lib/api/include/irods/syncMountedColl.h"
    "/src/irods/lib/api/include/irods/ticketAdmin.h"
    "/src/irods/lib/api/include/irods/touch.h"
    "/src/irods/lib/api/include/irods/unbunAndRegPhyBunfile.h"
    "/src/irods/lib/api/include/irods/unregDataObj.h"
    "/src/irods/lib/api/include/irods/update_replica_access_time.h"
    "/src/irods/lib/api/include/irods/userAdmin.h"
    "/src/irods/lib/api/include/irods/zone_report.h"
    )
endif()

