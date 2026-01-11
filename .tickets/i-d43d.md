---
id: i-d43d
status: closed
deps: [i-adac]
links: []
created: 2026-01-12T17:57:16Z
type: task
priority: 3
assignee: Terrell Russell
parent: i-d794
---
# Design and implement type guard predicates


## Notes

**2026-01-13T12:42:33Z**

Analyzed type guard predicates: functions/checks that narrow type from broad type (like T_DYNAMIC) to specific type. Example: isString(x), isInt(x) return bool and narrow x's type in then-branch. Implementation: (1) extend parser/type system to recognize type narrowing in conditionals, (2) after 'if isString(x)' block, x has type string, (3) type system verifies narrowing matches predicate definition. Foundation: pattern matching (i-adac) already handles type narrowing in match cases, similar logic applies to if guards.

**2026-01-13T13:45:29Z**

Type guard predicates design: (1) functions that return bool and narrow type, (2) example: isString(x) returns true/false and narrows x to string type in then-branch, (3) type system tracks narrowing context for safe type checking, (4) enables runtime validation of dynamic types.
