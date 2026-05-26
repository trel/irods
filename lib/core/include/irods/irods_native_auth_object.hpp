#ifndef __NATIVE_AUTH_OBJECT_HPP__
#define __NATIVE_AUTH_OBJECT_HPP__

#include "irods/irods_error.hpp"
#include "irods/irods_auth_object.hpp"

#include "irods/rcConnect.h"

// =-=-=-=-=-=-=-
// boost includes
#include <boost/shared_ptr.hpp>

namespace irods {

/// =-=-=-=-=-=-=-
/// @brief constant defining the native auth scheme string
    const std::string AUTH_NATIVE_SCHEME( "native" );

/// =-=-=-=-=-=-=-
/// @brief object for a native irods authentication scheme
    class native_auth_object : public auth_object {
        public:
            /// @brief Constructs a native authentication object.
            native_auth_object( rError_t* _r_error );

            /// @brief Copy-constructs a native authentication object.
            native_auth_object( const native_auth_object& );

            /// @brief Destroys the native authentication object.
            virtual ~native_auth_object();

            /// @brief Copies state from another native authentication object.
            virtual native_auth_object&  operator=( const native_auth_object& );

            /// =-=-=-=-=-=-=-
            /// @brief Comparison operator
            virtual bool operator==( const native_auth_object& ) const;

            /// =-=-=-=-=-=-=-
            /// @brief Plugin resolution operation
            virtual error resolve(
                const std::string&, // interface for which to resolve
                plugin_ptr& );      // ptr to resolved plugin

            /// @brief Populates rule-engine variables for this object.
            virtual error get_re_vars( rule_engine_vars_t& );

            /// @brief Returns the computed digest.
            const std::string& digest() const {
                return digest_;
            }

            /// @brief Sets the connection associated with this object.
            auto rcComm(RcComm* _comm) noexcept -> void
            {
                comm_ = _comm;
            }

            /// @brief Returns the connection associated with this object.
            [[nodiscard]] auto rcComm() const noexcept -> RcComm*
            {
                return comm_;
            }

            /// @brief Sets the computed digest.
            void digest( const std::string& _dd ) {
                digest_ = _dd;
            }

        private:
            /// =-=-=-=-=-=-=-
            /// @brief md5 digest computed
            std::string digest_;

            /// @brief Connection associated with the authentication request.
            RcComm* comm_ = nullptr;

    }; // class native_auth_object

    /// @brief Shared-pointer type for `native_auth_object` instances.
    typedef boost::shared_ptr<native_auth_object> native_auth_object_ptr;

}; // namespace irods

#endif // __NATIVE_AUTH_OBJECT_HPP__
