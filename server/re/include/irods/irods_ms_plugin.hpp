#ifndef __IRODS_MS_PLUGIN_HPP__
#define __IRODS_MS_PLUGIN_HPP__

// =-=-=-=-=-=-=-
// STL Includes
#include <string>

// =-=-=-=-=-=-=-
// dlopen, etc
#include <dlfcn.h>

// =-=-=-=-=-=-=-
// My Includes
#include "irods/irods_lookup_table.hpp"
#include "irods/irods_plugin_base.hpp"

namespace irods {

// =-=-=-=-=-=-=-
    /**
     * \author Jason M. Coposky
     * \brief  This is to be used by a microservice developer to provide a dynamic plugin
               to the microservice table found in server/re/include/reActions.header.
               Reference server/re/src/rules.cpp for loading and
               server/re/src/arithmetic.cpp for invocation.
     *
     **/
    class ms_table_entry : public plugin_base {
        public:

            /// @brief Function pointer type for a legacy microservice.
            typedef int ( *ms_func_ptr )( ... );

            // =-=-=-=-=-=-=-
            // Attributes

            // =-=-=-=-=-=-=-
            // Constructors
            /// @brief Constructs an empty microservice table entry.
            ms_table_entry( );

            // =-=-=-=-=-=-=-
            // NOTE :: this ctor should be called by plugin authors
            /// @brief Constructs a microservice table entry with an argument count.
            ms_table_entry(
                    int ); // num ms args

            // =-=-=-=-=-=-=-
            // NOTE :: called internally for static plugins
            //         with no type checking
            /// @brief Constructs a microservice table entry for a static plugin.
            ms_table_entry(
                const std::string&, // ms name
                unsigned int,                // num ms args
                boost::any );       // function pointer

            // =-=-=-=-=-=-=-
            // copy ctor
            /// @brief Copy-constructs a microservice table entry.
            ms_table_entry( const ms_table_entry& _rhs );

            // =-=-=-=-=-=-=-
            // Assignment Operator - necessary for stl containers
            /// @brief Assigns one microservice table entry to another.
            ms_table_entry& operator=( const ms_table_entry& _rhs );

            // =-=-=-=-=-=-=-
            // Destructor
            /// @brief Destroys a microservice table entry.
            virtual ~ms_table_entry();

            /// =-=-=-=-=-=-=-
            /// @brief adaptor from old microservice sig to new plugin sign
            template<typename... types_t>
                error add_operation(
                        const std::string& _op,
                        std::function<int(types_t...)> _f ) {

                    // =-=-=-=-=-=-=-
                    // check params
                    if ( _op.empty() ) {
                        std::stringstream msg;
                        msg << "empty operation key [" << _op << "]";
                        return ERROR(
                                SYS_INVALID_INPUT_PARAM,
                                msg.str() );
                    }

                    operation_name_ = _op;
                    operations_[operation_name_] = _f;

                    return SUCCESS();

                } // add_operation

            /// @brief Invokes the currently selected microservice operation.
            template<typename... types_t>
                int call_handler(types_t... _t ) {
                    if( !operations_.has_entry(operation_name_) ) {
                        rodsLog(
                            LOG_ERROR,
                            "missing microservice operation [%s]",
                            operation_name_.c_str() );
                        return SYS_INVALID_INPUT_PARAM;
                    }

                    try {
                        typedef std::function<int(types_t...)> fcn_t;
                        fcn_t fcn = boost::any_cast<fcn_t>( operations_[ operation_name_ ] );
                        return fcn(_t...);
                    }
                    catch( const boost::bad_any_cast& ) {
                        std::string msg( "failed for call - " );
                        msg += operation_name_;
                        irods::log( ERROR(
                                    INVALID_ANY_CAST,
                                    msg ) );
                        return INVALID_ANY_CAST;
                    }

                    return 0;

                } // call_handler

            /// @brief Invokes the microservice using the rule engine call interface.
            int call(ruleExecInfo_t*,std::vector<msParam_t*>&);

            /// @brief Returns the configured number of microservice arguments.
            unsigned int num_args() { return num_args_; }

        private:
            /// @brief Stores the active operation name.
            std::string operation_name_;

            /// @brief Stores the expected number of microservice arguments.
            unsigned int num_args_;

    }; // class ms_table_entry

// =-=-=-=-=-=-=-
// create a lookup table for ms_table_entry value type
    /// @brief Lookup table type for microservice table entries.
    typedef lookup_table<ms_table_entry*> ms_table;

// =-=-=-=-=-=-=-
// given the name of a microservice, try to load the shared object
// and then register that ms with the table
    /// @brief Loads and registers a microservice plugin by name.
    error load_microservice_plugin( ms_table& _table, const std::string& _ms );


}; // namespace irods

/// @brief Looks up an action table entry by action name.
int actionTableLookUp( irods::ms_table_entry&, char *action );

#endif // __IRODS_MS_PLUGIN_HPP__
