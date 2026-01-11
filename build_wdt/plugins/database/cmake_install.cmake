# Install script for directory: /src/irods/plugins/database

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

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-database-plugin-postgres" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/database/libpostgres.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/database/libpostgres.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/database/libpostgres.so"
         RPATH "/opt/irods-externals/boost1.81.0-2/lib:/opt/irods-externals/nanodbc2.13.0-3/lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/database" TYPE MODULE FILES "/src/irods/build_wdt/plugins/database/libpostgres.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/database/libpostgres.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/database/libpostgres.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/database/libpostgres.so"
         OLD_RPATH "\$ORIGIN/../../server:\$ORIGIN/../../lib/core:/opt/irods-externals/boost1.81.0-2/lib:/opt/irods-externals/nanodbc2.13.0-3/lib:\$ORIGIN/../../lib:"
         NEW_RPATH "/opt/irods-externals/boost1.81.0-2/lib:/opt/irods-externals/nanodbc2.13.0-3/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/database/libpostgres.so")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-database-plugin-postgres" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/packaging" TYPE FILE FILES "/src/irods/plugins/database/postgres/upgrade-3.3.xto4.0.0.sql")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-database-plugin-postgres" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/packaging" TYPE FILE PERMISSIONS OWNER_READ GROUP_READ WORLD_READ FILES "/src/irods/plugins/database/packaging/localhost_setup_postgres.input")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-database-plugin-postgres" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/packaging/sql" TYPE FILE PERMISSIONS OWNER_READ GROUP_READ WORLD_READ FILES
    "/src/irods/plugins/database/src/icatDropSysTables.sql"
    "/src/irods/plugins/database/src/icatSysInserts.sql"
    "/src/irods/plugins/database/src/icatPurgeRecycleBin.sql"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-database-plugin-postgres" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/packaging/sql" TYPE FILE PERMISSIONS OWNER_READ GROUP_READ WORLD_READ RENAME "icatSysTables.sql" FILES "/src/irods/build_wdt/plugins/database/icatSysTables_postgres.sql")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-database-plugin-mysql" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/database/libmysql.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/database/libmysql.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/database/libmysql.so"
         RPATH "/opt/irods-externals/boost1.81.0-2/lib:/opt/irods-externals/nanodbc2.13.0-3/lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/database" TYPE MODULE FILES "/src/irods/build_wdt/plugins/database/libmysql.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/database/libmysql.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/database/libmysql.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/database/libmysql.so"
         OLD_RPATH "\$ORIGIN/../../server:\$ORIGIN/../../lib/core:/opt/irods-externals/boost1.81.0-2/lib:/opt/irods-externals/nanodbc2.13.0-3/lib:\$ORIGIN/../../lib:"
         NEW_RPATH "/opt/irods-externals/boost1.81.0-2/lib:/opt/irods-externals/nanodbc2.13.0-3/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/database/libmysql.so")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-database-plugin-mysql" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/packaging" TYPE FILE FILES "/src/irods/plugins/database/mysql/upgrade-3.3.xto4.0.0.sql")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-database-plugin-mysql" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/packaging" TYPE FILE PERMISSIONS OWNER_READ GROUP_READ WORLD_READ FILES "/src/irods/plugins/database/packaging/localhost_setup_mysql.input")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-database-plugin-mysql" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/packaging/sql" TYPE FILE PERMISSIONS OWNER_READ GROUP_READ WORLD_READ FILES
    "/src/irods/plugins/database/src/icatDropSysTables.sql"
    "/src/irods/plugins/database/src/icatSysInserts.sql"
    "/src/irods/plugins/database/src/icatPurgeRecycleBin.sql"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-database-plugin-mysql" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/packaging/sql" TYPE FILE PERMISSIONS OWNER_READ GROUP_READ WORLD_READ RENAME "icatSysTables.sql" FILES "/src/irods/build_wdt/plugins/database/icatSysTables_mysql.sql")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-database-plugin-oracle" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/database/liboracle.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/database/liboracle.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/database/liboracle.so"
         RPATH "/opt/irods-externals/boost1.81.0-2/lib:/opt/irods-externals/nanodbc2.13.0-3/lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/database" TYPE MODULE FILES "/src/irods/build_wdt/plugins/database/liboracle.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/database/liboracle.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/database/liboracle.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/database/liboracle.so"
         OLD_RPATH "\$ORIGIN/../../server:\$ORIGIN/../../lib/core:/opt/irods-externals/boost1.81.0-2/lib:/opt/irods-externals/nanodbc2.13.0-3/lib:\$ORIGIN/../../lib:"
         NEW_RPATH "/opt/irods-externals/boost1.81.0-2/lib:/opt/irods-externals/nanodbc2.13.0-3/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/opt/irods-externals/clang16.0.6-0/bin/llvm-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/usr/lib/irods/plugins/database/liboracle.so")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-database-plugin-oracle" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/packaging" TYPE FILE FILES "/src/irods/plugins/database/oracle/upgrade-3.3.xto4.0.0.sql")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-database-plugin-oracle" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/packaging" TYPE FILE PERMISSIONS OWNER_READ GROUP_READ WORLD_READ FILES "/src/irods/plugins/database/packaging/localhost_setup_oracle.input")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-database-plugin-oracle" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/packaging/sql" TYPE FILE PERMISSIONS OWNER_READ GROUP_READ WORLD_READ FILES
    "/src/irods/plugins/database/src/icatDropSysTables.sql"
    "/src/irods/plugins/database/src/icatSysInserts.sql"
    "/src/irods/plugins/database/src/icatPurgeRecycleBin.sql"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-database-plugin-oracle" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/packaging/sql" TYPE FILE PERMISSIONS OWNER_READ GROUP_READ WORLD_READ RENAME "icatSysTables.sql" FILES "/src/irods/build_wdt/plugins/database/icatSysTables_oracle.sql")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "irods-database-plugin-mysql" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/var/lib/irods/packaging/sql" TYPE FILE FILES "/src/irods/build_wdt/plugins/database/src/mysql_functions.sql")
endif()

