# Install script for directory: /src/irods/unit_tests

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
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_access_time_queue" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_access_time_queue")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_access_time_queue"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests" TYPE EXECUTABLE FILES "/src/irods/build_wdt/unit_tests/irods_access_time_queue")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_access_time_queue" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_access_time_queue")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_access_time_queue"
         OLD_RPATH "\$ORIGIN/../server:\$ORIGIN/../lib/core:\$ORIGIN/../lib:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_access_time_queue")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_atomic_apply_acl_operations" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_atomic_apply_acl_operations")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_atomic_apply_acl_operations"
         RPATH "/opt/irods-externals/boost1.81.0-2/lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests" TYPE EXECUTABLE FILES "/src/irods/build_wdt/unit_tests/irods_atomic_apply_acl_operations")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_atomic_apply_acl_operations" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_atomic_apply_acl_operations")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_atomic_apply_acl_operations"
         OLD_RPATH "\$ORIGIN/../lib:\$ORIGIN/../lib/core:/opt/irods-externals/boost1.81.0-2/lib:"
         NEW_RPATH "/opt/irods-externals/boost1.81.0-2/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_atomic_apply_acl_operations")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_atomic_apply_metadata_operations" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_atomic_apply_metadata_operations")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_atomic_apply_metadata_operations"
         RPATH "/opt/irods-externals/boost1.81.0-2/lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests" TYPE EXECUTABLE FILES "/src/irods/build_wdt/unit_tests/irods_atomic_apply_metadata_operations")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_atomic_apply_metadata_operations" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_atomic_apply_metadata_operations")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_atomic_apply_metadata_operations"
         OLD_RPATH "\$ORIGIN/../lib:\$ORIGIN/../lib/core:/opt/irods-externals/boost1.81.0-2/lib:"
         NEW_RPATH "/opt/irods-externals/boost1.81.0-2/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_atomic_apply_metadata_operations")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_attr_typing" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_attr_typing")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_attr_typing"
         RPATH "/opt/irods-externals/boost1.81.0-2/lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests" TYPE EXECUTABLE FILES "/src/irods/build_wdt/unit_tests/irods_attr_typing")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_attr_typing" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_attr_typing")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_attr_typing"
         OLD_RPATH "\$ORIGIN/../lib:\$ORIGIN/../lib/core:/opt/irods-externals/boost1.81.0-2/lib:"
         NEW_RPATH "/opt/irods-externals/boost1.81.0-2/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_attr_typing")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_authenticate_client" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_authenticate_client")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_authenticate_client"
         RPATH "/opt/irods-externals/boost1.81.0-2/lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests" TYPE EXECUTABLE FILES "/src/irods/build_wdt/unit_tests/irods_authenticate_client")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_authenticate_client" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_authenticate_client")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_authenticate_client"
         OLD_RPATH "\$ORIGIN/../lib:/opt/irods-externals/boost1.81.0-2/lib:\$ORIGIN/../lib/core:"
         NEW_RPATH "/opt/irods-externals/boost1.81.0-2/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_authenticate_client")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_capped_memory_resource" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_capped_memory_resource")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_capped_memory_resource"
         RPATH "/opt/irods-externals/boost1.81.0-2/lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests" TYPE EXECUTABLE FILES "/src/irods/build_wdt/unit_tests/irods_capped_memory_resource")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_capped_memory_resource" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_capped_memory_resource")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_capped_memory_resource"
         OLD_RPATH "\$ORIGIN/../lib:/opt/irods-externals/boost1.81.0-2/lib:"
         NEW_RPATH "/opt/irods-externals/boost1.81.0-2/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_capped_memory_resource")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_client_connection" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_client_connection")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_client_connection"
         RPATH "/opt/irods-externals/boost1.81.0-2/lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests" TYPE EXECUTABLE FILES "/src/irods/build_wdt/unit_tests/irods_client_connection")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_client_connection" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_client_connection")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_client_connection"
         OLD_RPATH "\$ORIGIN/../lib:/opt/irods-externals/boost1.81.0-2/lib:\$ORIGIN/../lib/core:"
         NEW_RPATH "/opt/irods-externals/boost1.81.0-2/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_client_connection")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_client_server_negotiation" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_client_server_negotiation")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_client_server_negotiation"
         RPATH "/opt/irods-externals/boost1.81.0-2/lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests" TYPE EXECUTABLE FILES "/src/irods/build_wdt/unit_tests/irods_client_server_negotiation")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_client_server_negotiation" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_client_server_negotiation")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_client_server_negotiation"
         OLD_RPATH "\$ORIGIN/../lib:/opt/irods-externals/boost1.81.0-2/lib:\$ORIGIN/../lib/core:"
         NEW_RPATH "/opt/irods-externals/boost1.81.0-2/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_client_server_negotiation")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_connection_pool" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_connection_pool")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_connection_pool"
         RPATH "/opt/irods-externals/boost1.81.0-2/lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests" TYPE EXECUTABLE FILES "/src/irods/build_wdt/unit_tests/irods_connection_pool")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_connection_pool" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_connection_pool")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_connection_pool"
         OLD_RPATH "\$ORIGIN/../lib:/opt/irods-externals/boost1.81.0-2/lib:\$ORIGIN/../lib/core:"
         NEW_RPATH "/opt/irods-externals/boost1.81.0-2/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_connection_pool")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_data_object_finalize" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_data_object_finalize")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_data_object_finalize"
         RPATH "/opt/irods-externals/boost1.81.0-2/lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests" TYPE EXECUTABLE FILES "/src/irods/build_wdt/unit_tests/irods_data_object_finalize")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_data_object_finalize" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_data_object_finalize")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_data_object_finalize"
         OLD_RPATH "\$ORIGIN/../lib:\$ORIGIN/../lib/core:/opt/irods-externals/boost1.81.0-2/lib:"
         NEW_RPATH "/opt/irods-externals/boost1.81.0-2/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_data_object_finalize")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_data_object_modify_info" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_data_object_modify_info")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_data_object_modify_info"
         RPATH "/opt/irods-externals/boost1.81.0-2/lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests" TYPE EXECUTABLE FILES "/src/irods/build_wdt/unit_tests/irods_data_object_modify_info")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_data_object_modify_info" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_data_object_modify_info")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_data_object_modify_info"
         OLD_RPATH "\$ORIGIN/../lib:\$ORIGIN/../lib/core:/opt/irods-externals/boost1.81.0-2/lib:"
         NEW_RPATH "/opt/irods-externals/boost1.81.0-2/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_data_object_modify_info")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_data_object_proxy" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_data_object_proxy")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_data_object_proxy"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests" TYPE EXECUTABLE FILES "/src/irods/build_wdt/unit_tests/irods_data_object_proxy")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_data_object_proxy" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_data_object_proxy")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_data_object_proxy"
         OLD_RPATH "\$ORIGIN/../lib:\$ORIGIN/../lib/core:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_data_object_proxy")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_delay_hints_parser" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_delay_hints_parser")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_delay_hints_parser"
         RPATH "/opt/irods-externals/boost1.81.0-2/lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests" TYPE EXECUTABLE FILES "/src/irods/build_wdt/unit_tests/irods_delay_hints_parser")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_delay_hints_parser" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_delay_hints_parser")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_delay_hints_parser"
         OLD_RPATH "\$ORIGIN/../lib:\$ORIGIN/../lib/core:/opt/irods-externals/boost1.81.0-2/lib:"
         NEW_RPATH "/opt/irods-externals/boost1.81.0-2/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_delay_hints_parser")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_delay_rule_locking_api" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_delay_rule_locking_api")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_delay_rule_locking_api"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests" TYPE EXECUTABLE FILES "/src/irods/build_wdt/unit_tests/irods_delay_rule_locking_api")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_delay_rule_locking_api" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_delay_rule_locking_api")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_delay_rule_locking_api"
         OLD_RPATH "\$ORIGIN/../lib:\$ORIGIN/../lib/core:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_delay_rule_locking_api")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_deprecation_warnings" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_deprecation_warnings")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_deprecation_warnings"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests" TYPE EXECUTABLE FILES "/src/irods/build_wdt/unit_tests/irods_deprecation_warnings")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_deprecation_warnings" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_deprecation_warnings")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_deprecation_warnings"
         OLD_RPATH "\$ORIGIN/../lib:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_deprecation_warnings")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_dns_cache" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_dns_cache")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_dns_cache"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests" TYPE EXECUTABLE FILES "/src/irods/build_wdt/unit_tests/irods_dns_cache")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_dns_cache" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_dns_cache")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_dns_cache"
         OLD_RPATH "\$ORIGIN/../lib:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_dns_cache")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_dstream" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_dstream")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_dstream"
         RPATH "/opt/irods-externals/boost1.81.0-2/lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests" TYPE EXECUTABLE FILES "/src/irods/build_wdt/unit_tests/irods_dstream")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_dstream" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_dstream")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_dstream"
         OLD_RPATH "\$ORIGIN/../lib:/opt/irods-externals/boost1.81.0-2/lib:\$ORIGIN/../lib/core:"
         NEW_RPATH "/opt/irods-externals/boost1.81.0-2/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_dstream")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_environment_variables" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_environment_variables")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_environment_variables"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests" TYPE EXECUTABLE FILES "/src/irods/build_wdt/unit_tests/irods_environment_variables")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_environment_variables" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_environment_variables")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_environment_variables"
         OLD_RPATH "\$ORIGIN/../lib:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_environment_variables")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_file_object" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_file_object")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_file_object"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests" TYPE EXECUTABLE FILES "/src/irods/build_wdt/unit_tests/irods_file_object")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_file_object" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_file_object")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_file_object"
         OLD_RPATH "\$ORIGIN/../server:\$ORIGIN/../lib/core:\$ORIGIN/../lib:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_file_object")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_filesystem" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_filesystem")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_filesystem"
         RPATH "/opt/irods-externals/boost1.81.0-2/lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests" TYPE EXECUTABLE FILES "/src/irods/build_wdt/unit_tests/irods_filesystem")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_filesystem" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_filesystem")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_filesystem"
         OLD_RPATH "\$ORIGIN/../lib:\$ORIGIN/../lib/core:/opt/irods-externals/boost1.81.0-2/lib:"
         NEW_RPATH "/opt/irods-externals/boost1.81.0-2/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_filesystem")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_fixed_buffer_resource" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_fixed_buffer_resource")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_fixed_buffer_resource"
         RPATH "/opt/irods-externals/boost1.81.0-2/lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests" TYPE EXECUTABLE FILES "/src/irods/build_wdt/unit_tests/irods_fixed_buffer_resource")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_fixed_buffer_resource" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_fixed_buffer_resource")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_fixed_buffer_resource"
         OLD_RPATH "\$ORIGIN/../lib:/opt/irods-externals/boost1.81.0-2/lib:"
         NEW_RPATH "/opt/irods-externals/boost1.81.0-2/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_fixed_buffer_resource")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_fully_qualified_username" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_fully_qualified_username")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_fully_qualified_username"
         RPATH "/opt/irods-externals/boost1.81.0-2/lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests" TYPE EXECUTABLE FILES "/src/irods/build_wdt/unit_tests/irods_fully_qualified_username")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_fully_qualified_username" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_fully_qualified_username")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_fully_qualified_username"
         OLD_RPATH "\$ORIGIN/../lib:/opt/irods-externals/boost1.81.0-2/lib:"
         NEW_RPATH "/opt/irods-externals/boost1.81.0-2/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_fully_qualified_username")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_generate_random_alphanumeric_string" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_generate_random_alphanumeric_string")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_generate_random_alphanumeric_string"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests" TYPE EXECUTABLE FILES "/src/irods/build_wdt/unit_tests/irods_generate_random_alphanumeric_string")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_generate_random_alphanumeric_string" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_generate_random_alphanumeric_string")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_generate_random_alphanumeric_string"
         OLD_RPATH "\$ORIGIN/../lib:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_generate_random_alphanumeric_string")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_genquery1_flex_bison_parser" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_genquery1_flex_bison_parser")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_genquery1_flex_bison_parser"
         RPATH "/opt/irods-externals/boost1.81.0-2/lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests" TYPE EXECUTABLE FILES "/src/irods/build_wdt/unit_tests/irods_genquery1_flex_bison_parser")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_genquery1_flex_bison_parser" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_genquery1_flex_bison_parser")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_genquery1_flex_bison_parser"
         OLD_RPATH "\$ORIGIN/../lib:/opt/irods-externals/boost1.81.0-2/lib:"
         NEW_RPATH "/opt/irods-externals/boost1.81.0-2/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_genquery1_flex_bison_parser")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_getRodsEnv" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_getRodsEnv")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_getRodsEnv"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests" TYPE EXECUTABLE FILES "/src/irods/build_wdt/unit_tests/irods_getRodsEnv")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_getRodsEnv" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_getRodsEnv")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_getRodsEnv"
         OLD_RPATH "\$ORIGIN/../lib:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_getRodsEnv")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_get_delay_rule_info" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_get_delay_rule_info")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_get_delay_rule_info"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests" TYPE EXECUTABLE FILES "/src/irods/build_wdt/unit_tests/irods_get_delay_rule_info")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_get_delay_rule_info" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_get_delay_rule_info")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_get_delay_rule_info"
         OLD_RPATH "\$ORIGIN/../lib:\$ORIGIN/../lib/core:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_get_delay_rule_info")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_get_file_descriptor_info" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_get_file_descriptor_info")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_get_file_descriptor_info"
         RPATH "/opt/irods-externals/boost1.81.0-2/lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests" TYPE EXECUTABLE FILES "/src/irods/build_wdt/unit_tests/irods_get_file_descriptor_info")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_get_file_descriptor_info" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_get_file_descriptor_info")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_get_file_descriptor_info"
         OLD_RPATH "\$ORIGIN/../lib:\$ORIGIN/../lib/core:/opt/irods-externals/boost1.81.0-2/lib:"
         NEW_RPATH "/opt/irods-externals/boost1.81.0-2/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_get_file_descriptor_info")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_get_resource_info_for_operation" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_get_resource_info_for_operation")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_get_resource_info_for_operation"
         RPATH "/opt/irods-externals/boost1.81.0-2/lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests" TYPE EXECUTABLE FILES "/src/irods/build_wdt/unit_tests/irods_get_resource_info_for_operation")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_get_resource_info_for_operation" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_get_resource_info_for_operation")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_get_resource_info_for_operation"
         OLD_RPATH "\$ORIGIN/../lib:/opt/irods-externals/boost1.81.0-2/lib:\$ORIGIN/../lib/core:"
         NEW_RPATH "/opt/irods-externals/boost1.81.0-2/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_get_resource_info_for_operation")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_hashers" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_hashers")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_hashers"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests" TYPE EXECUTABLE FILES "/src/irods/build_wdt/unit_tests/irods_hashers")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_hashers" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_hashers")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_hashers"
         OLD_RPATH "\$ORIGIN/../lib:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_hashers")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_hierarchy_parser" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_hierarchy_parser")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_hierarchy_parser"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests" TYPE EXECUTABLE FILES "/src/irods/build_wdt/unit_tests/irods_hierarchy_parser")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_hierarchy_parser" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_hierarchy_parser")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_hierarchy_parser"
         OLD_RPATH "\$ORIGIN/../lib:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_hierarchy_parser")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_host_list_context_string" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_host_list_context_string")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_host_list_context_string"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests" TYPE EXECUTABLE FILES "/src/irods/build_wdt/unit_tests/irods_host_list_context_string")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_host_list_context_string" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_host_list_context_string")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_host_list_context_string"
         OLD_RPATH "\$ORIGIN/../lib:\$ORIGIN/../lib/core:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_host_list_context_string")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_hostname_cache" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_hostname_cache")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_hostname_cache"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests" TYPE EXECUTABLE FILES "/src/irods/build_wdt/unit_tests/irods_hostname_cache")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_hostname_cache" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_hostname_cache")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_hostname_cache"
         OLD_RPATH "\$ORIGIN/../lib:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_hostname_cache")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_json_apis_from_client" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_json_apis_from_client")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_json_apis_from_client"
         RPATH "/opt/irods-externals/boost1.81.0-2/lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests" TYPE EXECUTABLE FILES "/src/irods/build_wdt/unit_tests/irods_json_apis_from_client")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_json_apis_from_client" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_json_apis_from_client")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_json_apis_from_client"
         OLD_RPATH "\$ORIGIN/../lib:/opt/irods-externals/boost1.81.0-2/lib:\$ORIGIN/../lib/core:"
         NEW_RPATH "/opt/irods-externals/boost1.81.0-2/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_json_apis_from_client")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_json_events" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_json_events")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_json_events"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests" TYPE EXECUTABLE FILES "/src/irods/build_wdt/unit_tests/irods_json_events")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_json_events" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_json_events")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_json_events"
         OLD_RPATH "\$ORIGIN/../lib:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_json_events")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_key_value_proxy" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_key_value_proxy")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_key_value_proxy"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests" TYPE EXECUTABLE FILES "/src/irods/build_wdt/unit_tests/irods_key_value_proxy")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_key_value_proxy" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_key_value_proxy")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_key_value_proxy"
         OLD_RPATH "\$ORIGIN/../lib:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_key_value_proxy")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_legacy_compat" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_legacy_compat")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_legacy_compat"
         RPATH "/opt/irods-externals/boost1.81.0-2/lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests" TYPE EXECUTABLE FILES "/src/irods/build_wdt/unit_tests/irods_legacy_compat")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_legacy_compat" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_legacy_compat")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_legacy_compat"
         OLD_RPATH "\$ORIGIN/../lib:\$ORIGIN/../lib/core:/opt/irods-externals/boost1.81.0-2/lib:"
         NEW_RPATH "/opt/irods-externals/boost1.81.0-2/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_legacy_compat")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_legacy_construct_warnings" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_legacy_construct_warnings")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_legacy_construct_warnings"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests" TYPE EXECUTABLE FILES "/src/irods/build_wdt/unit_tests/irods_legacy_construct_warnings")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_legacy_construct_warnings" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_legacy_construct_warnings")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_legacy_construct_warnings"
         OLD_RPATH "\$ORIGIN/../lib:\$ORIGIN/../lib/core:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_legacy_construct_warnings")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_lifetime_manager" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_lifetime_manager")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_lifetime_manager"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests" TYPE EXECUTABLE FILES "/src/irods/build_wdt/unit_tests/irods_lifetime_manager")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_lifetime_manager" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_lifetime_manager")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_lifetime_manager"
         OLD_RPATH "\$ORIGIN/../lib:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_lifetime_manager")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_linked_list_iterator" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_linked_list_iterator")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_linked_list_iterator"
         RPATH "/opt/irods-externals/boost1.81.0-2/lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests" TYPE EXECUTABLE FILES "/src/irods/build_wdt/unit_tests/irods_linked_list_iterator")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_linked_list_iterator" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_linked_list_iterator")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_linked_list_iterator"
         OLD_RPATH "\$ORIGIN/../server:/opt/irods-externals/boost1.81.0-2/lib:\$ORIGIN/../lib/core:\$ORIGIN/../lib:"
         NEW_RPATH "/opt/irods-externals/boost1.81.0-2/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_linked_list_iterator")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_macros_templates" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_macros_templates")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_macros_templates"
         RPATH "/opt/irods-externals/boost1.81.0-2/lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests" TYPE EXECUTABLE FILES "/src/irods/build_wdt/unit_tests/irods_macros_templates")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_macros_templates" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_macros_templates")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_macros_templates"
         OLD_RPATH "\$ORIGIN/../lib:/opt/irods-externals/boost1.81.0-2/lib:\$ORIGIN/../lib/core:"
         NEW_RPATH "/opt/irods-externals/boost1.81.0-2/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_macros_templates")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_logical_locking" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_logical_locking")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_logical_locking"
         RPATH "/opt/irods-externals/boost1.81.0-2/lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests" TYPE EXECUTABLE FILES "/src/irods/build_wdt/unit_tests/irods_logical_locking")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_logical_locking" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_logical_locking")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_logical_locking"
         OLD_RPATH "\$ORIGIN/../lib:\$ORIGIN/../lib/core:/opt/irods-externals/boost1.81.0-2/lib:"
         NEW_RPATH "/opt/irods-externals/boost1.81.0-2/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_logical_locking")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_logical_paths_and_special_characters" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_logical_paths_and_special_characters")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_logical_paths_and_special_characters"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests" TYPE EXECUTABLE FILES "/src/irods/build_wdt/unit_tests/irods_logical_paths_and_special_characters")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_logical_paths_and_special_characters" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_logical_paths_and_special_characters")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_logical_paths_and_special_characters"
         OLD_RPATH "\$ORIGIN/../lib:\$ORIGIN/../lib/core:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_logical_paths_and_special_characters")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_metadata" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_metadata")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_metadata"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests" TYPE EXECUTABLE FILES "/src/irods/build_wdt/unit_tests/irods_metadata")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_metadata" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_metadata")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_metadata"
         OLD_RPATH "\$ORIGIN/../lib:\$ORIGIN/../lib/core:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_metadata")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_modern_syntax_enforcement" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_modern_syntax_enforcement")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_modern_syntax_enforcement"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests" TYPE EXECUTABLE FILES "/src/irods/build_wdt/unit_tests/irods_modern_syntax_enforcement")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_modern_syntax_enforcement" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_modern_syntax_enforcement")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_modern_syntax_enforcement"
         OLD_RPATH "\$ORIGIN/../server:\$ORIGIN/../lib/core:\$ORIGIN/../lib:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_modern_syntax_enforcement")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_optional_types" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_optional_types")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_optional_types"
         RPATH "/opt/irods-externals/boost1.81.0-2/lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests" TYPE EXECUTABLE FILES "/src/irods/build_wdt/unit_tests/irods_optional_types")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_optional_types" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_optional_types")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_optional_types"
         OLD_RPATH "\$ORIGIN/../lib:\$ORIGIN/../lib/core:/opt/irods-externals/boost1.81.0-2/lib:"
         NEW_RPATH "/opt/irods-externals/boost1.81.0-2/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_optional_types")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_packstruct" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_packstruct")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_packstruct"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests" TYPE EXECUTABLE FILES "/src/irods/build_wdt/unit_tests/irods_packstruct")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_packstruct" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_packstruct")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_packstruct"
         OLD_RPATH "\$ORIGIN/../lib:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_packstruct")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_parallel_transfer_engine" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_parallel_transfer_engine")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_parallel_transfer_engine"
         RPATH "/opt/irods-externals/boost1.81.0-2/lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests" TYPE EXECUTABLE FILES "/src/irods/build_wdt/unit_tests/irods_parallel_transfer_engine")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_parallel_transfer_engine" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_parallel_transfer_engine")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_parallel_transfer_engine"
         OLD_RPATH "\$ORIGIN/../lib:\$ORIGIN/../lib/core:/opt/irods-externals/boost1.81.0-2/lib:"
         NEW_RPATH "/opt/irods-externals/boost1.81.0-2/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_parallel_transfer_engine")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_procApiRequest_raw" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_procApiRequest_raw")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_procApiRequest_raw"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests" TYPE EXECUTABLE FILES "/src/irods/build_wdt/unit_tests/irods_procApiRequest_raw")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_procApiRequest_raw" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_procApiRequest_raw")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_procApiRequest_raw"
         OLD_RPATH "\$ORIGIN/../lib:\$ORIGIN/../lib/core:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_procApiRequest_raw")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_process_stash" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_process_stash")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_process_stash"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests" TYPE EXECUTABLE FILES "/src/irods/build_wdt/unit_tests/irods_process_stash")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_process_stash" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_process_stash")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_process_stash"
         OLD_RPATH "\$ORIGIN/../lib:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_process_stash")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_query_builder" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_query_builder")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_query_builder"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests" TYPE EXECUTABLE FILES "/src/irods/build_wdt/unit_tests/irods_query_builder")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_query_builder" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_query_builder")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_query_builder"
         OLD_RPATH "\$ORIGIN/../lib:\$ORIGIN/../lib/core:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_query_builder")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_query_type_inference" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_query_type_inference")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_query_type_inference"
         RPATH "/opt/irods-externals/boost1.81.0-2/lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests" TYPE EXECUTABLE FILES "/src/irods/build_wdt/unit_tests/irods_query_type_inference")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_query_type_inference" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_query_type_inference")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_query_type_inference"
         OLD_RPATH "\$ORIGIN/../lib:\$ORIGIN/../lib/core:/opt/irods-externals/boost1.81.0-2/lib:"
         NEW_RPATH "/opt/irods-externals/boost1.81.0-2/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_query_type_inference")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_rcConnect" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_rcConnect")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_rcConnect"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests" TYPE EXECUTABLE FILES "/src/irods/build_wdt/unit_tests/irods_rcConnect")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_rcConnect" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_rcConnect")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_rcConnect"
         OLD_RPATH "\$ORIGIN/../lib:\$ORIGIN/../lib/core:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_rcConnect")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_rcTicketAdmin" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_rcTicketAdmin")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_rcTicketAdmin"
         RPATH "/opt/irods-externals/boost1.81.0-2/lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests" TYPE EXECUTABLE FILES "/src/irods/build_wdt/unit_tests/irods_rcTicketAdmin")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_rcTicketAdmin" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_rcTicketAdmin")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_rcTicketAdmin"
         OLD_RPATH "\$ORIGIN/../lib:/opt/irods-externals/boost1.81.0-2/lib:\$ORIGIN/../lib/core:"
         NEW_RPATH "/opt/irods-externals/boost1.81.0-2/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_rcTicketAdmin")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_rc_check_auth_credentials" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_rc_check_auth_credentials")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_rc_check_auth_credentials"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests" TYPE EXECUTABLE FILES "/src/irods/build_wdt/unit_tests/irods_rc_check_auth_credentials")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_rc_check_auth_credentials" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_rc_check_auth_credentials")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_rc_check_auth_credentials"
         OLD_RPATH "\$ORIGIN/../lib:\$ORIGIN/../lib/core:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_rc_check_auth_credentials")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_rc_data_obj" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_rc_data_obj")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_rc_data_obj"
         RPATH "/opt/irods-externals/boost1.81.0-2/lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests" TYPE EXECUTABLE FILES "/src/irods/build_wdt/unit_tests/irods_rc_data_obj")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_rc_data_obj" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_rc_data_obj")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_rc_data_obj"
         OLD_RPATH "\$ORIGIN/../lib:\$ORIGIN/../lib/core:/opt/irods-externals/boost1.81.0-2/lib:"
         NEW_RPATH "/opt/irods-externals/boost1.81.0-2/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_rc_data_obj")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_rc_data_obj_repl" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_rc_data_obj_repl")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_rc_data_obj_repl"
         RPATH "/opt/irods-externals/boost1.81.0-2/lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests" TYPE EXECUTABLE FILES "/src/irods/build_wdt/unit_tests/irods_rc_data_obj_repl")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_rc_data_obj_repl" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_rc_data_obj_repl")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_rc_data_obj_repl"
         OLD_RPATH "\$ORIGIN/../lib:\$ORIGIN/../lib/core:/opt/irods-externals/boost1.81.0-2/lib:"
         NEW_RPATH "/opt/irods-externals/boost1.81.0-2/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_rc_data_obj_repl")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_rc_genquery2" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_rc_genquery2")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_rc_genquery2"
         RPATH "/opt/irods-externals/boost1.81.0-2/lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests" TYPE EXECUTABLE FILES "/src/irods/build_wdt/unit_tests/irods_rc_genquery2")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_rc_genquery2" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_rc_genquery2")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_rc_genquery2"
         OLD_RPATH "\$ORIGIN/../lib:/opt/irods-externals/boost1.81.0-2/lib:\$ORIGIN/../lib/core:"
         NEW_RPATH "/opt/irods-externals/boost1.81.0-2/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_rc_genquery2")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_rc_get_library_features" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_rc_get_library_features")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_rc_get_library_features"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests" TYPE EXECUTABLE FILES "/src/irods/build_wdt/unit_tests/irods_rc_get_library_features")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_rc_get_library_features" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_rc_get_library_features")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_rc_get_library_features"
         OLD_RPATH "\$ORIGIN/../lib:\$ORIGIN/../lib/core:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_rc_get_library_features")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_rc_mod_data_obj_meta" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_rc_mod_data_obj_meta")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_rc_mod_data_obj_meta"
         RPATH "/opt/irods-externals/boost1.81.0-2/lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests" TYPE EXECUTABLE FILES "/src/irods/build_wdt/unit_tests/irods_rc_mod_data_obj_meta")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_rc_mod_data_obj_meta" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_rc_mod_data_obj_meta")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_rc_mod_data_obj_meta"
         OLD_RPATH "\$ORIGIN/../lib:\$ORIGIN/../lib/core:/opt/irods-externals/boost1.81.0-2/lib:"
         NEW_RPATH "/opt/irods-externals/boost1.81.0-2/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_rc_mod_data_obj_meta")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_rc_switch_user" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_rc_switch_user")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_rc_switch_user"
         RPATH "/opt/irods-externals/boost1.81.0-2/lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests" TYPE EXECUTABLE FILES "/src/irods/build_wdt/unit_tests/irods_rc_switch_user")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_rc_switch_user" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_rc_switch_user")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_rc_switch_user"
         OLD_RPATH "\$ORIGIN/../lib:/opt/irods-externals/boost1.81.0-2/lib:\$ORIGIN/../lib/core:"
         NEW_RPATH "/opt/irods-externals/boost1.81.0-2/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_rc_switch_user")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_rc_update_replica_access_time" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_rc_update_replica_access_time")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_rc_update_replica_access_time"
         RPATH "/opt/irods-externals/boost1.81.0-2/lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests" TYPE EXECUTABLE FILES "/src/irods/build_wdt/unit_tests/irods_rc_update_replica_access_time")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_rc_update_replica_access_time" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_rc_update_replica_access_time")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_rc_update_replica_access_time"
         OLD_RPATH "\$ORIGIN/../lib:/opt/irods-externals/boost1.81.0-2/lib:\$ORIGIN/../lib/core:"
         NEW_RPATH "/opt/irods-externals/boost1.81.0-2/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_rc_update_replica_access_time")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_re_serialization" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_re_serialization")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_re_serialization"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests" TYPE EXECUTABLE FILES "/src/irods/build_wdt/unit_tests/irods_re_serialization")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_re_serialization" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_re_serialization")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_re_serialization"
         OLD_RPATH "\$ORIGIN/../server:\$ORIGIN/../lib/core:\$ORIGIN/../lib:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_re_serialization")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_replica" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_replica")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_replica"
         RPATH "/opt/irods-externals/boost1.81.0-2/lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests" TYPE EXECUTABLE FILES "/src/irods/build_wdt/unit_tests/irods_replica")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_replica" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_replica")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_replica"
         OLD_RPATH "\$ORIGIN/../lib:/opt/irods-externals/boost1.81.0-2/lib:\$ORIGIN/../lib/core:"
         NEW_RPATH "/opt/irods-externals/boost1.81.0-2/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_replica")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_replica_access_table" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_replica_access_table")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_replica_access_table"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests" TYPE EXECUTABLE FILES "/src/irods/build_wdt/unit_tests/irods_replica_access_table")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_replica_access_table" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_replica_access_table")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_replica_access_table"
         OLD_RPATH "\$ORIGIN/../server:\$ORIGIN/../lib/core:\$ORIGIN/../lib:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_replica_access_table")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_replica_open_and_close" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_replica_open_and_close")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_replica_open_and_close"
         RPATH "/opt/irods-externals/boost1.81.0-2/lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests" TYPE EXECUTABLE FILES "/src/irods/build_wdt/unit_tests/irods_replica_open_and_close")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_replica_open_and_close" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_replica_open_and_close")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_replica_open_and_close"
         OLD_RPATH "\$ORIGIN/../lib:\$ORIGIN/../lib/core:/opt/irods-externals/boost1.81.0-2/lib:"
         NEW_RPATH "/opt/irods-externals/boost1.81.0-2/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_replica_open_and_close")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_replica_state_table" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_replica_state_table")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_replica_state_table"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests" TYPE EXECUTABLE FILES "/src/irods/build_wdt/unit_tests/irods_replica_state_table")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_replica_state_table" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_replica_state_table")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_replica_state_table"
         OLD_RPATH "\$ORIGIN/../server:\$ORIGIN/../lib/core:\$ORIGIN/../lib:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_replica_state_table")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_replica_truncate" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_replica_truncate")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_replica_truncate"
         RPATH "/opt/irods-externals/boost1.81.0-2/lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests" TYPE EXECUTABLE FILES "/src/irods/build_wdt/unit_tests/irods_replica_truncate")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_replica_truncate" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_replica_truncate")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_replica_truncate"
         OLD_RPATH "\$ORIGIN/../lib:\$ORIGIN/../lib/core:/opt/irods-externals/boost1.81.0-2/lib:"
         NEW_RPATH "/opt/irods-externals/boost1.81.0-2/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_replica_truncate")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_rerror_stack" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_rerror_stack")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_rerror_stack"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests" TYPE EXECUTABLE FILES "/src/irods/build_wdt/unit_tests/irods_rerror_stack")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_rerror_stack" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_rerror_stack")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_rerror_stack"
         OLD_RPATH "\$ORIGIN/../lib:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_rerror_stack")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_resource_administration" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_resource_administration")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_resource_administration"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests" TYPE EXECUTABLE FILES "/src/irods/build_wdt/unit_tests/irods_resource_administration")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_resource_administration" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_resource_administration")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_resource_administration"
         OLD_RPATH "\$ORIGIN/../lib:\$ORIGIN/../lib/core:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_resource_administration")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_scoped_privileged_client" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_scoped_privileged_client")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_scoped_privileged_client"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests" TYPE EXECUTABLE FILES "/src/irods/build_wdt/unit_tests/irods_scoped_privileged_client")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_scoped_privileged_client" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_scoped_privileged_client")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_scoped_privileged_client"
         OLD_RPATH "\$ORIGIN/../server:\$ORIGIN/../lib/core:\$ORIGIN/../lib:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_scoped_privileged_client")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_server_properties" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_server_properties")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_server_properties"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests" TYPE EXECUTABLE FILES "/src/irods/build_wdt/unit_tests/irods_server_properties")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_server_properties" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_server_properties")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_server_properties"
         OLD_RPATH "\$ORIGIN/../lib:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_server_properties")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_server_utilities" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_server_utilities")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_server_utilities"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests" TYPE EXECUTABLE FILES "/src/irods/build_wdt/unit_tests/irods_server_utilities")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_server_utilities" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_server_utilities")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_server_utilities"
         OLD_RPATH "\$ORIGIN/../server:\$ORIGIN/../lib/core:\$ORIGIN/../lib:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_server_utilities")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_shared_memory_object" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_shared_memory_object")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_shared_memory_object"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests" TYPE EXECUTABLE FILES "/src/irods/build_wdt/unit_tests/irods_shared_memory_object")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_shared_memory_object" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_shared_memory_object")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_shared_memory_object"
         OLD_RPATH "\$ORIGIN/../lib:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_shared_memory_object")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_system_error" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_system_error")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_system_error"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests" TYPE EXECUTABLE FILES "/src/irods/build_wdt/unit_tests/irods_system_error")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_system_error" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_system_error")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_system_error"
         OLD_RPATH "\$ORIGIN/../lib:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_system_error")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_ticket_administration" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_ticket_administration")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_ticket_administration"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests" TYPE EXECUTABLE FILES "/src/irods/build_wdt/unit_tests/irods_ticket_administration")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_ticket_administration" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_ticket_administration")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_ticket_administration"
         OLD_RPATH "\$ORIGIN/../lib:\$ORIGIN/../lib/core:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_ticket_administration")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_union_constraint_solving" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_union_constraint_solving")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_union_constraint_solving"
         RPATH "/opt/irods-externals/boost1.81.0-2/lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests" TYPE EXECUTABLE FILES "/src/irods/build_wdt/unit_tests/irods_union_constraint_solving")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_union_constraint_solving" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_union_constraint_solving")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_union_constraint_solving"
         OLD_RPATH "\$ORIGIN/../lib:\$ORIGIN/../lib/core:/opt/irods-externals/boost1.81.0-2/lib:"
         NEW_RPATH "/opt/irods-externals/boost1.81.0-2/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_union_constraint_solving")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_union_types" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_union_types")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_union_types"
         RPATH "/opt/irods-externals/boost1.81.0-2/lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests" TYPE EXECUTABLE FILES "/src/irods/build_wdt/unit_tests/irods_union_types")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_union_types" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_union_types")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_union_types"
         OLD_RPATH "\$ORIGIN/../lib:\$ORIGIN/../lib/core:/opt/irods-externals/boost1.81.0-2/lib:"
         NEW_RPATH "/opt/irods-externals/boost1.81.0-2/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_union_types")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_user_administration" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_user_administration")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_user_administration"
         RPATH "/opt/irods-externals/boost1.81.0-2/lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests" TYPE EXECUTABLE FILES "/src/irods/build_wdt/unit_tests/irods_user_administration")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_user_administration" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_user_administration")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_user_administration"
         OLD_RPATH "\$ORIGIN/../lib:/opt/irods-externals/boost1.81.0-2/lib:\$ORIGIN/../lib/core:"
         NEW_RPATH "/opt/irods-externals/boost1.81.0-2/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_user_administration")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_user_deprecated_warnings" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_user_deprecated_warnings")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_user_deprecated_warnings"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests" TYPE EXECUTABLE FILES "/src/irods/build_wdt/unit_tests/irods_user_deprecated_warnings")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_user_deprecated_warnings" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_user_deprecated_warnings")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_user_deprecated_warnings"
         OLD_RPATH "\$ORIGIN/../server:\$ORIGIN/../lib/core:\$ORIGIN/../lib:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_user_deprecated_warnings")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_version" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_version")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_version"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests" TYPE EXECUTABLE FILES "/src/irods/build_wdt/unit_tests/irods_version")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_version" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_version")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_version"
         OLD_RPATH "\$ORIGIN/../lib:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_version")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_with_durability" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_with_durability")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_with_durability"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests" TYPE EXECUTABLE FILES "/src/irods/build_wdt/unit_tests/irods_with_durability")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_with_durability" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_with_durability")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_with_durability"
         OLD_RPATH "\$ORIGIN/../lib:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_with_durability")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_zone_administration" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_zone_administration")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_zone_administration"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests" TYPE EXECUTABLE FILES "/src/irods/build_wdt/unit_tests/irods_zone_administration")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_zone_administration" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_zone_administration")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_zone_administration"
         OLD_RPATH "\$ORIGIN/../lib:\$ORIGIN/../lib/core:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_zone_administration")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_zone_report" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_zone_report")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_zone_report"
         RPATH "/opt/irods-externals/boost1.81.0-2/lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests" TYPE EXECUTABLE FILES "/src/irods/build_wdt/unit_tests/irods_zone_report")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_zone_report" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_zone_report")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_zone_report"
         OLD_RPATH "\$ORIGIN/../lib:/opt/irods-externals/boost1.81.0-2/lib:\$ORIGIN/../lib/core:"
         NEW_RPATH "/opt/irods-externals/boost1.81.0-2/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests/irods_zone_report")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests" TYPE FILE FILES "/src/irods/unit_tests/unit_tests_list.json")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/unit_tests" TYPE FILE FILES "/src/irods/unit_tests/genquery1_backwards_compatibility_queries.txt")
endif()

