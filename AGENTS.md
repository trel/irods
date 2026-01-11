use 30 cores for building

do not write plans in markdown files.  use the ticket system instead.

# Agent Instructions for iRODS Development

This document provides guidance for AI agents working on the iRODS codebase, particularly for the rule language improvement project.

## Ticket System Setup

The project uses **[wedow/ticket](https://github.com/wedow/ticket)** for task tracking. The ticket command is located at:

```bash
./ticket <command> [args]
```

### Quick Ticket Commands

List all open tickets:
```bash
./ticket ls
./ticket ls --status=open
```

Show a specific ticket (with full notes):
```bash
./ticket show i-eb98
```

View dependency tree for epic:
```bash
./ticket dep tree i-c56b --full
```

Find blocked tickets (with unresolved dependencies):
```bash
./ticket blocked
```

Find ready tickets (no blockers):
```bash
./ticket ready
```

Start work on a ticket:
```bash
./ticket start i-eb98
```

Add progress notes:
```bash
./ticket add-note i-eb98 "Completed parser.cpp error handling, 3 sites remaining"
```

Close a completed ticket:
```bash
./ticket close i-eb98
```

Query tickets as JSON:
```bash
./ticket query '.[] | select(.status == "in_progress")'
```

## Epic and Tickets

### Epic: i-c56b - iRODS Rule Language: Fix Core Weaknesses

The main epic tracking all rule language improvements. Five sub-tasks:

1. **i-eb98** (Priority 0) - Complete TODO error handling (19 sites)
   - Status: Open
   - Effort: 15-20 hours
   - Files: parser.cpp, typing.cpp, conversion.cpp, etc.

2. **i-7b2d** (Priority 0) - Type query nodes instead of T_DYNAMIC
   - Status: Open
   - Effort: 20-30 hours
   - Files: typing.cpp, reVariableMap.hpp

3. **i-c458** (Priority 1) - Refactor backward-compat parsing to separate module
   - Status: Open
   - Effort: 12-18 hours
   - Files: parser.cpp, parser.hpp, arithmetics.cpp

4. **i-339c** (Priority 1) - Audit and fix memory leaks (3-4 sites)
   - Status: Open
   - Effort: 8-12 hours
   - Files: conversion.cpp, functions.cpp, parser.cpp

5. **i-b269** (Priority 1) - Improve parser error recovery
   - Status: Open
   - Effort: 15-20 hours
   - Files: parser.hpp, parser.cpp, restructs.hpp

## File Locations

Key files for each work area:

### Parser & Lexer
- `/src/irods/plugins/rule_engines/irods_rule_language/src/parser.cpp` (3,568 lines)
- `/src/irods/plugins/rule_engines/irods_rule_language/include/irods/private/re/parser.hpp` (435 lines)

### Type System
- `/src/irods/plugins/rule_engines/irods_rule_language/src/typing.cpp` (1,228 lines)
- `/src/irods/plugins/rule_engines/irods_rule_language/include/irods/private/re/typing.hpp`

### Runtime & Execution
- `/src/irods/plugins/rule_engines/irods_rule_language/src/arithmetics.cpp`
- `/src/irods/plugins/rule_engines/irods_rule_language/src/rules.cpp` (711 lines)
- `/src/irods/plugins/rule_engines/irods_rule_language/src/functions.cpp`

### Memory & Structures
- `/src/irods/plugins/rule_engines/irods_rule_language/include/irods/private/re/restructs.hpp` (382 lines)
- `/src/irods/plugins/rule_engines/irods_rule_language/src/restructs.cpp`

### Type & Variable Maps
- `/src/irods/plugins/rule_engines/irods_rule_language/include/irods/private/re/reVariableMap.gen.hpp`
- `/src/irods/plugins/rule_engines/irods_rule_language/include/irods/private/re/reVariableMap.hpp`

## Project Structure

```
/src/irods/
├── .tickets/                    # Ticket data directory
├── PLAN.md                      # Detailed project plan
├── AGENTS.md                    # This file
├── ticket                       # Ticket command script
└── plugins/rule_engines/irods_rule_language/
    ├── include/irods/private/re/
    │   ├── parser.hpp           # Parser macros and declarations
    │   ├── restructs.hpp        # AST node definitions
    │   ├── typing.hpp           # Type inference
    │   └── ...
    └── src/
        ├── parser.cpp           # Main parser implementation
        ├── typing.cpp           # Type checking
        ├── arithmetics.cpp      # Expression evaluation
        └── ...
```

## Code Patterns

### Error Handling Pattern
The iRODS error system uses `rError_t` for error chains:

```c
// Old pattern (to be replaced):
/* todo error handling */

// New pattern:
char errbuf[ERR_MSG_LEN];
generateErrMsg("message", node->expr, node->base, errbuf);
addRErrorMsg(errmsg, RE_TYPE_ERROR, errbuf);
```

### Memory Management Pattern
Prefer Region-based allocation:

```c
// Region allocation (preferred):
Node *n = (Node*)region_alloc(r, sizeof(Node));

// Malloc (legacy, use only for short-lived objects):
ParserContext *pc = (ParserContext*)malloc(sizeof(ParserContext));
// ... use pc ...
free(pc);
```

### Parser Macros
The parser uses heavy macro patterns:

```c
PARSER_FUNC_BEGIN(RuleName)
    NEXT_TOKEN_BASIC;
    // ... parsing logic ...
    BUILD_NODE(N_RULE_NAME, "rule-name", FPOS, 2, 2);
PARSER_FUNC_END(RuleName)
```

## Testing

### Unit Tests
Located in `/src/irods/unit_tests/`

Run tests relevant to rule language:
```bash
cd /src/irods
cmake --build . -- test
```

### Integration Tests
Located in `/src/irods/test/`

### Creating New Tests
1. Add test to appropriate file in unit_tests/
2. Link against rule engine plugin
3. Use rError_t for error capture
4. Document expected behavior in test comments

## Debugging Tips

### Parser Issues
- Enable `#define DEBUG_VERBOSE` in parser.cpp to see token trace
- Check `context->errloc` for error location
- Use `printTree(node, indent)` to visualize AST

### Type Checking Issues
- Review `typeExpression3()` function flow
- Check `dereference()` for variable substitution
- Use `typeName_ExprType()` to print type for debugging

### Memory Issues
Run under valgrind:
```bash
valgrind --leak-check=full --show-leak-kinds=all \
    /path/to/test 2>&1 | grep -E "ERROR|definitely"
```

## Workflow

1. **Pick a ticket**: Choose from `./ticket ready` (no blockers)
2. **Start work**: `./ticket start <id>`
3. **Make changes**: Edit source files in plugins/rule_engines/irods_rule_language/
4. **Test changes**: Run relevant unit/integration tests
5. **Commit progress**: `git commit -m "Ticket <id>: description"`
6. **Add notes**: `./ticket add-note <id> "Progress update"`
7. **Close when done**: `./ticket close <id>`

## Performance Baseline

Before making changes, establish baseline:
```bash
# Parse large rule set and measure time
time ./ticket_parser large_ruleset.re
```

After changes, verify no regression:
```bash
time ./ticket_parser large_ruleset.re  # Should be ~same
```

Document performance changes in ticket notes.

## Documentation

Update documentation for:
- New error codes (rodsErrorTable.h)
- Type system changes (typing.hpp comments)
- Parser error recovery (parser.hpp)
- Deprecated syntax (PLAN.md deprecation section)

## Build Optimization

### Using ccache for Faster Rebuilds

The iRODS build can be significantly accelerated using **ccache** (compiler cache), especially when doing iterative development with frequent rebuilds. ccache caches object files from previous compilations and reuses them when the source hasn't changed.

#### Installation

Install ccache on Ubuntu/Debian:
```bash
apt-get install -y ccache
```

For other systems, see https://ccache.dev/install.html

#### Enabling ccache with CMake

Configure CMake to use ccache as the compiler launcher:

**Option 1: Via environment variables (one-time)**
```bash
cd /src/irods
rm -rf build
CMAKE_C_COMPILER_LAUNCHER=ccache CMAKE_CXX_COMPILER_LAUNCHER=ccache cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j 30
```

**Option 2: Via CMake command line**
```bash
cmake -B build \
  -DCMAKE_C_COMPILER_LAUNCHER=ccache \
  -DCMAKE_CXX_COMPILER_LAUNCHER=ccache \
  -DCMAKE_BUILD_TYPE=Release
cmake --build build -j 30
```

**Option 3: Shell alias (persistent, recommended)**

Add to your `~/.bashrc`:
```bash
alias irods-cmake='CMAKE_C_COMPILER_LAUNCHER=ccache CMAKE_CXX_COMPILER_LAUNCHER=ccache cmake'
```

Then use:
```bash
irods-cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j 30
```

#### Configuration & Tuning

Check ccache status and statistics:
```bash
ccache -s                    # Show cache stats
ccache --version             # Show version and features
ccache -c                    # Clear cache
```

Adjust cache size (default 5 GB, useful for large projects):
```bash
ccache -M 10G                # Set max cache size to 10 GB
ccache -M 0                  # Unlimited cache
```

For optimal performance with iRODS, recommended settings:
```bash
ccache -M 5G                 # 5 GB is usually sufficient
ccache --set-config=compression=true  # Compress cached files
```

#### Performance Observations

With ccache enabled in the iRODS build:
- **First build (cold cache)**: ~same speed as without ccache (slight overhead)
- **Subsequent rebuilds**: 50-70% faster when only a few files changed
- **Full rebuild from clean**: ~5-10% overhead due to ccache checks

Expected metrics with ccache active:
```
ccache -s
Cache statistics after rebuild:
  Cacheable calls:    758 / 758 (100.0%)
  Hits:              ~50-100 / 758 (varies)
  Misses:            ~650-700 / 758
  Cache size (GiB):   0.1-0.2 / 5.0
```

#### Verification

To verify ccache is being used:

1. **Check that CMAKE_C_COMPILER_LAUNCHER is set:**
   ```bash
   cmake --build build -- VERBOSE=1 2>&1 | grep -E "ccache|/bin/cc|clang"
   ```
   
   Should show `ccache` in the command line if enabled.

2. **Monitor cache hits:**
   ```bash
   ccache -z                 # Reset stats
   cmake --build build -j 30 # Rebuild
   ccache -s                 # View stats
   ```

3. **Verify cache directory:**
   ```bash
   du -h ~/.cache/ccache/    # Check cache size on disk
   ```

#### Troubleshooting

**ccache not being used:**
- Verify it's installed: `which ccache`
- Check CMake was configured with launcher: `grep CMAKE_C_COMPILER_LAUNCHER build/CMakeCache.txt`
- Try clearing and reconfiguring: `rm -rf build && irods-cmake -B build ...`

**Cache not producing hits:**
- Check that the same source files are being compiled: `ccache -s` should show >0 hits after second build
- If preprocessor output changes (timestamps, paths), hits are missed
- Use `ccache -d` to enable debug logging

**Performance regression:**
- Profile without ccache: `CMAKE_C_COMPILER_LAUNCHER= cmake --build build -j 30`
- ccache should not slow down builds after warmup; if it does, check I/O performance or disable: `ccache --disable`

## Contact & References

- **iRODS Documentation**: https://docs.irods.org
- **iRODS GitHub**: https://github.com/irods/irods
- **Ticket System**: https://github.com/wedow/ticket
- **Plan Document**: PLAN.md (this repository)
- **ccache**: https://ccache.dev
