#undef IRODS_FILESYSTEM_COLLECTION_ITERATOR_HPP_INCLUDE_HEADER

#if defined(IRODS_FILESYSTEM_ENABLE_SERVER_SIDE_API)
#  if !defined(IRODS_FILESYSTEM_COLLECTION_ITERATOR_HPP_FOR_SERVER)
#    define IRODS_FILESYSTEM_COLLECTION_ITERATOR_HPP_FOR_SERVER
#    define IRODS_FILESYSTEM_COLLECTION_ITERATOR_HPP_INCLUDE_HEADER
#  endif
#elif !defined(IRODS_FILESYSTEM_COLLECTION_ITERATOR_HPP_FOR_CLIENT)
#  define IRODS_FILESYSTEM_COLLECTION_ITERATOR_HPP_FOR_CLIENT
#  define IRODS_FILESYSTEM_COLLECTION_ITERATOR_HPP_INCLUDE_HEADER
#endif

#ifdef IRODS_FILESYSTEM_COLLECTION_ITERATOR_HPP_INCLUDE_HEADER

#include "irods/filesystem/config.hpp"
#include "irods/filesystem/filesystem.hpp"
#include "irods/filesystem/collection_entry.hpp"
#include "irods/filesystem/path.hpp"

#ifndef IRODS_FILESYSTEM_ENABLE_SERVER_SIDE_API
#  include "irods/miscUtil.h"
#endif // IRODS_FILESYSTEM_ENABLE_SERVER_SIDE_API

#include <iterator>
#include <memory>

namespace irods::experimental::filesystem::NAMESPACE_IMPL
{
    /// Options controlling collection iteration behavior.
    enum class collection_options
    {
        none, ///< Do not enable any optional behavior.
        skip_permission_denied ///< Continue iteration when permission is denied.
    };

    /// Input iterator over the direct contents of a collection.
    class collection_iterator
    {
    public:
        // clang-format off
        using value_type        = collection_entry;
        using difference_type   = std::ptrdiff_t;
        using pointer           = const value_type*;
        using reference         = const value_type&;
        using iterator_category = std::input_iterator_tag;
        // clang-format on

        // Constructors and destructor

        /// Constructs an end iterator.
        collection_iterator() = default;

        /// Constructs an iterator over the contents of a collection.
        ///
        /// \param[in] _comm The connection used to query collection entries.
        /// \param[in] _p The collection path to iterate.
        /// \param[in] _opts Optional iterator behavior flags.
        collection_iterator(rxComm& _comm,
                            const path& _p,
                            collection_options _opts = collection_options::none);

        /// Copies an existing collection iterator.
        ///
        /// \param[in] _other The iterator to copy.
        collection_iterator(const collection_iterator& _other) = default;
        /// Assigns from another collection iterator.
        ///
        /// \param[in] _other The iterator to copy.
        /// \return A reference to this iterator.
        auto operator=(const collection_iterator& _other) -> collection_iterator& = default;

        /// Moves an existing collection iterator.
        ///
        /// \param[in] _other The iterator to move from.
        collection_iterator(collection_iterator&& _other) = default;
        /// Move-assigns from another collection iterator.
        ///
        /// \param[in] _other The iterator to move from.
        /// \return A reference to this iterator.
        auto operator=(collection_iterator&& _other) -> collection_iterator& = default;

        /// Destroys the iterator and releases any active collection handle.
        ~collection_iterator();

        // Observers

        auto connection() -> rxComm* { return ctx_->comm; } ///< Returns the underlying iRODS connection, or null for the end iterator.

        // clang-format off
        auto operator*() const -> reference { return ctx_->entry; } ///< Returns a reference to the current collection entry.
        auto operator->() const -> pointer  { return &ctx_->entry; } ///< Returns a pointer to the current collection entry.
        // clang-format on

        // Modifiers

        /// Advances the iterator to the next collection entry.
        ///
        /// \return A reference to the advanced iterator.
        auto operator++() -> collection_iterator&;

        // Compare

        // clang-format off
        auto operator==(const collection_iterator& _rhs) const noexcept -> bool { return _rhs.ctx_ == ctx_; } ///< Returns true if both iterators share the same traversal state.
        auto operator!=(const collection_iterator& _rhs) const noexcept -> bool { return !(*this == _rhs); } ///< Returns true if the iterators do not share the same traversal state.
        // clang-format on

    private:
        struct context
        {
            rxComm* comm{}; ///< Connection used for iteration.
            path path{}; ///< Collection currently being traversed.
#ifdef IRODS_FILESYSTEM_ENABLE_SERVER_SIDE_API
            int handle{}; ///< Server-side collection handle.
#else
            collHandle_t handle{}; ///< Client-side collection handle.
#endif // IRODS_FILESYSTEM_ENABLE_SERVER_SIDE_API
            value_type entry{}; ///< Current collection entry.
        };

        /// Closes the active collection handle, if any.
        auto close() -> void;

        std::shared_ptr<context> ctx_; ///< Shared traversal state.
    };

    // Enables support for range-based for-loops.

    /// Returns the iterator unchanged for range-based for loops.
    inline auto begin(collection_iterator _iter) noexcept -> collection_iterator
    {
        return _iter;
    }

    /// Returns the end iterator for range-based for loops.
    inline auto end([[maybe_unused]] const collection_iterator& _iter) noexcept -> collection_iterator
    {
        return {};
    }
} // namespace irods::experimental::filesystem::NAMESPACE_IMPL

#endif // IRODS_FILESYSTEM_COLLECTION_ITERATOR_HPP_INCLUDE_HEADER
