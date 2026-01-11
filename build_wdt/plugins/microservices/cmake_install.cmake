# Install script for directory: /src/irods/plugins/microservices

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

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_atomic_apply_acl_operations.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_atomic_apply_acl_operations.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_atomic_apply_acl_operations.so"
         RPATH "/opt/irods-externals/boost1.81.0-2/lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices" TYPE MODULE FILES "/src/irods/build_wdt/plugins/microservices/libmsi_atomic_apply_acl_operations.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_atomic_apply_acl_operations.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_atomic_apply_acl_operations.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_atomic_apply_acl_operations.so"
         OLD_RPATH "\$ORIGIN/../../server:/opt/irods-externals/boost1.81.0-2/lib:\$ORIGIN/../../lib/core:\$ORIGIN/../../lib:"
         NEW_RPATH "/opt/irods-externals/boost1.81.0-2/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_atomic_apply_acl_operations.so")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_atomic_apply_metadata_operations.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_atomic_apply_metadata_operations.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_atomic_apply_metadata_operations.so"
         RPATH "/opt/irods-externals/boost1.81.0-2/lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices" TYPE MODULE FILES "/src/irods/build_wdt/plugins/microservices/libmsi_atomic_apply_metadata_operations.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_atomic_apply_metadata_operations.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_atomic_apply_metadata_operations.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_atomic_apply_metadata_operations.so"
         OLD_RPATH "\$ORIGIN/../../server:/opt/irods-externals/boost1.81.0-2/lib:\$ORIGIN/../../lib/core:\$ORIGIN/../../lib:"
         NEW_RPATH "/opt/irods-externals/boost1.81.0-2/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_atomic_apply_metadata_operations.so")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_get_agent_pid.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_get_agent_pid.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_get_agent_pid.so"
         RPATH "/opt/irods-externals/boost1.81.0-2/lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices" TYPE MODULE FILES "/src/irods/build_wdt/plugins/microservices/libmsi_get_agent_pid.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_get_agent_pid.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_get_agent_pid.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_get_agent_pid.so"
         OLD_RPATH "\$ORIGIN/../../server:/opt/irods-externals/boost1.81.0-2/lib:\$ORIGIN/../../lib/core:\$ORIGIN/../../lib:"
         NEW_RPATH "/opt/irods-externals/boost1.81.0-2/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_get_agent_pid.so")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_get_file_descriptor_info.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_get_file_descriptor_info.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_get_file_descriptor_info.so"
         RPATH "/opt/irods-externals/boost1.81.0-2/lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices" TYPE MODULE FILES "/src/irods/build_wdt/plugins/microservices/libmsi_get_file_descriptor_info.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_get_file_descriptor_info.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_get_file_descriptor_info.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_get_file_descriptor_info.so"
         OLD_RPATH "\$ORIGIN/../../server:/opt/irods-externals/boost1.81.0-2/lib:\$ORIGIN/../../lib/core:\$ORIGIN/../../lib:"
         NEW_RPATH "/opt/irods-externals/boost1.81.0-2/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_get_file_descriptor_info.so")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_get_hostname.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_get_hostname.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_get_hostname.so"
         RPATH "/opt/irods-externals/boost1.81.0-2/lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices" TYPE MODULE FILES "/src/irods/build_wdt/plugins/microservices/libmsi_get_hostname.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_get_hostname.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_get_hostname.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_get_hostname.so"
         OLD_RPATH "\$ORIGIN/../../server:/opt/irods-externals/boost1.81.0-2/lib:\$ORIGIN/../../lib/core:\$ORIGIN/../../lib:"
         NEW_RPATH "/opt/irods-externals/boost1.81.0-2/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_get_hostname.so")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_get_open_data_obj_l1desc_index.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_get_open_data_obj_l1desc_index.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_get_open_data_obj_l1desc_index.so"
         RPATH "/opt/irods-externals/boost1.81.0-2/lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices" TYPE MODULE FILES "/src/irods/build_wdt/plugins/microservices/libmsi_get_open_data_obj_l1desc_index.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_get_open_data_obj_l1desc_index.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_get_open_data_obj_l1desc_index.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_get_open_data_obj_l1desc_index.so"
         OLD_RPATH "\$ORIGIN/../../server:/opt/irods-externals/boost1.81.0-2/lib:\$ORIGIN/../../lib/core:\$ORIGIN/../../lib:"
         NEW_RPATH "/opt/irods-externals/boost1.81.0-2/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_get_open_data_obj_l1desc_index.so")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_get_server_property.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_get_server_property.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_get_server_property.so"
         RPATH "/opt/irods-externals/boost1.81.0-2/lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices" TYPE MODULE FILES "/src/irods/build_wdt/plugins/microservices/libmsi_get_server_property.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_get_server_property.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_get_server_property.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_get_server_property.so"
         OLD_RPATH "\$ORIGIN/../../server:/opt/irods-externals/boost1.81.0-2/lib:\$ORIGIN/../../lib/core:\$ORIGIN/../../lib:"
         NEW_RPATH "/opt/irods-externals/boost1.81.0-2/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_get_server_property.so")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_json_compare.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_json_compare.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_json_compare.so"
         RPATH "/opt/irods-externals/boost1.81.0-2/lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices" TYPE MODULE FILES "/src/irods/build_wdt/plugins/microservices/libmsi_json_compare.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_json_compare.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_json_compare.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_json_compare.so"
         OLD_RPATH "\$ORIGIN/../../server:/opt/irods-externals/boost1.81.0-2/lib:\$ORIGIN/../../lib/core:\$ORIGIN/../../lib:"
         NEW_RPATH "/opt/irods-externals/boost1.81.0-2/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_json_compare.so")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_json_contains.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_json_contains.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_json_contains.so"
         RPATH "/opt/irods-externals/boost1.81.0-2/lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices" TYPE MODULE FILES "/src/irods/build_wdt/plugins/microservices/libmsi_json_contains.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_json_contains.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_json_contains.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_json_contains.so"
         OLD_RPATH "\$ORIGIN/../../server:/opt/irods-externals/boost1.81.0-2/lib:\$ORIGIN/../../lib/core:\$ORIGIN/../../lib:"
         NEW_RPATH "/opt/irods-externals/boost1.81.0-2/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_json_contains.so")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_json_dump.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_json_dump.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_json_dump.so"
         RPATH "/opt/irods-externals/boost1.81.0-2/lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices" TYPE MODULE FILES "/src/irods/build_wdt/plugins/microservices/libmsi_json_dump.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_json_dump.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_json_dump.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_json_dump.so"
         OLD_RPATH "\$ORIGIN/../../server:/opt/irods-externals/boost1.81.0-2/lib:\$ORIGIN/../../lib/core:\$ORIGIN/../../lib:"
         NEW_RPATH "/opt/irods-externals/boost1.81.0-2/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_json_dump.so")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_json_free.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_json_free.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_json_free.so"
         RPATH "/opt/irods-externals/boost1.81.0-2/lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices" TYPE MODULE FILES "/src/irods/build_wdt/plugins/microservices/libmsi_json_free.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_json_free.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_json_free.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_json_free.so"
         OLD_RPATH "\$ORIGIN/../../server:/opt/irods-externals/boost1.81.0-2/lib:\$ORIGIN/../../lib/core:\$ORIGIN/../../lib:"
         NEW_RPATH "/opt/irods-externals/boost1.81.0-2/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_json_free.so")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_json_handles.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_json_handles.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_json_handles.so"
         RPATH "/opt/irods-externals/boost1.81.0-2/lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices" TYPE MODULE FILES "/src/irods/build_wdt/plugins/microservices/libmsi_json_handles.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_json_handles.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_json_handles.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_json_handles.so"
         OLD_RPATH "\$ORIGIN/../../server:/opt/irods-externals/boost1.81.0-2/lib:\$ORIGIN/../../lib/core:\$ORIGIN/../../lib:"
         NEW_RPATH "/opt/irods-externals/boost1.81.0-2/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_json_handles.so")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_json_names.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_json_names.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_json_names.so"
         RPATH "/opt/irods-externals/boost1.81.0-2/lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices" TYPE MODULE FILES "/src/irods/build_wdt/plugins/microservices/libmsi_json_names.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_json_names.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_json_names.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_json_names.so"
         OLD_RPATH "\$ORIGIN/../../server:/opt/irods-externals/boost1.81.0-2/lib:\$ORIGIN/../../lib/core:\$ORIGIN/../../lib:"
         NEW_RPATH "/opt/irods-externals/boost1.81.0-2/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_json_names.so")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_json_parse.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_json_parse.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_json_parse.so"
         RPATH "/opt/irods-externals/boost1.81.0-2/lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices" TYPE MODULE FILES "/src/irods/build_wdt/plugins/microservices/libmsi_json_parse.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_json_parse.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_json_parse.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_json_parse.so"
         OLD_RPATH "\$ORIGIN/../../server:/opt/irods-externals/boost1.81.0-2/lib:\$ORIGIN/../../lib/core:\$ORIGIN/../../lib:"
         NEW_RPATH "/opt/irods-externals/boost1.81.0-2/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_json_parse.so")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_json_size.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_json_size.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_json_size.so"
         RPATH "/opt/irods-externals/boost1.81.0-2/lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices" TYPE MODULE FILES "/src/irods/build_wdt/plugins/microservices/libmsi_json_size.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_json_size.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_json_size.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_json_size.so"
         OLD_RPATH "\$ORIGIN/../../server:/opt/irods-externals/boost1.81.0-2/lib:\$ORIGIN/../../lib/core:\$ORIGIN/../../lib:"
         NEW_RPATH "/opt/irods-externals/boost1.81.0-2/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_json_size.so")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_json_type.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_json_type.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_json_type.so"
         RPATH "/opt/irods-externals/boost1.81.0-2/lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices" TYPE MODULE FILES "/src/irods/build_wdt/plugins/microservices/libmsi_json_type.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_json_type.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_json_type.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_json_type.so"
         OLD_RPATH "\$ORIGIN/../../server:/opt/irods-externals/boost1.81.0-2/lib:\$ORIGIN/../../lib/core:\$ORIGIN/../../lib:"
         NEW_RPATH "/opt/irods-externals/boost1.81.0-2/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_json_type.so")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_json_value.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_json_value.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_json_value.so"
         RPATH "/opt/irods-externals/boost1.81.0-2/lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices" TYPE MODULE FILES "/src/irods/build_wdt/plugins/microservices/libmsi_json_value.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_json_value.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_json_value.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_json_value.so"
         OLD_RPATH "\$ORIGIN/../../server:/opt/irods-externals/boost1.81.0-2/lib:\$ORIGIN/../../lib/core:\$ORIGIN/../../lib:"
         NEW_RPATH "/opt/irods-externals/boost1.81.0-2/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_json_value.so")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsisync_to_archive.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsisync_to_archive.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsisync_to_archive.so"
         RPATH "/opt/irods-externals/boost1.81.0-2/lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices" TYPE MODULE FILES "/src/irods/build_wdt/plugins/microservices/libmsisync_to_archive.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsisync_to_archive.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsisync_to_archive.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsisync_to_archive.so"
         OLD_RPATH "\$ORIGIN/../../server:/opt/irods-externals/boost1.81.0-2/lib:\$ORIGIN/../../lib/core:\$ORIGIN/../../lib:"
         NEW_RPATH "/opt/irods-externals/boost1.81.0-2/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsisync_to_archive.so")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_touch.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_touch.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_touch.so"
         RPATH "/opt/irods-externals/boost1.81.0-2/lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices" TYPE MODULE FILES "/src/irods/build_wdt/plugins/microservices/libmsi_touch.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_touch.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_touch.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_touch.so"
         OLD_RPATH "\$ORIGIN/../../server:/opt/irods-externals/boost1.81.0-2/lib:\$ORIGIN/../../lib/core:\$ORIGIN/../../lib:"
         NEW_RPATH "/opt/irods-externals/boost1.81.0-2/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_touch.so")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_update_unixfilesystem_resource_free_space.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_update_unixfilesystem_resource_free_space.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_update_unixfilesystem_resource_free_space.so"
         RPATH "/opt/irods-externals/boost1.81.0-2/lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices" TYPE MODULE FILES "/src/irods/build_wdt/plugins/microservices/libmsi_update_unixfilesystem_resource_free_space.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_update_unixfilesystem_resource_free_space.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_update_unixfilesystem_resource_free_space.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_update_unixfilesystem_resource_free_space.so"
         OLD_RPATH "\$ORIGIN/../../server:/opt/irods-externals/boost1.81.0-2/lib:\$ORIGIN/../../lib/core:\$ORIGIN/../../lib:"
         NEW_RPATH "/opt/irods-externals/boost1.81.0-2/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/microservices/libmsi_update_unixfilesystem_resource_free_space.so")
    endif()
  endif()
endif()

