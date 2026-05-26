#ifndef ___IRODS_NETWORK_PLUGIN_HPP__
#define ___IRODS_NETWORK_PLUGIN_HPP__

/// \file
/// \brief Declares the base type for network plugins.

// =-=-=-=-=-=-=-
#include "irods/irods_plugin_base.hpp"
#include "irods/irods_network_types.hpp"

#include <iostream>

namespace irods {

// =-=-=-=-=-=-=-
    /**
     * \author Jason M. Coposky
     * \brief Base class for network plugins.
     **/
    class network : public plugin_base {
        public:
            /// \brief Constructs a network plugin instance.
            network(
                    const std::string& _inst,
                    const std::string& _ctx ) :
                plugin_base(
                        _inst,
                        _ctx ) {
                } // ctor

            /// \brief Destroys the network plugin.
            virtual ~network( ) {
            } // dtor

            /// \brief Copy-constructs a network plugin.
            network(const network& _rhs) :
                plugin_base( _rhs ) {
                } // cctor

            /// \brief Assigns one network plugin to another.
            network& operator=(
                    const network& _rhs ) {
                if ( &_rhs == this ) {
                    return *this;
                }

                plugin_base::operator=( _rhs );

                return *this;

            } // operator=

    }; // class network

}; // namespace irods


#endif // ___IRODS_NETWORK_PLUGIN_HPP__

