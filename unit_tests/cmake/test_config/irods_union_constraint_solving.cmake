set(IRODS_TEST_TARGET irods_union_constraint_solving)

set(IRODS_TEST_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/test_union_constraint_solving.cpp
                            ${CMAKE_CURRENT_SOURCE_DIR}/src/test_union_constraint_solving_stubs.cpp
                            ${CMAKE_IRODS_SOURCE_DIR}/plugins/rule_engines/irods_rule_language/src/restructs.cpp
                            ${CMAKE_IRODS_SOURCE_DIR}/plugins/rule_engines/irods_rule_language/src/typing.cpp
                            ${CMAKE_IRODS_SOURCE_DIR}/plugins/rule_engines/irods_rule_language/src/utils.cpp)

set(IRODS_TEST_INCLUDE_PATH ${IRODS_EXTERNALS_FULLPATH_BOOST}/include
                            ${CMAKE_IRODS_SOURCE_DIR}/plugins/rule_engines/irods_rule_language/include
                            ${CMAKE_IRODS_SOURCE_DIR}/server/main_server/include)

set(IRODS_TEST_LINK_LIBRARIES irods_common
                              irods_client
                              irods_plugin_dependencies
                              ${IRODS_EXTERNALS_FULLPATH_BOOST}/lib/libboost_filesystem.so
                              ${IRODS_EXTERNALS_FULLPATH_BOOST}/lib/libboost_system.so
                              ${IRODS_EXTERNALS_FULLPATH_BOOST}/lib/libboost_regex.so
                              fmt::fmt)
