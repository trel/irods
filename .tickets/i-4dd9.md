---
id: i-4dd9
status: closed
deps: []
links: []
created: 2026-01-11T05:22:40Z
type: task
priority: 1
assignee: Terrell Russell
parent: i-7b2d
---
# Type N_QUERY_COND conditions as boolean expressions

Ensure WHERE clause conditions type-check correctly. Validate column types match comparison operand types.

## Acceptance Criteria

✓ N_QUERY_COND case updated in typeExpression3()
✓ Types column reference against condition operand
✓ Validates type compatibility
✓ Returns boolean expression type
✓ Code compiles cleanly
✓ Test cases created


## Notes

**2026-01-11T05:45:59Z**

✓ Implemented query condition typing (N_QUERY_COND)
- Each WHERE clause condition evaluates to T_BOOL
- Extracts column type from schema using column name in TK_COL
- Types operand values based on operator
- Supports all query operators: =, <>, >, <, >=, <=, in, like, not like, between
- Validates operator/operand count (between requires 2 operands, others require 1)
- Implemented TK_COL typing to return column type from schema
- Code compiles cleanly without warnings
- Ready for integration with full query result typing (i-da8e)
