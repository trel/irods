---
id: i-cf07
status: closed
deps: []
links: []
created: 2026-01-12T17:57:16Z
type: task
priority: 2
assignee: Terrell Russell
parent: i-72d9
---
# Modernize parser error recovery with multi-error collection


## Notes

**2026-01-13T02:03:06Z**

Analyzed current error handling: parser uses single error flag (context->error) + error location (context->errloc). rError_t supports error stack (MAX_ERROR_MESSAGES=100, array of ErrorMessage). Current issue: parser stops on first error and breaks immediately. Enhancement plan: (1) Track multiple parse errors in error stack, (2) Continue parsing after non-fatal errors to find additional issues, (3) Better error recovery with multi-error reporting. Implementation: Modify CASCADE macro behavior and error propagation in buildNode* functions to collect errors instead of breaking immediately. Will require: refactored error handling (i-e399 predecessor), testing framework for multi-error cases.
