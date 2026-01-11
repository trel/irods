---
id: i-14fe
status: closed
deps: []
links: []
created: 2026-01-12T17:57:16Z
type: task
priority: 2
assignee: Terrell Russell
parent: i-72d9
---
# Create parser debugging/tracing infrastructure


## Notes

**2026-01-13T02:04:37Z**

Implemented parser debugging/tracing infrastructure in parser_helpers: (1) Environment variable control: IRODS_PARSER_TRACE, IRODS_PARSER_TRACE_VERBOSE, IRODS_PARSER_TRACE_STACK; (2) Event types: ENTER, EXIT, SUCCESS, FAILURE, ERROR, BACKTRACK, TOKEN, NODE, STACK; (3) traceParserEvent() for emitting events with indentation; (4) dumpParserState() shows error, stack, token queue status; (5) dumpNodeStack() displays top N stack items; (6) debugPrintNode() prints node info. Enables debugging refactored parser without code changes - just set env vars. Ready for profiling and error investigation.
