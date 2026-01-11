# Install script for directory: /src/irods/plugins/rule_engines

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

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/src/irods/build_wdt/plugins/rule_engines/irods_rule_language/cmake_install.cmake")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/rule_engines/libirods_rule_engine_plugin-cpp_default_policy.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/rule_engines/libirods_rule_engine_plugin-cpp_default_policy.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/rule_engines/libirods_rule_engine_plugin-cpp_default_policy.so"
         RPATH "/opt/irods-externals/boost1.81.0-2/lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/rule_engines" TYPE MODULE FILES "/src/irods/build_wdt/plugins/rule_engines/libirods_rule_engine_plugin-cpp_default_policy.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/rule_engines/libirods_rule_engine_plugin-cpp_default_policy.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/rule_engines/libirods_rule_engine_plugin-cpp_default_policy.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/rule_engines/libirods_rule_engine_plugin-cpp_default_policy.so"
         OLD_RPATH "\$ORIGIN/../../server:/opt/irods-externals/boost1.81.0-2/lib:\$ORIGIN/../../lib/core:\$ORIGIN/../../lib:"
         NEW_RPATH "/opt/irods-externals/boost1.81.0-2/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/rule_engines/libirods_rule_engine_plugin-cpp_default_policy.so")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/rule_engines/libirods_rule_engine_plugin-passthrough.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/rule_engines/libirods_rule_engine_plugin-passthrough.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/rule_engines/libirods_rule_engine_plugin-passthrough.so"
         RPATH "/opt/irods-externals/boost1.81.0-2/lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/rule_engines" TYPE MODULE FILES "/src/irods/build_wdt/plugins/rule_engines/libirods_rule_engine_plugin-passthrough.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/rule_engines/libirods_rule_engine_plugin-passthrough.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/rule_engines/libirods_rule_engine_plugin-passthrough.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/rule_engines/libirods_rule_engine_plugin-passthrough.so"
         OLD_RPATH "\$ORIGIN/../../server:/opt/irods-externals/boost1.81.0-2/lib:\$ORIGIN/../../lib/core:\$ORIGIN/../../lib:"
         NEW_RPATH "/opt/irods-externals/boost1.81.0-2/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/rule_engines/libirods_rule_engine_plugin-passthrough.so")
    endif()
  endif()
endif()

