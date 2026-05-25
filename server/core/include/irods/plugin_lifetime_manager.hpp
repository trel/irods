#ifndef PLUGIN_LIFETIME_MANAGER
#define PLUGIN_LIFETIME_MANAGER

#include "irods/experimental_plugin_framework.hpp"

#include <map>
#include <string>

namespace irods::experimental::api {

    /// @brief Owns loaded experimental API plugins for the server lifetime.
    class plugin_lifetime_manager
    {
    public:
        /// @brief Map storing plugins by operation name.
        using map_type = std::map<std::string, std::unique_ptr<irods::experimental::api::base>>;

        /// @brief Returns the singleton plugin manager instance.
        /// @return Process-wide plugin manager.
        static auto instance() -> plugin_lifetime_manager&;

        /// @brief Destroys the singleton plugin manager instance.
        static void destroy();

        /// @brief Returns the managed plugin map.
        /// @return Map of loaded plugins.
        auto plugins() -> map_type&;

    private:
        /// @brief Loaded plugins keyed by operation name.
        map_type p;

    }; // plugin_lifetime_manager

} // namespace irods::experimental::api

#endif // PLUGIN_LIFETIME_MANAGER
