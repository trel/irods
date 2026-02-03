#include "irods/irods_exec_rule_text_guard.hpp"

namespace irods {

// Thread-local counter tracking the depth of exec_rule_text() calls in the callstack
thread_local int exec_rule_text_depth = 0;

} // namespace irods
