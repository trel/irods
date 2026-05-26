#ifndef __IRODS_POSTGRES_OBJECT_HPP__
#define __IRODS_POSTGRES_OBJECT_HPP__

// =-=-=-=-=-=-=-
// irods includes
#include "irods/irods_database_object.hpp"

// =-=-=-=-=-=-=-
// boost includes
#include <boost/shared_ptr.hpp>

namespace irods {

    /// @brief Plugin type string for the PostgreSQL database plugin.
    const std::string POSTGRES_DATABASE_PLUGIN( "postgres" );

    /// @brief Database object representing the PostgreSQL plugin.
    class postgres_object : public database_object {
        public:
            /// @brief Constructs a PostgreSQL database object.
            postgres_object();

            /// @brief Copy-constructs a PostgreSQL database object.
            postgres_object( const postgres_object& );

            /// @brief Destroys the PostgreSQL database object.
            virtual ~postgres_object();

            /// @brief Copies state from another PostgreSQL database object.
            virtual postgres_object& operator=( const postgres_object& );

            // =-=-=-=-=-=-=-
            /// @brief Comparison operator
            virtual bool operator==( const postgres_object& _rhs ) const;

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

    }; // postgres_object

    /// @brief Shared-pointer type for `postgres_object` instances.
    typedef boost::shared_ptr< postgres_object > postgres_object_ptr;

}; // namespace irods

#endif // __IRODS_POSTGRES_OBJECT_HPP__


