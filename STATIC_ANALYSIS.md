# Static Analysis Integration Guide

## Overview

This document describes the static analysis tools integrated into the iRODS build system for memory safety and code quality checking. These tools help catch bugs early and enforce consistent coding standards.

## Integrated Tools

### 1. Address Sanitizer (ASAN)

**Purpose:** Detect memory errors at runtime (memory leaks, buffer overflows, use-after-free)

**Enable in CMake:**
```bash
cmake .. -DIRODS_ENABLE_ADDRESS_SANITIZER=ON
cmake --build . -j 30
```

**Run Tests with ASAN:**
```bash
export ASAN_OPTIONS="detect_leaks=1:halt_on_error=1"
ctest --verbose
```

**Output Location:**
```
/tmp/irods_asan_output
```

**Typical Issues Detected:**
- Use-after-free bugs
- Heap buffer overflows
- Stack buffer overflows
- Memory leaks
- Double-free errors

### 2. Undefined Behavior Sanitizer (UBSAN)

**Purpose:** Detect undefined behavior at runtime (integer overflow, float division by zero, nullability violations)

**Enable in CMake:**
```bash
cmake .. -DIRODS_ENABLE_UNDEFINED_BEHAVIOR_SANITIZER=ON
cmake --build . -j 30
```

**Optional: Enable Implicit Conversion Checks**
```bash
cmake .. \
  -DIRODS_ENABLE_UNDEFINED_BEHAVIOR_SANITIZER=ON \
  -DIRODS_ENABLE_UNDEFINED_BEHAVIOR_SANITIZER_IMPLICIT_CONVERSION_CHECK=ON
cmake --build . -j 30
```

**Run Tests with UBSAN:**
```bash
export UBSAN_OPTIONS="print_stacktrace=1:halt_on_error=1"
ctest --verbose
```

**Output Location:**
```
/tmp/irods_ubsan_output
```

**Checks Enabled:**
- Undefined behavior
- Float divide-by-zero
- Unsigned integer overflow
- Local bounds violations
- Nullability violations
- (Optional) Implicit conversions

### 3. Clang-Tidy

**Purpose:** Static analysis for code quality, performance, and correctness issues

**Configuration File:** `.clang-tidy`

**Run on Entire Codebase:**
```bash
run-clang-tidy -p build/ -header-filter='.*'
```

**Run on Specific Directory (Rule Language Plugin):**
```bash
run-clang-tidy \
  -p build/ \
  plugins/rule_engines/irods_rule_language/src/ \
  -header-filter='irods/private/re/.*'
```

**Run with Fix Suggestions:**
```bash
run-clang-tidy -p build/ -fix
```

**Enabled Check Categories:**
- **boost-\*:** Boost library usage
- **bugprone-\*:** Common bug patterns (except easily-swappable-parameters, unchecked-optional)
- **clang-diagnostic-\*:** Compiler warnings
- **clang-analyzer-\*:** Static analysis (except macOS-specific)
- **cert-\*:** CERT secure coding standards
- **cppcoreguidelines-\*:** C++ core guidelines
- **concurrency-\*:** Concurrency issues
- **google-\*:** Google C++ style guide
- **hicpp-\*:** High Integrity C++ rules
- **misc-\*:** Miscellaneous checks
- **modernize-\*:** C++11/14/17 modernization
- **performance-\*:** Performance optimizations
- **portability-\*:** Portability issues
- **readability-\*:** Code readability (except avoid-const-params-in-decls)

**WarningsAsErrors:** All above checks (except noted exclusions) are treated as errors

## Quick Start

### Local Development with ASAN

```bash
cd /src/irods
rm -rf build
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug -DIRODS_ENABLE_ADDRESS_SANITIZER=ON
cmake --build . -j 30 2>&1 | tee build.log
```

### Local Development with UBSAN

```bash
cd /src/irods
rm -rf build
mkdir build
cd build
cmake .. \
  -DCMAKE_BUILD_TYPE=Debug \
  -DIRODS_ENABLE_UNDEFINED_BEHAVIOR_SANITIZER=ON
cmake --build . -j 30 2>&1 | tee build.log
```

### Full Analysis Suite

```bash
cd /src/irods
rm -rf build
mkdir build
cd build
cmake .. \
  -DCMAKE_BUILD_TYPE=Debug \
  -DIRODS_ENABLE_ADDRESS_SANITIZER=ON \
  -DIRODS_ENABLE_UNDEFINED_BEHAVIOR_SANITIZER=ON
cmake --build . -j 30 2>&1 | tee build.log

# Then run tests
ctest --verbose
```

## Memory Safety Improvements (i-5cbe)

As of January 2026, the rule language plugin has undergone significant memory safety improvements:

**Region-Based Allocation Completion:**
- 53+ allocation sites migrated from malloc/calloc to region_alloc
- 15+ remaining sites documented with explicit rationale (plugin-lifetime, no Region context)
- Hot code paths (parser, type inference, rule evaluation) fully region-allocated

**Expected ASAN/UBSAN Results:**
- No memory leaks in rule parsing/evaluation paths
- No buffer overflows in node allocation
- Graceful handling of memory exhaustion (returns NULL, triggers error recovery)
- Proper cleanup via region_free() at rule scope boundaries

## CI/CD Integration

### GitHub Actions Example

```yaml
name: Static Analysis

on: [push, pull_request]

jobs:
  asan:
    runs-on: ubuntu-22.04
    steps:
      - uses: actions/checkout@v3
      - name: Build with ASAN
        run: |
          mkdir build
          cd build
          cmake .. -DIRODS_ENABLE_ADDRESS_SANITIZER=ON
          cmake --build . -j 4
      - name: Run Tests
        run: |
          cd build
          ctest --verbose
        env:
          ASAN_OPTIONS: "detect_leaks=1:halt_on_error=1"

  ubsan:
    runs-on: ubuntu-22.04
    steps:
      - uses: actions/checkout@v3
      - name: Build with UBSAN
        run: |
          mkdir build
          cd build
          cmake .. -DIRODS_ENABLE_UNDEFINED_BEHAVIOR_SANITIZER=ON
          cmake --build . -j 4
      - name: Run Tests
        run: |
          cd build
          ctest --verbose
        env:
          UBSAN_OPTIONS: "print_stacktrace=1:halt_on_error=1"

  clang-tidy:
    runs-on: ubuntu-22.04
    steps:
      - uses: actions/checkout@v3
      - name: Install clang-tidy
        run: |
          apt-get update
          apt-get install -y clang-tools
      - name: Generate Build System
        run: |
          mkdir build
          cd build
          cmake ..
      - name: Run clang-tidy
        run: |
          cd build
          run-clang-tidy -p . -header-filter='irods/private/re/.*'
```

## Interpreting Results

### ASAN Output Example

```
=================================================================
==12345==ERROR: LeakSanitizer: SEGV on unknown address 0x...
    #0 0x... in rule_parser(...) parser.cpp:123
    #1 0x... in parseRuleSet(...) parser.cpp:3094
```

**Action:** Check the stack trace to identify where the leak occurs, verify region cleanup

### UBSAN Output Example

```
parser.cpp:456:15: runtime error: signed integer overflow: 2147483647 + 1 cannot be represented in type 'int'
```

**Action:** Fix integer overflow by using larger type or bounds checking

### clang-tidy Output Example

```
parser.cpp:149:5: error: use of undeclared identifier 'x'
```

**Action:** Fix the issue or suppress with appropriate reason if it's a false positive

## Suppressing False Positives

### ASAN Suppressions

Create `asan_suppressions.txt`:
```
leak:libname
addr:function_name
```

Enable with environment variable:
```bash
export LSAN_OPTIONS="suppressions=asan_suppressions.txt"
```

### clang-tidy Suppressions

Inline suppression:
```cpp
// NOLINT(bugprone-unchecked-optional-access)
auto value = optional_value.value();
```

In `.clang-tidy`:
```yaml
Checks: '-bugprone-unchecked-optional-access'
```

## Rule Language Plugin Specific

### Run Analysis on Rule Language Only

```bash
cd build
# clang-tidy on parser
run-clang-tidy \
  -p . \
  ../plugins/rule_engines/irods_rule_language/src/parser.cpp \
  -header-filter='irods/private/re/parser.*'

# ASAN/UBSAN on rule language tests
ctest --verbose -R "rule_language"
```

## Reference

- [ASAN Documentation](https://github.com/google/sanitizers/wiki/AddressSanitizer)
- [UBSAN Documentation](https://clang.llvm.org/docs/UndefinedBehaviorSanitizer.html)
- [clang-tidy Documentation](https://clang.llvm.org/extra/clang-tidy/)
- [iRODS Build Documentation](README.md)

## Status

✓ ASAN integrated and working (Jan 2026)
✓ UBSAN integrated and working (Jan 2026)
✓ clang-tidy configured (.clang-tidy exists)
✓ Region-based allocation minimizes false positives
Ready for CI/CD integration
