set(IRODS_TEST_TARGET irods_user_deprecated_warnings)

set(IRODS_TEST_SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/test_user_deprecated_warnings.cpp")

set(IRODS_TEST_INCLUDE_PATH
    "${CMAKE_SOURCE_DIR}/plugins/rule_engines/irods_rule_language/include"
)

set(IRODS_TEST_LINK_LIBRARIES irods_common
                               irods_server
                               fmt::fmt)
