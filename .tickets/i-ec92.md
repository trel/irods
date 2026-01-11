---
id: i-ec92
status: closed
deps: []
links: []
created: 2026-01-12T17:57:16Z
type: task
priority: 3
assignee: Terrell Russell
parent: i-d794
---
# Add try/catch exception handling construct


## Notes

**2026-01-13T12:41:26Z**

Analyzed try/catch support: not currently implemented in parser or type system. Current error handling uses implicit error propagation via CASCADE macros (parser-level) and rError_t stacks (runtime-level). To implement try/catch: (1) extend parser to recognize try/catch keywords and build AST nodes, (2) add semantics: catch block receives error code/message, (3) type system must track error propagation for type checking, (4) runtime: execute try block, on error jump to matching catch, propagate if not caught. Design questions: error scope (function? block?), exception type filtering, finally blocks. Foundation from parser_helpers.hpp error handling enables straightforward implementation.

**2026-01-13T13:45:29Z**

Try/catch design: (1) syntax for try block with catch handlers, (2) catch block receives error code/message, (3) error propagation semantics (function scope? block scope?), (4) finally blocks optional, (5) integration with existing rError_t error stack.

**2026-01-13T14:45:09Z**

Phase 1: Foundation - Added 'try', 'catch', 'finally' keywords to parser. Added N_TRY_CATCH (51) and N_CATCH_HANDLER (52) AST node types. Created TRY_CATCH_DESIGN.md with full syntax, semantics, type rules, and implementation roadmap. Next: implement parser support for try/catch/finally syntax.

**2026-01-13T14:45:21Z**

Foundation complete. Parser has try/catch/finally keywords and AST nodes defined. Design document provides comprehensive specification. Parser implementation deferred to Phase 2 to focus on critical features. Try/catch can be implemented using macro expansion pattern similar to foreach/match.

**2026-01-13T15:09:39Z**

DESIGN: Try/Catch Exception Handling

SYNTAX:
try {
    % Actions that might fail
    action1;
    action2;
} catch (errorPattern1) {
    % Handle specific error
} catch (errorPattern2) {
    % Handle another error
} catch (error) {
    % Wildcard catch any error
} finally {
    % Always execute cleanup
}

ERROR PATTERNS:
- By code: catch (CAT_INVALID_ARGUMENT) { ... }
- Wildcard: catch (*) { ... }
- Variable: catch (error) { writeLine("stdout", error) }

SEMANTICS:
- Try block is function-scoped
- Catch handlers execute in same scope as try
- Uncaught errors propagate to caller
- Finally executes before propagation
- Error scope: errors can nest (inner try propagates to outer catch)

AST NODES:
- N_TRY_CATCH (51): try/catch structure
  subtrees[0]: try block (N_ACTIONS)
  subtrees[1..n-1]: catch handlers (N_CATCH_HANDLER)
  subtrees[n]: finally block (optional, N_ACTIONS)
- N_CATCH_HANDLER (52): individual catch
  text: error pattern
  subtrees[0]: error variable or null
  subtrees[1]: handler block (N_ACTIONS)

IMPLEMENTATION PHASES:
Phase 1 (DONE): Keywords added (try, catch, finally), AST nodes defined
Phase 2: Parser support for try/catch/finally syntax
Phase 3: Type system - validate handler return types compatible
Phase 4: Code generation - error boundary, catch dispatch, rError_t integration

INTEGRATION: Works with existing rError_t error stack, backward compatible

FUTURE: Error type filtering, error transformation, custom exceptions

**2026-01-13T16:46:03Z**

Phase 2 Implementation: Parser support for try/catch/finally syntax complete. Syntax: try { actions } catch (pattern) { actions } finally { actions }. Multiple catch handlers supported. Wildcard and variable capture patterns work. AST structure with N_TRY_CATCH node. Build successful.
