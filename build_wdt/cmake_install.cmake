# Install script for directory: /src/irods

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
  include("/src/irods/build_wdt/lib/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/src/irods/build_wdt/server/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/src/irods/build_wdt/plugins/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/src/irods/build_wdt/test/c_api_test/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/src/irods/build_wdt/test/post_install_test/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/src/irods/build_wdt/unit_tests/cmake_install.cmake")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-runtime" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/usr/share/doc/irods/irods-runtime" TYPE FILE RENAME "copyright" FILES "/src/irods/LICENSE")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-server" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/usr/share/doc/irods/irods-server" TYPE FILE RENAME "copyright" FILES "/src/irods/LICENSE")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-dev" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/usr/share/doc/irods/irods-dev" TYPE FILE RENAME "copyright" FILES "/src/irods/LICENSE")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-database-plugin-postgres" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/usr/share/doc/irods/irods-database-plugin-postgres" TYPE FILE RENAME "copyright" FILES "/src/irods/LICENSE")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-database-plugin-mysql" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/usr/share/doc/irods/irods-database-plugin-mysql" TYPE FILE RENAME "copyright" FILES "/src/irods/LICENSE")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-database-plugin-oracle" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/usr/share/doc/irods/irods-database-plugin-oracle" TYPE FILE RENAME "copyright" FILES "/src/irods/LICENSE")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-dev" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/usr/lib/cmake/irods-5.0" TYPE FILE RENAME "IRODSConfig.cmake" FILES "/src/irods/build_wdt/IRODSConfig.cmake.not_yet_installed")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-dev" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/usr/lib/cmake/irods-5.0" TYPE FILE FILES
    "/src/irods/build_wdt/IRODSConfigVersion.cmake"
    "/src/irods/cmake/IRODSTargetsWrapper.cmake"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-dev" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/usr/lib/cmake/irods-5.0/Modules" TYPE FILE FILES
    "/src/irods/cmake/Modules/RequireOutOfSourceBuild.cmake"
    "/src/irods/cmake/Modules/GetGitRevisionDescription.cmake"
    "/src/irods/cmake/Modules/GetGitRevisionDescription.cmake.in"
    "/src/irods/cmake/Modules/CheckCPPMacroDefinition.cmake"
    "/src/irods/cmake/Modules/CheckCPPMacroDefinition.c.in"
    "/src/irods/cmake/Modules/FindCatch2.cmake"
    "/src/irods/cmake/Modules/Findnlohmann_json.cmake"
    "/src/irods/cmake/Modules/Findsystemd.cmake"
    "/src/irods/cmake/Modules/IrodsExternals.cmake"
    "/src/irods/cmake/Modules/IrodsCXXCompiler.cmake"
    "/src/irods/cmake/Modules/ObjectTargetHelpers.cmake"
    "/src/irods/cmake/Modules/IrodsCPackPlatform.cmake"
    "/src/irods/cmake/Modules/IrodsCPackCommon.cmake"
    "/src/irods/cmake/Modules/IrodsRunpathDefaults.cmake"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-dev" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/usr/lib/cmake/irods-5.0/Modules" TYPE FILE PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE FILES "/src/irods/scripts/irods/distro_distill.py")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-dev" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/cmake/irods-5.0/IRODSTargets.cmake")
    file(DIFFERENT _cmake_export_file_changed FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/cmake/irods-5.0/IRODSTargets.cmake"
         "/src/irods/build_wdt/CMakeFiles/Export/6e3cbc166b85626eb3101805e5beb732/IRODSTargets.cmake")
    if(_cmake_export_file_changed)
      file(GLOB _cmake_old_config_files "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/cmake/irods-5.0/IRODSTargets-*.cmake")
      if(_cmake_old_config_files)
        string(REPLACE ";" ", " _cmake_old_config_files_text "${_cmake_old_config_files}")
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/cmake/irods-5.0/IRODSTargets.cmake\" will be replaced.  Removing files [${_cmake_old_config_files_text}].")
        unset(_cmake_old_config_files_text)
        file(REMOVE ${_cmake_old_config_files})
      endif()
      unset(_cmake_old_config_files)
    endif()
    unset(_cmake_export_file_changed)
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/usr/lib/cmake/irods-5.0" TYPE FILE FILES "/src/irods/build_wdt/CMakeFiles/Export/6e3cbc166b85626eb3101805e5beb732/IRODSTargets.cmake")
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/usr/lib/cmake/irods-5.0" TYPE FILE FILES "/src/irods/build_wdt/CMakeFiles/Export/6e3cbc166b85626eb3101805e5beb732/IRODSTargets-relwithdebinfo.cmake")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "/src/irods/build_wdt/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
