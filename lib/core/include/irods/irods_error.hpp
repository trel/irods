#ifndef __IRODS_ERROR_HPP__
#define __IRODS_ERROR_HPP__

// =-=-=-=-=-=-=-
// STL Includes
#include <string>
#include <vector>
#include <cstdarg>

#include <boost/optional.hpp>

// =-=-=-=-=-=-=-
// irods includes
#include "irods/rodsType.h"
#include "irods/irods_exception.hpp"

#include <boost/assert.hpp>
#include <boost/format.hpp>

namespace irods {
/// =-=-=-=-=-=-=-
/// @brief error stack object which holds error history
    class error {
        public:
            // =-=-=-=-=-=-=-
            // Constructors
            /// Constructs a default success/failure object.
            error();

            /// Constructs an error object with explicit status information.
            /// Inputs: status flag, error code, message, source file, source line, and source function.
            error(
                bool,          // status
                long long,     // error code
                std::string,   // message
                std::string,   // file name
                int,           // line number
                std::string ); // function

            /// Constructs an error object with a formatted message.
            /// Inputs: status flag, error code, formatted message, source file, source line, and source function.
            error(
                bool,          // status
                long long,     // error code
                boost::format, // message
                std::string,   // file name
                int,           // line number
                std::string ); // function

            /// Constructs an error object and appends a previous error.
            /// Inputs: status flag, error code, message, source file, source line, source function, and previous error.
            error(                  // deprecated since 4.0.3
                bool,           // status
                long long,      // error code
                std::string,    // message
                std::string,    // file name
                int,            // line number
                std::string,    // function
                const error& ); // previous error

            /// Constructs an error object from a message and previous error.
            /// Inputs: message, source file, source line, source function, and previous error.
            error(
                std::string,    // message
                std::string,    // file name
                int,            // line number
                std::string,    // function
                const error& ); // previous error

            /// Constructs an error object by copying another error.
            /// Input: the error to copy.
            error( const error& );

            /// Constructs an error object from an exception.
            /// Input: the exception to translate.
            error( const exception& );

            // =-=-=-=-=-=-=-
            // Destructor
            /// Destroys the error object.
            ~error();

            // =-=-=-=-=-=-=-
            // Operators
            /// Assigns the contents of another error object.
            /// Input: the error to copy.
            /// \return A reference to this object.
            error& operator=( const error& );

            // =-=-=-=-=-=-=-
            // Members
            /// Returns the status flag.
            /// \return \c true if the error represents success; otherwise, \c false.
            bool        status() const;

            /// Returns the stored error code.
            /// \return The numeric error code.
            long long   code() const;

            /// Returns the full formatted result string.
            /// \return The full error result string.
            std::string result() const;

            /// Returns the user-facing result string.
            /// \return The user-facing error result string.
            std::string user_result() const;

            /// Returns the status flag.
            /// \return \c true if the error represents success; otherwise, \c false.
            bool        ok();          // deprecated since 4.0.3

            /// Returns the status flag.
            /// \return \c true if the error represents success; otherwise, \c false.
            bool        ok() const;

            // =-=-=-=-=-=-=-
            // Mutators
            /// Updates the stored error code.
            /// \param[in] _code The new error code.
            void code( long long _code ) {
                code_   = _code;
            }

            /// Updates the status flag.
            /// \param[in] _status The new status value.
            void status( bool      _status ) {
                status_ = _status;
            }

            /// Updates the error message.
            /// \param[in] _message The new message string.
            void message( const std::string& _message ) {
                message_ = _message;
            }

        private:
            // =-=-=-=-=-=-=-
            // Attributes
            bool        status_;
            long long   code_;
            std::string message_;
            std::vector< std::string > result_stack_;
            boost::optional< exception > exception_;

            // These are hard-coded strings that are used in creating
            // the strings that go in result_stack_, and that are searched
            // for as such.  See static declarations in irods_error.cpp for initialization.
            static const char *iRODS_token_;
            static const char *colon_token_;
            static const char *status_token_;

            // =-=-=-=-=-=-=-
            // Members
            /// Builds a formatted stack entry.
            /// Inputs: source file name, source line number, and source function name.
            /// \return The formatted stack entry.
            std::string build_result_string( std::string, int, std::string );

    }; // class error
}; // namespace irods

#define ERROR( code_, message_ ) ( irods::error( false, code_, message_, __FILE__, __LINE__, __PRETTY_FUNCTION__ ) ) // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
#define PASS( prev_error_ ) (irods::error( "", __FILE__, __LINE__, __PRETTY_FUNCTION__, prev_error_ ) ) // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
#define PASSMSG( message_, prev_error_ ) (irods::error( message_, __FILE__, __LINE__, __PRETTY_FUNCTION__, prev_error_ ) ) // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
#define CODE( code_ ) ( irods::error( true, code_, "", __FILE__, __LINE__, __PRETTY_FUNCTION__ ) ) // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
#define SUCCESS( ) ( irods::error( true, 0, "", __FILE__, __LINE__, __PRETTY_FUNCTION__ ) ) // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)

#endif // __IRODS_ERROR_HPP__
