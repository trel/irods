#ifndef __IRODS_DATABASE_OBJECT_HPP__
#define __IRODS_DATABASE_OBJECT_HPP__

// =-=-=-=-=-=-=-
// irods includes
#include "irods/irods_first_class_object.hpp"

// =-=-=-=-=-=-=-
// irods includes
#include "irods/rcConnect.h"

// =-=-=-=-=-=-=-
// boost includes
#include <boost/shared_ptr.hpp>

namespace irods {
// =-=-=-=-=-=-=-
// network object base class
    /// Base class for first-class objects backed by the catalog.
    class database_object : public first_class_object {
        public:
            // =-=-=-=-=-=-=-
            // Constructors
            /// Constructs an empty database object.
            database_object();

            /// Copies a database object.
            database_object( const database_object& );

            // =-=-=-=-=-=-=-
            // Destructors
            /// Destroys the database object.
            virtual ~database_object();

            // =-=-=-=-=-=-=-
            // Operators
            /// Assigns from another database object.
            virtual database_object& operator=( const database_object& );

            // =-=-=-=-=-=-=-
            /// Compares this object with another database object.
            ///
            /// \param[in] _rhs The object to compare against.
            /// \return True if the objects are equal; otherwise false.
            virtual bool operator==( const database_object& _rhs ) const;

            // =-=-=-=-=-=-=-
            // plugin resolution operation
            /// Resolves a plugin for the requested interface.
            virtual error resolve(
                const std::string&, // plugin interface
                plugin_ptr& ) = 0;  // resolved plugin

            // =-=-=-=-=-=-=-
            // accessor for rule engine variables
            /// Populates rule engine variables for this object.
            virtual error get_re_vars( rule_engine_vars_t& );

            // =-=-=-=-=-=-=-
            // Accessors

            // =-=-=-=-=-=-=-
            // Mutators

        private:
            // =-=-=-=-=-=-=-
            // Attributes

    }; // database_object

// =-=-=-=-=-=-=-
// helpful typedef for sock comm interface & factory
    /// Shared pointer type for database objects.
    typedef boost::shared_ptr< database_object > database_object_ptr;

}; // namespace irods

#endif // __IRODS_DATABASE_OBJECT_HPP__

