/*
 * Test: Legacy construct deprecation warnings
 * 
 * Tests that deprecated legacy constructs (forExec, whileExec, assign, etc.)
 * emit proper deprecation warnings when detected during parsing.
 */

#include <catch2/catch_all.hpp>
#include "irods/private/re/deprecation.hpp"
#include <cstring>

/**
 * Test 1: Check if forExec is marked as legacy
 */
TEST_CASE("ForExecIsLegacy") {
    int result = isLegacySystemFunction("forExec");
    CHECK(result == 1);
}

/**
 * Test 2: Check if whileExec is marked as legacy
 */
TEST_CASE("WhileExecIsLegacy") {
    int result = isLegacySystemFunction("whileExec");
    CHECK(result == 1);
}

/**
 * Test 3: Check if forEachExec is marked as legacy
 */
TEST_CASE("ForEachExecIsLegacy") {
    int result = isLegacySystemFunction("forEachExec");
    CHECK(result == 1);
}

/**
 * Test 4: Check if delayExec is marked as legacy
 */
TEST_CASE("DelayExecIsLegacy") {
    int result = isLegacySystemFunction("delayExec");
    CHECK(result == 1);
}

/**
 * Test 5: Check if remoteExec is marked as legacy
 */
TEST_CASE("RemoteExecIsLegacy") {
    int result = isLegacySystemFunction("remoteExec");
    CHECK(result == 1);
}

/**
 * Test 6: Check if assign is marked as legacy
 */
TEST_CASE("AssignIsLegacy") {
    int result = isLegacySystemFunction("assign");
    CHECK(result == 1);
}

/**
 * Test 7: Check if ifExec is marked as legacy
 */
TEST_CASE("IfExecIsLegacy") {
    int result = isLegacySystemFunction("ifExec");
    CHECK(result == 1);
}

/**
 * Test 8: Check deprecation info for forExec
 */
TEST_CASE("ForExecDeprecationInfo") {
    const DeprecationInfo *info = getLegacyFunctionDeprecationInfo("forExec");
    REQUIRE(info != nullptr);
    CHECK(strcmp(info->name, "forExec") == 0);
    CHECK(info->since_version != nullptr);
    CHECK(info->removal_version != nullptr);
    CHECK(info->alternative != nullptr);
    CHECK(info->reason != nullptr);
}

/**
 * Test 9: Check deprecation info for whileExec
 */
TEST_CASE("WhileExecDeprecationInfo") {
    const DeprecationInfo *info = getLegacyFunctionDeprecationInfo("whileExec");
    REQUIRE(info != nullptr);
    CHECK(strcmp(info->name, "whileExec") == 0);
    CHECK(info->alternative != nullptr);
}

/**
 * Test 10: Check deprecation info for delayExec
 */
TEST_CASE("DelayExecDeprecationInfo") {
    const DeprecationInfo *info = getLegacyFunctionDeprecationInfo("delayExec");
    REQUIRE(info != nullptr);
    CHECK(strcmp(info->name, "delayExec") == 0);
    CHECK(strlen(info->name) > 0);
    CHECK(strlen(info->since_version) > 0);
    CHECK(strlen(info->removal_version) > 0);
    CHECK(strlen(info->alternative) > 0);
    CHECK(strlen(info->reason) > 0);
}

/**
 * Test 11: Check deprecation info for forEachExec
 */
TEST_CASE("ForEachExecDeprecationInfo") {
    const DeprecationInfo *info = getLegacyFunctionDeprecationInfo("forEachExec");
    REQUIRE(info != nullptr);
    CHECK(strcmp(info->name, "forEachExec") == 0);
}

/**
 * Test 12: Check deprecation info for remoteExec
 */
TEST_CASE("RemoteExecDeprecationInfo") {
    const DeprecationInfo *info = getLegacyFunctionDeprecationInfo("remoteExec");
    REQUIRE(info != nullptr);
    CHECK(strcmp(info->name, "remoteExec") == 0);
}

/**
 * Test 13: Check deprecation info for assign
 */
TEST_CASE("AssignDeprecationInfo") {
    const DeprecationInfo *info = getLegacyFunctionDeprecationInfo("assign");
    REQUIRE(info != nullptr);
    CHECK(strcmp(info->name, "assign") == 0);
}

/**
 * Test 14: Check deprecation info for ifExec
 */
TEST_CASE("IfExecDeprecationInfo") {
    const DeprecationInfo *info = getLegacyFunctionDeprecationInfo("ifExec");
    REQUIRE(info != nullptr);
    CHECK(strcmp(info->name, "ifExec") == 0);
}

/**
 * Test 15: Deprecation level is valid
 */
TEST_CASE("DeprecationLevelValid") {
    const char *deprecated[] = {
        "forExec", "whileExec", "forEachExec", 
        "delayExec", "remoteExec", "assign", "ifExec"
    };
    
    for (const char *func : deprecated) {
        const DeprecationInfo *info = getLegacyFunctionDeprecationInfo(func);
        REQUIRE(info != nullptr);
        
        bool valid_level = (info->level == DEPRECATION_NOTICE || 
                            info->level == DEPRECATION_WARNING || 
                            info->level == DEPRECATION_ERROR);
        CHECK(valid_level);
    }
}

/**
 * Test 16: Non-legacy functions don't return deprecation info
 */
TEST_CASE("ModernFunctionsNoDeprecation") {
    const char *modern[] = {"for", "while", "foreach", "delay", "remote", "if"};
    
    for (const char *func : modern) {
        int result = isLegacySystemFunction(func);
        CHECK(result == 0);
    }
}

/**
 * Test 17: Alternative suggestion is provided for forExec
 */
TEST_CASE("ForExecHasAlternative") {
    const DeprecationInfo *info = getLegacyFunctionDeprecationInfo("forExec");
    REQUIRE(info != nullptr);
    REQUIRE(info->alternative != nullptr);
    CHECK(strlen(info->alternative) > 0);
}

/**
 * Test 18: Deprecation level is correct for all functions
 */
TEST_CASE("AllDeprecatedFunctionsHaveLevel") {
    const char *deprecated[] = {
        "forExec", "whileExec", "forEachExec", 
        "delayExec", "remoteExec", "assign", "ifExec"
    };
    
    for (const char *func : deprecated) {
        const DeprecationInfo *info = getLegacyFunctionDeprecationInfo(func);
        REQUIRE(info != nullptr);
        int level = (int)info->level;
        CHECK(level >= (int)DEPRECATION_NOTICE);
        CHECK(level <= (int)DEPRECATION_ERROR);
    }
}

/**
 * Test 19: Null function name returns null
 */
TEST_CASE("NullFunctionNameReturnsNull") {
    const DeprecationInfo *info = getLegacyFunctionDeprecationInfo(nullptr);
    CHECK(info == nullptr);
}

/**
 * Test 20: Unknown function returns null
 */
TEST_CASE("UnknownFunctionReturnsNull") {
    const DeprecationInfo *info = getLegacyFunctionDeprecationInfo("unknownFunc");
    CHECK(info == nullptr);
}
