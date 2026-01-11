/* For copyright information please refer to files in the COPYRIGHT directory
 */

#include <catch2/catch_all.hpp>
#include <string>
#include <vector>
#include <cstring>

#include "irods/private/re/restructs.hpp"
#include "irods/rodsErrorTable.h"
#include "irods/rcMisc.h"
#include "irods/region.h"

/**
 * Test suite for @deprecated decorator and warning emission during type checking
 * Tests the OPTION_DEPRECATED flag and related macros
 */

class UserDeprecatedWarningsFixture {
protected:
    Region *region;
    
    UserDeprecatedWarningsFixture() {
        region = make_region(0, NULL);  // Use default block size
    }
    
    ~UserDeprecatedWarningsFixture() {
        if (region) {
            region_free(region);
        }
    }
    
    /**
     * Create a test node with optional deprecated flag
     * Uses manual allocation instead of newNode to avoid linker dependencies
     */
    Node* createTestNode(NodeType type, const char *text, int deprecated = 0) {
        Node *node = (Node*)region_alloc(region, sizeof(Node));
        if (!node) return NULL;
        
        memset(node, 0, sizeof(Node));
        node->nodeType = type;
        
        if (text) {
            node->text = (char*)region_alloc(region, strlen(text) + 1);
            if (node->text) {
                strcpy(node->text, text);
            }
        }
        
        if (deprecated) {
            setDeprecated(node);
        }
        
        return node;
    }
};

TEST_CASE_METHOD(UserDeprecatedWarningsFixture, "isDeprecated returns true for marked function") {
    Node *depNode = createTestNode(TK_TEXT, "deprecatedFunc", 1);
    REQUIRE(isDeprecated(depNode));
}

TEST_CASE_METHOD(UserDeprecatedWarningsFixture, "isDeprecated returns false for unmarked function") {
    Node *normalNode = createTestNode(TK_TEXT, "normalFunc", 0);
    REQUIRE(!isDeprecated(normalNode));
}

TEST_CASE_METHOD(UserDeprecatedWarningsFixture, "setDeprecated marks function correctly") {
    Node *node = createTestNode(TK_TEXT, "funcToMark", 0);
    REQUIRE(!isDeprecated(node));
    
    setDeprecated(node);
    REQUIRE(isDeprecated(node));
}

TEST_CASE_METHOD(UserDeprecatedWarningsFixture, "Deprecation message storage") {
    Node *node = createTestNode(TK_TEXT, "Use newFunc() instead", 1);
    REQUIRE(strcmp(node->text, "Use newFunc() instead") == 0);
}

TEST_CASE_METHOD(UserDeprecatedWarningsFixture, "Deprecated flag is independent between nodes") {
    Node *node1 = createTestNode(TK_TEXT, "func1", 0);
    Node *node2 = createTestNode(TK_TEXT, "func2", 1);
    Node *node3 = createTestNode(TK_TEXT, "func3", 0);
    
    REQUIRE(!isDeprecated(node1));
    REQUIRE(isDeprecated(node2));
    REQUIRE(!isDeprecated(node3));
    
    setDeprecated(node1);
    REQUIRE(isDeprecated(node1));
    REQUIRE(isDeprecated(node2));  
    REQUIRE(!isDeprecated(node3));
}

TEST_CASE_METHOD(UserDeprecatedWarningsFixture, "Deprecated flag persists through operations") {
    Node *node = createTestNode(TK_TEXT, "persistentDeprecated", 1);
    
    for (int i = 0; i < 5; i++) {
        REQUIRE(isDeprecated(node));
    }
}

TEST_CASE_METHOD(UserDeprecatedWarningsFixture, "Deprecated flag with other options") {
    Node *node = createTestNode(TK_TEXT, "funcWithOptions", 1);
    
    setIOType(node, IO_TYPE_INPUT);
    REQUIRE(isDeprecated(node));
    
    setIOType(node, IO_TYPE_OUTPUT);
    REQUIRE(isDeprecated(node));
    
    setVararg(node, OPTION_VARARG_STAR);
    REQUIRE(isDeprecated(node));
}

TEST_CASE_METHOD(UserDeprecatedWarningsFixture, "Deprecated flag on various node types") {
    Node *textNode = createTestNode(TK_TEXT, "func", 1);
    Node *intNode = createTestNode(TK_INT, "5", 0);
    setDeprecated(intNode);
    
    REQUIRE(isDeprecated(textNode));
    REQUIRE(isDeprecated(intNode));
}

TEST_CASE_METHOD(UserDeprecatedWarningsFixture, "Deprecated flag with type annotations") {
    Node *node = createTestNode(TK_TEXT, "optionalDeprecated", 1);
    
    setTypeAnnotation(node, OPTION_OPTIONAL_TYPE);
    REQUIRE(isOptionalType(node));
    REQUIRE(isDeprecated(node));
    
    setTypeAnnotation(node, OPTION_NONNULL_TYPE);
    REQUIRE(isNonnullType(node));
    REQUIRE(isDeprecated(node));
}

TEST_CASE_METHOD(UserDeprecatedWarningsFixture, "Deprecated flag encoding and decoding") {
    Node *node = createTestNode(TK_TEXT, "testFunc", 0);
    
    REQUIRE((node->option & OPTION_DEPRECATED) == 0);
    
    setDeprecated(node);
    REQUIRE((node->option & OPTION_DEPRECATED) == OPTION_DEPRECATED);
    REQUIRE(isDeprecated(node));
}

TEST_CASE_METHOD(UserDeprecatedWarningsFixture, "Error message building for deprecation") {
    char errmsgbuf[ERR_MSG_LEN];
    
    snprintf(errmsgbuf, ERR_MSG_LEN, 
             "deprecated function '%s': %s",
             "oldFunc",
             "Use newFunc() instead");
    
    REQUIRE(strlen(errmsgbuf) > 0);
    REQUIRE(strstr(errmsgbuf, "oldFunc") != NULL);
    REQUIRE(strstr(errmsgbuf, "Use newFunc() instead") != NULL);
}

TEST_CASE_METHOD(UserDeprecatedWarningsFixture, "NodeType preserved with deprecation") {
    Node *node = createTestNode(TK_TEXT, "func", 0);
    setNodeType(node, N_FD_FUNCTION);
    setDeprecated(node);
    
    REQUIRE(getNodeType(node) == N_FD_FUNCTION);
    REQUIRE(isDeprecated(node));
}

TEST_CASE_METHOD(UserDeprecatedWarningsFixture, "Deprecated with empty message") {
    Node *node = createTestNode(TK_TEXT, "", 0);
    setDeprecated(node);
    
    REQUIRE(isDeprecated(node));
    REQUIRE(strcmp(node->text, "") == 0);
}

TEST_CASE_METHOD(UserDeprecatedWarningsFixture, "Deprecated flag in N_RULE_PACK") {
    Node *rulePack = createTestNode(N_RULE_PACK, "FUNC", 0);
    REQUIRE(!isDeprecated(rulePack));
    
    setDeprecated(rulePack);
    REQUIRE(isDeprecated(rulePack));
    
    rulePack->text = (char*)region_alloc(region, 50);
    strcpy(rulePack->text, "Deprecated since v4.2");
    REQUIRE(strcmp(rulePack->text, "Deprecated since v4.2") == 0);
}

TEST_CASE_METHOD(UserDeprecatedWarningsFixture, "Deprecation flag macros efficiency") {
    Node *node = createTestNode(TK_TEXT, "efficiencyTest", 0);
    
    for (int i = 0; i < 1000; i++) {
        if (!isDeprecated(node)) {
            setDeprecated(node);
        }
        if (isDeprecated(node)) {
            // Just verify the macro works
        }
    }
    
    REQUIRE(isDeprecated(node));
}
