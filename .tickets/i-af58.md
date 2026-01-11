---
id: i-af58
status: closed
deps: []
links: []
created: 2026-01-12T17:57:16Z
type: task
priority: 2
assignee: Terrell Russell
parent: i-72d9
---
# Extract parser state machine from macros into explicit functions


## Notes

**2026-01-13T02:02:36Z**

Analyzed parser function sizes and complexity. Key findings: 20 parsing functions total, ranging from 13 lines (PathExpression) to 250+ lines (Term). TRY/OR pattern most common for choice points. Identified extraction strategy: PathExpression is good starting point (uses TRY/OR + BUILD_APP_NODE). Plan: extract state machine using ChoicePoint API + explicit control flow. Example extraction in PARSER_EXTRACTION_EXAMPLE.txt showing macro expansion vs refactored code. Refactoring reduces hidden macros and makes control flow visible.
