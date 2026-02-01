#!/bin/bash
cd /src/irods/build/unit_tests || exit 1
total=0; passed=0; failed=0; failed_tests=""
for test_exe in irods_*; do
  [ -x "$test_exe" ] || continue
  ((total++))
  # Run with 5-second timeout per test
  if timeout 5s ./"$test_exe" >/dev/null 2>&1; then
    ((passed++)); echo "✓ $test_exe"
  else
    ((failed++)); echo "✗ $test_exe"; failed_tests="$failed_tests\n  - $test_exe"
  fi
done
echo ""; echo "Total: $total | Passed: $passed | Failed: $failed"
[ $failed -eq 0 ]
