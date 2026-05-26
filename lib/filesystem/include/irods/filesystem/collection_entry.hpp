#undef IRODS_FILESYSTEM_COLLECTION_ENTRY_HPP_INCLUDE_HEADER

#if defined(IRODS_FILESYSTEM_ENABLE_SERVER_SIDE_API)
#  if !defined(IRODS_FILESYSTEM_COLLECTION_ENTRY_HPP_FOR_SERVER)
#    define IRODS_FILESYSTEM_COLLECTION_ENTRY_HPP_FOR_SERVER
#    define IRODS_FILESYSTEM_COLLECTION_ENTRY_HPP_INCLUDE_HEADER
#  endif
#elif !defined(IRODS_FILESYSTEM_COLLECTION_ENTRY_HPP_FOR_CLIENT)
#  define IRODS_FILESYSTEM_COLLECTION_ENTRY_HPP_FOR_CLIENT
#  define IRODS_FILESYSTEM_COLLECTION_ENTRY_HPP_INCLUDE_HEADER
#endif

#ifdef IRODS_FILESYSTEM_COLLECTION_ENTRY_HPP_INCLUDE_HEADER

#include "irods/filesystem/config.hpp"
#include "irods/filesystem/filesystem.hpp"
#include "irods/filesystem/path.hpp"
#include "irods/filesystem/object_status.hpp"

#include <chrono>

namespace irods::experimental::filesystem::NAMESPACE_IMPL
{
    class collection_iterator;
    class recursive_collection_iterator;

    /// Cached filesystem entry describing a collection or data object.
    class collection_entry
    {
    public:
        // Observers

        // clang-format off
        // NOLINTNEXTLINE(google-explicit-constructor)
        operator const path&() const noexcept                     { return path_; } ///< Returns the entry path as a filesystem path reference.
        auto path() const noexcept -> const path&                 { return path_; } ///< Returns the path associated with the entry.
        auto exists() const noexcept -> bool                      { return filesystem::NAMESPACE_IMPL::exists(status_); } ///< Returns true if the entry exists.
        auto is_data_object() const noexcept -> bool              { return filesystem::NAMESPACE_IMPL::is_data_object(status_); } ///< Returns true if the entry describes a data object.
        auto is_collection() const noexcept -> bool               { return filesystem::NAMESPACE_IMPL::is_collection(status_); } ///< Returns true if the entry describes a collection.
        auto is_other() const noexcept -> bool                    { return filesystem::NAMESPACE_IMPL::is_other(status_); } ///< Returns true if the entry is neither a data object nor a collection.
        auto creation_time() const noexcept -> object_time_type   { return ctime_; } ///< Returns the creation timestamp for the entry.
        auto last_write_time() const noexcept -> object_time_type { return mtime_; } ///< Returns the last modification timestamp for the entry.
        auto status() const noexcept -> const object_status&      { return status_; } ///< Returns the cached object status for the entry.
        auto data_mode() const noexcept -> unsigned               { return data_mode_; } ///< Returns the mode bits associated with the entry.
        auto data_id() const noexcept -> const std::string&       { return data_id_; } ///< Returns the catalog data identifier for the entry.
        auto checksum() const noexcept -> const std::string&      { return checksum_; } ///< Returns the checksum stored for the entry.
        auto owner() const noexcept -> const std::string&         { return owner_; } ///< Returns the owner name associated with the entry.
        auto data_type() const noexcept -> const std::string&     { return data_type_; } ///< Returns the data type name stored for the entry.
        auto data_size() const noexcept -> std::uintmax_t         { return data_size_; } ///< Returns the size of the data object in bytes.

        // Comparisons

        auto operator==(const collection_entry& _rhs) const noexcept -> bool { return path_ == _rhs.path_; } ///< Returns true if both entries refer to the same path.
        auto operator!=(const collection_entry& _rhs) const noexcept -> bool { return path_ != _rhs.path_; } ///< Returns true if the entries refer to different paths.
        auto operator< (const collection_entry& _rhs) const noexcept -> bool { return path_ <  _rhs.path_; } ///< Returns true if this entry sorts before \p _rhs by path.
        auto operator<=(const collection_entry& _rhs) const noexcept -> bool { return path_ <= _rhs.path_; } ///< Returns true if this entry does not sort after \p _rhs by path.
        auto operator> (const collection_entry& _rhs) const noexcept -> bool { return path_ >  _rhs.path_; } ///< Returns true if this entry sorts after \p _rhs by path.
        auto operator>=(const collection_entry& _rhs) const noexcept -> bool { return path_ >= _rhs.path_; } ///< Returns true if this entry does not sort before \p _rhs by path.
        // clang-format on

    private:
        friend class collection_iterator;
        friend class recursive_collection_iterator;

        mutable class path path_;
        mutable object_status status_;
        mutable unsigned data_mode_;
        mutable std::uintmax_t data_size_;
        mutable std::string data_id_;
        mutable object_time_type ctime_;
        mutable object_time_type mtime_;
        mutable std::string checksum_;
        mutable std::string owner_;
        mutable std::string data_type_;
    };
} // namespace irods::experimental::filesystem

#endif // IRODS_FILESYSTEM_COLLECTION_ENTRY_HPP_INCLUDE_HEADER
