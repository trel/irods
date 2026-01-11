---
id: i-a19e
status: closed
deps: []
links: []
created: 2026-01-12T17:05:43Z
type: task
priority: 2
assignee: Terrell Russell
parent: i-cc56
---
# nre.reLib1: Document long-lived database read allocations

Lines 458-461, 551-556, 597-599, 648-649, 709-716: Document why 30+ strdup allocations in readRuleSetFromDB, readDVarSetFromDB, readFMapSetFromDB, readMsrvcSetFromDB should remain malloc (stored in static structures).


## Notes

**2026-01-12T17:12:55Z**

Documented 4 database read functions (readRuleStructFromDB, readDVMapStructFromDB, readFNMapStructFromDB, readMsrvcStructFromDB) explaining why 18 strdup allocations must remain malloc: stored in static plugin-lifetime structures, freed in clear* functions. Build successful.
