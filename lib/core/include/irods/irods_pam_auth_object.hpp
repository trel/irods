#ifndef __PAM_AUTH_OBJECT_HPP__
#define __PAM_AUTH_OBJECT_HPP__

#include "irods/irods_error.hpp"
#include "irods/irods_auth_object.hpp"

#include "irods/rcConnect.h"

// =-=-=-=-=-=-=-
// boost includes
#include <boost/shared_ptr.hpp>

namespace irods {

/// =-=-=-=-=-=-=-
/// @brief constant defining the PAM auth scheme string
    const std::string AUTH_PAM_SCHEME( "pam" );

/// =-=-=-=-=-=-=-
/// @brief object for a PAM irods authentication scheme
    class pam_auth_object : public auth_object {
        public:
            /// @brief Constructs a PAM authentication object.
            pam_auth_object( rError_t* _r_error );

            /// @brief Destroys the PAM authentication object.
            virtual ~pam_auth_object();

            /// @brief Copy-constructs a PAM authentication object.
            pam_auth_object( const pam_auth_object& );

            /// @brief Copies state from another PAM authentication object.
            virtual pam_auth_object&  operator=( const pam_auth_object& );

            /// =-=-=-=-=-=-=-
            /// @brief Comparison operator
            virtual bool operator==( const pam_auth_object& ) const;

            /// =-=-=-=-=-=-=-
            /// @brief Plugin resolution operation
            virtual error resolve(
                const std::string&, // interface for which to resolve
                plugin_ptr& );      // ptr to resolved plugin

            /// @brief Populates rule-engine variables for this object.
            virtual error get_re_vars( rule_engine_vars_t& );

        private:

    }; // class pam_auth_object

    /// @brief Shared-pointer type for `pam_auth_object` instances.
    typedef boost::shared_ptr<pam_auth_object> pam_auth_object_ptr;

}; // namespace irods

#endif // __PAM_AUTH_OBJECT_HPP__
