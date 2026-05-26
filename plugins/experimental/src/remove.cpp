#include "irods/private/parallel_filesystem_operation.hpp"

namespace irods::experimental::api {
    /// @brief Removes data objects or collections from iRODS.
    class remove : public parallel_filesystem_operation {
        protected:
        /// @brief Removes a single data object.
        /// @param[in] comm Communication handle for the operation.
        /// @param[in] path Logical path of the data object.
        /// @param[in] req JSON request payload.
        void process_object(rcComm_t& comm, const fs::path& path, const json& req) override
        {
            filesystem::extended_remove_options opts{};
            opts.no_trash   = req.contains("no_trash");
            opts.verbose    = false;
            opts.progress   = false;
            opts.recursive  = false;
            opts.unregister = req.contains("unregister");

            fscl::remove(comm, path, opts);
        }

        /// @brief Removes the collection after all contained objects are processed.
        /// @param[in] comm Communication handle for the operation.
        /// @param[in] req JSON request payload.
        void collection_postcondition(rcComm_t& comm, const json& req) override
        {
            fscl::remove_all(comm, req.at("logical_path"));
        }

        public:
        /// @brief Constructs the remove plugin.
        remove() : parallel_filesystem_operation("remove") {}
    }; // remove

} // namespace irods::experimental::api

/// @brief Constructs the remove plugin instance for the loader.
/// @details Accepts the loader-provided plugin name and context string, but does not use them.
/// @return Newly allocated remove plugin instance.
extern "C"
irods::experimental::api::remove* plugin_factory(const std::string&, const std::string&) {
    return new irods::experimental::api::remove{};
}
