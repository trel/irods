---
id: i-adac
status: closed
deps: []
links: []
created: 2026-01-12T17:57:16Z
type: task
priority: 3
assignee: Terrell Russell
parent: i-d794
---
# Add pattern matching construct for type checking


## Notes

**2026-01-13T12:41:41Z**

Analyzed pattern matching: not currently supported. Rule language has 'match with' construct (grep shows in parser.cpp line ~975). Existing match: matches value against patterns with '=>' syntax (e.g., 'match x with a => ... | b => ...'). Enhancement needed: type guard patterns like 'match x with (int) => ...' to narrow type from T_DYNAMIC. Implementation: (1) extend parser to recognize type patterns in match cases, (2) type system narrows variable type in pattern case body, (3) verify narrowed types match assumptions. Foundation: parser_helpers.hpp combinators enable clean alternation parsing without macro hell.

**2026-01-13T13:45:29Z**

Pattern matching enhancement: existing match construct uses pattern => expr syntax. Enhancement: support type patterns like (int), (string) for type-based matching and narrowing. Implementation: parser recognizes type patterns, type system narrows matched variable type in case body.

**2026-01-13T14:46:34Z**

Created comprehensive PATTERN_MATCHING_DESIGN.md with: (1) enhancement to existing match construct, (2) type pattern syntax like (int), (string), (_), (3) type narrowing rules with examples, (4) AST representation, (5) three-phase implementation roadmap with parser and type system integration, (6) exhaustiveness checking, (7) integration with optional types and try/catch, (8) error messages and performance analysis. Ready for Phase 1 implementation.

**2026-01-13T15:09:53Z**

DESIGN: Pattern Matching with Type Narrowing

ENHANCEMENT: Extend existing match construct with type-based patterns and type narrowing.

CURRENT: match x with pattern1 => expr | pattern2 => expr (value-based only)

PROPOSED SYNTAX:
match value with
    (int) => expr1           % Match if int type
    | (string) => expr2      % Match if string type
    | (double) => expr3
    | (_) => exprDefault     % Match any type

TYPE PATTERNS:
- Basic: (int), (string), (double), (bool)
- Compound: (List<int>), (Tuple<string, int>)
- Wildcard: (_) or (x) for wildcard

TYPE NARROWING:
- Matched variable has narrower type in case body
- Local to match case (doesn't affect outer scope)
- Example: match data with (int) => { x = data; result = x + 1 }

SEMANTICS:
- Type narrowing is compile-time only (zero runtime cost)
- Pattern matching dispatch: O(1) per pattern
- Exhaustiveness checking: warn if not all types covered

AST: Extend existing match structure, mark patterns with type_pattern flag

IMPLEMENTATION PHASES:
1. Parser: recognize (type) patterns in match cases
2. Type system: implement type narrowing in case bodies
3. Semantic analysis: exhaustiveness checking
4. Error messages: clear type mismatch guidance

EXAMPLES:
1. Safe casting: match data with (int) => count = data
2. Query results: match result with (GenQueryOut) => process rows
3. Error checking: match status with (int) => check code
4. Nested: match tuple with (int, (string, double)) => process

INTEGRATION:
- Works with try/catch for error handling
- Combines with @optional/@nonnull for null safety
- Type guards future enhancement: (int) where value > 0 => positive

PERF: Type pattern dispatch O(1), narrowing zero runtime cost

BACKWARD COMPAT: Additive to existing match, no breaking changes
