#ifndef IRODS_ENVIRONMENT_PROPERTIES_HPP_
#define IRODS_ENVIRONMENT_PROPERTIES_HPP_

/// \file
/// \brief Declares access to iRODS environment configuration properties.

#include "irods/irods_configuration_parser.hpp"
#include "irods/irods_configuration_keywords.hpp"
#include "irods/irods_exception.hpp"

#include <map>
#include <unordered_map>

namespace irods {

    /// \brief Default path to the per-user JSON environment file.
    static const std::string IRODS_JSON_ENV_FILE = "/.irods/irods_environment.json";

    /// \brief Resolves the environment and session file paths.
    error get_json_environment_file( std::string& _env_file, std::string& _session_file);

    /// \brief Singleton wrapper around parsed environment properties.
    class environment_properties {

        public:
            /// \brief Returns the singleton instance.
            static environment_properties& instance();

            /// \brief Returns a copy of the captured configuration.
            static std::unordered_map<std::string, boost::any> copy_configuration()
            {
                return instance().config_props_.map();
            } // copy_configuration

            /// \brief Replaces the captured configuration.
            void set_configuration(std::unordered_map<std::string, boost::any> _config)
            {
                config_props_.map() = std::move(_config);
            } // set_configuration

            /// \brief Reads environment configuration into the property map.
            void capture();

            /// \brief Retrieves a property into \p _val and converts type mismatches to irods::error.
            template< typename T >
            error get_property( const std::string& _key, T& _val ) {
                try {
                    _val = get_property<T>( _key );
                } catch ( const irods::exception& e ) {
                    return irods::error(e);
                }
                return SUCCESS();
            }

            /// \brief Returns a reference to the property stored at \p _key.
            template< typename T >
            T& get_property( const std::string& _key ) {
                return config_props_.get< T >( _key );
            }

            /// \brief Sets the property at \p _key to \p _val.
            template< typename T >
            T& set_property( const std::string& _key, const T& _val ) {
                return config_props_.set< T >( _key, _val );
            }

            /// \brief Removes the property at \p _key and returns its value.
            template< typename T >
            T remove( const std::string& _key ) {
                return config_props_.remove(_key);
            }

            /// \brief Removes the property at \p _key without returning it.
            void remove( const std::string& _key );

            /// \brief Returns the underlying property map.
            std::unordered_map<std::string, boost::any>& map() {
                return config_props_.map();
            }
        private:
            /// \cond IRODS_DOXYGEN_INTERNAL
            environment_properties( environment_properties const& );
            /* Internal constructor for the singleton. */
            environment_properties( );
            void operator=( environment_properties const& );

            /* Internal helper for reading a JSON environment file. */
            void capture_json( const std::string& );

            /// \brief Property lookup table.
            configuration_parser config_props_;
            /// \endcond

    }; // class environment_properties

    /// \brief Returns a reference to the requested environment property.
    template< typename T >
    T& get_environment_property( const std::string& _prop ) {
        return irods::environment_properties::instance().get_property<T>(_prop);
    }

    /// \brief Sets an environment property and returns a reference to the stored value.
    template< typename T >
    T& set_environment_property( const std::string& _prop, const T& _val ) {
        return irods::environment_properties::instance().set_property<T>(_prop, _val);
    }
} // namespace irods

#endif /* IRODS_ENVIRONMENT_PROPERTIES_HPP_ */
