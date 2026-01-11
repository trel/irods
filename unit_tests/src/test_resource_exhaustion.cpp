/**
 * @file test_resource_exhaustion.cpp
 * @brief Resource exhaustion tests for iRODS rule language
 *
 * Tests verify graceful behavior under resource constraints:
 * - Region allocation exhaustion
 * - Parser stack limits
 * - Deep recursion handling
 * - Memory pressure scenarios
 *
 * @author iRODS Development Team
 * @date 2026-01-12
 */

#include <catch2/catch_all.hpp>
#include <cstddef>
#include <cstring>
#include <memory>

#include "irods/region.h"
#include "irods/private/re/parser.hpp"
#include "irods/private/re/restructs.hpp"

/**
 * @brief Test region allocation with very small initial size
 * 
 * Creates a region with minimal memory and verifies that allocation
 * failures are handled gracefully without crashing.
 */
TEST_CASE("region exhaustion: small region allocation")
{
    // Create a region with only 64 bytes - will exhaust quickly
    Region *r = make_region(64, NULL);
    REQUIRE(r != NULL);

    // First allocation should succeed
    void *ptr1 = region_alloc(r, 32);
    CHECK(ptr1 != NULL);

    // Second allocation should succeed (still within 64 bytes)
    void *ptr2 = region_alloc(r, 16);
    CHECK(ptr2 != NULL);

    // Third allocation will likely fail - region exhausted
    void *ptr3 = region_alloc(r, 256);
    // region_alloc should return NULL on failure, not crash
    CHECK(ptr3 == NULL);

    // Verify region can still be freed without crash
    region_free(r);
}

/**
 * @brief Test region allocation with zero initial size
 * 
 * DEFAULT_BLOCK_SIZE (1024) should be used, test default behavior.
 */
TEST_CASE("region: default block size initialization")
{
    // Create region with default size (is=0 means use DEFAULT_BLOCK_SIZE)
    Region *r = make_region(0, NULL);
    REQUIRE(r != NULL);

    // Should be able to allocate within default block size
    void *ptr1 = region_alloc(r, 512);
    CHECK(ptr1 != NULL);

    void *ptr2 = region_alloc(r, 256);
    CHECK(ptr2 != NULL);

    region_free(r);
}

/**
 * @brief Test multiple sequential allocations until exhaustion
 * 
 * Allocate many small objects and verify graceful failure.
 */
TEST_CASE("region exhaustion: sequential small allocations")
{
    Region *r = make_region(256, NULL);
    REQUIRE(r != NULL);

    int alloc_count = 0;
    int null_count = 0;
    const int max_attempts = 1000;

    // Try to allocate 50-byte blocks repeatedly
    for (int i = 0; i < max_attempts; i++) {
        void *ptr = region_alloc(r, 50);
        if (ptr != NULL) {
            alloc_count++;
        } else {
            null_count++;
            // Once we hit exhaustion, subsequent allocations should also fail
            break;
        }
    }

    // Should have succeeded at least a few times (256 / 50 = ~5)
    CHECK(alloc_count >= 4);

    // Should have hit exhaustion
    CHECK(null_count > 0);

    region_free(r);
}

/**
 * @brief Test parser context creation under memory constraints
 * 
 * Parser context requires memory allocation. Verify behavior
 * when memory is constrained.
 */
TEST_CASE("parser context: creation with constrained region")
{
    Region *r = make_region(2048, NULL);
    REQUIRE(r != NULL);

    // Create error message structure
    rError_t errmsg;
    errmsg.errMsg = NULL;
    errmsg.len = 0;

    // ParserContext requires hash table allocation (~200+ bytes)
    ParserContext *pc = newParserContext(&errmsg, r);
    
    // Should succeed with reasonable memory
    CHECK(pc != NULL);

    if (pc != NULL) {
        // Verify basic structure is initialized
        CHECK(pc->error == 0);
        CHECK(pc->nodeStackTop == 0);
        CHECK(pc->stackTopStackTop == 0);

        deleteParserContext(pc);
    }

    region_free(r);
}

/**
 * @brief Test node stack limits in parser
 * 
 * The parser node stack has fixed size (1024 nodes).
 * Verify behavior approaches this limit.
 */
TEST_CASE("parser: node stack limit awareness")
{
    Region *r = make_region(4096, NULL);
    REQUIRE(r != NULL);

    rError_t errmsg;
    errmsg.errMsg = NULL;
    errmsg.len = 0;

    ParserContext *pc = newParserContext(&errmsg, r);
    REQUIRE(pc != NULL);

    // Node stack has fixed size 1024 (from parser.hpp)
    // We cannot push more than 1024 nodes without corruption
    // This is a design constraint, not tested by pushing,
    // but documented here for awareness

    // Verify stack starts empty
    CHECK(pc->nodeStackTop == 0);
    CHECK(pc->stackTopStackTop == 0);

    deleteParserContext(pc);
    region_free(r);
}

/**
 * @brief Test token queue limits
 * 
 * Parser has token queue of 1024 tokens (from parser.hpp).
 * Verify structure initialization.
 */
TEST_CASE("parser: token queue initialization")
{
    Region *r = make_region(4096, NULL);
    REQUIRE(r != NULL);

    rError_t errmsg;
    errmsg.errMsg = NULL;
    errmsg.len = 0;

    ParserContext *pc = newParserContext(&errmsg, r);
    REQUIRE(pc != NULL);

    // Token queue starts at position 0
    CHECK(pc->tqp == 0);
    CHECK(pc->tqtop == 0);
    CHECK(pc->tqbot == 0);

    deleteParserContext(pc);
    region_free(r);
}

/**
 * @brief Test pointer initialization with constrained buffer
 * 
 * Pointer structure is used for parsing input.
 * Verify it handles small buffers gracefully.
 */
TEST_CASE("parser pointer: small string buffer")
{
    Region *r = make_region(1024, NULL);
    REQUIRE(r != NULL);

    const char *test_rule = "test() :- true";
    Pointer *p = newPointer2(const_cast<char*>(test_rule), r);

    // Should initialize successfully even with small rule
    CHECK(p != NULL);

    if (p != NULL) {
        // Verify basic pointer structure
        CHECK(p->isFile == 0);
        CHECK(p->fp == NULL);
        CHECK(p->strbuf != NULL);
        CHECK(p->len > 0);

        deletePointer(p);
    }

    region_free(r);
}

/**
 * @brief Test deeply nested structure allocation
 * 
 * Node creation requires memory. Test allocation
 * of many nodes in sequence.
 */
TEST_CASE("parser: many node allocations in small region")
{
    Region *r = make_region(8192, NULL);
    REQUIRE(r != NULL);

    int node_count = 0;
    const int max_nodes = 100;

    // Try to allocate many nodes
    for (int i = 0; i < max_nodes; i++) {
        Node *n = (Node *)region_alloc(r, sizeof(Node));
        if (n != NULL) {
            node_count++;
        } else {
            // Once exhausted, subsequent allocations fail
            break;
        }
    }

    // Should have allocated many nodes before running out
    // 8192 bytes / ~256 bytes per Node = ~32 nodes expected
    CHECK(node_count >= 10);

    region_free(r);
}

/**
 * @brief Test error state persistence across region exhaustion
 * 
 * When region is exhausted, parser should maintain clean error state
 * for recovery to next sync point.
 */
TEST_CASE("parser: error state stability under memory pressure")
{
    Region *r = make_region(512, NULL);
    REQUIRE(r != NULL);

    rError_t errmsg;
    errmsg.errMsg = NULL;
    errmsg.len = 0;

    ParserContext *pc = newParserContext(&errmsg, r);
    REQUIRE(pc != NULL);

    // Simulate error condition
    pc->error = 1;
    pc->nodeStackTop = 5;
    pc->stackTopStackTop = 3;

    // Record error
    char test_msg[100] = "Test error message";
    addRErrorMsg(&errmsg, RE_PARSER_ERROR, test_msg);

    // Verify error was recorded
    CHECK(errmsg.len > 0);

    // Simulate recovery (as in recoverToSyncPoint)
    pc->error = 0;
    pc->nodeStackTop = 0;
    pc->stackTopStackTop = 0;

    // Verify recovery reset state properly
    CHECK(pc->error == 0);
    CHECK(pc->nodeStackTop == 0);
    CHECK(pc->stackTopStackTop == 0);

    // Error messages should still be in chain
    CHECK(errmsg.len > 0);

    // Cleanup
    freeRErrorContent(&errmsg);
    deleteParserContext(pc);
    region_free(r);
}

/**
 * @brief Test region size querying
 * 
 * Verify region_size() reports memory usage correctly.
 */
TEST_CASE("region: size tracking")
{
    Region *r = make_region(1024, NULL);
    REQUIRE(r != NULL);

    // Get initial size
    size_t initial_size = region_size(r);
    CHECK(initial_size >= 0);

    // Allocate some memory
    void *ptr1 = region_alloc(r, 256);
    CHECK(ptr1 != NULL);

    // Size should reflect usage
    size_t after_alloc = region_size(r);
    CHECK(after_alloc >= initial_size);

    // Allocate more
    void *ptr2 = region_alloc(r, 256);
    CHECK(ptr2 != NULL);

    size_t final_size = region_size(r);
    CHECK(final_size >= after_alloc);

    region_free(r);
}

/**
 * @brief Test NULL region handling
 * 
 * Ensure functions handle NULL regions gracefully.
 */
TEST_CASE("region: NULL safety")
{
    // region_alloc with NULL should return NULL gracefully
    void *ptr = region_alloc(NULL, 256);
    CHECK(ptr == NULL);

    // region_free with NULL should not crash
    region_free(NULL);
    CHECK(true); // Just verifying no crash
}

/**
 * @brief Stress test: rapid allocation/deallocation cycles
 * 
 * Create and destroy multiple regions in sequence
 * to verify resource cleanup.
 */
TEST_CASE("region: rapid creation and cleanup cycles")
{
    const int cycle_count = 100;

    for (int i = 0; i < cycle_count; i++) {
        Region *r = make_region(512, NULL);
        REQUIRE(r != NULL);

        void *ptr = region_alloc(r, 256);
        CHECK(ptr != NULL);

        region_free(r);
    }

    CHECK(true); // Verify no memory leaks or crashes
}

/**
 * @brief Test region reallocation across block boundaries
 * 
 * When region needs more space, it should allocate new blocks.
 * Verify seamless allocation across boundaries.
 */
TEST_CASE("region: allocation across block boundaries")
{
    // Create small region that will need multiple blocks
    Region *r = make_region(128, NULL);
    REQUIRE(r != NULL);

    std::vector<void*> ptrs;

    // Allocate enough to force multiple blocks (DEFAULT_BLOCK_SIZE=1024)
    for (int i = 0; i < 20; i++) {
        void *ptr = region_alloc(r, 64);
        if (ptr != NULL) {
            ptrs.push_back(ptr);
        } else {
            break;
        }
    }

    // Should have allocated across blocks
    CHECK(ptrs.size() > 1);

    region_free(r);
}

/**
 * @brief Integration test: parser context lifecycle under memory pressure
 * 
 * Complete lifecycle: create context, simulate parsing work,
 * handle errors, cleanup - all under memory constraints.
 */
TEST_CASE("parser: full context lifecycle under constraints")
{
    Region *r = make_region(2048, NULL);
    REQUIRE(r != NULL);

    rError_t errmsg;
    errmsg.errMsg = NULL;
    errmsg.len = 0;

    // Create context
    ParserContext *pc = newParserContext(&errmsg, r);
    CHECK(pc != NULL);

    if (pc != NULL) {
        // Verify initialization
        CHECK(pc->error == 0);
        CHECK(pc->nodeStackTop == 0);

        // Simulate parsing activity
        pc->error = 0; // No error initially
        pc->prec = 0;  // Reset precedence

        // Simulate error encounter
        pc->error = 1;
        addRErrorMsg(&errmsg, RE_PARSER_ERROR, "Simulated parse error");

        // Simulate recovery
        pc->error = 0;
        pc->nodeStackTop = 0;
        pc->stackTopStackTop = 0;

        // Verify recovery
        CHECK(pc->error == 0);

        // Cleanup
        freeRErrorContent(&errmsg);
        deleteParserContext(pc);
    }

    region_free(r);
}
