# iRODS Rule Language Improvement Project - Setup Complete

**Date:** 2026-01-10  
**Status:** ✓ Ticket system initialized, plan documented, ready to begin work

## What Was Set Up

### 1. Ticket Tracking System
- **Tool:** [wedow/ticket](https://github.com/wedow/ticket) - git-native task tracking
- **Location:** `.tickets/` directory with markdown-based tickets
- **Command:** `./ticket <command> [args]`

### 2. Project Documentation
- **PLAN.md** - 250+ line detailed implementation plan with design notes
- **AGENTS.md** - Agent workflow instructions and code patterns
- **SETUP_SUMMARY.md** - This file (executive summary)

### 3. Tickets Created

**Epic: i-c56b - iRODS Rule Language: Fix Core Weaknesses**

Five sub-tickets with priority, effort estimates, design notes, and acceptance criteria:

| ID | Title | Priority | Effort | Status |
|---|---|---|---|---|
| i-eb98 | Complete TODO error handling (19 sites) | 0 | 15-20h | open |
| i-7b2d | Type query nodes instead of T_DYNAMIC | 0 | 20-30h | open |
| i-c458 | Refactor backward-compat parsing to separate module | 1 | 12-18h | open |
| i-339c | Audit and fix memory leaks (3-4 sites) | 1 | 8-12h | open |
| i-b269 | Improve parser error recovery | 1 | 15-20h | open |

## Why These Five Issues

### Problem Analysis
Surveyed 5,500 lines of iRODS rule language compiler code and identified core weaknesses:

1. **Error Handling Gaps** - 19 `/* todo error handling */` comments block proper error reporting
2. **Type System Deficits** - Query and attribute nodes default to T_DYNAMIC with no analysis
3. **Legacy Cruft** - Backward compatibility flags scattered throughout parser (15+ locations)
4. **Memory Leaks** - 3-4 identified leak sites, mixed malloc/Region allocation
5. **Parser Brittleness** - Single error stops all parsing; limited recovery

### Why Fixable
These are concentrated weaknesses, not architectural problems:
- Clear pattern for error handling (generateErrMsg helper exists)
- Type analysis infrastructure already present (reVariableMap metadata)
- Legacy code isolated to specific functions (easy to separate)
- Leaks are documented with TODO comments (path known)
- Parser macros exist for recovery (just need integration)

### Expected Impact
- **Reliability:** Better error messages for users developing rules
- **Correctness:** Type checking catches more bugs at compile time
- **Maintainability:** Backward-compat code isolated, easier to deprecate
- **Stability:** No more memory leaks in core compiler
- **Developer Experience:** Parser continues after errors, multiple errors reported

## Implementation Roadmap

### Phase 1 (Weeks 1-2): Foundation
1. **i-eb98** - Error handling (foundation for everything else)
2. **i-7b2d** - Query type inference (parallel to error handling)
3. **i-339c** - Memory cleanup (independent work)

### Phase 2 (Week 3): Refactoring
4. **i-c458** - Parser module separation (depends on robust error handling)
5. **i-b269** - Error recovery (builds on Phase 1)

**Total Estimate:** 70-100 hours

## How to Get Started

### For an AI Agent
```bash
# 1. See what needs doing
./ticket ready

# 2. Start a ticket
./ticket start i-eb98

# 3. Read the design notes
./ticket show i-eb98

# 4. Look at the file locations (documented in AGENTS.md)
# 5. Make changes, commit, add notes
./ticket add-note i-eb98 "Fixed parser.cpp:2011 error handling"

# 6. Close when done
./ticket close i-eb98
```

### For Code Review
```bash
# View all open work
./ticket ls

# See dependency graph
./ticket dep tree i-c56b --full

# Check progress on specific ticket
./ticket show i-eb98

# View recent completions
./ticket closed --limit=10
```

## Key Files

| Path | Lines | Purpose |
|------|-------|---------|
| parser.cpp | 3,568 | Recursive descent parser, macros for AST building |
| typing.cpp | 1,228 | Type inference, constraint solving |
| arithmetics.cpp | ~1,500 | Expression evaluation, microservice dispatch |
| parser.hpp | 435 | Parser context, macros (CASCADE, BUILD_NODE, etc.) |
| restructs.hpp | 382 | AST node types (NodeType enum, Node struct) |

## Design Patterns to Know

### Error Handling
```c
// Current (broken):
/* todo error handling */

// Target (correct):
char errbuf[ERR_MSG_LEN];
generateErrMsg("message", exprloc, base, errbuf);
addRErrorMsg(errmsg, RE_TYPE_ERROR, errbuf);
```

### Memory Allocation
Prefer Region allocation over malloc:
```c
// Good:
Node *n = (Node*)region_alloc(r, sizeof(Node));

// Legacy (only for short-lived objects):
ParserContext *pc = (ParserContext*)malloc(sizeof(ParserContext));
// ... use
free(pc);
```

### Parser Macros
```c
PARSER_FUNC_BEGIN(RuleName)
    NEXT_TOKEN_BASIC;
    // parsing logic
    BUILD_NODE(N_RULE_NAME, "rule-name", FPOS, 2, 2);
PARSER_FUNC_END(RuleName)
```

## Next Steps

1. ✓ Ticket system initialized
2. ✓ PLAN.md written with full design notes
3. ✓ AGENTS.md created with instructions
4. → Pick first ticket: `./ticket start i-eb98`
5. → Read PLAN.md section for i-eb98
6. → Begin implementation

## Questions?

Refer to:
- **PLAN.md** - Detailed design for each ticket
- **AGENTS.md** - Workflow and code patterns
- **Ticket notes** - `./ticket show <id>` for specific guidance

---

**Status:** Ready to begin Phase 1 implementation
**Tickets tracked in:** .tickets/ (git-backed, mergeable)
**Plans documented in:** PLAN.md and AGENTS.md
