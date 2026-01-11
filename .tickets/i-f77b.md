---
id: i-f77b
status: closed
deps: [i-470c]
links: []
created: 2026-01-13T17:49:36Z
type: task
priority: 2
assignee: Terrell Russell
parent: i-cf46
---
# Emit @deprecated warnings during type checking


## Notes

**2026-01-18T02:50:19Z**

Status: [X] Added OPTION_DEPRECATED macro infrastructure to restructs.hpp, [X] Implemented deprecation warning emission in typing.cpp for function calls, [X] Integrated legacy system function checks, [X] Created comprehensive unit test suite (20+ tests), [X] Added test rule file with @deprecated examples. Integration points: typeFunction3() for function calls, typeExpression3() for direct references. Functions: isDeprecated(), setDeprecated() macros. Build verified: rule language plugin compiles successfully.
