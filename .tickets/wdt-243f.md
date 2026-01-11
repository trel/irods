---
id: wdt-243f
status: closed
deps: []
links: []
created: 2026-01-18T14:44:11Z
type: task
priority: 2
assignee: Terrell Russell
---
# Enable test: legacy_construct_warnings


## Notes

**2026-01-18T14:44:33Z**

Test: test_legacy_construct_warnings.cpp
Line 8: #include <gtest/gtest.h>
Issue: Same as deprecation_warnings - GoogleTest not available

Root Cause:
- Test framework mismatch (GoogleTest vs Catch2)
- Project standardized on Catch2 v3.4.0

Fix Strategy:
1. Migrate from GoogleTest to Catch2 framework
2. Files to update: test_legacy_construct_warnings.cpp
3. Files to update: unit_tests/cmake/test_config/irods_legacy_construct_warnings.cmake
4. Change test linking from GTest libraries to Catch2::Catch2WithMain

**2026-01-18T17:29:26Z**

Status: Fixed - migrated to Catch2, [20] tests pass, 77 assertions
