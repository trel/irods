set(IRODS_TEST_TARGET irods_legacy_construct_warnings)

set(IRODS_TEST_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/test_legacy_construct_warnings.cpp
                             ${CMAKE_SOURCE_DIR}/plugins/rule_engines/irods_rule_language/src/deprecation.cpp)

set(IRODS_TEST_INCLUDE_PATH 
    ${IRODS_EXTERNALS_FULLPATH_BOOST}/include
    ${CMAKE_SOURCE_DIR}/plugins/rule_engines/irods_rule_language/include)

set(IRODS_TEST_LINK_LIBRARIES irods_common
                               irods_client
                               irods_plugin_dependencies
                               Catch2::Catch2WithMain
                               fmt::fmt)
