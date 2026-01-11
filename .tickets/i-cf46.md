---
id: i-cf46
status: closed
deps: [i-045a]
links: []
created: 2026-01-12T17:57:16Z
type: task
priority: 2
assignee: Terrell Russell
parent: i-26cb
---
# Implement @deprecated warnings for legacy constructs


## Notes

**2026-01-13T12:43:46Z**

Implement @deprecated warnings: add decorator to mark constructs as deprecated. Warnings emitted at parse/type-check time. Example: '@deprecated(since=2.0, use=alternative) rule oldRule { ... }' generates compiler warning when used.

**2026-01-13T13:45:29Z**

Implement @deprecated decorator: (1) extend parser to recognize @deprecated(since=version, use=alternative) decorators on rules/functions, (2) during type checking, emit compiler warning when decorated construct is called, (3) include deprecation message and suggested alternative in warning.
