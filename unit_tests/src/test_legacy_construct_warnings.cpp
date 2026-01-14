/*
 * Test: Legacy construct deprecation warnings
 * 
 * Tests that deprecated legacy constructs (forExec, whileExec, assign, etc.)
 * emit proper deprecation warnings when detected during parsing.
 */

#include <gtest/gtest.h>
#include "irods/private/re/deprecation.hpp"
#include "irods/private/re/parser.hpp"
#include "irods/private/re/restructs.hpp"
#include "irods/private/re/configuration.hpp"
#include "irods/region.h"
#include "irods/rcMisc.h"
#include "irods/rodsErrorTable.h"
#include <cstring>

/* Forward declarations from rule engine */
extern ParserContext *newParserContext(rError_t *errmsg, Region *r);

/**
 * Test suite for legacy construct warning emission
 */
class LegacyConstructWarningTest : public ::testing::Test {
protected:
    Region *region;
    rError_t errmsg;
    ParserContext *context;

    void SetUp() override {
        /* Initialize config */
        memset(&ruleEngineConfig, 0, sizeof(Cache));
        ruleEngineConfig.strictTypeChecking = 0;
        ruleEngineConfig.disableLegacySyntax = 0;
        
        /* Setup region and error handling */
        region = make_region(0, nullptr);
        memset(&errmsg, 0, sizeof(rError_t));
        errmsg.status = 0;
        
        context = newParserContext(&errmsg, region);
        ASSERT_NE(nullptr, context);
    }

    void TearDown() override {
        if (region) {
            region_free(region);
        }
    }

    /* Helper to create a source location */
    Label createLocation(int line, int col, int exprloc) {
        Label loc;
        loc.line = line;
        loc.col = col;
        loc.exprloc = exprloc;
        return loc;
    }

    /* Helper to count error messages in the error queue */
    int countErrorMessages(rError_t *errmsg) {
        if (!errmsg) return 0;
        int count = 0;
        for (rErrMsg_t *p = errmsg->errMsg; p != nullptr; p = p->next) {
            count++;
        }
        return count;
    }

    /* Helper to check if error message contains text */
    bool errorMessageContains(rError_t *errmsg, const char *text) {
        if (!errmsg || !text) return false;
        for (rErrMsg_t *p = errmsg->errMsg; p != nullptr; p = p->next) {
            if (p->msg && strstr(p->msg, text) != nullptr) {
                return true;
            }
        }
        return false;
    }
};

/**
 * Test 1: forExec emits deprecation warning
 */
TEST_F(LegacyConstructWarningTest, ForExecEmitsWarning) {
    Label loc = createLocation(10, 5, 42);
    const DeprecationInfo *info = getLegacyFunctionDeprecationInfo("forExec");
    
    ASSERT_NE(nullptr, info);
    int msgsBefore = countErrorMessages(&errmsg);
    
    emitDeprecationWarning(context, info, &loc);
    
    int msgsAfter = countErrorMessages(&errmsg);
    EXPECT_GT(msgsAfter, msgsBefore);
    EXPECT_TRUE(errorMessageContains(&errmsg, "forExec"));
    EXPECT_TRUE(errorMessageContains(&errmsg, "Deprecated"));
}

/**
 * Test 2: whileExec emits deprecation warning
 */
TEST_F(LegacyConstructWarningTest, WhileExecEmitsWarning) {
    Label loc = createLocation(15, 8, 100);
    const DeprecationInfo *info = getLegacyFunctionDeprecationInfo("whileExec");
    
    ASSERT_NE(nullptr, info);
    emitDeprecationWarning(context, info, &loc);
    
    EXPECT_TRUE(errorMessageContains(&errmsg, "whileExec"));
    EXPECT_TRUE(errorMessageContains(&errmsg, "modern"));
}

/**
 * Test 3: forEachExec emits deprecation warning
 */
TEST_F(LegacyConstructWarningTest, ForEachExecEmitsWarning) {
    Label loc = createLocation(20, 3, 150);
    const DeprecationInfo *info = getLegacyFunctionDeprecationInfo("forEachExec");
    
    ASSERT_NE(nullptr, info);
    emitDeprecationWarning(context, info, &loc);
    
    EXPECT_TRUE(errorMessageContains(&errmsg, "forEachExec"));
}

/**
 * Test 4: delayExec emits deprecation warning
 */
TEST_F(LegacyConstructWarningTest, DelayExecEmitsWarning) {
    Label loc = createLocation(25, 2, 200);
    const DeprecationInfo *info = getLegacyFunctionDeprecationInfo("delayExec");
    
    ASSERT_NE(nullptr, info);
    emitDeprecationWarning(context, info, &loc);
    
    EXPECT_TRUE(errorMessageContains(&errmsg, "delayExec"));
}

/**
 * Test 5: remoteExec emits deprecation warning
 */
TEST_F(LegacyConstructWarningTest, RemoteExecEmitsWarning) {
    Label loc = createLocation(30, 4, 250);
    const DeprecationInfo *info = getLegacyFunctionDeprecationInfo("remoteExec");
    
    ASSERT_NE(nullptr, info);
    emitDeprecationWarning(context, info, &loc);
    
    EXPECT_TRUE(errorMessageContains(&errmsg, "remoteExec"));
}

/**
 * Test 6: assign emits deprecation warning
 */
TEST_F(LegacyConstructWarningTest, AssignEmitsWarning) {
    Label loc = createLocation(35, 6, 300);
    const DeprecationInfo *info = getLegacyFunctionDeprecationInfo("assign");
    
    ASSERT_NE(nullptr, info);
    emitDeprecationWarning(context, info, &loc);
    
    EXPECT_TRUE(errorMessageContains(&errmsg, "assign"));
    EXPECT_TRUE(errorMessageContains(&errmsg, "direct assignment"));
}

/**
 * Test 7: ifExec emits deprecation warning
 */
TEST_F(LegacyConstructWarningTest, IfExecEmitsWarning) {
    Label loc = createLocation(40, 7, 350);
    const DeprecationInfo *info = getLegacyFunctionDeprecationInfo("ifExec");
    
    ASSERT_NE(nullptr, info);
    emitDeprecationWarning(context, info, &loc);
    
    EXPECT_TRUE(errorMessageContains(&errmsg, "ifExec"));
}

/**
 * Test 8: Warning contains version information
 */
TEST_F(LegacyConstructWarningTest, WarningContainsVersionInfo) {
    Label loc = createLocation(45, 9, 400);
    const DeprecationInfo *info = getLegacyFunctionDeprecationInfo("forExec");
    
    ASSERT_NE(nullptr, info);
    emitDeprecationWarning(context, info, &loc);
    
    EXPECT_TRUE(errorMessageContains(&errmsg, "1.0"));
    EXPECT_TRUE(errorMessageContains(&errmsg, "3.0"));
}

/**
 * Test 9: Warning contains reason
 */
TEST_F(LegacyConstructWarningTest, WarningContainsReason) {
    Label loc = createLocation(50, 10, 450);
    const DeprecationInfo *info = getLegacyFunctionDeprecationInfo("whileExec");
    
    ASSERT_NE(nullptr, info);
    emitDeprecationWarning(context, info, &loc);
    
    EXPECT_TRUE(
        errorMessageContains(&errmsg, "loop") ||
        errorMessageContains(&errmsg, "syntax") ||
        errorMessageContains(&errmsg, "replaced")
    );
}

/**
 * Test 10: Custom deprecation warning
 */
TEST_F(LegacyConstructWarningTest, CustomDeprecationWarning) {
    Label loc = createLocation(55, 11, 500);
    
    int msgsBefore = countErrorMessages(&errmsg);
    emitDeprecationWarningCustom(context, "legacyPattern", 
                                  "This custom pattern is deprecated", &loc);
    int msgsAfter = countErrorMessages(&errmsg);
    
    EXPECT_GT(msgsAfter, msgsBefore);
    EXPECT_TRUE(errorMessageContains(&errmsg, "legacyPattern"));
}

/**
 * Test 11: Multiple warnings accumulate
 */
TEST_F(LegacyConstructWarningTest, MultipleWarningsAccumulate) {
    Label loc1 = createLocation(10, 1, 100);
    Label loc2 = createLocation(20, 2, 200);
    Label loc3 = createLocation(30, 3, 300);
    
    const DeprecationInfo *info1 = getLegacyFunctionDeprecationInfo("forExec");
    const DeprecationInfo *info2 = getLegacyFunctionDeprecationInfo("whileExec");
    const DeprecationInfo *info3 = getLegacyFunctionDeprecationInfo("assign");
    
    emitDeprecationWarning(context, info1, &loc1);
    emitDeprecationWarning(context, info2, &loc2);
    emitDeprecationWarning(context, info3, &loc3);
    
    int count = countErrorMessages(&errmsg);
    EXPECT_GE(count, 3);
    EXPECT_TRUE(errorMessageContains(&errmsg, "forExec"));
    EXPECT_TRUE(errorMessageContains(&errmsg, "whileExec"));
    EXPECT_TRUE(errorMessageContains(&errmsg, "assign"));
}

/**
 * Test 12: Deprecation level is correct
 */
TEST_F(LegacyConstructWarningTest, DeprecationLevelCorrect) {
    const char *deprecated[] = {
        "forExec", "whileExec", "forEachExec", 
        "delayExec", "remoteExec", "assign", "ifExec"
    };
    
    for (const char *func : deprecated) {
        const DeprecationInfo *info = getLegacyFunctionDeprecationInfo(func);
        ASSERT_NE(nullptr, info);
        
        EXPECT_TRUE(
            info->level == DEPRECATION_WARNING || 
            info->level == DEPRECATION_ERROR
        ) << "Function " << func << " has incorrect level " << (int)info->level;
    }
}

/**
 * Test 13: Non-legacy functions don't trigger warnings
 */
TEST_F(LegacyConstructWarningTest, ModernFunctionsNoWarning) {
    const char *modern[] = {"for", "while", "foreach", "delay", "remote", "if"};
    
    for (const char *func : modern) {
        EXPECT_FALSE(isLegacySystemFunction(func))
            << "Modern function " << func << " should not be marked as legacy";
    }
}

/**
 * Test 14: Alternative suggestion is provided
 */
TEST_F(LegacyConstructWarningTest, AlternativeSuggestionProvided) {
    Label loc = createLocation(60, 12, 600);
    const DeprecationInfo *info = getLegacyFunctionDeprecationInfo("forExec");
    
    ASSERT_NE(nullptr, info);
    ASSERT_NE(nullptr, info->alternative);
    EXPECT_GT(strlen(info->alternative), 0);
    EXPECT_TRUE(strstr(info->alternative, "for") != nullptr);
    
    emitDeprecationWarning(context, info, &loc);
    EXPECT_TRUE(errorMessageContains(&errmsg, "Modern alternative"));
}

/**
 * Test 15: Null safety in warning emission
 */
TEST_F(LegacyConstructWarningTest, NullSafetyInWarningEmission) {
    Label loc = createLocation(65, 13, 650);
    const DeprecationInfo *info = getLegacyFunctionDeprecationInfo("forExec");
    
    /* Should not crash with null context */
    emitDeprecationWarning(nullptr, info, &loc);
    
    /* Should not crash with null info */
    emitDeprecationWarning(context, nullptr, &loc);
    
    /* Should not crash with null location */
    emitDeprecationWarning(context, info, nullptr);
}

/**
 * Test 16: Deprecation info contains complete metadata
 */
TEST_F(LegacyConstructWarningTest, DeprecationInfoComplete) {
    const DeprecationInfo *info = getLegacyFunctionDeprecationInfo("delayExec");
    
    ASSERT_NE(nullptr, info);
    EXPECT_NE(nullptr, info->name);
    EXPECT_NE(nullptr, info->since_version);
    EXPECT_NE(nullptr, info->removal_version);
    EXPECT_NE(nullptr, info->alternative);
    EXPECT_NE(nullptr, info->reason);
    EXPECT_GT(strlen(info->name), 0);
    EXPECT_GT(strlen(info->since_version), 0);
    EXPECT_GT(strlen(info->removal_version), 0);
    EXPECT_GT(strlen(info->alternative), 0);
    EXPECT_GT(strlen(info->reason), 0);
}
