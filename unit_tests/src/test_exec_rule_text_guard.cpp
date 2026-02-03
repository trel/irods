#include <catch2/catch_all.hpp>

#include "irods/irods_exec_rule_text_guard.hpp"

namespace irods {

// Forward declare the external thread-local variable for testing
extern thread_local int exec_rule_text_depth;

} // namespace irods

TEST_CASE("exec_rule_text_guard: is_exec_rule_text_active returns false by default")
{
    // Reset the depth to ensure clean state
    irods::exec_rule_text_depth = 0;
    
    REQUIRE(!irods::is_exec_rule_text_active());
    REQUIRE(irods::exec_rule_text_depth == 0);
}

TEST_CASE("exec_rule_text_guard: guard increments and decrements counter")
{
    irods::exec_rule_text_depth = 0;
    
    REQUIRE(!irods::is_exec_rule_text_active());
    REQUIRE(irods::exec_rule_text_depth == 0);
    
    {
        irods::exec_rule_text_guard guard;
        
        REQUIRE(irods::is_exec_rule_text_active());
        REQUIRE(irods::exec_rule_text_depth == 1);
    }
    
    // After scope exit, depth should be decremented
    REQUIRE(!irods::is_exec_rule_text_active());
    REQUIRE(irods::exec_rule_text_depth == 0);
}

TEST_CASE("exec_rule_text_guard: nested guards work correctly")
{
    irods::exec_rule_text_depth = 0;
    
    REQUIRE(irods::exec_rule_text_depth == 0);
    
    {
        irods::exec_rule_text_guard guard1;
        REQUIRE(irods::exec_rule_text_depth == 1);
        REQUIRE(irods::is_exec_rule_text_active());
        
        {
            irods::exec_rule_text_guard guard2;
            REQUIRE(irods::exec_rule_text_depth == 2);
            REQUIRE(irods::is_exec_rule_text_active());
            
            {
                irods::exec_rule_text_guard guard3;
                REQUIRE(irods::exec_rule_text_depth == 3);
                REQUIRE(irods::is_exec_rule_text_active());
            }
            
            REQUIRE(irods::exec_rule_text_depth == 2);
            REQUIRE(irods::is_exec_rule_text_active());
        }
        
        REQUIRE(irods::exec_rule_text_depth == 1);
        REQUIRE(irods::is_exec_rule_text_active());
    }
    
    REQUIRE(irods::exec_rule_text_depth == 0);
    REQUIRE(!irods::is_exec_rule_text_active());
}

TEST_CASE("exec_rule_text_guard: multiple guards in same scope")
{
    irods::exec_rule_text_depth = 0;
    
    {
        irods::exec_rule_text_guard guard1;
        REQUIRE(irods::exec_rule_text_depth == 1);
        
        irods::exec_rule_text_guard guard2;
        REQUIRE(irods::exec_rule_text_depth == 2);
        
        irods::exec_rule_text_guard guard3;
        REQUIRE(irods::exec_rule_text_depth == 3);
    }
    
    REQUIRE(irods::exec_rule_text_depth == 0);
    REQUIRE(!irods::is_exec_rule_text_active());
}

TEST_CASE("exec_rule_text_guard: guard is exception-safe (RAII semantics)")
{
    irods::exec_rule_text_depth = 0;
    
    // Guard should decrement even if exception is thrown
    try {
        irods::exec_rule_text_guard guard;
        REQUIRE(irods::exec_rule_text_depth == 1);
        
        throw std::runtime_error("Test exception");
    }
    catch (const std::runtime_error&) {
        // Exception caught, guard should have decremented already due to RAII
    }
    
    REQUIRE(irods::exec_rule_text_depth == 0);
    REQUIRE(!irods::is_exec_rule_text_active());
}

TEST_CASE("exec_rule_text_guard: guard is exception-safe with nested guards")
{
    irods::exec_rule_text_depth = 0;
    
    try {
        irods::exec_rule_text_guard guard1;
        REQUIRE(irods::exec_rule_text_depth == 1);
        
        {
            irods::exec_rule_text_guard guard2;
            REQUIRE(irods::exec_rule_text_depth == 2);
            
            throw std::runtime_error("Nested exception");
        }
    }
    catch (const std::runtime_error&) {
        // Both guards should have decremented
    }
    
    REQUIRE(irods::exec_rule_text_depth == 0);
    REQUIRE(!irods::is_exec_rule_text_active());
}

TEST_CASE("exec_rule_text_guard: copying is deleted")
{
    irods::exec_rule_text_guard guard;
    
    // These should not compile (compile-time check)
    // auto guard_copy = guard;  // deleted copy constructor
    // guard = guard;             // deleted copy assignment
    
    // At runtime, we can only verify the type has these deleted
    STATIC_REQUIRE(!std::is_copy_constructible_v<irods::exec_rule_text_guard>);
    STATIC_REQUIRE(!std::is_copy_assignable_v<irods::exec_rule_text_guard>);
}

TEST_CASE("exec_rule_text_guard: moving is allowed")
{
    // Move operations should be available (either defaulted or explicit)
    STATIC_REQUIRE(std::is_move_constructible_v<irods::exec_rule_text_guard>);
    STATIC_REQUIRE(std::is_move_assignable_v<irods::exec_rule_text_guard>);
}

TEST_CASE("exec_rule_text_guard: is_exec_rule_text_active only true when depth > 0")
{
    irods::exec_rule_text_depth = 0;
    REQUIRE(!irods::is_exec_rule_text_active());
    
    irods::exec_rule_text_depth = 1;
    REQUIRE(irods::is_exec_rule_text_active());
    
    irods::exec_rule_text_depth = 5;
    REQUIRE(irods::is_exec_rule_text_active());
    
    irods::exec_rule_text_depth = 0;
    REQUIRE(!irods::is_exec_rule_text_active());
    
    // Reset for other tests
    irods::exec_rule_text_depth = 0;
}

TEST_CASE("exec_rule_text_guard: complex nested scenario")
{
    irods::exec_rule_text_depth = 0;
    
    // Simulate a complex call hierarchy
    {
        irods::exec_rule_text_guard level1;
        REQUIRE(irods::exec_rule_text_depth == 1);
        REQUIRE(irods::is_exec_rule_text_active());
        
        // Some other code that doesn't use guard
        int dummy = 42;
        (void)dummy;
        
        {
            irods::exec_rule_text_guard level2;
            REQUIRE(irods::exec_rule_text_depth == 2);
            
            {
                irods::exec_rule_text_guard level3;
                REQUIRE(irods::exec_rule_text_depth == 3);
                
                // Simulate nested exception handling
                try {
                    irods::exec_rule_text_guard level4;
                    REQUIRE(irods::exec_rule_text_depth == 4);
                    
                    throw std::logic_error("Inner error");
                }
                catch (const std::logic_error&) {
                    // level4 guard destroyed by exception
                    REQUIRE(irods::exec_rule_text_depth == 3);
                }
                
                REQUIRE(irods::exec_rule_text_depth == 3);
            }
            
            REQUIRE(irods::exec_rule_text_depth == 2);
        }
        
        REQUIRE(irods::exec_rule_text_depth == 1);
    }
    
    REQUIRE(irods::exec_rule_text_depth == 0);
    REQUIRE(!irods::is_exec_rule_text_active());
}
