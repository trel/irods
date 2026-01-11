# Install script for directory: /src/irods/plugins/api

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
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libexperimental_api_plugin_adaptor_client.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libexperimental_api_plugin_adaptor_client.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libexperimental_api_plugin_adaptor_client.so"
         RPATH "/opt/irods-externals/boost1.81.0-2/lib:/opt/irods-externals/nanodbc2.13.0-3/lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api" TYPE MODULE FILES "/src/irods/build_wdt/plugins/api/libexperimental_api_plugin_adaptor_client.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libexperimental_api_plugin_adaptor_client.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libexperimental_api_plugin_adaptor_client.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libexperimental_api_plugin_adaptor_client.so"
         OLD_RPATH "\$ORIGIN/../../lib:/opt/irods-externals/boost1.81.0-2/lib:/opt/irods-externals/nanodbc2.13.0-3/lib:\$ORIGIN/../../lib/core:"
         NEW_RPATH "/opt/irods-externals/boost1.81.0-2/lib:/opt/irods-externals/nanodbc2.13.0-3/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libexperimental_api_plugin_adaptor_client.so")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-runtime" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libexperimental_api_plugin_adaptor_server.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libexperimental_api_plugin_adaptor_server.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libexperimental_api_plugin_adaptor_server.so"
         RPATH "/opt/irods-externals/boost1.81.0-2/lib:/opt/irods-externals/nanodbc2.13.0-3/lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api" TYPE MODULE FILES "/src/irods/build_wdt/plugins/api/libexperimental_api_plugin_adaptor_server.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libexperimental_api_plugin_adaptor_server.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libexperimental_api_plugin_adaptor_server.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libexperimental_api_plugin_adaptor_server.so"
         OLD_RPATH "\$ORIGIN/../../server:/opt/irods-externals/boost1.81.0-2/lib:/opt/irods-externals/nanodbc2.13.0-3/lib:\$ORIGIN/../../lib/core:\$ORIGIN/../../lib:"
         NEW_RPATH "/opt/irods-externals/boost1.81.0-2/lib:/opt/irods-externals/nanodbc2.13.0-3/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libexperimental_api_plugin_adaptor_server.so")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-runtime" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libhelloworld_client.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libhelloworld_client.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libhelloworld_client.so"
         RPATH "/opt/irods-externals/boost1.81.0-2/lib:/opt/irods-externals/nanodbc2.13.0-3/lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api" TYPE MODULE FILES "/src/irods/build_wdt/plugins/api/libhelloworld_client.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libhelloworld_client.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libhelloworld_client.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libhelloworld_client.so"
         OLD_RPATH "\$ORIGIN/../../lib:/opt/irods-externals/boost1.81.0-2/lib:/opt/irods-externals/nanodbc2.13.0-3/lib:\$ORIGIN/../../lib/core:"
         NEW_RPATH "/opt/irods-externals/boost1.81.0-2/lib:/opt/irods-externals/nanodbc2.13.0-3/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libhelloworld_client.so")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-runtime" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libhelloworld_server.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libhelloworld_server.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libhelloworld_server.so"
         RPATH "/opt/irods-externals/boost1.81.0-2/lib:/opt/irods-externals/nanodbc2.13.0-3/lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api" TYPE MODULE FILES "/src/irods/build_wdt/plugins/api/libhelloworld_server.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libhelloworld_server.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libhelloworld_server.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libhelloworld_server.so"
         OLD_RPATH "\$ORIGIN/../../server:/opt/irods-externals/boost1.81.0-2/lib:/opt/irods-externals/nanodbc2.13.0-3/lib:\$ORIGIN/../../lib/core:\$ORIGIN/../../lib:"
         NEW_RPATH "/opt/irods-externals/boost1.81.0-2/lib:/opt/irods-externals/nanodbc2.13.0-3/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libhelloworld_server.so")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-runtime" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_atomic_apply_acl_operations_client.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_atomic_apply_acl_operations_client.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_atomic_apply_acl_operations_client.so"
         RPATH "/opt/irods-externals/boost1.81.0-2/lib:/opt/irods-externals/nanodbc2.13.0-3/lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api" TYPE MODULE FILES "/src/irods/build_wdt/plugins/api/libirods_atomic_apply_acl_operations_client.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_atomic_apply_acl_operations_client.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_atomic_apply_acl_operations_client.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_atomic_apply_acl_operations_client.so"
         OLD_RPATH "\$ORIGIN/../../lib:/opt/irods-externals/boost1.81.0-2/lib:/opt/irods-externals/nanodbc2.13.0-3/lib:\$ORIGIN/../../lib/core:"
         NEW_RPATH "/opt/irods-externals/boost1.81.0-2/lib:/opt/irods-externals/nanodbc2.13.0-3/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_atomic_apply_acl_operations_client.so")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-runtime" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_atomic_apply_acl_operations_server.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_atomic_apply_acl_operations_server.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_atomic_apply_acl_operations_server.so"
         RPATH "/opt/irods-externals/boost1.81.0-2/lib:/opt/irods-externals/nanodbc2.13.0-3/lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api" TYPE MODULE FILES "/src/irods/build_wdt/plugins/api/libirods_atomic_apply_acl_operations_server.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_atomic_apply_acl_operations_server.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_atomic_apply_acl_operations_server.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_atomic_apply_acl_operations_server.so"
         OLD_RPATH "\$ORIGIN/../../server:/opt/irods-externals/boost1.81.0-2/lib:/opt/irods-externals/nanodbc2.13.0-3/lib:\$ORIGIN/../../lib/core:\$ORIGIN/../../lib:"
         NEW_RPATH "/opt/irods-externals/boost1.81.0-2/lib:/opt/irods-externals/nanodbc2.13.0-3/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_atomic_apply_acl_operations_server.so")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-runtime" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_atomic_apply_metadata_operations_client.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_atomic_apply_metadata_operations_client.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_atomic_apply_metadata_operations_client.so"
         RPATH "/opt/irods-externals/boost1.81.0-2/lib:/opt/irods-externals/nanodbc2.13.0-3/lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api" TYPE MODULE FILES "/src/irods/build_wdt/plugins/api/libirods_atomic_apply_metadata_operations_client.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_atomic_apply_metadata_operations_client.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_atomic_apply_metadata_operations_client.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_atomic_apply_metadata_operations_client.so"
         OLD_RPATH "\$ORIGIN/../../lib:/opt/irods-externals/boost1.81.0-2/lib:/opt/irods-externals/nanodbc2.13.0-3/lib:\$ORIGIN/../../lib/core:"
         NEW_RPATH "/opt/irods-externals/boost1.81.0-2/lib:/opt/irods-externals/nanodbc2.13.0-3/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_atomic_apply_metadata_operations_client.so")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-runtime" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_atomic_apply_metadata_operations_server.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_atomic_apply_metadata_operations_server.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_atomic_apply_metadata_operations_server.so"
         RPATH "/opt/irods-externals/boost1.81.0-2/lib:/opt/irods-externals/nanodbc2.13.0-3/lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api" TYPE MODULE FILES "/src/irods/build_wdt/plugins/api/libirods_atomic_apply_metadata_operations_server.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_atomic_apply_metadata_operations_server.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_atomic_apply_metadata_operations_server.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_atomic_apply_metadata_operations_server.so"
         OLD_RPATH "\$ORIGIN/../../server:/opt/irods-externals/boost1.81.0-2/lib:/opt/irods-externals/nanodbc2.13.0-3/lib:\$ORIGIN/../../lib/core:\$ORIGIN/../../lib:"
         NEW_RPATH "/opt/irods-externals/boost1.81.0-2/lib:/opt/irods-externals/nanodbc2.13.0-3/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_atomic_apply_metadata_operations_server.so")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-runtime" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_authenticate_client.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_authenticate_client.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_authenticate_client.so"
         RPATH "/opt/irods-externals/boost1.81.0-2/lib:/opt/irods-externals/nanodbc2.13.0-3/lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api" TYPE MODULE FILES "/src/irods/build_wdt/plugins/api/libirods_authenticate_client.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_authenticate_client.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_authenticate_client.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_authenticate_client.so"
         OLD_RPATH "\$ORIGIN/../../lib:/opt/irods-externals/boost1.81.0-2/lib:/opt/irods-externals/nanodbc2.13.0-3/lib:\$ORIGIN/../../lib/core:"
         NEW_RPATH "/opt/irods-externals/boost1.81.0-2/lib:/opt/irods-externals/nanodbc2.13.0-3/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_authenticate_client.so")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-runtime" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_authenticate_server.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_authenticate_server.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_authenticate_server.so"
         RPATH "/opt/irods-externals/boost1.81.0-2/lib:/opt/irods-externals/nanodbc2.13.0-3/lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api" TYPE MODULE FILES "/src/irods/build_wdt/plugins/api/libirods_authenticate_server.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_authenticate_server.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_authenticate_server.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_authenticate_server.so"
         OLD_RPATH "\$ORIGIN/../../server:/opt/irods-externals/boost1.81.0-2/lib:/opt/irods-externals/nanodbc2.13.0-3/lib:\$ORIGIN/../../lib/core:\$ORIGIN/../../lib:"
         NEW_RPATH "/opt/irods-externals/boost1.81.0-2/lib:/opt/irods-externals/nanodbc2.13.0-3/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_authenticate_server.so")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-runtime" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_data_object_finalize_client.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_data_object_finalize_client.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_data_object_finalize_client.so"
         RPATH "/opt/irods-externals/boost1.81.0-2/lib:/opt/irods-externals/nanodbc2.13.0-3/lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api" TYPE MODULE FILES "/src/irods/build_wdt/plugins/api/libirods_data_object_finalize_client.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_data_object_finalize_client.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_data_object_finalize_client.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_data_object_finalize_client.so"
         OLD_RPATH "\$ORIGIN/../../lib:/opt/irods-externals/boost1.81.0-2/lib:/opt/irods-externals/nanodbc2.13.0-3/lib:\$ORIGIN/../../lib/core:"
         NEW_RPATH "/opt/irods-externals/boost1.81.0-2/lib:/opt/irods-externals/nanodbc2.13.0-3/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_data_object_finalize_client.so")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-runtime" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_data_object_finalize_server.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_data_object_finalize_server.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_data_object_finalize_server.so"
         RPATH "/opt/irods-externals/boost1.81.0-2/lib:/opt/irods-externals/nanodbc2.13.0-3/lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api" TYPE MODULE FILES "/src/irods/build_wdt/plugins/api/libirods_data_object_finalize_server.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_data_object_finalize_server.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_data_object_finalize_server.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_data_object_finalize_server.so"
         OLD_RPATH "\$ORIGIN/../../server:/opt/irods-externals/boost1.81.0-2/lib:/opt/irods-externals/nanodbc2.13.0-3/lib:\$ORIGIN/../../lib/core:\$ORIGIN/../../lib:"
         NEW_RPATH "/opt/irods-externals/boost1.81.0-2/lib:/opt/irods-externals/nanodbc2.13.0-3/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_data_object_finalize_server.so")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-runtime" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_data_object_modify_info_client.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_data_object_modify_info_client.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_data_object_modify_info_client.so"
         RPATH "/opt/irods-externals/boost1.81.0-2/lib:/opt/irods-externals/nanodbc2.13.0-3/lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api" TYPE MODULE FILES "/src/irods/build_wdt/plugins/api/libirods_data_object_modify_info_client.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_data_object_modify_info_client.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_data_object_modify_info_client.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_data_object_modify_info_client.so"
         OLD_RPATH "\$ORIGIN/../../lib:/opt/irods-externals/boost1.81.0-2/lib:/opt/irods-externals/nanodbc2.13.0-3/lib:\$ORIGIN/../../lib/core:"
         NEW_RPATH "/opt/irods-externals/boost1.81.0-2/lib:/opt/irods-externals/nanodbc2.13.0-3/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_data_object_modify_info_client.so")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-runtime" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_data_object_modify_info_server.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_data_object_modify_info_server.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_data_object_modify_info_server.so"
         RPATH "/opt/irods-externals/boost1.81.0-2/lib:/opt/irods-externals/nanodbc2.13.0-3/lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api" TYPE MODULE FILES "/src/irods/build_wdt/plugins/api/libirods_data_object_modify_info_server.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_data_object_modify_info_server.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_data_object_modify_info_server.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_data_object_modify_info_server.so"
         OLD_RPATH "\$ORIGIN/../../server:/opt/irods-externals/boost1.81.0-2/lib:/opt/irods-externals/nanodbc2.13.0-3/lib:\$ORIGIN/../../lib/core:\$ORIGIN/../../lib:"
         NEW_RPATH "/opt/irods-externals/boost1.81.0-2/lib:/opt/irods-externals/nanodbc2.13.0-3/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_data_object_modify_info_server.so")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-runtime" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_get_delay_rule_info_client.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_get_delay_rule_info_client.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_get_delay_rule_info_client.so"
         RPATH "/opt/irods-externals/boost1.81.0-2/lib:/opt/irods-externals/nanodbc2.13.0-3/lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api" TYPE MODULE FILES "/src/irods/build_wdt/plugins/api/libirods_get_delay_rule_info_client.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_get_delay_rule_info_client.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_get_delay_rule_info_client.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_get_delay_rule_info_client.so"
         OLD_RPATH "\$ORIGIN/../../lib:/opt/irods-externals/boost1.81.0-2/lib:/opt/irods-externals/nanodbc2.13.0-3/lib:\$ORIGIN/../../lib/core:"
         NEW_RPATH "/opt/irods-externals/boost1.81.0-2/lib:/opt/irods-externals/nanodbc2.13.0-3/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_get_delay_rule_info_client.so")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-runtime" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_get_delay_rule_info_server.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_get_delay_rule_info_server.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_get_delay_rule_info_server.so"
         RPATH "/opt/irods-externals/boost1.81.0-2/lib:/opt/irods-externals/nanodbc2.13.0-3/lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api" TYPE MODULE FILES "/src/irods/build_wdt/plugins/api/libirods_get_delay_rule_info_server.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_get_delay_rule_info_server.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_get_delay_rule_info_server.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_get_delay_rule_info_server.so"
         OLD_RPATH "\$ORIGIN/../../server:/opt/irods-externals/boost1.81.0-2/lib:/opt/irods-externals/nanodbc2.13.0-3/lib:\$ORIGIN/../../lib/core:\$ORIGIN/../../lib:"
         NEW_RPATH "/opt/irods-externals/boost1.81.0-2/lib:/opt/irods-externals/nanodbc2.13.0-3/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_get_delay_rule_info_server.so")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-runtime" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_get_file_descriptor_info_client.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_get_file_descriptor_info_client.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_get_file_descriptor_info_client.so"
         RPATH "/opt/irods-externals/boost1.81.0-2/lib:/opt/irods-externals/nanodbc2.13.0-3/lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api" TYPE MODULE FILES "/src/irods/build_wdt/plugins/api/libirods_get_file_descriptor_info_client.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_get_file_descriptor_info_client.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_get_file_descriptor_info_client.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_get_file_descriptor_info_client.so"
         OLD_RPATH "\$ORIGIN/../../lib:/opt/irods-externals/boost1.81.0-2/lib:/opt/irods-externals/nanodbc2.13.0-3/lib:\$ORIGIN/../../lib/core:"
         NEW_RPATH "/opt/irods-externals/boost1.81.0-2/lib:/opt/irods-externals/nanodbc2.13.0-3/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_get_file_descriptor_info_client.so")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-runtime" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_get_file_descriptor_info_server.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_get_file_descriptor_info_server.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_get_file_descriptor_info_server.so"
         RPATH "/opt/irods-externals/boost1.81.0-2/lib:/opt/irods-externals/nanodbc2.13.0-3/lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api" TYPE MODULE FILES "/src/irods/build_wdt/plugins/api/libirods_get_file_descriptor_info_server.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_get_file_descriptor_info_server.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_get_file_descriptor_info_server.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_get_file_descriptor_info_server.so"
         OLD_RPATH "\$ORIGIN/../../server:/opt/irods-externals/boost1.81.0-2/lib:/opt/irods-externals/nanodbc2.13.0-3/lib:\$ORIGIN/../../lib/core:\$ORIGIN/../../lib:"
         NEW_RPATH "/opt/irods-externals/boost1.81.0-2/lib:/opt/irods-externals/nanodbc2.13.0-3/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_get_file_descriptor_info_server.so")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-runtime" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_get_grid_configuration_value_client.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_get_grid_configuration_value_client.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_get_grid_configuration_value_client.so"
         RPATH "/opt/irods-externals/boost1.81.0-2/lib:/opt/irods-externals/nanodbc2.13.0-3/lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api" TYPE MODULE FILES "/src/irods/build_wdt/plugins/api/libirods_get_grid_configuration_value_client.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_get_grid_configuration_value_client.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_get_grid_configuration_value_client.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_get_grid_configuration_value_client.so"
         OLD_RPATH "\$ORIGIN/../../lib:/opt/irods-externals/boost1.81.0-2/lib:/opt/irods-externals/nanodbc2.13.0-3/lib:\$ORIGIN/../../lib/core:"
         NEW_RPATH "/opt/irods-externals/boost1.81.0-2/lib:/opt/irods-externals/nanodbc2.13.0-3/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_get_grid_configuration_value_client.so")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-runtime" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_get_grid_configuration_value_server.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_get_grid_configuration_value_server.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_get_grid_configuration_value_server.so"
         RPATH "/opt/irods-externals/boost1.81.0-2/lib:/opt/irods-externals/nanodbc2.13.0-3/lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api" TYPE MODULE FILES "/src/irods/build_wdt/plugins/api/libirods_get_grid_configuration_value_server.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_get_grid_configuration_value_server.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_get_grid_configuration_value_server.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_get_grid_configuration_value_server.so"
         OLD_RPATH "\$ORIGIN/../../server:/opt/irods-externals/boost1.81.0-2/lib:/opt/irods-externals/nanodbc2.13.0-3/lib:\$ORIGIN/../../lib/core:\$ORIGIN/../../lib:"
         NEW_RPATH "/opt/irods-externals/boost1.81.0-2/lib:/opt/irods-externals/nanodbc2.13.0-3/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_get_grid_configuration_value_server.so")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-runtime" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_set_grid_configuration_value_client.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_set_grid_configuration_value_client.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_set_grid_configuration_value_client.so"
         RPATH "/opt/irods-externals/boost1.81.0-2/lib:/opt/irods-externals/nanodbc2.13.0-3/lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api" TYPE MODULE FILES "/src/irods/build_wdt/plugins/api/libirods_set_grid_configuration_value_client.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_set_grid_configuration_value_client.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_set_grid_configuration_value_client.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_set_grid_configuration_value_client.so"
         OLD_RPATH "\$ORIGIN/../../lib:/opt/irods-externals/boost1.81.0-2/lib:/opt/irods-externals/nanodbc2.13.0-3/lib:\$ORIGIN/../../lib/core:"
         NEW_RPATH "/opt/irods-externals/boost1.81.0-2/lib:/opt/irods-externals/nanodbc2.13.0-3/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_set_grid_configuration_value_client.so")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-runtime" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_set_grid_configuration_value_server.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_set_grid_configuration_value_server.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_set_grid_configuration_value_server.so"
         RPATH "/opt/irods-externals/boost1.81.0-2/lib:/opt/irods-externals/nanodbc2.13.0-3/lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api" TYPE MODULE FILES "/src/irods/build_wdt/plugins/api/libirods_set_grid_configuration_value_server.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_set_grid_configuration_value_server.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_set_grid_configuration_value_server.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_set_grid_configuration_value_server.so"
         OLD_RPATH "\$ORIGIN/../../server:/opt/irods-externals/boost1.81.0-2/lib:/opt/irods-externals/nanodbc2.13.0-3/lib:\$ORIGIN/../../lib/core:\$ORIGIN/../../lib:"
         NEW_RPATH "/opt/irods-externals/boost1.81.0-2/lib:/opt/irods-externals/nanodbc2.13.0-3/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_set_grid_configuration_value_server.so")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-runtime" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_set_delay_server_migration_info_client.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_set_delay_server_migration_info_client.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_set_delay_server_migration_info_client.so"
         RPATH "/opt/irods-externals/boost1.81.0-2/lib:/opt/irods-externals/nanodbc2.13.0-3/lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api" TYPE MODULE FILES "/src/irods/build_wdt/plugins/api/libirods_set_delay_server_migration_info_client.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_set_delay_server_migration_info_client.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_set_delay_server_migration_info_client.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_set_delay_server_migration_info_client.so"
         OLD_RPATH "\$ORIGIN/../../lib:/opt/irods-externals/boost1.81.0-2/lib:/opt/irods-externals/nanodbc2.13.0-3/lib:\$ORIGIN/../../lib/core:"
         NEW_RPATH "/opt/irods-externals/boost1.81.0-2/lib:/opt/irods-externals/nanodbc2.13.0-3/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_set_delay_server_migration_info_client.so")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-runtime" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_set_delay_server_migration_info_server.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_set_delay_server_migration_info_server.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_set_delay_server_migration_info_server.so"
         RPATH "/opt/irods-externals/boost1.81.0-2/lib:/opt/irods-externals/nanodbc2.13.0-3/lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api" TYPE MODULE FILES "/src/irods/build_wdt/plugins/api/libirods_set_delay_server_migration_info_server.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_set_delay_server_migration_info_server.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_set_delay_server_migration_info_server.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_set_delay_server_migration_info_server.so"
         OLD_RPATH "\$ORIGIN/../../server:/opt/irods-externals/boost1.81.0-2/lib:/opt/irods-externals/nanodbc2.13.0-3/lib:\$ORIGIN/../../lib/core:\$ORIGIN/../../lib:"
         NEW_RPATH "/opt/irods-externals/boost1.81.0-2/lib:/opt/irods-externals/nanodbc2.13.0-3/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_set_delay_server_migration_info_server.so")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-runtime" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_register_physical_path_client.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_register_physical_path_client.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_register_physical_path_client.so"
         RPATH "/opt/irods-externals/boost1.81.0-2/lib:/opt/irods-externals/nanodbc2.13.0-3/lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api" TYPE MODULE FILES "/src/irods/build_wdt/plugins/api/libirods_register_physical_path_client.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_register_physical_path_client.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_register_physical_path_client.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_register_physical_path_client.so"
         OLD_RPATH "\$ORIGIN/../../lib:/opt/irods-externals/boost1.81.0-2/lib:/opt/irods-externals/nanodbc2.13.0-3/lib:\$ORIGIN/../../lib/core:"
         NEW_RPATH "/opt/irods-externals/boost1.81.0-2/lib:/opt/irods-externals/nanodbc2.13.0-3/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_register_physical_path_client.so")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-runtime" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_register_physical_path_server.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_register_physical_path_server.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_register_physical_path_server.so"
         RPATH "/opt/irods-externals/boost1.81.0-2/lib:/opt/irods-externals/nanodbc2.13.0-3/lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api" TYPE MODULE FILES "/src/irods/build_wdt/plugins/api/libirods_register_physical_path_server.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_register_physical_path_server.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_register_physical_path_server.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_register_physical_path_server.so"
         OLD_RPATH "\$ORIGIN/../../server:/opt/irods-externals/boost1.81.0-2/lib:/opt/irods-externals/nanodbc2.13.0-3/lib:\$ORIGIN/../../lib/core:\$ORIGIN/../../lib:"
         NEW_RPATH "/opt/irods-externals/boost1.81.0-2/lib:/opt/irods-externals/nanodbc2.13.0-3/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_register_physical_path_server.so")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-runtime" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_replica_close_client.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_replica_close_client.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_replica_close_client.so"
         RPATH "/opt/irods-externals/boost1.81.0-2/lib:/opt/irods-externals/nanodbc2.13.0-3/lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api" TYPE MODULE FILES "/src/irods/build_wdt/plugins/api/libirods_replica_close_client.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_replica_close_client.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_replica_close_client.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_replica_close_client.so"
         OLD_RPATH "\$ORIGIN/../../lib:/opt/irods-externals/boost1.81.0-2/lib:/opt/irods-externals/nanodbc2.13.0-3/lib:\$ORIGIN/../../lib/core:"
         NEW_RPATH "/opt/irods-externals/boost1.81.0-2/lib:/opt/irods-externals/nanodbc2.13.0-3/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_replica_close_client.so")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-runtime" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_replica_close_server.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_replica_close_server.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_replica_close_server.so"
         RPATH "/opt/irods-externals/boost1.81.0-2/lib:/opt/irods-externals/nanodbc2.13.0-3/lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api" TYPE MODULE FILES "/src/irods/build_wdt/plugins/api/libirods_replica_close_server.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_replica_close_server.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_replica_close_server.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_replica_close_server.so"
         OLD_RPATH "\$ORIGIN/../../server:/opt/irods-externals/boost1.81.0-2/lib:/opt/irods-externals/nanodbc2.13.0-3/lib:\$ORIGIN/../../lib/core:\$ORIGIN/../../lib:"
         NEW_RPATH "/opt/irods-externals/boost1.81.0-2/lib:/opt/irods-externals/nanodbc2.13.0-3/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_replica_close_server.so")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-runtime" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_replica_open_client.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_replica_open_client.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_replica_open_client.so"
         RPATH "/opt/irods-externals/boost1.81.0-2/lib:/opt/irods-externals/nanodbc2.13.0-3/lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api" TYPE MODULE FILES "/src/irods/build_wdt/plugins/api/libirods_replica_open_client.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_replica_open_client.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_replica_open_client.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_replica_open_client.so"
         OLD_RPATH "\$ORIGIN/../../lib:/opt/irods-externals/boost1.81.0-2/lib:/opt/irods-externals/nanodbc2.13.0-3/lib:\$ORIGIN/../../lib/core:"
         NEW_RPATH "/opt/irods-externals/boost1.81.0-2/lib:/opt/irods-externals/nanodbc2.13.0-3/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_replica_open_client.so")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-runtime" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_replica_open_server.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_replica_open_server.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_replica_open_server.so"
         RPATH "/opt/irods-externals/boost1.81.0-2/lib:/opt/irods-externals/nanodbc2.13.0-3/lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api" TYPE MODULE FILES "/src/irods/build_wdt/plugins/api/libirods_replica_open_server.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_replica_open_server.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_replica_open_server.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_replica_open_server.so"
         OLD_RPATH "\$ORIGIN/../../server:/opt/irods-externals/boost1.81.0-2/lib:/opt/irods-externals/nanodbc2.13.0-3/lib:\$ORIGIN/../../lib/core:\$ORIGIN/../../lib:"
         NEW_RPATH "/opt/irods-externals/boost1.81.0-2/lib:/opt/irods-externals/nanodbc2.13.0-3/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_replica_open_server.so")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-runtime" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_switch_user_client.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_switch_user_client.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_switch_user_client.so"
         RPATH "/opt/irods-externals/boost1.81.0-2/lib:/opt/irods-externals/nanodbc2.13.0-3/lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api" TYPE MODULE FILES "/src/irods/build_wdt/plugins/api/libirods_switch_user_client.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_switch_user_client.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_switch_user_client.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_switch_user_client.so"
         OLD_RPATH "\$ORIGIN/../../lib:/opt/irods-externals/boost1.81.0-2/lib:/opt/irods-externals/nanodbc2.13.0-3/lib:\$ORIGIN/../../lib/core:"
         NEW_RPATH "/opt/irods-externals/boost1.81.0-2/lib:/opt/irods-externals/nanodbc2.13.0-3/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_switch_user_client.so")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-runtime" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_switch_user_server.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_switch_user_server.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_switch_user_server.so"
         RPATH "/opt/irods-externals/boost1.81.0-2/lib:/opt/irods-externals/nanodbc2.13.0-3/lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api" TYPE MODULE FILES "/src/irods/build_wdt/plugins/api/libirods_switch_user_server.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_switch_user_server.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_switch_user_server.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_switch_user_server.so"
         OLD_RPATH "\$ORIGIN/../../server:/opt/irods-externals/boost1.81.0-2/lib:/opt/irods-externals/nanodbc2.13.0-3/lib:\$ORIGIN/../../lib/core:\$ORIGIN/../../lib:"
         NEW_RPATH "/opt/irods-externals/boost1.81.0-2/lib:/opt/irods-externals/nanodbc2.13.0-3/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_switch_user_server.so")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-runtime" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_touch_client.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_touch_client.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_touch_client.so"
         RPATH "/opt/irods-externals/boost1.81.0-2/lib:/opt/irods-externals/nanodbc2.13.0-3/lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api" TYPE MODULE FILES "/src/irods/build_wdt/plugins/api/libirods_touch_client.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_touch_client.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_touch_client.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_touch_client.so"
         OLD_RPATH "\$ORIGIN/../../lib:/opt/irods-externals/boost1.81.0-2/lib:/opt/irods-externals/nanodbc2.13.0-3/lib:\$ORIGIN/../../lib/core:"
         NEW_RPATH "/opt/irods-externals/boost1.81.0-2/lib:/opt/irods-externals/nanodbc2.13.0-3/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_touch_client.so")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-runtime" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_touch_server.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_touch_server.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_touch_server.so"
         RPATH "/opt/irods-externals/boost1.81.0-2/lib:/opt/irods-externals/nanodbc2.13.0-3/lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api" TYPE MODULE FILES "/src/irods/build_wdt/plugins/api/libirods_touch_server.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_touch_server.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_touch_server.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_touch_server.so"
         OLD_RPATH "\$ORIGIN/../../server:/opt/irods-externals/boost1.81.0-2/lib:/opt/irods-externals/nanodbc2.13.0-3/lib:\$ORIGIN/../../lib/core:\$ORIGIN/../../lib:"
         NEW_RPATH "/opt/irods-externals/boost1.81.0-2/lib:/opt/irods-externals/nanodbc2.13.0-3/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/api/libirods_touch_server.so")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-dev" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/usr/include/irods/plugins/api" TYPE FILE FILES
    "/src/irods/plugins/api/include/irods/plugins/api/api_plugin_number.h"
    "/src/irods/plugins/api/include/irods/plugins/api/api_plugin_number_map.hpp"
    "/src/irods/plugins/api/include/irods/plugins/api/api_plugin_number_data.h"
    "/src/irods/plugins/api/include/irods/plugins/api/delay_server_migration_types.h"
    "/src/irods/plugins/api/include/irods/plugins/api/grid_configuration_types.h"
    "/src/irods/plugins/api/include/irods/plugins/api/switch_user_types.h"
    )
endif()

