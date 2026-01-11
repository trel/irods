# Install script for directory: /src/irods/server/core

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
    "/src/irods/server/core/include/irods/access_time_queue.hpp"
    "/src/irods/server/core/include/irods/administration_utilities.hpp"
    "/src/irods/server/core/include/irods/atomic_apply_database_operations.hpp"
    "/src/irods/server/core/include/irods/authentication_server_utils.hpp"
    "/src/irods/server/core/include/irods/catalog.hpp"
    "/src/irods/server/core/include/irods/catalog_utilities.hpp"
    "/src/irods/server/core/include/irods/client_api_allowlist.hpp"
    "/src/irods/server/core/include/irods/collection.hpp"
    "/src/irods/server/core/include/irods/dataObjOpr.hpp"
    "/src/irods/server/core/include/irods/replica_access_table.hpp"
    "/src/irods/server/core/include/irods/replica_state_table.hpp"
    "/src/irods/server/core/include/irods/fileOpr.hpp"
    "/src/irods/server/core/include/irods/finalize_utilities.hpp"
    "/src/irods/server/core/include/irods/initServer.hpp"
    "/src/irods/server/core/include/irods/irods_api_calling_functions.hpp"
    "/src/irods/server/core/include/irods/irods_api_number_validator.hpp"
    "/src/irods/server/core/include/irods/irods_collection_object.hpp"
    "/src/irods/server/core/include/irods/irods_data_object.hpp"
    "/src/irods/server/core/include/irods/irods_database_constants.hpp"
    "/src/irods/server/core/include/irods/irods_database_factory.hpp"
    "/src/irods/server/core/include/irods/irods_database_manager.hpp"
    "/src/irods/server/core/include/irods/irods_database_object.hpp"
    "/src/irods/server/core/include/irods/irods_database_plugin.hpp"
    "/src/irods/server/core/include/irods/irods_database_types.hpp"
    "/src/irods/server/core/include/irods/irods_delay_queue.hpp"
    "/src/irods/server/core/include/irods/irods_file_object.hpp"
    "/src/irods/server/core/include/irods/irods_generic_database_object.hpp"
    "/src/irods/server/core/include/irods/irods_get_l1desc.hpp"
    "/src/irods/server/core/include/irods/irods_linked_list_iterator.hpp"
    "/src/irods/server/core/include/irods/irods_mysql_object.hpp"
    "/src/irods/server/core/include/irods/irods_oracle_object.hpp"
    "/src/irods/server/core/include/irods/irods_physical_object.hpp"
    "/src/irods/server/core/include/irods/irods_postgres_object.hpp"
    "/src/irods/server/core/include/irods/irods_report_plugins_in_json.hpp"
    "/src/irods/server/core/include/irods/irods_resource_backport.hpp"
    "/src/irods/server/core/include/irods/irods_resource_constants.hpp"
    "/src/irods/server/core/include/irods/irods_resource_manager.hpp"
    "/src/irods/server/core/include/irods/irods_resource_plugin.hpp"
    "/src/irods/server/core/include/irods/irods_resource_plugin_impostor.hpp"
    "/src/irods/server/core/include/irods/irods_resource_redirect.hpp"
    "/src/irods/server/core/include/irods/irods_resource_types.hpp"
    "/src/irods/server/core/include/irods/irods_rs_comm_query.hpp"
    "/src/irods/server/core/include/irods/irods_server_api_call.hpp"
    "/src/irods/server/core/include/irods/irods_server_api_table.hpp"
    "/src/irods/server/core/include/irods/irods_server_state.hpp"
    "/src/irods/server/core/include/irods/irods_signal.hpp"
    "/src/irods/server/core/include/irods/irods_structured_object.hpp"
    "/src/irods/server/core/include/irods/json_deserialization.hpp"
    "/src/irods/server/core/include/irods/json_serialization.hpp"
    "/src/irods/server/core/include/irods/logical_locking.hpp"
    "/src/irods/server/core/include/irods/miscServerFunct.hpp"
    "/src/irods/server/core/include/irods/notify_service_manager.hpp"
    "/src/irods/server/core/include/irods/objDesc.hpp"
    "/src/irods/server/core/include/irods/objMetaOpr.hpp"
    "/src/irods/server/core/include/irods/password_hash.hpp"
    "/src/irods/server/core/include/irods/physPath.hpp"
    "/src/irods/server/core/include/irods/plugin_lifetime_manager.hpp"
    "/src/irods/server/core/include/irods/process_manager.hpp"
    "/src/irods/server/core/include/irods/replication_utilities.hpp"
    "/src/irods/server/core/include/irods/resource.hpp"
    "/src/irods/server/core/include/irods/rodsConnect.h"
    "/src/irods/server/core/include/irods/rsApiHandler.hpp"
    "/src/irods/server/core/include/irods/rsGlobalExtern.hpp"
    "/src/irods/server/core/include/irods/rsIcatOpr.hpp"
    "/src/irods/server/core/include/irods/scoped_client_identity.hpp"
    "/src/irods/server/core/include/irods/scoped_permission.hpp"
    "/src/irods/server/core/include/irods/scoped_privileged_client.hpp"
    "/src/irods/server/core/include/irods/server_utilities.hpp"
    "/src/irods/server/core/include/irods/specColl.hpp"
    "/src/irods/server/core/include/irods/user_validation_utilities.hpp"
    "/src/irods/server/core/include/irods/voting.hpp"
    )
endif()

