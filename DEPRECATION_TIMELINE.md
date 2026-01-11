# iRODS Rule Language: Legacy Syntax Deprecation Timeline

**Status:** Official Deprecation Schedule  
**Last Updated:** 2026-01-13  
**Ticket:** i-3257 - Document legacy syntax deprecation timeline

---

## Overview

The iRODS Rule Language is transitioning from legacy syntax (v1.x era) to modern syntax (v2.0+). This document provides an official deprecation timeline showing when legacy constructs will generate warnings, errors, and eventual removal.

**Quick Reference:**
| Phase | Version | Status | Action |
|-------|---------|--------|--------|
| Support | v2.0-v2.4 | Both syntaxes work | No action needed |
| Warnings | v2.5+ | Both syntaxes work with warnings | Update to modern |
| Errors | v3.0+ | Modern syntax only | **Required upgrade** |

---

## Deprecation Phases

### Phase 1: Dual Support (v2.0 - v2.4)
**Status:** CURRENT / IN PROGRESS  
**Duration:** v2.0, v2.1, v2.2, v2.3, v2.4 (estimated 18-24 months)

**What Works:**
- ✅ Legacy syntax accepted and parsed correctly
- ✅ Modern syntax supported alongside legacy
- ✅ Both syntaxes produce semantically equivalent results
- ✅ Auto-detection mode (`@backwardCompatible auto`)
- ✅ Per-rule compatibility markers (`@backwardCompatible true/false`)

**Compiler Behavior:**
```
No warnings or errors - legacy code runs without issue
```

**User Actions:**
- No immediate action required
- Begin planning migration to modern syntax
- Test rules with `@backwardCompatible false` to identify legacy usage
- Read LEGACY_MIGRATION_GUIDE.md for conversion patterns

**Example Configuration:**
```
# rules.re
@backwardCompatible auto   # Auto-detect (default in v2.0-v2.4)

# Old rules work fine
testLegacy($path) :-
  *result = queryPath($path) |
  msiDebugPrint(*result)

# New rules also work
testModern($path : string) : string :-
  *result : string = queryPath($path) |
  msiDebugPrint(*result) |
  *result
```

---

### Phase 2: Deprecation Warnings (v2.5+)
**Status:** PLANNED  
**Duration:** v2.5, v2.6, ... (estimated 12-18 months)

**What Works:**
- ✅ Legacy syntax still accepted and functional
- ⚠️ Legacy constructs generate deprecation warnings at parse/type-check time
- ✅ Modern syntax with no warnings
- ⚠️ Configuration can suppress warnings (for gradual migration)

**Compiler Behavior:**
```
WARNING: Legacy syntax "ifExec" is deprecated since v2.0.
         Use modern "if/then/else" instead.
         Legacy syntax will be removed in v3.0.
         (Suppress with: @suppress legacy-syntax)
```

**Affected Constructs:** (See detailed list below)

**User Actions:**
- **REQUIRED:** Begin migrating rules to modern syntax
- Update CI/CD pipelines to fail on new legacy code
- Use `@suppress legacy-syntax` only for temporary exceptions (document why)
- Plan to complete migration before v3.0 release

**Example Configuration:**
```irods
# rules.re

# Modern syntax - NO WARNINGS
@backwardCompatible false
testModern($path : string) : string :-
  *result : string = queryPath($path) |
  msiDebugPrint(*result) |
  *result

# Legacy syntax - GENERATES WARNINGS
@backwardCompatible true
testLegacy($path) :-
  *result = queryPath($path) |  # WARNING: Implicit typing deprecated
  msiDebugPrint(*result)

# Legacy syntax - WARNINGS SUPPRESSED (use sparingly)
@suppress legacy-syntax
@backwardCompatible true
testLegacyOK($path) :-
  *result = queryPath($path) |
  msiDebugPrint(*result)
```

---

### Phase 3: Modern Syntax Only (v3.0+)
**Status:** PLANNED  
**Duration:** v3.0 and beyond (indefinite)

**What Works:**
- ✅ Modern syntax only
- ❌ Legacy syntax rejected with compilation errors
- ❌ All @backwardCompatible directives ignored
- ❌ Legacy constructs cause immediate failure

**Compiler Behavior:**
```
ERROR: Legacy syntax "ifExec" is no longer supported.
       Use modern "if/then/else" instead.
       See migration guide: https://docs.irods.org/rule-language/migration
```

**User Actions:**
- **MANDATORY:** All legacy rules must be updated before upgrading
- Legacy rules will not load or execute
- No workarounds or suppression available

**Example Configuration:**
```irods
# rules.re

# WORKS - Modern syntax required
testModern($path : string) : string :-
  *result : string = queryPath($path) |
  msiDebugPrint(*result) |
  *result

# FAILS - Legacy syntax not supported
testLegacy($path) :-
  *result = queryPath($path) |
  msiDebugPrint(*result)
```

---

## Detailed Deprecation Reference

### Legacy Functions → Modern Syntax

| Legacy Construct | Introduced | Deprecated | Removed | Modern Alternative |
|------------------|------------|------------|---------|-------------------|
| **Control Flow** |
| `ifExec(cond, a1, a2, a3, a4)` | v1.0 | v2.5 | v3.0 | `if (cond) then { a1 } else { a2 }` |
| `whileExec(cond, actions)` | v1.0 | v2.5 | v3.0 | `while (cond) { actions }` |
| `forEachExec(var, actions1, actions2)` | v1.0 | v2.5 | v3.0 | `foreach (var) { actions }` |
| `forExec(init; cond; update; actions)` | v1.0 | v2.5 | v3.0 | `for (init; cond; update) { actions }` |
| **Assignment & Variables** |
| `assign(*var, value)` | v1.0 | v2.5 | v3.0 | `*var = value` or `$var = value` |
| Unquoted string arguments | v1.0 | v2.5 | v3.0 | `"quoted_string"` |
| `*$var` (reference syntax) | v1.0 | v2.5 | v3.0 | `*var` (modern reference) |
| Implicit variable typing | v1.0 | v2.5 | v3.0 | `@type variable : string` |
| **Operators** |
| `++` (string concat) | v1.0 | v2.5 | v3.0 | `+` (in string context) or `concat()` |
| `like` (pattern match) | v1.0 | v2.5 | v3.0 | `like regex` or pattern functions |
| **Delay/Remote Execution** |
| `delayExec("INST", actions)` | v1.0 | v2.5 | v3.0 | `delay(...) { actions }` |
| `remoteExec(host, actions)` | v1.0 | v2.5 | v3.0 | `remote(host) { actions }` |
| **Type System** |
| Implicit type coercion | v1.0 | v2.5 | v3.0 | Explicit type annotations |
| `T_DYNAMIC` type placeholder | v1.0 | v2.5 | v3.0 | Concrete types with `@type` or `: type` |
| **Error Handling** |
| Implicit error propagation | v1.0 | v2.5 | v3.0 | `try { ... } catch { ... }` |
| Silent failures (unchecked returns) | v1.0 | v2.5 | v3.0 | Explicit error handling |

### Warning Details

Each legacy construct generates a specific warning message:

#### Control Flow Constructs
**Warning Format:**
```
<file>:<line>:<col>: WARNING: Legacy function '<function>' is deprecated since v2.0.
    Use modern '<modern_equiv>' instead.
    Legacy syntax support will be removed in v3.0.
    (Suppress with: @suppress legacy-syntax)
```

**Examples:**
```c
// Legacy usage (v2.5+ generates warning)
ifExec($x > 0, msiDebugPrint("yes"), msiDebugPrint("no"), "", "")
        ↓
WARNING: Legacy function 'ifExec' is deprecated since v2.0.
         Use modern 'if (cond) then { ... } else { ... }' instead.
         Legacy syntax support will be removed in v3.0.

// Modern equivalent (no warning)
if ($x > 0) then {
  msiDebugPrint("yes")
} else {
  msiDebugPrint("no")
}
```

#### Implicit Type System
**Warning Format:**
```
<file>:<line>:<col>: WARNING: Implicit variable typing is deprecated since v2.0.
    Add explicit type annotation: <variable> : <inferred_type>
    Legacy syntax support will be removed in v3.0.
    (Suppress with: @suppress legacy-syntax)
```

**Examples:**
```c
// Legacy (v2.5+ generates warning)
*result = queryPath($path)  // Type inferred from assignment
     ↓
WARNING: Implicit variable typing is deprecated since v2.0.
         Add explicit type annotation: *result : string
         Legacy syntax support will be removed in v3.0.

// Modern equivalent (no warning)
*result : string = queryPath($path)
```

#### Unquoted String Arguments
**Warning Format:**
```
<file>:<line>:<col>: WARNING: Unquoted string argument is deprecated since v2.0.
    Quote the argument: "unquoted_value"
    Legacy syntax support will be removed in v3.0.
    (Suppress with: @suppress legacy-syntax)
```

**Examples:**
```c
// Legacy (v2.5+ generates warning)
msiDataObjCreate(path_without_quotes, mode, $fd)
                 ^^^^^^^^^^^^^^^^^^^^
WARNING: Unquoted string argument is deprecated since v2.0.
         Quote the argument: "path_without_quotes"
         Legacy syntax support will be removed in v3.0.

// Modern equivalent (no warning)
msiDataObjCreate("path_with_quotes", "mode", $fd)
```

---

## Migration Strategy

### For Individual Users

**Timeline:**
1. **Now (v2.0-v2.4):** Read migration guide, test with modern syntax
2. **v2.5 Release:** Eliminate new legacy code, begin converting existing code
3. **Before v3.0:** Complete migration of all legacy rules

**Steps:**
```bash
# 1. Identify legacy usage in your rules
grep -E "ifExec|whileExec|forEachExec|assign\(" rules.re

# 2. Test with legacy syntax disabled
irods-rule-engine --no-legacy rules.re

# 3. Convert rules using patterns from LEGACY_MIGRATION_GUIDE.md

# 4. Verify modern syntax works
irods-rule-engine --modern-syntax-only rules.re

# 5. Test in staging before production
```

### For Deployments

**Production Schedule:**
| When | Action | Risk |
|------|--------|------|
| v2.0-v2.4 | Keep current rules as-is | **None** - full compatibility |
| At v2.5 | Add deprecation warnings to CI/CD warnings list | **Low** - warnings only |
| v2.6-v2.9 | Require conversion of new rules; warn on legacy | **Medium** - gradual migration |
| Before v3.0 | Convert all remaining legacy rules | **High** - required for upgrade |
| v3.0+ | Upgrade (modern syntax required) | **None** - rules already converted |

**Pre-v3.0 Checklist:**
- [ ] Audit all rules for legacy constructs
- [ ] Identify rules that need conversion
- [ ] Plan conversion work (estimate 1-2 hours per 100 lines)
- [ ] Test converted rules in staging
- [ ] Schedule conversion in non-critical periods
- [ ] Verify all rules run in v2.9 with `--modern-syntax-only` flag

---

## Configuration & Control

### Per-File Control: @backwardCompatible Directive

```irods
# Enable for entire file (all subsequent rules use legacy mode)
@backwardCompatible true

# Disable for entire file (all subsequent rules must use modern syntax)
@backwardCompatible false

# Auto-detect (default in v2.0-v2.4; will change in v2.5)
@backwardCompatible auto
```

### Suppressing Warnings: @suppress Directive

```irods
# Suppress all legacy syntax warnings for a rule
@suppress legacy-syntax
testLegacy($path) :-
  *result = queryPath($path) |
  msiDebugPrint(*result)

# Suppress only specific warning type
@suppress legacy-ifexec
testWithIfExec($x) :-
  ifExec($x > 0, msiDebugPrint("yes"), msiDebugPrint("no"), "", "")
```

**Note:** Use `@suppress` sparingly with clear documentation of why legacy syntax must be maintained.

### Global Configuration (Future - v2.5+)

```
# In ruleEngineConfig.json
{
  "legacy_syntax_mode": "auto",      // "auto", "warning", "error"
  "legacy_syntax_severity": "warning", // "warning", "error"
  "legacy_syntax_suppress_patterns": [
    "deprecated_*",     // Suppress for rules matching pattern
    "legacy_*"
  ]
}
```

---

## Version Reference

### Current: v2.0-v2.4 Releases
- **v2.0** - Released: Full legacy/modern support
- **v2.1** - In progress: Complete deprecation infrastructure (i-cf46)
- **v2.2** - Planned: Enhanced testing and tooling
- **v2.3** - Planned: Further refinement
- **v2.4** - Planned: Final support release before v2.5

### Upcoming: v2.5+ Releases
- **v2.5** - Planned: Deprecation warnings enabled
- **v2.6-v2.9** - Planned: Continued support with warnings
- **v3.0** - Planned: Legacy syntax removed

### Release Schedule
- **v2.1** - Q2 2026 (estimated)
- **v2.5** - Q4 2026 (estimated)
- **v3.0** - Q2 2027 (estimated)

---

## Related Documentation

### User-Facing Guides
- **[LEGACY_MIGRATION_GUIDE.md](file:///src/irods/LEGACY_MIGRATION_GUIDE.md)** - Pattern-by-pattern migration guide
- **[LEGACY_SYNTAX_TEST_SUMMARY.md](file:///src/irods/LEGACY_SYNTAX_TEST_SUMMARY.md)** - Test coverage for legacy constructs

### Developer Documentation
- **[BACKWARD_COMPAT_REFACTORING.md](file:///src/irods/BACKWARD_COMPAT_REFACTORING.md)** - Parser refactoring details
- **[AGENTS.md](file:///src/irods/AGENTS.md)** - Development guidelines

### Related Tickets
- **i-cf46** - Implement @deprecated warnings for legacy constructs
- **i-296a** - Remove legacy parser functions (v3.0+)
- **i-26cb** - Legacy Syntax Deprecation: Complete v2.0+ Transition (Epic)

---

## Key Dates & Milestones

| Date | Milestone | Impact |
|------|-----------|--------|
| v2.0 (2025-12) | Modern syntax introduced, legacy still works | Users begin transition planning |
| v2.1 (Q1-Q2 2026) | @deprecated warnings infrastructure ready | Developers can mark deprecated features |
| v2.5 (Q4 2026) | Deprecation warnings enabled by default | Users must plan migration for v3.0 |
| v3.0 (Q2 2027) | Legacy syntax removed | **Mandatory upgrade/migration** |

---

## FAQ

### Q: Will my legacy rules break when I upgrade to v2.5?
**A:** No. Legacy rules continue to work in v2.5 - you'll just see deprecation warnings at parse time.

### Q: Can I suppress the deprecation warnings?
**A:** Yes, use `@suppress legacy-syntax` for specific rules. However, you'll still need to update before upgrading to v3.0.

### Q: How long do I have to migrate?
**A:** From v2.5 release to v3.0 release (estimated 12-18 months). Plan to complete migration well before v3.0.

### Q: Which constructs are affected?
**A:** See the "Detailed Deprecation Reference" table above. Main categories are: control flow functions (ifExec, etc.), implicit typing, unquoted strings, and implicit error handling.

### Q: Will the migration guide help me update my rules?
**A:** Yes! See LEGACY_MIGRATION_GUIDE.md for detailed pattern-by-pattern conversion examples.

### Q: What if I have hundreds of legacy rules?
**A:** Start early and migrate incrementally. The patterns are repetitive - batch conversion tools are planned.

### Q: Is there a tool to automatically convert legacy rules?
**A:** Not yet, but planned for v2.6. Manual migration follows clear patterns documented in LEGACY_MIGRATION_GUIDE.md.

---

## Support & Questions

For questions about deprecation timeline or migration:
- Check **[LEGACY_MIGRATION_GUIDE.md](file:///src/irods/LEGACY_MIGRATION_GUIDE.md)** for specific patterns
- Review test files: `test_legacy_compat.re`, `test_modern_syntax.re`
- Contact iRODS development team
- File issues on GitHub: https://github.com/irods/irods

---

**Document Version:** 1.0  
**Last Updated:** 2026-01-13  
**Status:** FINAL - Ready for Release Planning
