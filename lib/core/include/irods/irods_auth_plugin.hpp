#ifndef _AUTH_HPP_
#define _AUTH_HPP_

/// \file
/// \brief Declares the base type for authentication plugins.

#include "irods/irods_error.hpp"
#include "irods/irods_auth_types.hpp"
#include "irods/irods_load_plugin.hpp"
#include <dlfcn.h>

#include <utility>
#include <boost/any.hpp>

namespace irods {

    /**
     * @brief Base class for auth plugins
     */
    class auth : public plugin_base {
        public:
            /// \brief Constructs an authentication plugin instance.
            auth(
                const std::string& _inst,
                const std::string& _ctx ) :
                plugin_base( _inst, _ctx ) {

            }

            /// \brief Destroys the authentication plugin.
            virtual ~auth() {
            }

            /// \brief Copy-constructs an authentication plugin.
            auth(
                const auth& _rhs ) :
                plugin_base( _rhs ) {
            }

            /// \brief Assigns one authentication plugin to another.
            auth& operator=(
                const auth& _rhs ) {
                if ( &_rhs == this ) {
                    return *this;
                }

                plugin_base::operator=( _rhs );

                return *this;
            }
    };

}; // namespace irods

#endif // _AUTH_HPP_
