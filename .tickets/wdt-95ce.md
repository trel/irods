---
id: wdt-95ce
status: closed
deps: []
links: []
created: 2026-01-18T14:44:12Z
type: task
priority: 2
assignee: Terrell Russell
---
# Enable test: system_error


## Notes

**2026-01-18T14:44:33Z**

Test: test_system_error.cpp
Issue: Test currently builds individually but fails in all-unit_tests target

Status: VERIFY - Test may actually build correctly now
Strategy:
1. Run full test suite to identify actual blocking issue
2. May be transitive dependency issue
3. May be linking order issue in CMakeLists.txt
4. Check if test actually works when built in isolation

**2026-02-01T23:31:01Z**

Test verified: builds successfully with irods_system_error target, passes all 27 assertions in 6 test cases. Test file, cmake config, and CMakeLists entry all present and correct.
