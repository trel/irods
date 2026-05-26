#ifndef IRODS_FILESYSTEM_OBJECT_STATUS_HPP
#define IRODS_FILESYSTEM_OBJECT_STATUS_HPP

#include "irods/filesystem/permissions.hpp"

#include <vector>

namespace irods::experimental::filesystem
{
    /// \brief Identifies the kind of catalog object represented by a status object.
    enum class object_type
    {
        none,               ///< No object type has been recorded.
        not_found,          ///< The path does not exist in the catalog.
        data_object,        ///< The path refers to a data object.
        collection,         ///< The path refers to a collection.
        special_collection, ///< The path refers to a special collection.
        unknown             ///< The object type could not be determined.
    };

    /// \brief Stores the observed type and permissions of a catalog object.
    class object_status
    {
    public:
        // Constructors and destructor

        /// \brief Constructs an empty status object.
        object_status() noexcept
            : object_status{object_type::none}
        {
        }

        /// \brief Constructs a status object by copying another instance.
        object_status(const object_status& _other) = default;

        /// \brief Constructs a status object with a type and permissions.
        explicit object_status(object_type _type, const std::vector<entity_permission>& _perms = {})
            : type_{_type}
            , perms_{_perms}
            , inheritance_{}
        {
        }

        /// \brief Destroys the status object.
        ~object_status() noexcept = default;

        // Assignment operators

        /// \brief Replaces this status object with another instance.
        auto operator=(const object_status& _other) -> object_status& = default;

        // clang-format off

        // Observers

        /// \brief Returns the recorded object type.
        auto type() const noexcept -> object_type { return type_; }
        /// \brief Returns the recorded permissions.
        auto permissions() const noexcept -> const std::vector<entity_permission>& { return perms_; }
        /// \brief Returns whether inheritance is enabled.
        auto is_inheritance_enabled() const noexcept -> bool { return inheritance_; }

        // Modifiers

        /// \brief Sets the recorded object type.
        auto type(object_type _ot) noexcept -> void { type_ = _ot; }
        /// \brief Sets the recorded permissions.
        auto permissions(const std::vector<entity_permission>& _perms) -> void { perms_ = _perms; }
        /// \brief Sets whether inheritance is enabled.
        auto inheritance(bool _value) noexcept -> void { inheritance_ = _value; }

        // clang-format on

    private:
        object_type type_;                    ///< The recorded object type.
        std::vector<entity_permission> perms_; ///< The recorded permissions.
        bool inheritance_;                   ///< Indicates whether inheritance is enabled.
    };
} // namespace irods::experimental::filesystem

#endif // IRODS_FILESYSTEM_OBJECT_STATUS_HPP
