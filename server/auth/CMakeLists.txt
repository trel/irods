if (NOT PAM_LIBRARY)
  find_library(PAM_LIBRARY pam)
  if (PAM_LIBRARY)
    message(STATUS "Found pam: ${PAM_LIBRARY}")
  else()
    message(FATAL_ERROR "pam library not found")
  endif()
endif()

add_executable(
  irodsPamAuthCheck
  "${CMAKE_CURRENT_SOURCE_DIR}/src/irodsPamAuthCheck.cpp"
)
target_link_libraries(
  irodsPamAuthCheck
  PRIVATE
  "${PAM_LIBRARY}"
)

# ASan's crypt_r interceptor resolves the real function at process startup.
# PAM loads pam_unix (and therefore libcrypt) only during authentication, so
# make libcrypt available before the interceptor is initialized.
if (IRODS_ENABLE_ADDRESS_SANITIZER)
  find_library(CRYPT_LIBRARY crypt REQUIRED)
  target_link_options(irodsPamAuthCheck PRIVATE -Wl,--no-as-needed)
  target_link_libraries(irodsPamAuthCheck PRIVATE "${CRYPT_LIBRARY}")
endif()

add_dependencies(all-server irodsPamAuthCheck)
install(
  TARGETS
  irodsPamAuthCheck
  RUNTIME
  DESTINATION "${CMAKE_INSTALL_SBINDIR}"
  COMPONENT ${IRODS_PACKAGE_COMPONENT_SERVER_NAME}
  PERMISSIONS SETUID OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)
