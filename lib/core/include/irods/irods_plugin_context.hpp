#ifndef __IRODS_PLUGIN_CONTEXT_HPP__
#define __IRODS_PLUGIN_CONTEXT_HPP__

// =-=-=-=-=-=-=-
// stl includes
#include <string>
#include <boost/pointer_cast.hpp>

// =-=-=-=-=-=-=-
#include "irods/rodsErrorTable.h"
#include "irods/irods_lookup_table.hpp"
#include "irods/irods_first_class_object.hpp"

namespace irods {

    /// @brief Carries communication state and objects into plugin operations.
    class plugin_context {
        public:
            /// @brief Constructs a context without a server communication handle.
            /// @param[in] _prop_map Plugin property map.
            /// @param[in] _fco First-class object associated with the operation.
            /// @param[in] _results Rule-engine results string.
            plugin_context(
                irods::plugin_property_map& _prop_map,
                first_class_object_ptr      _fco,
                const std::string&          _results )  :
                comm_( nullptr ),
                prop_map_( &_prop_map ),
                fco_( _fco ),
                results_( _results )  {

            } // ctor

            /// @brief Constructs a context with an explicit server communication handle.
            /// @param[in] _comm Server communication handle.
            /// @param[in] _prop_map Plugin property map.
            /// @param[in] _fco First-class object associated with the operation.
            /// @param[in] _results Rule-engine results string.
            plugin_context(
                rsComm_t*                   _comm,
                irods::plugin_property_map& _prop_map,
                first_class_object_ptr      _fco,
                const std::string&          _results )  :
                comm_( _comm ),
                prop_map_( &_prop_map ),
                fco_( _fco ),
                results_( _results )  {

            } // ctor

            /// @brief Replaces this context using copy-and-swap semantics.
            /// @param[in] other Source context copy.
            /// @return Reference to this context.
            plugin_context& operator=( plugin_context other ) {
                swap(*this, other);
                return *this;
            }

            /// @brief Constructs a context with only connection and properties.
            /// @param[in] _comm Server communication handle.
            /// @param[in] _prop_map Plugin property map.
            plugin_context(
                rsComm_t* _comm,
                irods::plugin_property_map& _prop_map ) :
                comm_( _comm ),
                prop_map_( &_prop_map ) {
            }

            /// @brief Swaps the contents of two plugin contexts.
            /// @param[in,out] first First context.
            /// @param[in,out] second Second context.
            friend void swap(plugin_context& first, plugin_context& second) // nothrow
            {
                std::swap(first.comm_, second.comm_);
                std::swap(first.fco_, second.fco_);
                std::swap(first.prop_map_, second.prop_map_);
                std::swap(first.results_, second.results_);
            }

            /// @brief Destroys the plugin context.
            virtual ~plugin_context() {

            } // dtor

            /// @brief Validates the current context state.
            /// @return Success unless a derived class reports an error.
            virtual error valid() {
                return SUCCESS();

            } // valid

            /// @brief Validates the context and first-class object type.
            /// @tparam OBJ_TYPE Expected first-class object type.
            /// @return Error for an invalid cast, otherwise `valid()`.
            template < typename OBJ_TYPE >
            error valid() {
                // trap case of incorrect type for first class object
                return boost::dynamic_pointer_cast< OBJ_TYPE >( fco_.get() ) == NULL ?
                       ERROR( INVALID_DYNAMIC_CAST, "invalid type for fco cast" ) :
                       valid();

            } // valid

            /// @brief Returns the server communication handle.
            /// @return Stored `rsComm_t` pointer.
            virtual rsComm_t* comm() {
                return comm_;
            }

            /// @brief Returns the plugin property map.
            /// @return Reference to the stored property map.
            virtual irods::plugin_property_map&   prop_map()     {
                return *prop_map_;
            }

            /// @brief Returns the associated first-class object.
            /// @return Shared pointer to the stored first-class object.
            virtual first_class_object_ptr fco()          {
                return fco_;
            }

            /// @brief Returns rule-engine results captured for the operation.
            /// @return Stored results string.
            virtual const std::string      rule_results() {
                return results_;
            }

            /// @brief Sets the server communication handle.
            /// @param[in] _c Communication handle to store.
            virtual void comm( rsComm_t* _c ) {
                comm_ = _c;
            }

            /// @brief Sets the stored rule-engine results string.
            /// @param[in] _s Results string to store.
            virtual void rule_results( const std::string& _s ) {
                results_ = _s;
            }

        protected:
            // =-=-=-=-=-=-=-
            // attributes
            rsComm_t*                   comm_;      ///< Server communication handle.
            irods::plugin_property_map* prop_map_;  ///< Plugin property map.
            first_class_object_ptr      fco_;       ///< First-class object bound to the operation.
            std::string                 results_;   ///< Results returned by the pre-operation rule.

    }; // class plugin_context

    /// @brief Function-pointer type for generic plugin operations.
    typedef error( *plugin_operation )( plugin_context&, ... );

}; // namespace irods

#endif // __IRODS_PLUGIN_CONTEXT_HPP__
