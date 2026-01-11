---
id: i-fb61
status: closed
deps: []
links: []
created: 2026-01-12T17:57:16Z
type: task
priority: 2
assignee: Terrell Russell
parent: i-72d9
---
# Analyze macro coupling and identify refactor points


## Notes

**2026-01-13T02:00:31Z**

Created comprehensive macro coupling analysis (MACRO_ANALYSIS.md). Identified 23 macros across 6 categories (control flow, choice/branch, loops, token matching, stack ops). Documented explicit and implicit dependencies, usage statistics, and refactoring roadmap with 6 phases. Ready for detailed refactoring work.

**2026-01-13T02:02:01Z**

Completed macro analysis. Key findings: 23 macros in 6 categories (control flow, choice/branch, loops, token matching, stack ops, other). BUILD_NODE (11 lines) is most critical. CASCADE and TRY/OR heavily used. Dependencies: BUILD_NODE calls CASCADE, TRY/OR use CHOICE_BEGIN/BRANCH_BEGIN, REPEAT/LIST compose multiple macros. Stack management uses two structures (nodeStack, tokenQueue). Error handling cascades implicitly. Refactor phases: 1) simple functions 2) node construction 3) choice/branch 4) loops 5) parser functions 6) error handling. Usage: BUILD_NODE ~200+, TTEXT ~80+, TRY/OR ~30+, NT* ~100+.
