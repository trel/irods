set(_IRODS_UNIT_TESTS_BUILD_DOCSTR "Build unit tests")
set(_IRODS_UNIT_TESTS_ENABLE_ALL_DOCSTR "Enables all unit tests")
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(_IRODS_UNIT_TESTS_ENABLE_ALL_DEFAULT ON)
else()
  set(_IRODS_UNIT_TESTS_ENABLE_ALL_DEFAULT OFF)
endif()

set(IRODS_UNIT_TESTS_BUILD NO CACHE BOOL "${_IRODS_UNIT_TESTS_BUILD_DOCSTR}")
set(IRODS_UNIT_TESTS_REPORTING_STYLE "junit" CACHE STRING "The style of output used for unit test reporting [console, compact, junit, xml]")
set(IRODS_UNIT_TESTS_REPORT_DIR "${CMAKE_CURRENT_BINARY_DIR}/reports" CACHE STRING "The directory of the unit test reports")
set(IRODS_UNIT_TESTS_REPORT_FILENAME_PREFIX "" CACHE STRING "The filename prefix of the unit test report")
set(IRODS_UNIT_TESTS_REPORT_FILENAME_SUFFIX ".xml" CACHE STRING "The filename suffix of the unit test report")
set(IRODS_UNIT_TESTS_ENABLE_ALL ${_IRODS_UNIT_TESTS_ENABLE_ALL_DEFAULT} CACHE BOOL "${_IRODS_UNIT_TESTS_ENABLE_ALL_DOCSTR}")

if (IRODS_ENABLE_ALL_TESTS)
  set(IRODS_UNIT_TESTS_BUILD YES CACHE BOOL "${_IRODS_UNIT_TESTS_BUILD_DOCSTR}" FORCE)
  set(IRODS_UNIT_TESTS_ENABLE_ALL YES CACHE BOOL "${_IRODS_UNIT_TESTS_ENABLE_ALL_DOCSTR}" FORCE)
endif()

if (DEFINED IRODS_UNIT_TESTS_BUILD_WITH_INSTALL_RPATH_INIT)
  set(DEFAULT_UNIT_TESTS_BUILD_WITH_INSTALL_RPATH ${IRODS_UNIT_TESTS_BUILD_WITH_INSTALL_RPATH_INIT})
else()
  set(DEFAULT_UNIT_TESTS_BUILD_WITH_INSTALL_RPATH NO)
endif()
set(IRODS_UNIT_TESTS_BUILD_WITH_INSTALL_RPATH "${DEFAULT_UNIT_TESTS_BUILD_WITH_INSTALL_RPATH}" CACHE BOOL "Build unit tests with install RPATH/RUNPATH")

if (NOT IRODS_UNIT_TESTS_BUILD)
  return()
endif()

find_package(Catch2 3.4)
if (NOT Catch2_FOUND)
  find_package(Catch2 2.13.2 REQUIRED)
endif()

include(Catch)

# create phony target to build all unit tests
add_custom_target(all-unit_tests)

# Enable CTest support.
enable_testing()

# Include helper functions and other utilities.
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/utils.cmake")

# Each file in the ./cmake/test_config directory defines variables for a specific test.
# New tests should be added to this list.
set(
  IRODS_UNIT_TESTS
  access_time_queue
  attr_typing
  capped_memory_resource
  client_server_negotiation
  data_object_proxy
  delay_hints_parser
  deprecation_warnings
  environment_variables
  file_object
  fixed_buffer_resource
  fully_qualified_username
  generate_random_alphanumeric_string
  getRodsEnv
  hashers
  hierarchy_parser
  host_list_context_string
  json_events
  key_value_proxy
  legacy_compat
  legacy_construct_warnings
  lifetime_manager
  linked_list_iterator
  logical_paths_and_special_characters
  macros_templates
  modern_syntax_enforcement
  optional_types
  packstruct
  process_stash
  query_type_inference
  rcConnect
  re_serialization
  replica_access_table
  replica_state_table
  rerror_stack
  scoped_privileged_client
  server_properties
  server_utilities
  shared_memory_object
  system_error
  union_constraint_solving
  union_types
  user_deprecated_warnings
  version
  with_durability
)

foreach(test IN LISTS IRODS_UNIT_TESTS)
  unset_irods_test_variables()

  include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/test_config/irods_${test}.cmake")
  add_executable(
    ${IRODS_TEST_TARGET}
    ${IRODS_TEST_SOURCE_FILES}
    "${CMAKE_CURRENT_SOURCE_DIR}/src/irods_error_enum_matcher.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/unit_test_utils.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/test_config/irods_${test}.cmake"
  )
  if (Catch2_VERSION VERSION_LESS "3.0.0")
    target_sources(
      ${IRODS_TEST_TARGET}
      PRIVATE
      "${CMAKE_CURRENT_SOURCE_DIR}/src/catch2_compat_include/catch2/catch_all.hpp"
    )
    target_link_libraries(
      ${IRODS_TEST_TARGET}
      PRIVATE
      Catch2::Catch2
    )
    target_include_directories(
      ${IRODS_TEST_TARGET}
      PRIVATE
      "${CMAKE_CURRENT_SOURCE_DIR}/src/catch2_compat_include"
    )
    if (NOT IRODS_TEST_PROVIDES_MAIN)
      target_sources(
        ${IRODS_TEST_TARGET}
        PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp"
      )
    endif()
  else()
    if (NOT IRODS_TEST_PROVIDES_MAIN)
      target_link_libraries(
        ${IRODS_TEST_TARGET}
        PRIVATE
        Catch2::Catch2WithMain
      )
    else()
      target_link_libraries(
        ${IRODS_TEST_TARGET}
        PRIVATE
        Catch2::Catch2
      )
    endif()
  endif()
  target_compile_definitions(${IRODS_TEST_TARGET} PRIVATE ${IRODS_COMPILE_DEFINITIONS_PRIVATE})
  if (DEFINED IRODS_TEST_LINK_OBJLIBRARIES)
    target_link_objects(${IRODS_TEST_TARGET} PRIVATE ${IRODS_TEST_LINK_OBJLIBRARIES})
  endif()
  target_link_libraries(${IRODS_TEST_TARGET} PRIVATE ${IRODS_TEST_LINK_LIBRARIES})
  if (DEFINED IRODS_TEST_INCLUDE_PATH)
    target_include_directories(${IRODS_TEST_TARGET} PRIVATE ${IRODS_TEST_INCLUDE_PATH})
  endif()
  set_property(TARGET ${IRODS_TEST_TARGET} PROPERTY BUILD_WITH_INSTALL_RPATH ${IRODS_UNIT_TESTS_BUILD_WITH_INSTALL_RPATH})
  if (IRODS_UNIT_TESTS_ENABLE_ALL)
    target_compile_definitions(${IRODS_TEST_TARGET} PRIVATE IRODS_ENABLE_ALL_UNIT_TESTS)
  endif()

  add_dependencies(all-unit_tests ${IRODS_TEST_TARGET})
  install(
    TARGETS
    ${IRODS_TEST_TARGET}
    RUNTIME
    DESTINATION "${IRODS_HOME_DIRECTORY}/unit_tests"
    COMPONENT ${IRODS_PACKAGE_COMPONENT_SERVER_NAME}
  )

  # Make the new test available to CTest.
  catch_discover_tests(
    ${IRODS_TEST_TARGET}
    REPORTER "${IRODS_UNIT_TESTS_REPORTING_STYLE}"
    OUTPUT_DIR "${IRODS_UNIT_TESTS_REPORT_DIR}"
    OUTPUT_PREFIX "${IRODS_UNIT_TESTS_REPORT_FILENAME_PREFIX}"
    OUTPUT_SUFFIX "${IRODS_UNIT_TESTS_REPORT_FILENAME_SUFFIX}"
  )
endforeach()

install(
  FILES
  "${CMAKE_CURRENT_SOURCE_DIR}/unit_tests_list.json"
  DESTINATION "${IRODS_HOME_DIRECTORY}/unit_tests"
  COMPONENT ${IRODS_PACKAGE_COMPONENT_SERVER_NAME}
)

install(
  FILES
  "${CMAKE_CURRENT_SOURCE_DIR}/genquery1_backwards_compatibility_queries.txt"
  DESTINATION "${IRODS_HOME_DIRECTORY}/unit_tests"
  COMPONENT ${IRODS_PACKAGE_COMPONENT_SERVER_NAME}
)
