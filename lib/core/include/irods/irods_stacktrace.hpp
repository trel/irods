#ifndef IRODS_STACKTRACE_HPP
#define IRODS_STACKTRACE_HPP

#include <string>

namespace irods
{
    /// @brief Provides access to a textual stack trace.
    class stacktrace final
    {
    public:
        /// @brief Returns the current stack trace as a string.
        std::string dump() const;
    }; // class stacktrace
} // namespace irods

#endif // IRODS_STACKTRACE_HPP
