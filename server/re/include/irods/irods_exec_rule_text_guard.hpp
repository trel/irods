#ifndef IRODS_EXEC_RULE_TEXT_GUARD_HPP
#define IRODS_EXEC_RULE_TEXT_GUARD_HPP

namespace irods {

/// Thread-local counter tracking the depth of exec_rule_text() calls in the callstack
extern thread_local int exec_rule_text_depth;

/// RAII guard that increments exec_rule_text_depth on construction
/// and decrements on destruction.
/// 
/// Usage:
///     {
///         exec_rule_text_guard guard;
///         // ... execute rule text code ...
///     } // guard destructor called, depth decremented
class exec_rule_text_guard {
public:
    /// Constructor: increments the exec_rule_text_depth counter
    exec_rule_text_guard() noexcept {
        ++exec_rule_text_depth;
    }

    /// Destructor: decrements the exec_rule_text_depth counter
    ~exec_rule_text_guard() noexcept {
        --exec_rule_text_depth;
    }

    // Prevent copying
    exec_rule_text_guard(const exec_rule_text_guard&) = delete;
    exec_rule_text_guard& operator=(const exec_rule_text_guard&) = delete;

    // Allow moving
    exec_rule_text_guard(exec_rule_text_guard&&) = default;
    exec_rule_text_guard& operator=(exec_rule_text_guard&&) = default;
};

/// Returns true if exec_rule_text() is currently active in the callstack
/// (i.e., if exec_rule_text_depth > 0)
inline bool is_exec_rule_text_active() noexcept {
    return exec_rule_text_depth > 0;
}

} // namespace irods

#endif // IRODS_EXEC_RULE_TEXT_GUARD_HPP
