#ifndef ___IRODS_RESC_PLUGIN_HPP__
#define ___IRODS_RESC_PLUGIN_HPP__

/// \file

// =-=-=-=-=-=-=-
#include "irods/irods_file_object.hpp"
#include "irods/irods_plugin_base.hpp"
#include "irods/irods_resource_constants.hpp"
#include "irods/irods_resource_types.hpp"

#include <iostream>
#include <utility>
#include <boost/any.hpp>

namespace irods {

    /// \brief Property key for the child resource map.
    const std::string RESC_CHILD_MAP_PROP( "resource_child_map_property" );
    /// \brief Property key for the parent resource pointer.
    const std::string RESC_PARENT_PROP( "resource_parent_property" );
    /// \brief Lookup table type used to store child resources by name.
    typedef lookup_table< std::pair< std::string, resource_ptr > > resource_child_map;

// =-=-=-=-=-=-=-
    /**
     * \author Jason M. Coposky
     * \brief Base class for iRODS resource plugins.
     *
     * Stores parent and child resource relationships and exposes helper
     * functions used by resource plugin implementations.
     **/
    class resource : public plugin_base {
        public:
            // =-=-=-=-=-=-=-
            /**
             * @brief Constructs a resource plugin wrapper.
             * @param[in] _inst Plugin instance name.
             * @param[in] _ctx Plugin context string.
             * @post Initializes child and parent properties for property lookups.
             */
            resource(
                const std::string& _inst,
                const std::string& _ctx ) :
                    plugin_base(
                        _inst,
                        _ctx ) {
                properties_.set(
                    RESC_CHILD_MAP_PROP,
                    &children_ );
                properties_.set(
                    RESC_PARENT_PROP,
                    parent_ );
            } // ctor

            // =-=-=-=-=-=-=-
            /**
             * @brief Destroys the resource object.
             */
            virtual ~resource() {
            }

            // =-=-=-=-=-=-=-
            /**
             * @brief Copies a resource object.
             * @param[in] _rhs Resource object to copy.
             */
            resource( const resource& _rhs ) :
              plugin_base{ _rhs },
              children_{_rhs.children_},
              parent_{_rhs.parent_} {
            } // cctor

            // =-=-=-=-=-=-=-
            /**
             * @brief Assigns one resource object to another.
             * @param[in] _rhs Resource object supplying the new state.
             * @return Reference to this resource after assignment.
             */
            resource& operator=( const resource& _rhs ) {
                if ( &_rhs == this ) {
                    return *this;
                }
                plugin_base::operator=( _rhs );
                children_ = _rhs.children_;
                parent_   = _rhs.parent_;
                return *this;
            }

            // =-=-=-=-=-=-=-
            /**
             * @brief Adds a child resource entry.
             * Inputs are the child hierarchy key, child resource name, and
             * child resource pointer.
             * @return Error object describing success or failure.
             */
            virtual error add_child( const std::string&, const std::string&, resource_ptr );
            /**
             * @brief Removes a child resource entry.
             * Input is the child hierarchy key identifying the entry to remove.
             * @return Error object describing success or failure.
             */
            virtual error remove_child( const std::string& );
            /**
             * @brief Returns the number of registered child resources.
             * @return Count of child resources.
             */
            virtual size_t num_children() {
                return children_.size();
            }
            /**
             * @brief Reports whether a child resource entry exists.
             * @param[in] _name Child resource hierarchy key.
             * @return True if the child exists, otherwise false.
             */
            virtual bool has_child(
                const std::string& _name ) {
                return children_.has_entry( _name );
            }
            /**
             * @brief Collects the names of registered child resources.
             * Output is the vector populated with child resource names.
             */
            virtual void children( std::vector<std::string>& );

            // =-=-=-=-=-=-=-
            /**
             * @brief Sets the parent resource pointer.
             * Input is the parent resource pointer to store.
             * @return Error object describing success or failure.
             */
            virtual error set_parent( const resource_ptr& );
            /**
             * @brief Retrieves the parent resource pointer.
             * Output is the stored parent resource pointer.
             * @return Error object describing success or failure.
             */
            virtual error get_parent( resource_ptr& );

        protected:
            // =-=-=-=-=-=-=-
            /// @brief Child resources keyed by hierarchy component.
            resource_child_map  children_;
            /// @brief Parent resource for this resource instance.
            resource_ptr        parent_;

    }; // class resource

    /// \brief Convenience function for getting resource name from plugin context
    /// \param[in] ctx - Plugin context from which resource name will be extracted
    /// \throws irods::exception - thrown if the error object returned by get() is not ok()
    auto get_resource_name(plugin_context& ctx) -> std::string;
    /// \brief Convenience function for getting resource status from plugin context
    /// \param[in] ctx - Plugin context from which resource status will be extracted
    /// \throws irods::exception - thrown if the error object returned by get() is not ok()
    auto get_resource_status(plugin_context& ctx) -> int;
    /// \brief Convenience function for getting resource location from plugin context
    /// \param[in] ctx - Plugin context from which resource location will be extracted
    /// \throws irods::exception - thrown if the error object returned by get() is not ok()
    auto get_resource_location(plugin_context& ctx) -> std::string;

}; // namespace irods


#endif // ___IRODS_RESC_PLUGIN_HPP__
