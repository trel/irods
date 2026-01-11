---
id: i-a5f0
status: closed
deps: []
links: []
created: 2026-01-12T17:57:16Z
type: task
priority: 2
assignee: Terrell Russell
parent: i-3ad9
---
# Extract MSI function type information from function registry


## Notes

**2026-01-13T01:35:34Z**

ANALYSIS: Extract MSI function type information from function registry

Current State:
- getMSIFunctionDescriptors() (msi_functions.cpp) creates function descriptors from microservice table
- Each descriptor uses type signature string format: 'type1 * type2 * ... * typeN->return_type'
- msi_type_registry.cpp provides MSI type metadata (40+ entries)
- System functions (getSystemFunctions) demonstrate descriptor pattern

Task Requirements:
1. Extract runtime info from ms_table_entry (num_args, function pointers)
2. Map to type metadata from registry
3. Generate improved function descriptors for each MSI
4. Support future extraction from plugin-loaded MSIs

Implementation Strategy:
- Create getMSIFunctionInfo() to bridge ms_table_entry -> MSITypeInfo -> FunctionDesc
- Enhance getMSIFunctionDescriptors() to use real MSI types instead of all-?
- Add helper function to format MSI semantics in signature (e.g., 'o string' for output)
- Document how runtime MSI discovery will work

**2026-01-13T01:36:41Z**

COMPLETED: Extract MSI function type information from registry

✓ Enhanced getMSITypeSignature() to extract full type info:
  - Maps MSI registry entries to semantic annotations (e, o, a, ?)
  - Generates rich signatures: 'e string * o GenQueryOut->int'
  - Falls back gracefully for unregistered MSIs

✓ Added semanticsToAnnotation() helper:
  - INPUT -> 'e' (expression/read-only)
  - OUTPUT -> 'o' (output/write)
  - INOUT -> 'a' (assignable/read-write)
  - DYNAMIC -> '' (no annotation)

✓ Added public helper functions:
  - getMSITypeRegistrySize() - query registry coverage
  - isMSITypeRegistered() - check if MSI has type info

✓ Enhanced documentation in msi_functions.hpp with:
  - Explanation of semantic annotations
  - Examples of generated signatures
  - Note about fallback behavior

Signatures now provide full type information for compilation and type checking.
