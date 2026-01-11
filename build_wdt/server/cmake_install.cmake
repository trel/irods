# Install script for directory: /src/irods/server

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
  include("/src/irods/build_wdt/server/core/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/src/irods/build_wdt/server/genquery2/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/src/irods/build_wdt/server/api/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/src/irods/build_wdt/server/icat/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/src/irods/build_wdt/server/re/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/src/irods/build_wdt/server/drivers/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/src/irods/build_wdt/server/auth/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/src/irods/build_wdt/server/main_server/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/src/irods/build_wdt/server/hostname_resolves_to_local_address/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/src/irods/build_wdt/server/delay_server/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/src/irods/build_wdt/server/harness/cmake_install.cmake")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-runtime" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/libirods_server.so.5.0.90" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/libirods_server.so.5.0.90")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/libirods_server.so.5.0.90"
         RPATH "/opt/irods-externals/boost1.81.0-2/lib:/opt/irods-externals/nanodbc2.13.0-3/lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/usr/lib" TYPE SHARED_LIBRARY FILES "/src/irods/build_wdt/server/libirods_server.so.5.0.90")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/libirods_server.so.5.0.90" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/libirods_server.so.5.0.90")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/libirods_server.so.5.0.90"
         OLD_RPATH "/opt/irods-externals/boost1.81.0-2/lib:/opt/irods-externals/nanodbc2.13.0-3/lib:\$ORIGIN/../lib/core:\$ORIGIN/../lib:"
         NEW_RPATH "/opt/irods-externals/boost1.81.0-2/lib:/opt/irods-externals/nanodbc2.13.0-3/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/libirods_server.so.5.0.90")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-dev" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/usr/lib" TYPE SHARED_LIBRARY FILES "/src/irods/build_wdt/server/libirods_server.so")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/configuration_schemas/v5" TYPE FILE FILES "/src/irods/build_wdt/schemas/configuration/v5/version.json")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/configuration_schemas/v5" TYPE FILE FILES "/src/irods/build_wdt/schemas/configuration/v5/client_environment.json")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/configuration_schemas/v5" TYPE FILE FILES "/src/irods/build_wdt/schemas/configuration/v5/client_hints.json")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/configuration_schemas/v5" TYPE FILE FILES "/src/irods/build_wdt/schemas/configuration/v5/configuration_directory.json")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/configuration_schemas/v5" TYPE FILE FILES "/src/irods/build_wdt/schemas/configuration/v5/database_config.json")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/configuration_schemas/v5" TYPE FILE FILES "/src/irods/build_wdt/schemas/configuration/v5/host_access_control.json")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/configuration_schemas/v5" TYPE FILE FILES "/src/irods/build_wdt/schemas/configuration/v5/host_resolution.json")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/configuration_schemas/v5" TYPE FILE FILES "/src/irods/build_wdt/schemas/configuration/v5/plugin.json")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/configuration_schemas/v5" TYPE FILE FILES "/src/irods/build_wdt/schemas/configuration/v5/resource.json")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/configuration_schemas/v5" TYPE FILE FILES "/src/irods/build_wdt/schemas/configuration/v5/rule_engine.json")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/configuration_schemas/v5" TYPE FILE FILES "/src/irods/build_wdt/schemas/configuration/v5/server.json")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/configuration_schemas/v5" TYPE FILE FILES "/src/irods/build_wdt/schemas/configuration/v5/server_config.json")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/configuration_schemas/v5" TYPE FILE FILES "/src/irods/build_wdt/schemas/configuration/v5/service_account_environment.json")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/configuration_schemas/v5" TYPE FILE FILES "/src/irods/build_wdt/schemas/configuration/v5/unattended_installation.json")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/configuration_schemas/v5" TYPE FILE FILES "/src/irods/build_wdt/schemas/configuration/v5/zone_bundle.json")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/config/lockFileDir" TYPE DIRECTORY FILES "")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods" TYPE FILE FILES "/src/irods/build_wdt/version.json.dist")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/log" TYPE DIRECTORY FILES "")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/etc/irods" TYPE DIRECTORY FILES "")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/run/irods" TYPE DIRECTORY DIR_PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_WRITE GROUP_READ WORLD_READ FILES "")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/usr/share/doc/irods" TYPE FILE RENAME "readme" FILES "/src/irods/README.md")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/usr/share/doc/irods" TYPE FILE FILES "/src/irods/LICENSE")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/packaging" TYPE FILE PERMISSIONS OWNER_READ GROUP_READ WORLD_READ FILES
    "/src/irods/packaging/connectControl.config.template"
    "/src/irods/packaging/core.dvm.template"
    "/src/irods/packaging/core.fnm.template"
    "/src/irods/packaging/core.re.template"
    "/src/irods/packaging/irodsMonPerf.config.in"
    "/src/irods/build_wdt/packaging/server_config.json.template"
    "/src/irods/packaging/server_setup_instructions.txt"
    "/src/irods/packaging/server_upgrade_instructions.txt"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/packaging" TYPE FILE PERMISSIONS OWNER_READ GROUP_READ WORLD_READ FILES "/src/irods/packaging/irods.service.template")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/packaging" TYPE FILE PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ WORLD_READ FILES
    "/src/irods/packaging/find_os.sh"
    "/src/irods/packaging/postinstall.sh"
    "/src/irods/packaging/preremove.sh"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods" TYPE DIRECTORY FILES "/src/irods/scripts" USE_SOURCE_PERMISSIONS REGEX "/\\_\\_pycache\\_\\_$" EXCLUDE REGEX "/[^/]*\\.pyc$" EXCLUDE REGEX "/[^/]*\\.pyd$" EXCLUDE REGEX "/[^/]*\\.pyo$" EXCLUDE)
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/scripts/irods" TYPE FILE FILES "/src/irods/build_wdt/scripts/irods/paths_cmake.py")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/msiExecCmd_bin" TYPE FILE PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ WORLD_READ FILES
    "/src/irods/msiExecCmd_bin/test_execstream.py"
    "/src/irods/msiExecCmd_bin/hello.template"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/msiExecCmd_bin" TYPE FILE PERMISSIONS OWNER_READ GROUP_READ WORLD_READ FILES
    "/src/irods/msiExecCmd_bin/irodsServerMonPerf.template"
    "/src/irods/msiExecCmd_bin/univMSSInterface.sh.template"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/test" TYPE FILE FILES "/src/irods/test/test_framework_configuration.json")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/test" TYPE DIRECTORY FILES "/src/irods/test/filesystem")
endif()

