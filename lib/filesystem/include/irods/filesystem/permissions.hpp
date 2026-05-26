#ifndef IRODS_FILESYSTEM_PERMISSIONS_HPP
#define IRODS_FILESYSTEM_PERMISSIONS_HPP

#include <string>

namespace irods::experimental::filesystem
{
    /// \brief Lists the supported catalog permissions for users and groups.
    enum class perms
    {
        null,             ///< No permission.
        read_metadata,    ///< Permission to read metadata.
        read_object,      ///< Permission to read object contents.
        read,             ///< Permission to read object contents and metadata.
        create_metadata,  ///< Permission to add metadata.
        modify_metadata,  ///< Permission to update metadata.
        delete_metadata,  ///< Permission to remove metadata.
        create_object,    ///< Permission to create objects.
        modify_object,    ///< Permission to modify object contents.
        write,            ///< Permission to create and modify objects and metadata.
        delete_object,    ///< Permission to remove objects.
        own               ///< Full ownership permission.
    };

    /// \brief Describes a permission assigned to a catalog entity.
    struct entity_permission
    {
        std::string name; ///< The user or group name.
        std::string zone; ///< The zone containing the user or group.
        perms prms;       ///< The assigned permission.
        std::string type; ///< The entity type, such as user or group.
    };
} // namespace irods::experimental::filesystem

#endif // IRODS_FILESYSTEM_PERMISSIONS_HPP
