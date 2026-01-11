set(IRODS_TEST_TARGET irods_deprecation_warnings)

set(IRODS_TEST_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/test_deprecation_warnings.cpp)

set(IRODS_TEST_INCLUDE_PATH ${IRODS_EXTERNALS_FULLPATH_BOOST}/include)

set(IRODS_TEST_LINK_LIBRARIES irods_rule_engine_plugin_irods_rule_language
                               irods_common
                               irods_client
                               irods_plugin_dependencies)
set(IRODS_TEST_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/test_deprecation_warnings.cpp
                             ${CMAKE_SOURCE_DIR}/plugins/rule_engines/irods_rule_language/src/deprecation.cpp)

set(IRODS_TEST_INCLUDE_PATH
    ${IRODS_EXTERNALS_FULLPATH_BOOST}/include
    ${CMAKE_SOURCE_DIR}/plugins/rule_engines/irods_rule_language/include)

set(IRODS_TEST_LINK_LIBRARIES irods_common
                               fmt::fmt)
