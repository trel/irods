#ifndef IRODS_EXCEPTION_HPP
#define IRODS_EXCEPTION_HPP

#include <cinttypes>
#include <exception>
#include <string>
#include <sstream>
#include <vector>
#include <boost/format.hpp>

#include "irods/irods_stacktrace.hpp"

namespace irods {

    /// Represents an iRODS exception with contextual diagnostic information.
    class exception : public std::exception {
        public:
            /// Constructs an exception from an error code and message string.
            exception(
                const int64_t      _code,
                const std::string& _message,
                const std::string& _file_name,
                const uint32_t     _line_number,
                const std::string& _function_name );

            /// Constructs an exception from an error code and formatted message.
            exception(
                const int64_t        _code,
                const boost::format& _message,
                const std::string&   _file_name,
                const uint32_t       _line_number,
                const std::string&   _function_name );

            /// Constructs an exception by copying another exception.
            exception( const exception& );

            /// Destroys the exception.
            virtual ~exception() throw();

            /// Returns the full exception message for internal use.
            virtual const char* what() const throw();

            /// Returns a client-facing exception message.
            virtual const char* client_display_what() const throw();

            // accessors
            int64_t    code() const { return code_; } ///< Returns the error code.
            std::vector< std::string > message_stack() const { return message_stack_; } ///< Returns the stacked diagnostic messages.
            std::string file_name() const { return file_name_; } ///< Returns the source file where the exception was created.
            uint32_t    line_number() const { return line_number_; } ///< Returns the source line where the exception was created.
            std::string function_name() const { return function_name_; } ///< Returns the source function where the exception was created.
            irods::stacktrace stacktrace() const { return stacktrace_; } ///< Returns the captured stack trace.

            // mutators
            /// Appends an additional message to the diagnostic stack.
            void add_message( const std::string& _m ) { message_stack_.push_back( _m ); }

        private:
            /// Assembles the internal full-display message buffer.
            void assemble_full_display_what() const throw();
            /// Assembles the internal client-display message buffer.
            void assemble_client_display_what() const throw();

        private:
            int64_t                    code_; ///< Error code associated with the exception.
            std::vector< std::string > message_stack_; ///< Accumulated diagnostic messages.
            uint32_t                   line_number_; ///< Source line number where the exception originated.
            std::string                function_name_; ///< Source function where the exception originated.
            std::string                file_name_; ///< Source file where the exception originated.
            irods::stacktrace          stacktrace_; ///< Captured stack trace.
            mutable std::string        what_; ///< Cached rendered message.

    }; // class exception


}; // namespace irods

/// Throws an `irods::exception` with the current source location.
#define THROW( _code, _msg ) ( throw irods::exception( _code, _msg, __FILE__, __LINE__, __PRETTY_FUNCTION__ ) ) // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
/// Appends a message to an exception and rethrows it.
#define RE_THROW( _msg, _excp ) _excp.add_message( _msg ); throw _excp; // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
/// Logs an `irods::exception` from an expression and returns its error code.
#define CATCH_EXC_AND_RETURN( throwing_expression ) try { throwing_expression; } catch (const irods::exception& e) { rodsLog( LOG_ERROR, e.code(), "%s encountered an exception on line %d in file %s:\n%s", __PRETTY_FUNCTION__, __LINE__, __FILE__, e.what()); return e.code() } // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
/// Logs an `irods::exception` from an expression and suppresses it.
#define CATCH_EXC_AND_LOG( throwing_expression ) try { throwing_expression; } catch (const irods::exception& e) { rodsLog( LOG_ERROR, e.code(), "%s encountered an exception on line %d in file %s:\n%s", __PRETTY_FUNCTION__, __LINE__, __FILE__, e.what()); } // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)

#endif // IRODS_EXCEPTION_HPP
