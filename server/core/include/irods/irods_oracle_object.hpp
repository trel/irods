#ifndef __IRODS_ORACLE_OBJECT_HPP__
#define __IRODS_ORACLE_OBJECT_HPP__

// =-=-=-=-=-=-=-
// irods includes
#include "irods/irods_database_object.hpp"

// =-=-=-=-=-=-=-
// boost includes
#include <boost/shared_ptr.hpp>

namespace irods {

    /// @brief Plugin type string for the Oracle database plugin.
    const std::string ORACLE_DATABASE_PLUGIN( "oracle" );

    /// @brief Database object representing the Oracle plugin.
    class oracle_object : public database_object {
        public:
            /// @brief Constructs an Oracle database object.
            oracle_object();

            /// @brief Copy-constructs an Oracle database object.
            oracle_object( const oracle_object& );

            /// @brief Destroys the Oracle database object.
            virtual ~oracle_object();

            /// @brief Copies state from another Oracle database object.
            virtual oracle_object& operator=( const oracle_object& );

            // =-=-=-=-=-=-=-
            /// @brief Comparison operator
            virtual bool operator==( const oracle_object& _rhs ) const;

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

    }; // oracle_object

    /// @brief Shared-pointer type for `oracle_object` instances.
    typedef boost::shared_ptr< oracle_object > oracle_object_ptr;

}; // namespace irods

#endif // __IRODS_ORACLE_OBJECT_HPP__


