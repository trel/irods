---
id: i-1fbe
status: closed
deps: []
links: []
created: 2026-01-11T05:22:35Z
type: task
priority: 1
assignee: Terrell Russell
parent: i-7b2d
---
# Implement tuple type composition

Build composite tuple types from column type lists. Enables queries to return typed tuples instead of T_DYNAMIC.

## Acceptance Criteria

✓ Function createTupleTypeFromColumns() implemented
✓ Builds tuple type from column type list
✓ Handles single/multiple columns
✓ Returns properly typed composite type
✓ Code compiles cleanly
✓ Test cases created


## Notes

**2026-01-11T05:41:08Z**

✓ Implemented createTupleTypeFromColumns() function
- Builds composite tuple types from column name lists
- Maps each column name to its iRODS type using getColumnTypeFromSchema()
- Returns tuple type with elements matching column sequence (e.g., [COLL_NAME, DATA_NAME] -> tuple<string, string>)
- Handles single/multiple/many columns (tested with 1-10 columns)
- Returns NULL for NULL or empty column lists (safe API)
- Graceful degradation with T_DYNAMIC for unknown columns
- Code compiles cleanly
- Ready for integration with query condition typing (i-4dd9)
