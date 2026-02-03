set(IRODS_TEST_TARGET irods_run_as_admin)

set(IRODS_TEST_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/test_run_as_admin.cpp
                             ${CMAKE_IRODS_SOURCE_DIR}/server/re/src/irods_exec_rule_text_guard.cpp)

set(IRODS_TEST_INCLUDE_PATH ${IRODS_EXTERNALS_FULLPATH_BOOST}/include)

set(IRODS_TEST_LINK_LIBRARIES irods_common)
