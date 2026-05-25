#ifndef __IRODS_AUTH_SCHEME_HPP__
#define __IRODS_AUTH_SCHEME_HPP__

// =-=-=-=-=-=-=-
// stl includs
#include <string>

namespace irods {
    /// @brief Stores the active pluggable authentication scheme name.
    class pluggable_auth_scheme {
        private:
            /// @brief Constructs the singleton instance.
            pluggable_auth_scheme() {}
            std::string scheme_; ///< Stored authentication scheme name.
        public:
            /// @brief Returns the configured authentication scheme.
            /// @return The stored scheme name.
            std::string get() const;

            /// @brief Updates the configured authentication scheme.
            void set( const std::string& );

            /// @brief Returns the singleton authentication scheme instance.
            /// @return Reference to the process-wide scheme store.
            static pluggable_auth_scheme& get_instance();
    };


}; // namespace irods

#endif // __IRODS_AUTH_SCHEME_HPP__


