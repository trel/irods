# Install script for directory: /src/irods/server/re

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
    "/src/irods/server/re/include/irods/irods_ms_plugin.hpp"
    "/src/irods/server/re/include/irods/irods_re_plugin.hpp"
    "/src/irods/server/re/include/irods/irods_re_namespaceshelper.hpp"
    "/src/irods/server/re/include/irods/irods_re_structs.hpp"
    "/src/irods/server/re/include/irods/irods_re_ruleexistshelper.hpp"
    "/src/irods/server/re/include/irods/irods_re_serialization.hpp"
    "/src/irods/server/re/include/irods/locks.hpp"
    "/src/irods/server/re/include/irods/msiHelper.hpp"
    "/src/irods/server/re/include/irods/reAction.hpp"
    "/src/irods/server/re/include/irods/reDataObjOpr.hpp"
    "/src/irods/server/re/include/irods/reDefines.h"
    "/src/irods/server/re/include/irods/reIn2p3SysRule.hpp"
    "/src/irods/server/re/include/irods/reNaraMetaData.hpp"
    "/src/irods/server/re/include/irods/reSysDataObjOpr.hpp"
    "/src/irods/server/re/include/irods/reconstants.hpp"
    "/src/irods/server/re/include/irods/sharedmemory.hpp"
    )
endif()

