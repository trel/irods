#ifndef __IRODS_GENERIC_DATABASE_OBJECT_HPP__
#define __IRODS_GENERIC_DATABASE_OBJECT_HPP__

/// \file

// =-=-=-=-=-=-=-
// irods includes
#include "irods/irods_database_object.hpp"

// =-=-=-=-=-=-=-
// boost includes
#include <boost/shared_ptr.hpp>

namespace irods {


// =-=-=-=-=-=-=-
    /// @brief Generic database object class.
    class generic_database_object : public database_object {
        public:
            // =-=-=-=-=-=-=-
            // Constructors
            /// @brief Constructs a generic database object for the given type.
            explicit generic_database_object(const std::string &);

            /// @brief Copy constructor.
            generic_database_object( const generic_database_object& );

            // =-=-=-=-=-=-=-
            // Destructors
            /// @brief Destructor.
            virtual ~generic_database_object();

            // =-=-=-=-=-=-=-
            // Operators
            /// @brief Assignment operator.
            virtual generic_database_object& operator=( const generic_database_object& );

            // =-=-=-=-=-=-=-
            /// @brief Comparison operator
            virtual bool operator==( const generic_database_object& _rhs ) const;

            // =-=-=-=-=-=-=-
            /// @brief Resolves the named plugin.
            virtual error resolve(
                const std::string&, // plugin interface
                plugin_ptr& );      // resolved plugin

            // =-=-=-=-=-=-=-
            /// @brief Serializes this object into rule engine variables.
            virtual error get_re_vars( rule_engine_vars_t& );

            // =-=-=-=-=-=-=-
            // Accessors

            // =-=-=-=-=-=-=-
            // Mutators

        private:
            // =-=-=-=-=-=-=-
            /// @brief Database object type name.
	    std::string type_;
    }; // generic_database_object

// =-=-=-=-=-=-=-
    /// @brief Shared pointer to a generic database object.
    typedef boost::shared_ptr< generic_database_object > generic_database_object_ptr;

}; // namespace irods

#endif // __IRODS_GENERIC_DATABASE_OBJECT_HPP__

