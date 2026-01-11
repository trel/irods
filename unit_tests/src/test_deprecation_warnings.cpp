#include <catch2/catch_all.hpp>
#include "irods/private/re/deprecation.hpp"
#include <cstring>

/**
 * Test suite for deprecation warning system
 * Tests legacy construct detection and warning generation
 */

/**
 * Test: Legacy function detection - forExec
 * forExec should be detected as deprecated
 */
TEST_CASE("DeprecationWarningTest: DetectForExecLegacy")
{
    CHECK(isLegacySystemFunction("forExec"));
    const DeprecationInfo *info = getLegacyFunctionDeprecationInfo("forExec");
    REQUIRE(info != nullptr);
    REQUIRE(std::string(info->name) == "forExec");
    REQUIRE(std::string(info->since_version) == "1.0");
    REQUIRE(std::string(info->removal_version) == "3.0");
    REQUIRE(info->level == DEPRECATION_WARNING);
}

/**
 * Test: Legacy function detection - whileExec
 * whileExec should be detected as deprecated
 */
TEST_CASE("DeprecationWarningTest: DetectWhileExecLegacy")
{
    CHECK(isLegacySystemFunction("whileExec"));
    const DeprecationInfo *info = getLegacyFunctionDeprecationInfo("whileExec");
    REQUIRE(info != nullptr);
    REQUIRE(std::string(info->name) == "whileExec");
}

/**
 * Test: Legacy function detection - forEachExec
 * forEachExec should be detected as deprecated
 */
TEST_CASE("DeprecationWarningTest: DetectForEachExecLegacy")
{
    CHECK(isLegacySystemFunction("forEachExec"));
    const DeprecationInfo *info = getLegacyFunctionDeprecationInfo("forEachExec");
    REQUIRE(info != nullptr);
    REQUIRE(std::string(info->name) == "forEachExec");
}

/**
 * Test: Legacy function detection - delayExec
 * delayExec should be detected as deprecated
 */
TEST_CASE("DeprecationWarningTest: DetectDelayExecLegacy")
{
    CHECK(isLegacySystemFunction("delayExec"));
    const DeprecationInfo *info = getLegacyFunctionDeprecationInfo("delayExec");
    REQUIRE(info != nullptr);
    REQUIRE(std::string(info->name) == "delayExec");
}

/**
 * Test: Legacy function detection - remoteExec
 * remoteExec should be detected as deprecated
 */
TEST_CASE("DeprecationWarningTest: DetectRemoteExecLegacy")
{
    CHECK(isLegacySystemFunction("remoteExec"));
    const DeprecationInfo *info = getLegacyFunctionDeprecationInfo("remoteExec");
    REQUIRE(info != nullptr);
    REQUIRE(std::string(info->name) == "remoteExec");
}

/**
 * Test: Legacy function detection - assign
 * assign should be detected as deprecated
 */
TEST_CASE("DeprecationWarningTest: DetectAssignLegacy")
{
    CHECK(isLegacySystemFunction("assign"));
    const DeprecationInfo *info = getLegacyFunctionDeprecationInfo("assign");
    REQUIRE(info != nullptr);
    REQUIRE(std::string(info->name) == "assign");
    REQUIRE(info->alternative != nullptr);
    REQUIRE(std::string(info->alternative).find("direct assignment") != std::string::npos);
}

/**
 * Test: Legacy function detection - ifExec
 * ifExec should be detected as deprecated
 */
TEST_CASE("DeprecationWarningTest: DetectIfExecLegacy")
{
    CHECK(isLegacySystemFunction("ifExec"));
    const DeprecationInfo *info = getLegacyFunctionDeprecationInfo("ifExec");
    REQUIRE(info != nullptr);
    REQUIRE(std::string(info->name) == "ifExec");
}

/**
 * Test: Non-legacy function detection
 * Modern functions should not be detected as deprecated
 */
TEST_CASE("DeprecationWarningTest: ModernFunctionNotLegacy")
{
    CHECK_FALSE(isLegacySystemFunction("for"));
    CHECK_FALSE(isLegacySystemFunction("while"));
    CHECK_FALSE(isLegacySystemFunction("foreach"));
    CHECK_FALSE(isLegacySystemFunction("delay"));
    CHECK_FALSE(isLegacySystemFunction("remote"));
    CHECK_FALSE(isLegacySystemFunction("if"));
    CHECK_FALSE(isLegacySystemFunction(nullptr));
    CHECK_FALSE(isLegacySystemFunction(""));
}

/**
 * Test: Null pointer handling in isLegacySystemFunction
 */
TEST_CASE("DeprecationWarningTest: NullPointerHandling")
{
    CHECK_FALSE(isLegacySystemFunction(nullptr));
    REQUIRE(getLegacyFunctionDeprecationInfo(nullptr) == nullptr);
}

/**
 * Test: Alternative suggestions for deprecated constructs
 * Each deprecated construct should have a modern alternative
 */
TEST_CASE("DeprecationWarningTest: HasModernAlternatives")
{
    const char *deprecated[] = {"forExec", "whileExec", "forEachExec", "delayExec", "remoteExec", "assign", "ifExec"};
    
    for (const char *func : deprecated) {
        const DeprecationInfo *info = getLegacyFunctionDeprecationInfo(func);
        REQUIRE(info != nullptr);
        REQUIRE(info->alternative != nullptr);
        REQUIRE(strlen(info->alternative) > 0);
    }
}

/**
 * Test: Deprecation levels are correct
 * All legacy constructs should be WARNING or ERROR level
 */
TEST_CASE("DeprecationWarningTest: CorrectDeprecationLevels")
{
    const char *deprecated[] = {"forExec", "whileExec", "forEachExec", "delayExec", "remoteExec", "assign", "ifExec"};
    
    for (const char *func : deprecated) {
        const DeprecationInfo *info = getLegacyFunctionDeprecationInfo(func);
        REQUIRE(info != nullptr);
        REQUIRE((int)info->level >= (int)DEPRECATION_NOTICE);
        REQUIRE((int)info->level <= (int)DEPRECATION_ERROR);
    }
}
