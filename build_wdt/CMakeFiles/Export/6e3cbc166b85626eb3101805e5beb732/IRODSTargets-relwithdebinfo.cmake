#----------------------------------------------------------------
# Generated CMake target import file for configuration "RelWithDebInfo".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "irods_plugin_dependencies" for configuration "RelWithDebInfo"
set_property(TARGET irods_plugin_dependencies APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)
set_target_properties(irods_plugin_dependencies PROPERTIES
  IMPORTED_LOCATION_RELWITHDEBINFO "${_IMPORT_PREFIX}/usr/lib/libirods_plugin_dependencies.so.5.0.90"
  IMPORTED_SONAME_RELWITHDEBINFO "libirods_plugin_dependencies.so.5.0.90"
  )

list(APPEND _cmake_import_check_targets irods_plugin_dependencies )
list(APPEND _cmake_import_check_files_for_irods_plugin_dependencies "${_IMPORT_PREFIX}/usr/lib/libirods_plugin_dependencies.so.5.0.90" )

# Import target "irods_common" for configuration "RelWithDebInfo"
set_property(TARGET irods_common APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)
set_target_properties(irods_common PROPERTIES
  IMPORTED_LOCATION_RELWITHDEBINFO "${_IMPORT_PREFIX}/usr/lib/libirods_common.so.5.0.90"
  IMPORTED_SONAME_RELWITHDEBINFO "libirods_common.so.5.0.90"
  )

list(APPEND _cmake_import_check_targets irods_common )
list(APPEND _cmake_import_check_files_for_irods_common "${_IMPORT_PREFIX}/usr/lib/libirods_common.so.5.0.90" )

# Import target "irods_client" for configuration "RelWithDebInfo"
set_property(TARGET irods_client APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)
set_target_properties(irods_client PROPERTIES
  IMPORTED_LOCATION_RELWITHDEBINFO "${_IMPORT_PREFIX}/usr/lib/libirods_client.so.5.0.90"
  IMPORTED_SONAME_RELWITHDEBINFO "libirods_client.so.5.0.90"
  )

list(APPEND _cmake_import_check_targets irods_client )
list(APPEND _cmake_import_check_files_for_irods_client "${_IMPORT_PREFIX}/usr/lib/libirods_client.so.5.0.90" )

# Import target "RodsAPIs" for configuration "RelWithDebInfo"
set_property(TARGET RodsAPIs APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)
set_target_properties(RodsAPIs PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELWITHDEBINFO "CXX"
  IMPORTED_LOCATION_RELWITHDEBINFO "${_IMPORT_PREFIX}/usr/lib/libRodsAPIs.a"
  )

list(APPEND _cmake_import_check_targets RodsAPIs )
list(APPEND _cmake_import_check_files_for_RodsAPIs "${_IMPORT_PREFIX}/usr/lib/libRodsAPIs.a" )

# Import target "irods_server" for configuration "RelWithDebInfo"
set_property(TARGET irods_server APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)
set_target_properties(irods_server PROPERTIES
  IMPORTED_LOCATION_RELWITHDEBINFO "${_IMPORT_PREFIX}/usr/lib/libirods_server.so.5.0.90"
  IMPORTED_SONAME_RELWITHDEBINFO "libirods_server.so.5.0.90"
  )

list(APPEND _cmake_import_check_targets irods_server )
list(APPEND _cmake_import_check_files_for_irods_server "${_IMPORT_PREFIX}/usr/lib/libirods_server.so.5.0.90" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
