#ifndef __IRODS_TCP_OBJECT_HPP__
#define __IRODS_TCP_OBJECT_HPP__

/// \file

// =-=-=-=-=-=-=-
#include "irods/irods_network_object.hpp"

namespace irods {
/// @brief Constant key for the TCP network plugin.
    const std::string TCP_NETWORK_PLUGIN( "tcp" );

    /// @brief Network object implementation for TCP connections.
    class tcp_object : public network_object {
        public:
            // =-=-=-=-=-=-=-
            // Constructors
            /// @brief Default constructor.
            tcp_object();

            /// @brief Constructs a TCP object from a client connection.
            tcp_object( const rcComm_t& );

            /// @brief Constructs a TCP object from a server connection.
            tcp_object( const rsComm_t& );

            /// @brief Copy constructor.
            tcp_object( const tcp_object& );

            // =-=-=-=-=-=-=-
            // Destructors
            /// @brief Destructor.
            virtual ~tcp_object();

            // =-=-=-=-=-=-=-
            // Operators
            /// @brief Assignment operator.
            virtual tcp_object& operator=( const tcp_object& );

            /// @brief Comparison operator
            virtual bool operator==( const tcp_object& _rhs ) const;

            // =-=-=-=-=-=-=-
            /// @brief Resolves the named plugin.
            virtual error resolve(
                const std::string&, // plugin interface
                plugin_ptr& );      // resolved plugin instance

            // =-=-=-=-=-=-=-
            /// @brief Serializes this object into rule engine variables.
            virtual error get_re_vars( rule_engine_vars_t& );

            // =-=-=-=-=-=-=-
            // Accessors

        private:

    }; // class tcp_object

    /// @brief Shared pointer to a TCP object.
    typedef boost::shared_ptr< tcp_object > tcp_object_ptr;

}; // namespace irods

#endif // __IRODS_TCP_OBJECT_HPP__
