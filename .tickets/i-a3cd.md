---
id: i-a3cd
status: closed
deps: []
links: []
created: 2026-01-12T17:57:16Z
type: task
priority: 2
assignee: Terrell Russell
parent: i-3ad9
---
# Document MSI type annotation requirements


## Notes

**2026-01-13T12:43:41Z**

Document MSI type annotation requirements: Microservice Implementations (MSI) need type annotations for safety. Document: (1) how to annotate MSI parameters with @input/@output/@optional, (2) type signature syntax, (3) examples of well-typed MSIs, (4) validation/testing of MSI types.

**2026-01-13T13:45:29Z**

MSI type annotation requirements: document (1) how to annotate MSI parameters with @input/@output/@optional/@nonnull, (2) type signature syntax for MSI declarations, (3) examples of well-typed MSIs, (4) validation and testing of MSI types.

**2026-01-13T14:43:31Z**

Created comprehensive MSI_TYPE_ANNOTATIONS.md documenting: (1) annotation syntax for @input/@output/@optional/@nonnull, (2) type signature examples, (3) validation and enforcement rules, (4) testing strategies, (5) migration guide for existing MSIs, (6) best practices

**2026-01-13T15:09:31Z**

DESIGN: MSI Type Annotations

OVERVIEW: Document how to annotate MSI parameters with type information for type safety.

ANNOTATION SYNTAX:
- @input: parameter provided by caller
- @output: parameter computed by function
- @optional: parameter can be null
- @nonnull: parameter must never be null
- Example: @input @nonnull string path

SUPPORTED TYPES:
- Primitives: int, string, double, bool
- iRODS types: backtick-quoted like `GenQueryOut`
- Collections: List<int>, Tuple<string, int>
- Dynamic: dynamic (type inference fallback)

EXAMPLES:
1. File operations:
   msiDataObjOpen(@input @nonnull string path, @input @nonnull string flags, @output int fd)
   
2. Query:
   msiExecGenQuery(@input @nonnull string query, @input @optional int limit, @output GenQueryOut result)

3. Metadata:
   msiSetMetadata(@input @nonnull string path, @input @optional string attr, @input string value, @output int code)

VALIDATION:
- Compile-time: parameter count, type compatibility, nullability constraints
- @nonnull parameters cannot receive null
- @optional parameters can receive null
- Exhaustiveness checking if not all types covered

TESTING: Unit tests for (1) nullability constraints (2) type mismatches (3) optional parameters

BACKWARD COMPAT: Additive feature, no breaking changes
