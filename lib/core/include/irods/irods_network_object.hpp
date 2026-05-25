#ifndef __IRODS_NETWORK_OBJECT_HPP__
#define __IRODS_NETWORK_OBJECT_HPP__

// =-=-=-=-=-=-=-
#include "irods/irods_first_class_object.hpp"

// =-=-=-=-=-=-=-
// irods includes
#include "irods/rcConnect.h"

// =-=-=-=-=-=-=-
// boost includes
#include <boost/shared_ptr.hpp>

namespace irods {
    /// @brief Base class for network-oriented first-class objects.
    class network_object : public first_class_object {
        public:
            /// @brief Constructs an empty network object.
            network_object();

            /// @brief Constructs a network object from a client connection.
            /// @details Input is a source client communication object.
            network_object( const rcComm_t& );

            /// @brief Constructs a network object from a server connection.
            /// @details Input is a source server communication object.
            network_object( const rsComm_t& );

            /// @brief Copies another network object.
            /// @details Input is the source object to copy.
            network_object( const network_object& );

            /// @brief Destroys the network object.
            virtual ~network_object();

            /// @brief Copies state from another network object.
            /// @details Input is the source object.
            /// @return Reference to this object.
            virtual network_object& operator=( const network_object& );

            // =-=-=-=-=-=-=-
            /// @brief Comparison operator
            virtual bool operator==( const network_object& _rhs ) const;

            /// @brief Resolves a plugin for the requested interface.
            /// @details Input is a plugin interface name. Output is a resolved plugin instance.
            /// @return Error status describing resolution success or failure.
            virtual error resolve(
                const std::string&, // plugin interface
                plugin_ptr& ) = 0;  // resolved plugin

            /// @brief Serializes this object into a client communication structure.
            /// @details Output is a populated client communication structure.
            /// @return Error status describing conversion success or failure.
            virtual error to_client( rcComm_t* );

            /// @brief Serializes this object into a server communication structure.
            /// @details Output is a populated server communication structure.
            /// @return Error status describing conversion success or failure.
            virtual error to_server( rsComm_t* );

            /// @brief Exposes rule-engine variables for this network object.
            /// @details Output is a populated rule-engine variable map.
            /// @return Error status describing extraction success or failure.
            virtual error get_re_vars( rule_engine_vars_t& );

            /// @brief Returns the associated socket descriptor.
            /// @return Socket handle value.
            virtual int socket_handle() const {
                return socket_handle_;
            }

            /// @brief Sets the associated socket descriptor.
            /// @param[in] _s Socket handle value.
            virtual void socket_handle( int _s ) {
                socket_handle_ = _s;
            }

        private:
            // =-=-=-=-=-=-=-
            // Attributes
            int socket_handle_; // socket descriptor

    }; // network_object

    /// @brief Shared-pointer type for `network_object`.
    typedef boost::shared_ptr< network_object > network_object_ptr;

}; // namespace irods

#endif // __IRODS_NETWORK_OBJECT_HPP__
