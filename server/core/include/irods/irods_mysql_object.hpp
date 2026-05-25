#ifndef __IRODS_MYSQL_OBJECT_HPP__
#define __IRODS_MYSQL_OBJECT_HPP__

// =-=-=-=-=-=-=-
// irods includes
#include "irods/irods_database_object.hpp"

// =-=-=-=-=-=-=-
// boost includes
#include <boost/shared_ptr.hpp>

namespace irods {

    /// @brief Plugin type string for the MySQL database plugin.
    const std::string MYSQL_DATABASE_PLUGIN( "mysql" );

    /// @brief Database object representing the MySQL plugin.
    class mysql_object : public database_object {
        public:
            /// @brief Constructs a MySQL database object.
            mysql_object();

            /// @brief Copy-constructs a MySQL database object.
            mysql_object( const mysql_object& );

            /// @brief Destroys the MySQL database object.
            virtual ~mysql_object();

            /// @brief Copies state from another MySQL database object.
            virtual mysql_object& operator=( const mysql_object& );

            // =-=-=-=-=-=-=-
            /// @brief Comparison operator
            virtual bool operator==( const mysql_object& _rhs ) const;

            /// @brief Resolves the database plugin for the requested interface.
            virtual error resolve(
                const std::string&, // plugin interface
                plugin_ptr& );      // resolved plugin

            /// @brief Populates rule-engine variables for this object.
            virtual error get_re_vars( rule_engine_vars_t& );

            // =-=-=-=-=-=-=-
            // Accessors

            // =-=-=-=-=-=-=-
            // Mutators

        private:
            // =-=-=-=-=-=-=-
            // Attributes

    }; // mysql_object

    /// @brief Shared-pointer type for `mysql_object` instances.
    typedef boost::shared_ptr< mysql_object > mysql_object_ptr;

}; // namespace irods

#endif // __IRODS_MYSQL_OBJECT_HPP__


