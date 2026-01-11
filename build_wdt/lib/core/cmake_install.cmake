# Install script for directory: /src/irods/lib/core

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

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-runtime" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/libirods_plugin_dependencies.so.5.0.90" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/libirods_plugin_dependencies.so.5.0.90")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/libirods_plugin_dependencies.so.5.0.90"
         RPATH "/opt/irods-externals/boost1.81.0-2/lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/usr/lib" TYPE SHARED_LIBRARY FILES "/src/irods/build_wdt/lib/core/libirods_plugin_dependencies.so.5.0.90")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/libirods_plugin_dependencies.so.5.0.90" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/libirods_plugin_dependencies.so.5.0.90")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/libirods_plugin_dependencies.so.5.0.90"
         OLD_RPATH "\$ORIGIN/../:/opt/irods-externals/boost1.81.0-2/lib:"
         NEW_RPATH "/opt/irods-externals/boost1.81.0-2/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/libirods_plugin_dependencies.so.5.0.90")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-dev" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/usr/lib" TYPE SHARED_LIBRARY FILES "/src/irods/build_wdt/lib/core/libirods_plugin_dependencies.so")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-dev" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/usr/include/irods" TYPE FILE FILES
    "/src/irods/lib/core/include/irods/alignPointer.hpp"
    "/src/irods/lib/core/include/irods/apiHandler.hpp"
    "/src/irods/lib/core/include/irods/authenticate.h"
    "/src/irods/lib/core/include/irods/authentication_client_utils.hpp"
    "/src/irods/lib/core/include/irods/authentication_plugin_framework.hpp"
    "/src/irods/lib/core/include/irods/base64.h"
    "/src/irods/lib/core/include/irods/base64.hpp"
    "/src/irods/lib/core/include/irods/bunUtil.h"
    "/src/irods/lib/core/include/irods/capped_memory_resource.hpp"
    "/src/irods/lib/core/include/irods/chksumUtil.h"
    "/src/irods/lib/core/include/irods/chrono.hpp"
    "/src/irods/lib/core/include/irods/client_connection.hpp"
    "/src/irods/lib/core/include/irods/connection_pool.hpp"
    "/src/irods/lib/core/include/irods/cpUtil.h"
    "/src/irods/lib/core/include/irods/data_object_proxy.hpp"
    "/src/irods/lib/core/include/irods/dispatch_processor.hpp"
    "/src/irods/lib/core/include/irods/dns_cache.hpp"
    "/src/irods/lib/core/include/irods/dstream.hpp"
    "/src/irods/lib/core/include/irods/escape_utilities.hpp"
    "/src/irods/lib/core/include/irods/experimental_plugin_framework.hpp"
    "/src/irods/lib/core/include/irods/fixed_buffer_resource.hpp"
    "/src/irods/lib/core/include/irods/fsckUtil.h"
    "/src/irods/lib/core/include/irods/fully_qualified_username.hpp"
    "/src/irods/lib/core/include/irods/future.hpp"
    "/src/irods/lib/core/include/irods/getRodsEnv.h"
    "/src/irods/lib/core/include/irods/getUtil.h"
    "/src/irods/lib/core/include/irods/guiProgressCallback.h"
    "/src/irods/lib/core/include/irods/hostname_cache.hpp"
    "/src/irods/lib/core/include/irods/json_events.hpp"
    "/src/irods/lib/core/include/irods/irods_at_scope_exit.hpp"
    "/src/irods/lib/core/include/irods/irods_auth_constants.hpp"
    "/src/irods/lib/core/include/irods/irods_auth_factory.hpp"
    "/src/irods/lib/core/include/irods/irods_auth_manager.hpp"
    "/src/irods/lib/core/include/irods/irods_auth_object.hpp"
    "/src/irods/lib/core/include/irods/irods_generic_auth_object.hpp"
    "/src/irods/lib/core/include/irods/irods_auth_plugin.hpp"
    "/src/irods/lib/core/include/irods/irods_auth_types.hpp"
    "/src/irods/lib/core/include/irods/irods_buffer_encryption.hpp"
    "/src/irods/lib/core/include/irods/irods_children_parser.hpp"
    "/src/irods/lib/core/include/irods/irods_client_api_table.hpp"
    "/src/irods/lib/core/include/irods/irods_client_server_negotiation.hpp"
    "/src/irods/lib/core/include/irods/irods_configuration_keywords.hpp"
    "/src/irods/lib/core/include/irods/irods_configuration_parser.hpp"
    "/src/irods/lib/core/include/irods/irods_environment_properties.hpp"
    "/src/irods/lib/core/include/irods/irods_error.hpp"
    "/src/irods/lib/core/include/irods/irods_exception.hpp"
    "/src/irods/lib/core/include/irods/irods_first_class_object.hpp"
    "/src/irods/lib/core/include/irods/irods_get_full_path_for_config_file.hpp"
    "/src/irods/lib/core/include/irods/irods_hash.hpp"
    "/src/irods/lib/core/include/irods/irods_hashtable.h"
    "/src/irods/lib/core/include/irods/irods_hierarchy_parser.hpp"
    "/src/irods/lib/core/include/irods/irods_hostname.hpp"
    "/src/irods/lib/core/include/irods/irods_kvp_string_parser.hpp"
    "/src/irods/lib/core/include/irods/irods_lexical_cast.hpp"
    "/src/irods/lib/core/include/irods/irods_list.h"
    "/src/irods/lib/core/include/irods/irods_load_plugin.hpp"
    "/src/irods/lib/core/include/irods/irods_log.hpp"
    "/src/irods/lib/core/include/irods/irods_logger.hpp"
    "/src/irods/lib/core/include/irods/irods_lookup_table.hpp"
    "/src/irods/lib/core/include/irods/irods_native_auth_object.hpp"
    "/src/irods/lib/core/include/irods/irods_network_constants.hpp"
    "/src/irods/lib/core/include/irods/irods_network_factory.hpp"
    "/src/irods/lib/core/include/irods/irods_network_manager.hpp"
    "/src/irods/lib/core/include/irods/irods_network_object.hpp"
    "/src/irods/lib/core/include/irods/irods_network_plugin.hpp"
    "/src/irods/lib/core/include/irods/irods_network_types.hpp"
    "/src/irods/lib/core/include/irods/irods_pack_table.hpp"
    "/src/irods/lib/core/include/irods/irods_pam_auth_object.hpp"
    "/src/irods/lib/core/include/irods/irods_parse_command_line_options.hpp"
    "/src/irods/lib/core/include/irods/irods_path_recursion.hpp"
    "/src/irods/lib/core/include/irods/irods_pluggable_auth_scheme.hpp"
    "/src/irods/lib/core/include/irods/irods_plugin_base.hpp"
    "/src/irods/lib/core/include/irods/irods_plugin_context.hpp"
    "/src/irods/lib/core/include/irods/irods_plugin_name_generator.hpp"
    "/src/irods/lib/core/include/irods/irods_query.hpp"
    "/src/irods/lib/core/include/irods/irods_random.hpp"
    "/src/irods/lib/core/include/irods/irods_serialization.hpp"
    "/src/irods/lib/core/include/irods/irods_server_properties.hpp"
    "/src/irods/lib/core/include/irods/irods_service_account.hpp"
    "/src/irods/lib/core/include/irods/irods_socket_information.hpp"
    "/src/irods/lib/core/include/irods/irods_ssl_object.hpp"
    "/src/irods/lib/core/include/irods/irods_stacktrace.hpp"
    "/src/irods/lib/core/include/irods/irods_state_table.h"
    "/src/irods/lib/core/include/irods/irods_string_tokenize.hpp"
    "/src/irods/lib/core/include/irods/irods_tcp_object.hpp"
    "/src/irods/lib/core/include/irods/irods_threads.hpp"
    "/src/irods/lib/core/include/irods/irods_virtual_path.hpp"
    "/src/irods/lib/core/include/irods/key_value_proxy.hpp"
    "/src/irods/lib/core/include/irods/library_features.h"
    "/src/irods/lib/core/include/irods/lifetime_manager.hpp"
    "/src/irods/lib/core/include/irods/lsUtil.h"
    "/src/irods/lib/core/include/irods/mcollUtil.h"
    "/src/irods/lib/core/include/irods/microservice.hpp"
    "/src/irods/lib/core/include/irods/miscUtil.h"
    "/src/irods/lib/core/include/irods/mkdirUtil.h"
    "/src/irods/lib/core/include/irods/msParam.h"
    "/src/irods/lib/core/include/irods/mvUtil.h"
    "/src/irods/lib/core/include/irods/obf.h"
    "/src/irods/lib/core/include/irods/objInfo.h"
    "/src/irods/lib/core/include/irods/packStruct.h"
    "/src/irods/lib/core/include/irods/parallel_transfer_engine.hpp"
    "/src/irods/lib/core/include/irods/parseCommandLine.h"
    "/src/irods/lib/core/include/irods/phymvUtil.h"
    "/src/irods/lib/core/include/irods/procApiRequest.h"
    "/src/irods/lib/core/include/irods/process_stash.hpp"
    "/src/irods/lib/core/include/irods/putUtil.h"
    "/src/irods/lib/core/include/irods/query_builder.hpp"
    "/src/irods/lib/core/include/irods/query_processor.hpp"
    "/src/irods/lib/core/include/irods/rcConnect.h"
    "/src/irods/lib/core/include/irods/rcGlobalExtern.h"
    "/src/irods/lib/core/include/irods/rcMisc.h"
    "/src/irods/lib/core/include/irods/rcPortalOpr.h"
    "/src/irods/lib/core/include/irods/regUtil.h"
    "/src/irods/lib/core/include/irods/region.h"
    "/src/irods/lib/core/include/irods/replUtil.h"
    "/src/irods/lib/core/include/irods/replica.hpp"
    "/src/irods/lib/core/include/irods/replica_proxy.hpp"
    "/src/irods/lib/core/include/irods/rmdirUtil.h"
    "/src/irods/lib/core/include/irods/rmUtil.h"
    "/src/irods/lib/core/include/irods/rmtrashUtil.h"
    "/src/irods/lib/core/include/irods/rods.h"
    "/src/irods/lib/core/include/irods/rodsClient.h"
    "/src/irods/lib/core/include/irods/rodsDef.h"
    "/src/irods/lib/core/include/irods/rodsError.h"
    "/src/irods/lib/core/include/irods/rodsErrorTable.h"
    "/src/irods/lib/core/include/irods/rodsGenQuery.h"
    "/src/irods/lib/core/include/irods/rodsGenQueryNames.h"
    "/src/irods/lib/core/include/irods/rodsKeyWdDef.h"
    "/src/irods/lib/core/include/irods/rodsLog.h"
    "/src/irods/lib/core/include/irods/rodsPackInstruct.h"
    "/src/irods/lib/core/include/irods/rodsPackTable.h"
    "/src/irods/lib/core/include/irods/rodsPath.h"
    "/src/irods/lib/core/include/irods/rodsQuota.h"
    "/src/irods/lib/core/include/irods/rodsType.h"
    "/src/irods/lib/core/include/irods/rodsUser.h"
    "/src/irods/lib/core/include/irods/rsyncUtil.h"
    "/src/irods/lib/core/include/irods/scanUtil.h"
    "/src/irods/lib/core/include/irods/shared_memory_object.hpp"
    "/src/irods/lib/core/include/irods/sockComm.h"
    "/src/irods/lib/core/include/irods/sockCommNetworkInterface.hpp"
    "/src/irods/lib/core/include/irods/socket_wrapper.hpp"
    "/src/irods/lib/core/include/irods/sslSockComm.h"
    "/src/irods/lib/core/include/irods/stream_factory_utility.hpp"
    "/src/irods/lib/core/include/irods/stringOpr.h"
    "/src/irods/lib/core/include/irods/system_error.hpp"
    "/src/irods/lib/core/include/irods/termiosUtil.hpp"
    "/src/irods/lib/core/include/irods/thread_pool.hpp"
    "/src/irods/lib/core/include/irods/trimUtil.h"
    "/src/irods/lib/core/include/irods/version.hpp"
    "/src/irods/lib/core/include/irods/with_durability.hpp"
    "/src/irods/build_wdt/lib/core/include/irods/irods_default_paths.hpp"
    "/src/irods/build_wdt/lib/core/include/irods/rodsVersion.h"
    "/src/irods/build_wdt/lib/core/include/irods/irods_version.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-dev" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/usr/include/irods/transport" TYPE FILE FILES
    "/src/irods/lib/core/include/irods/transport/transport.hpp"
    "/src/irods/lib/core/include/irods/transport/default_transport.hpp"
    )
endif()

