#include <catch2/catch_all.hpp>

#include <cstring>
#include <iostream>
#include <string>

#include "irods/irods_re_structs.hpp"
#include "irods/private/re/restructs.hpp"

// Functions are already declared in restructs.hpp, no need to redeclare

TEST_CASE("applyOptionalAnnotation sets the correct flag")
{
    struct node n = {};
    n.option = 0;  // Start with no options
    
    // Apply optional annotation
    applyOptionalAnnotation(&n);
    
    // Verify the flag was set
    CHECK(isOptionalType(&n));
    CHECK((n.option & OPTION_OPTIONAL_TYPE) == OPTION_OPTIONAL_TYPE);
}

TEST_CASE("applyNonnullAnnotation sets the correct flag")
{
    struct node node = {};
    node.option = 0;
    
    applyNonnullAnnotation(&node);
    
    CHECK(isNonnullType(&node));
    CHECK((node.option & OPTION_NONNULL_TYPE) == OPTION_NONNULL_TYPE);
}

TEST_CASE("applyOptionalAnnotation handles NULL pointers gracefully")
{
    // Should not crash with NULL pointer
    applyOptionalAnnotation(NULL);
    // If we reach here, it handled NULL gracefully
    SUCCEED();
}

TEST_CASE("applyNonnullAnnotation handles NULL pointers gracefully")
{
    applyNonnullAnnotation(NULL);
    SUCCEED();
}

TEST_CASE("getTypeAnnotationString returns @optional")
{
    struct node node = {};
    node.option = 0;
    applyOptionalAnnotation(&node);
    
    const char *annotation = getTypeAnnotationString(&node);
    CHECK(std::string(annotation) == "@optional");
}

TEST_CASE("getTypeAnnotationString returns @nonnull")
{
    struct node node = {};
    node.option = 0;
    applyNonnullAnnotation(&node);
    
    const char *annotation = getTypeAnnotationString(&node);
    CHECK(std::string(annotation) == "@nonnull");
}

TEST_CASE("getTypeAnnotationString returns empty for unannotated")
{
    struct node node = {};
    node.option = 0;  // No annotations
    
    const char *annotation = getTypeAnnotationString(&node);
    CHECK(std::string(annotation) == "");
}

TEST_CASE("getTypeAnnotationString handles NULL safely")
{
    const char *annotation = getTypeAnnotationString(NULL);
    CHECK(std::string(annotation) == "");
}

TEST_CASE("Both optional and nonnull flags cannot be set simultaneously")
{
    struct node node = {};
    node.option = 0;
    
    applyOptionalAnnotation(&node);
    CHECK(isOptionalType(&node));
    CHECK(!isNonnullType(&node));
    
    applyNonnullAnnotation(&node);
    CHECK(!isOptionalType(&node));
    CHECK(isNonnullType(&node));
}

TEST_CASE("Annotation flag operations are bitwise safe")
{
    struct node node = {};
    node.option = OPTION_VARARG_STAR;  // OPTION_VARARG_STAR is outside TYPE_MASK
    
    applyOptionalAnnotation(&node);
    
    // Optional flag should be set
    CHECK(isOptionalType(&node));
    
    // Other flags outside TYPE_MASK should be preserved
    CHECK((node.option & OPTION_VARARG_MASK) == OPTION_VARARG_STAR);
}

TEST_CASE("Multiple operations on same node")
{
    struct node node1 = {};
    node1.option = 0;
    applyOptionalAnnotation(&node1);
    CHECK(std::string(getTypeAnnotationString(&node1)) == "@optional");
    
    struct node node2 = {};
    node2.option = 0;
    applyNonnullAnnotation(&node2);
    CHECK(std::string(getTypeAnnotationString(&node2)) == "@nonnull");
}

TEST_CASE("Type annotation flag positions")
{
    // These tests verify the bit positions are as expected
    CHECK(OPTION_OPTIONAL_TYPE == 0x40);
    CHECK(OPTION_NONNULL_TYPE == 0x80);
    CHECK(OPTION_TYPE_MASK == 0xf0);
}

TEST_CASE("isOptionalType predicate correctness")
{
    struct node optional_node = {};
    optional_node.option = OPTION_OPTIONAL_TYPE;
    
    struct node nonnull_node = {};
    nonnull_node.option = OPTION_NONNULL_TYPE;
    
    struct node plain_node = {};
    plain_node.option = 0;
    
    CHECK(isOptionalType(&optional_node));
    CHECK(!isOptionalType(&nonnull_node));
    CHECK(!isOptionalType(&plain_node));
}

TEST_CASE("isNonnullType predicate correctness")
{
    struct node optional_node = {};
    optional_node.option = OPTION_OPTIONAL_TYPE;
    
    struct node nonnull_node = {};
    nonnull_node.option = OPTION_NONNULL_TYPE;
    
    struct node plain_node = {};
    plain_node.option = 0;
    
    CHECK(!isNonnullType(&optional_node));
    CHECK(isNonnullType(&nonnull_node));
    CHECK(!isNonnullType(&plain_node));
}

TEST_CASE("Annotation independence")
{
    // Create nodes with different annotation combinations
    struct node node1 = {};
    node1.option = 0;
    
    struct node node2 = {};
    node2.option = 0;
    
    applyOptionalAnnotation(&node1);
    applyNonnullAnnotation(&node2);
    
    // Modifying node1 shouldn't affect node2
    CHECK(isOptionalType(&node1));
    CHECK(!isOptionalType(&node2));
    CHECK(!isNonnullType(&node1));
    CHECK(isNonnullType(&node2));
}
