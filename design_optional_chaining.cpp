/**
 * Unit tests for optional chaining operator (?.) AST structure
 * 
 * These tests verify that the parser correctly builds AST nodes for
 * optional chaining syntax (design phase only - no execution).
 */

#include <catch2/catch_all.hpp>
#include <cstring>
#include <iostream>

#include "irods/irods_re_structs.hpp"
#include "irods/private/re/parser.hpp"
#include "irods/region.h"

// Forward declarations for parser functions
extern "C" {
    Node *parseTermRuleGen(Pointer *expr, int rulegen, ParserContext *pc);
    ParserContext *newParserContext(rError_t *errmsg, Region *r);
    Pointer *newPointer2(char *buf, Region *r);
}

class OptionalChainingParserTest {
public:
    OptionalChainingParserTest() {
        // Initialize region for memory allocation
        region_init(&region);
        errmsg = (rError_t *)malloc(sizeof(rError_t));
        errmsg->status = 0;
        errmsg->errMsg = NULL;
    }

    ~OptionalChainingParserTest() {
        region_free(&region);
        free(errmsg);
    }

    /**
     * Helper: Parse a simple expression and return AST node
     */
    Node *parseExpression(const char *exprStr) {
        Pointer *ptr = newPointer2((char *)exprStr, &region);
        ParserContext *pc = newParserContext(errmsg, &region);
        
        if (!pc) return NULL;
        
        Node *result = parseTermRuleGen(ptr, 0, pc);
        
        // Check for parsing errors
        if (pc->error != 0 || result == NULL) {
            return NULL;
        }
        
        return result;
    }

    /**
     * Helper: Check if node is optional chaining by structure
     */
    int isOptionalChainingNode(Node *node) {
        if (!node) return 0;
        
        // Optional chaining nodes should have:
        // - degree >= 2 (base object + access expression)
        // - text indicating operation type
        return (node->degree >= 2) &&
               (node->text != NULL) &&
               (strcmp(node->text, "optionalAccess") == 0 ||
                strcmp(node->text, "optionalIndex") == 0 ||
                strcmp(node->text, "optionalCall") == 0);
    }

private:
    Region region;
    rError_t *errmsg;
};

// ============================================================================
// TEST CASES
// ============================================================================

TEST_CASE("OptionalChaining: Basic field access (obj?.field)")
{
    OptionalChainingParserTest test;
    
    // Simple optional field access
    Node *ast = test.parseExpression("obj?.field");
    
    REQUIRE(ast != NULL);
    REQUIRE(ast->degree >= 2);
    
    // Optional chaining nodes have text describing operation
    // This is structure verification only
    CHECK(ast->subtrees[0] != NULL);  // base object
    CHECK(ast->subtrees[1] != NULL);  // field access
}

TEST_CASE("OptionalChaining: Chained field access (obj?.a?.b?.c)")
{
    OptionalChainingParserTest test;
    
    Node *ast = test.parseExpression("obj?.a?.b?.c");
    
    REQUIRE(ast != NULL);
    REQUIRE(ast->degree >= 2);
    
    // Chained accesses should create nested structure
    // Outer node should have chained inner node as base
    CHECK(ast->subtrees[0] != NULL);  // should be another chain or object
    if (ast->subtrees[0] != NULL && ast->subtrees[0]->degree >= 2) {
        // Recursively nested structure
        CHECK(ast->subtrees[0]->subtrees[0] != NULL);
    }
}

TEST_CASE("OptionalChaining: Index access (obj?[0])")
{
    OptionalChainingParserTest test;
    
    Node *ast = test.parseExpression("arr?[0]");
    
    REQUIRE(ast != NULL);
    REQUIRE(ast->degree >= 2);
    
    // Index access variant
    CHECK(ast->subtrees[0] != NULL);  // base array
    CHECK(ast->subtrees[1] != NULL);  // index expression
}

TEST_CASE("OptionalChaining: Mixed access (obj?.field[index]?.name)")
{
    OptionalChainingParserTest test;
    
    Node *ast = test.parseExpression("obj?.field[0]?.name");
    
    REQUIRE(ast != NULL);
    REQUIRE(ast->degree >= 2);
    
    // Complex chaining with mixed operators
    CHECK(ast->subtrees[0] != NULL);
    CHECK(ast->subtrees[1] != NULL);
}

TEST_CASE("OptionalChaining: Method call (obj?.method())")
{
    OptionalChainingParserTest test;
    
    Node *ast = test.parseExpression("obj?.method()");
    
    REQUIRE(ast != NULL);
    REQUIRE(ast->degree >= 2);
    
    // Method call variant
    CHECK(ast->subtrees[0] != NULL);  // base object
    CHECK(ast->subtrees[1] != NULL);  // method/args
}

TEST_CASE("OptionalChaining: Method with args (obj?.method(a, b))")
{
    OptionalChainingParserTest test;
    
    Node *ast = test.parseExpression("obj?.method(arg1, arg2)");
    
    REQUIRE(ast != NULL);
    REQUIRE(ast->degree >= 2);
    
    // Method with multiple arguments
    CHECK(ast->subtrees[0] != NULL);
    CHECK(ast->subtrees[1] != NULL);
}

TEST_CASE("OptionalChaining: In expression (obj?.x + 5)")
{
    OptionalChainingParserTest test;
    
    Node *ast = test.parseExpression("obj?.x + 5");
    
    REQUIRE(ast != NULL);
    
    // Optional chaining should be operand in binary expression
    // Check that structure allows optional chain as left operand
    if (ast->degree >= 2) {
        CHECK(ast->subtrees[0] != NULL);
    }
}

TEST_CASE("OptionalChaining: In condition (if (obj?.x == 5))")
{
    OptionalChainingParserTest test;
    
    // Parse just the condition expression
    Node *ast = test.parseExpression("obj?.x == 5");
    
    REQUIRE(ast != NULL);
    
    // Comparison operation with optional chaining operand
    CHECK(ast->degree >= 2);
    CHECK(ast->subtrees[0] != NULL);  // left side (optional chain)
}

TEST_CASE("OptionalChaining: Regular field access for contrast (obj.field)")
{
    OptionalChainingParserTest test;
    
    Node *ast = test.parseExpression("obj.field");
    
    REQUIRE(ast != NULL);
    REQUIRE(ast->degree >= 2);
    
    // Regular field access (not optional chaining)
    // Structure is similar but operator is different
    CHECK(ast->subtrees[0] != NULL);
    CHECK(ast->subtrees[1] != NULL);
}

TEST_CASE("OptionalChaining: Null coalescing (obj?.x || default)")
{
    OptionalChainingParserTest test;
    
    Node *ast = test.parseExpression("obj?.x || defaultValue");
    
    REQUIRE(ast != NULL);
    
    // Optional chain as left operand of ||
    // Verify structure accepts optional chain in this position
    if (ast->degree >= 2) {
        CHECK(ast->subtrees[0] != NULL);  // optional chain
        CHECK(ast->subtrees[1] != NULL);  // default value
    }
}

TEST_CASE("OptionalChaining: Variable binding (x = obj?.field)")
{
    OptionalChainingParserTest test;
    
    Node *ast = test.parseExpression("x = obj?.field");
    
    REQUIRE(ast != NULL);
    REQUIRE(ast->degree >= 2);
    
    // Assignment with optional chain on RHS
    CHECK(ast->subtrees[0] != NULL);  // assignment target
    CHECK(ast->subtrees[1] != NULL);  // optional chain expression
}

TEST_CASE("OptionalChaining: Nested tuple (obj1?.x, obj2?.y)")
{
    OptionalChainingParserTest test;
    
    Node *ast = test.parseExpression("(obj1?.x, obj2?.y)");
    
    REQUIRE(ast != NULL);
    
    // Tuple containing optional chains
    // Verify structure allows optional chains in tuple elements
    if (ast->degree >= 2) {
        CHECK(ast->subtrees[0] != NULL);
    }
}

TEST_CASE("OptionalChaining: Deeply nested (a?.b?.c?.d?.e?.f?.g)")
{
    OptionalChainingParserTest test;
    
    Node *ast = test.parseExpression("a?.b?.c?.d?.e?.f?.g");
    
    REQUIRE(ast != NULL);
    REQUIRE(ast->degree >= 2);
    
    // Very deep nesting should still parse
    CHECK(ast->subtrees[0] != NULL);
    
    // Recursively verify nested structure is preserved
    Node *current = ast;
    int depth = 0;
    while (current != NULL && depth < 8) {
        if (current->degree >= 2) {
            current = current->subtrees[0];
            depth++;
        } else {
            break;
        }
    }
    
    // Should have traversed at least 5 levels deep
    CHECK(depth >= 5);
}

TEST_CASE("OptionalChaining: Array literal with optional elements (arr = [obj?.x, obj?.y])")
{
    OptionalChainingParserTest test;
    
    Node *ast = test.parseExpression("[obj?.x, obj?.y]");
    
    REQUIRE(ast != NULL);
    
    // Array/collection with optional chain elements
    // Verify collection accepts optional chains
}

TEST_CASE("OptionalChaining: No error on valid syntax")
{
    OptionalChainingParserTest test;
    
    Node *ast = test.parseExpression("obj?.field?.method()?.value");
    
    // Valid optional chaining should parse without error
    CHECK(ast != NULL);
}

TEST_CASE("OptionalChaining: Comparison of two optional chains (a?.x == b?.y)")
{
    OptionalChainingParserTest test;
    
    Node *ast = test.parseExpression("a?.x == b?.y");
    
    REQUIRE(ast != NULL);
    REQUIRE(ast->degree >= 2);
    
    // Binary operation with optional chains on both sides
    CHECK(ast->subtrees[0] != NULL);  // left optional chain
    CHECK(ast->subtrees[1] != NULL);  // right optional chain
}

TEST_CASE("OptionalChaining: Optional on optional type (optional?.field)")
{
    OptionalChainingParserTest test;
    
    // Parse with optional-typed variable (syntax only)
    Node *ast = test.parseExpression("opt_var?.field");
    
    REQUIRE(ast != NULL);
    REQUIRE(ast->degree >= 2);
    
    // Syntactically valid regardless of base type
    CHECK(ast->subtrees[0] != NULL);
}

// ============================================================================
// EDGE CASE TESTS
// ============================================================================

TEST_CASE("OptionalChaining: Single-character field name (obj?.a)")
{
    OptionalChainingParserTest test;
    
    Node *ast = test.parseExpression("obj?.a");
    
    REQUIRE(ast != NULL);
    REQUIRE(ast->degree >= 2);
}

TEST_CASE("OptionalChaining: Multi-word expression (obj?.get_field())")
{
    OptionalChainingParserTest test;
    
    Node *ast = test.parseExpression("obj?.getField()");
    
    REQUIRE(ast != NULL);
    REQUIRE(ast->degree >= 2);
}

TEST_CASE("OptionalChaining: Numeric index (arr?[42])")
{
    OptionalChainingParserTest test;
    
    Node *ast = test.parseExpression("arr?[42]");
    
    REQUIRE(ast != NULL);
    REQUIRE(ast->degree >= 2);
}

TEST_CASE("OptionalChaining: Variable index (arr?[i])")
{
    OptionalChainingParserTest test;
    
    Node *ast = test.parseExpression("arr?[i]");
    
    REQUIRE(ast != NULL);
    REQUIRE(ast->degree >= 2);
}

TEST_CASE("OptionalChaining: Expression index (arr?[i+1])")
{
    OptionalChainingParserTest test;
    
    Node *ast = test.parseExpression("arr?[i+1]");
    
    REQUIRE(ast != NULL);
    REQUIRE(ast->degree >= 2);
}

TEST_CASE("OptionalChaining: No spaces (obj?.field)")
{
    OptionalChainingParserTest test;
    
    Node *ast = test.parseExpression("obj?.field");
    
    REQUIRE(ast != NULL);
}

TEST_CASE("OptionalChaining: Parenthesized base ((obj)?.field)")
{
    OptionalChainingParserTest test;
    
    Node *ast = test.parseExpression("(obj)?.field");
    
    REQUIRE(ast != NULL);
    REQUIRE(ast->degree >= 2);
}
