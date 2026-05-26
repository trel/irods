#ifndef __IRODS_COLLECTION_OBJECT_HPP__
#define __IRODS_COLLECTION_OBJECT_HPP__

// =-=-=-=-=-=-=-
// system includes
#include <sys/types.h>
#include <dirent.h>

// =-=-=-=-=-=-=-
#include "irods/irods_data_object.hpp"

namespace irods {

    /// Represents a collection as a first-class object.
    class collection_object : public data_object {
        public:
            // =-=-=-=-=-=-=-
            // Constructors
            /// Constructs an empty collection object.
            collection_object();

            /// Copies a collection object.
            collection_object( const collection_object& );

            /// Constructs a collection object from a physical path and resource identifier.
            collection_object(
                const std::string&, // phy path
                rodsLong_t,         // resc_id
                int, 				// mode
                int ); 				// flags

            /// Constructs a collection object from a physical path, resource identifier, and condition input.
            collection_object(
                const std::string&, // phy path
                rodsLong_t,         // resc_id
                int,                // mode
                int,				// flags
                const keyValPair_t& );	// cond_input

            /// Constructs a collection object from a physical path and resource hierarchy.
            collection_object(
                const std::string&, // phy path
                const std::string&, // resc hier
                int, 				// mode
                int ); 				// flags

            /// Constructs a collection object from a physical path, resource hierarchy, and condition input.
            collection_object(
                const std::string&, // phy path
                const std::string&, // resc hier
                int,                // mode
                int,				// flags
                const keyValPair_t& );	// cond_input

            // =-=-=-=-=-=-=-
            // Destructor
            /// Destroys the collection object.
            virtual ~collection_object();

            // =-=-=-=-=-=-=-
            // Operators
            /// Assigns from another collection object.
            virtual collection_object& operator=( const collection_object& );

            // =-=-=-=-=-=-=-
            // plugin resolution operation
            /// Resolves a plugin for the requested interface.
            virtual error resolve(
                const std::string&, // plugin interface name
                plugin_ptr& );      // resolved plugin instance

            // =-=-=-=-=-=-=-
            // accessor for rule engine variables
            /// Populates rule engine variables for this object.
            virtual error get_re_vars( rule_engine_vars_t& );

            // =-=-=-=-=-=-=-
            // Accessors
            /// Returns the open directory pointer associated with the collection.
            virtual DIR* directory_pointer() const {
                return directory_pointer_;
            }

            // =-=-=-=-=-=-=-
            // Mutators
            /// Sets the open directory pointer associated with the collection.
            ///
            /// \param[in] _p The directory pointer.
            virtual void directory_pointer( DIR* _p ) {
                directory_pointer_ = _p;
            }

        protected:
            // =-=-=-=-=-=-=-
            // Attributes
            // NOTE :: These are not guaranteed to be properly populated right now
            //      :: that will need be done later when these changes are pushed
            //      :: higher in the original design
            /// Points to the open filesystem directory for the collection.
            DIR* directory_pointer_;    // pointer to open filesystem directory

    }; // class collection_object

/// Shared pointer type for collection objects.
    typedef boost::shared_ptr< collection_object > collection_object_ptr;

}; // namespace irods

#endif // __IRODS_COLLECTION_OBJECT_HPP__

