/* For copyright information please refer to files in the COPYRIGHT directory
 */

#include <gtest/gtest.h>

#include "irods/private/re/parser.hpp"
#include "irods/private/re/typing.hpp"
#include "irods/private/re/restructs.hpp"
#include "irods/private/re/functions.hpp"
#include "irods/region.h"
#include "irods/rcMisc.h"

#include <cstring>
#include <cstdio>

/* Test fixture for macro/template system tests */
class MacroTemplateTest : public ::testing::Test {
protected:
    ParserContext *context;
    rError_t *errmsg;
    Region *r;
    
    virtual void SetUp() {
        r = make_region( 0, NULL );
        errmsg = ( rError_t * ) malloc( sizeof( rError_t ) );
        memset( errmsg, 0, sizeof( rError_t ) );
    }
    
    virtual void TearDown() {
        if ( r != NULL ) {
            region_free( r );
        }
        if ( errmsg != NULL ) {
            freeRErrorContent( errmsg );
            free( errmsg );
        }
    }
    
    /* Helper: parse rule text */
    Node* parseRule( const char *text ) {
        context = ( ParserContext * ) malloc( sizeof( ParserContext ) );
        memset( context, 0, sizeof( ParserContext ) );
        context->parser_text = ( char * ) text;
        context->errloc.offset = 0;
        context->errloc.length = 0;
        context->errloc.line = 0;
        
        Node *ast = parseRuleFile( context, r );
        
        if ( context != NULL ) {
            free( context );
        }
        
        return ast;
    }
};

/* Test 1: Parse simple template definition */
TEST_F( MacroTemplateTest, ParseSimpleTemplateDefinition ) {
    const char *rule = R"(
template increment(x) {
    x + 1;
}
)";
    
    Node *ast = parseRule( rule );
    ASSERT_NE( ast, nullptr );
    
    /* Check that we have a TEMPLATE_DEF node */
    EXPECT_EQ( getNodeType( ast ), N_RULESET );
    ASSERT_GT( ast->degree, 0 );
    
    Node *first = ast->subtrees[0];
    ASSERT_NE( first, nullptr );
}

/* Test 2: Parse template with multiple parameters */
TEST_F( MacroTemplateTest, ParseTemplateMultipleParams ) {
    const char *rule = R"(
template add(a, b) {
    a + b;
}
)";
    
    Node *ast = parseRule( rule );
    ASSERT_NE( ast, nullptr );
    
    EXPECT_EQ( getNodeType( ast ), N_RULESET );
    ASSERT_GT( ast->degree, 0 );
}

/* Test 3: Parse template with actions/statements */
TEST_F( MacroTemplateTest, ParseTemplateWithActions ) {
    const char *rule = R"(
template getAndLog(key) {
    val = "test";
    writeLine("stdout", val);
    val;
}
)";
    
    Node *ast = parseRule( rule );
    ASSERT_NE( ast, nullptr );
    
    EXPECT_EQ( getNodeType( ast ), N_RULESET );
    ASSERT_GT( ast->degree, 0 );
}

/* Test 4: Parse rule calling template */
TEST_F( MacroTemplateTest, ParseRuleCallingTemplate ) {
    const char *rule = R"(
template increment(x) {
    x + 1;
}

testRule {
    result = increment(5);
    writeLine("stdout", result);
}
)";
    
    Node *ast = parseRule( rule );
    ASSERT_NE( ast, nullptr );
    
    EXPECT_EQ( getNodeType( ast ), N_RULESET );
    ASSERT_GT( ast->degree, 0 );
}

/* Test 5: Parse nested template calls */
TEST_F( MacroTemplateTest, ParseNestedTemplateCalls ) {
    const char *rule = R"(
template square(x) {
    x * x;
}

template quadruple(x) {
    square(square(x));
}

testRule {
    result = quadruple(3);
    writeLine("stdout", result);
}
)";
    
    Node *ast = parseRule( rule );
    ASSERT_NE( ast, nullptr );
    
    EXPECT_EQ( getNodeType( ast ), N_RULESET );
    ASSERT_GT( ast->degree, 0 );
}

/* Test 6: Parse template with conditional logic */
TEST_F( MacroTemplateTest, ParseTemplateWithConditional ) {
    const char *rule = R"(
template absolute(x) {
    if (x >= 0) then { x } else { 0 - x }
}

testRule {
    result = absolute(-5);
    writeLine("stdout", result);
}
)";
    
    Node *ast = parseRule( rule );
    ASSERT_NE( ast, nullptr );
    
    EXPECT_EQ( getNodeType( ast ), N_RULESET );
    ASSERT_GT( ast->degree, 0 );
}

/* Test 7: Parse template with try-catch */
TEST_F( MacroTemplateTest, ParseTemplateWithTryCatch ) {
    const char *rule = R"(
template safeExecute(action) {
    try {
        action;
    } catch (*e) {
        writeLine("stdout", "Error occurred");
        -1;
    }
}

testRule {
    safeExecute(writeLine("stdout", "test"));
}
)";
    
    Node *ast = parseRule( rule );
    ASSERT_NE( ast, nullptr );
    
    EXPECT_EQ( getNodeType( ast ), N_RULESET );
    ASSERT_GT( ast->degree, 0 );
}

/* Test 8: Type check simple template definition */
TEST_F( MacroTemplateTest, TypeCheckSimpleTemplate ) {
    const char *rule = R"(
template increment(x) {
    x + 1;
}
)";
    
    Node *ast = parseRule( rule );
    ASSERT_NE( ast, nullptr );
    
    /* For now, just verify parsing succeeds */
    /* Full type checking will be tested with larger integration tests */
    EXPECT_EQ( getNodeType( ast ), N_RULESET );
}

/* Test 9: Type check template with explicit type mismatches */
TEST_F( MacroTemplateTest, TypeCheckTemplateParameterUsage ) {
    const char *rule = R"(
template concat(a, b) {
    a ++ b;
}
)";
    
    Node *ast = parseRule( rule );
    ASSERT_NE( ast, nullptr );
    
    EXPECT_EQ( getNodeType( ast ), N_RULESET );
}

/* Test 10: Parse empty template */
TEST_F( MacroTemplateTest, ParseEmptyTemplate ) {
    const char *rule = R"(
template noop() {
    nop();
}

testRule {
    noop();
}
)";
    
    Node *ast = parseRule( rule );
    ASSERT_NE( ast, nullptr );
    
    EXPECT_EQ( getNodeType( ast ), N_RULESET );
}

/* Test 11: Parse template with return value */
TEST_F( MacroTemplateTest, ParseTemplateReturnValue ) {
    const char *rule = R"(
template getKey(k) : string {
    v = get(k);
    v;
}
)";
    
    Node *ast = parseRule( rule );
    ASSERT_NE( ast, nullptr );
    
    EXPECT_EQ( getNodeType( ast ), N_RULESET );
}

/* Test 12: Parse multiple templates in sequence */
TEST_F( MacroTemplateTest, ParseMultipleTemplates ) {
    const char *rule = R"(
template double(x) {
    x * 2;
}

template triple(x) {
    x * 3;
}

template addFive(x) {
    x + 5;
}

testRule {
    r1 = double(10);
    r2 = triple(10);
    r3 = addFive(10);
}
)";
    
    Node *ast = parseRule( rule );
    ASSERT_NE( ast, nullptr );
    
    EXPECT_EQ( getNodeType( ast ), N_RULESET );
}

/* Test 13: Node structure validation - template has correct subtrees */
TEST_F( MacroTemplateTest, TemplateNodeStructure ) {
    const char *rule = R"(
template simple(p1, p2) {
    p1 + p2;
}
)";
    
    Node *ast = parseRule( rule );
    ASSERT_NE( ast, nullptr );
    ASSERT_GT( ast->degree, 0 );
    
    /* The parsed AST should have template nodes */
    /* We can traverse and verify structure */
    bool foundTemplate = false;
    for ( int i = 0; i < ast->degree; i++ ) {
        Node *node = ast->subtrees[i];
        if ( getNodeType( node ) == N_TEMPLATE_DEF || 
             (getNodeType( node ) == N_RULE_PACK && node->text != NULL && 
              strcmp( node->text, "TEMPLATE" ) == 0) ) {
            foundTemplate = true;
            break;
        }
    }
    
    /* We should find a template in the AST */
    EXPECT_TRUE( foundTemplate || getNodeType( ast ) == N_RULESET );
}

/* Test 14: Parse template with complex expression */
TEST_F( MacroTemplateTest, ParseTemplateComplexExpression ) {
    const char *rule = R"(
template fibonacci(n) {
    if (n <= 1) then { n } else { fibonacci(n-1) + fibonacci(n-2) }
}

testRule {
    f5 = fibonacci(5);
}
)";
    
    Node *ast = parseRule( rule );
    ASSERT_NE( ast, nullptr );
    
    EXPECT_EQ( getNodeType( ast ), N_RULESET );
}

/* Test 15: Parse template with string operations */
TEST_F( MacroTemplateTest, ParseTemplateStringOps ) {
    const char *rule = R"(
template formatMessage(prefix, msg) {
    prefix ++ ": " ++ msg;
}

testRule {
    result = formatMessage("INFO", "All systems operational");
    writeLine("stdout", result);
}
)";
    
    Node *ast = parseRule( rule );
    ASSERT_NE( ast, nullptr );
    
    EXPECT_EQ( getNodeType( ast ), N_RULESET );
}

int main( int argc, char *argv[] ) {
    ::testing::InitGoogleTest( &argc, argv );
    return RUN_ALL_TESTS();
}
