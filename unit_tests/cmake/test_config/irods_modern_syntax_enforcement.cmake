set(IRODS_TEST_TARGET irods_modern_syntax_enforcement)

set(IRODS_TEST_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/test_modern_syntax_enforcement.cpp)

set(IRODS_TEST_LINK_LIBRARIES irods_common
                              irods_server
                              fmt::fmt)
