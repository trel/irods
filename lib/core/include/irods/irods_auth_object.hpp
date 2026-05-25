#ifndef _AUTH_OBJECT_HPP_
#define _AUTH_OBJECT_HPP_

#include "irods/irods_error.hpp"
#include "irods/irods_first_class_object.hpp"

#include "irods/rcConnect.h"

// boost includes
#include <boost/shared_ptr.hpp>

namespace irods {
    /**
     * @brief Class for representing authorization scheme objects
     */
    class auth_object : public first_class_object {
        public:
            /// @brief Constructs an authorization object.
            /// @param[in] _r_error Error stack associated with the request.
            auth_object( rError_t* _r_error );

            /// @brief Copies another authorization object.
            /// @details Input is the source object to copy.
            auth_object( const auth_object& );

            /// @brief Destroys the authorization object.
            virtual ~auth_object();

            /// @brief Copies state from another authorization object.
            /// @details Input is the source object.
            /// @return Reference to this object.
            virtual auth_object&  operator=( const auth_object& _rhs );

            /// @brief Returns the error stack associated with this object.
            /// @return Stored `rError_t` pointer.
            virtual rError_t* r_error( void ) const {
                return r_error_;
            }

            /// @brief Returns the authentication result for the outgoing response.
            /// @return Result string such as a challenge or password.
            virtual const std::string& request_result() const {
                return request_result_;
            }

            /// @brief Sets the authentication result for the outgoing response.
            /// @param[in] _r Result string to store.
            virtual void request_result( const std::string& _r ) {
                request_result_ = _r;
            }

            /// @brief Returns contextual data associated with the request.
            /// @return Stored context string.
            virtual const std::string& context() const {
                return context_;
            }

            /// @brief Sets contextual data associated with the request.
            /// @param[in] _c Context string to store.
            virtual void context( const std::string& _c ) {
                context_ = _c;
            }

            /// @brief Returns the client user name.
            /// @return Stored user name.
            virtual const std::string& user_name() const {
                return user_name_;
            }

            /// @brief Returns the client zone name.
            /// @return Stored zone name.
            virtual const std::string& zone_name() const {
                return zone_name_;
            }

            /// @brief Sets the client user name.
            /// @param[in] _un User name to store.
            virtual void user_name( const std::string& _un ) {
                user_name_ = _un;
            }

            /// @brief Sets the client zone name.
            /// @param[in] _zn Zone name to store.
            virtual void zone_name( const std::string& _zn ) {
                zone_name_ = _zn;
            }

            /// @brief Comparison operator
            virtual bool operator==( const auth_object& _rhs ) const;

            /// @brief Plugin resolution operation
            virtual error resolve( const std::string& _plugin_name, plugin_ptr& _plugin ) = 0;

            /// @brief Exposes rule-engine variables for this authorization object.
            /// @details Output is a populated rule-engine variable map.
            /// @return Error status describing extraction success or failure.
            virtual error get_re_vars( rule_engine_vars_t& ) = 0;

        protected:
            rError_t*   r_error_;

            /// =-=-=-=-=-=-=-
            // result passed to outgoing auth request
            // struct back to client - challenge for native,
            // password for pam etc
            std::string request_result_;

            /// =-=-=-=-=-=-=-
            /// @brief user name - from rcConn
            std::string user_name_;

            /// =-=-=-=-=-=-=-
            /// @brief zone name - from rcConn
            std::string zone_name_;

            /// =-=-=-=-=-=-=-
            /// @brief context string which might hold anything
            ///        coming from a client call
            std::string context_;
    };

/// @brief Helpful typedef
    typedef boost::shared_ptr<auth_object> auth_object_ptr;

}; // namespace irods

#endif // _AUTH_OBJECT_HPP_
