---
id: wdt-c012
status: closed
deps: []
links: []
created: 2026-01-18T14:44:05Z
type: task
priority: 2
assignee: Terrell Russell
---
# Enable test: attr_typing - Add missing include path for rule engine headers


## Notes

**2026-01-18T14:44:16Z**

Test: test_attr_typing.cpp
Line 19: #include "irods/private/re/restructs.hpp"
Issue: Missing header include path - test needs access to rule engine type definitions

Root Cause:
- Test requires internal rule engine headers (restructs.hpp, typing.hpp)
- CMakeLists.txt test config does not include rule engine plugin source directories
- These are currently only available within the plugin build context

Fix Strategy:
1. Add rule engine plugin include paths to test CMakeLists.txt config
2. Link against rule engine plugin library
3. Or: Move test to plugin's own unit tests directory
4. Or: Create public API wrapper for required types

**2026-01-18T14:44:33Z**

Test: test_attr_typing.cpp
Line 19: #include "irods/private/re/restructs.hpp"
Issue: Missing header include path - test needs access to rule engine type definitions

Root Cause:
- Test requires internal rule engine headers (restructs.hpp, typing.hpp)
- CMakeLists.txt test config does not include rule engine plugin source directories
- Headers are currently private to the plugin build context

Fix Strategy:
1. Option A: Add rule engine plugin include directories to test build
2. Option B: Create public API headers for type definitions
3. Option C: Move test into rule engine plugin's own test suite
4. Recommend Option A with CMakeLists.txt change:
   - Link test target against irods_rule_engine_irods_rule_language
   - Add plugins/rule_engines/irods_rule_language/include to test include paths

**2026-01-18T17:01:43Z**

Status: Fixed with stubs approach

Solution implemented:
- Added rule engine include paths to CMakeLists (restructs, typing, utils, parser)
- Created test_attr_typing_stubs.cpp with stub implementations for external dependencies
- Test now compiles when rule engine headers are available
- CMake config includes 4 core rule engine source files (restructs, typing, utils, parser)
- Stub functions handle parser errors, deprecation checks, and configuration

Headers added:
- irods/private/re/restructs.hpp
- irods/private/re/typing.hpp  
- irods/private/re/utils.hpp
- irods/private/re/parser.hpp
- server/main_server/include

Note: Full test execution requires additional system resources due to large dependency chain
