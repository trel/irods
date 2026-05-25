#undef IRODS_FILESYSTEM_RECURSIVE_COLLECTION_ITERATOR_HPP_INCLUDE_HEADER

#if defined(IRODS_FILESYSTEM_ENABLE_SERVER_SIDE_API)
#  if !defined(IRODS_FILESYSTEM_RECURSIVE_COLLECTION_ITERATOR_HPP_FOR_SERVER)
#    define IRODS_FILESYSTEM_RECURSIVE_COLLECTION_ITERATOR_HPP_FOR_SERVER
#    define IRODS_FILESYSTEM_RECURSIVE_COLLECTION_ITERATOR_HPP_INCLUDE_HEADER
#  endif
#elif !defined(IRODS_FILESYSTEM_RECURSIVE_COLLECTION_ITERATOR_HPP_FOR_CLIENT)
#  define IRODS_FILESYSTEM_RECURSIVE_COLLECTION_ITERATOR_HPP_FOR_CLIENT
#  define IRODS_FILESYSTEM_RECURSIVE_COLLECTION_ITERATOR_HPP_INCLUDE_HEADER
#endif

#ifdef IRODS_FILESYSTEM_RECURSIVE_COLLECTION_ITERATOR_HPP_INCLUDE_HEADER

#include "irods/filesystem/config.hpp"
#include "irods/filesystem/collection_iterator.hpp"
#include "irods/filesystem/collection_entry.hpp"

#include <iterator>
#include <stack>
#include <memory>

namespace irods::experimental::filesystem::NAMESPACE_IMPL
{
    /// Input iterator over a collection hierarchy.
    class recursive_collection_iterator
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
        recursive_collection_iterator() = default;

        /// Constructs a recursive iterator over a collection hierarchy.
        ///
        /// \param[in] _comm The connection used to query collection entries.
        /// \param[in] _p The root collection path to traverse.
        /// \param[in] _opts Optional iterator behavior flags.
        recursive_collection_iterator(rxComm& _comm,
                                      const path& _p,
                                      collection_options _opts = collection_options::none);

        /// Copies an existing recursive collection iterator.
        ///
        /// \param[in] _other The iterator to copy.
        recursive_collection_iterator(const recursive_collection_iterator& _other) = default;
        /// Assigns from another recursive collection iterator.
        ///
        /// \param[in] _other The iterator to copy.
        /// \return A reference to this iterator.
        auto operator=(const recursive_collection_iterator& _other) -> recursive_collection_iterator& = default;

        /// Moves an existing recursive collection iterator.
        ///
        /// \param[in] _other The iterator to move from.
        recursive_collection_iterator(recursive_collection_iterator&& _other) = default;
        /// Move-assigns from another recursive collection iterator.
        ///
        /// \param[in] _other The iterator to move from.
        /// \return A reference to this iterator.
        auto operator=(recursive_collection_iterator&& _other) -> recursive_collection_iterator& = default;

        /// Destroys the iterator.
        ~recursive_collection_iterator() = default;

        // Observers

        auto connection() -> rxComm* { return ctx_->stack.empty() ? nullptr : (*this).connection(); } ///< Returns the connection used by the active iterator frame, or null for the end iterator.

        // clang-format off
        auto operator*() const -> reference { return *ctx_->stack.top(); } ///< Returns a reference to the current collection entry.
        auto operator->() const -> pointer  { return &*ctx_->stack.top(); } ///< Returns a pointer to the current collection entry.

        auto options() const noexcept -> collection_options { return ctx_->opts; } ///< Returns the option flags controlling traversal.
        auto depth() const noexcept -> int                  { return static_cast<int>(ctx_->stack.size()) - 1; } ///< Returns the current recursion depth.
        auto recursion_pending() const noexcept -> bool     { return ctx_->recurse; } ///< Returns true if recursion into the current entry is still enabled.
        // clang-format on

        // Modifiers

        /// Advances the iterator to the next entry in the recursive traversal.
        ///
        /// \return A reference to the advanced iterator.
        auto operator++() -> recursive_collection_iterator&;
        
        /// Skips the remainder of the current collection and resumes traversal at the parent level.
        auto pop() -> void;
        /// Prevents recursion into the current entry.
        auto disable_recursion_pending() noexcept -> void { ctx_->recurse = false; } ///< Prevents recursion into the current entry.

        // Compare

        // clang-format off
        auto operator==(const recursive_collection_iterator& _rhs) const noexcept -> bool { return _rhs.ctx_ == ctx_; } ///< Returns true if both iterators share the same traversal state.
        auto operator!=(const recursive_collection_iterator& _rhs) const noexcept -> bool { return !(*this == _rhs); } ///< Returns true if the iterators do not share the same traversal state.
        // clang-format on

    private:
        struct context
        {
            std::stack<collection_iterator> stack; ///< Active iterator frames for each recursion depth.
            collection_options opts = collection_options::none; ///< Traversal option flags.
            bool recurse = true; ///< Indicates whether the current entry should be recursed into.
        };

        std::shared_ptr<context> ctx_; ///< Shared traversal state.
    };

    // Enables support for range-based for-loops.

    /// Returns the iterator unchanged for range-based for loops.
    inline auto begin(recursive_collection_iterator _iter) noexcept -> recursive_collection_iterator
    {
        return _iter;
    }

    /// Returns the end iterator for range-based for loops.
    inline auto end([[maybe_unused]] const recursive_collection_iterator& _iter) noexcept
        -> recursive_collection_iterator
    {
        return {};
    }
} // namespace irods::experimental::filesystem::NAMESPACE_IMPL

#endif // IRODS_FILESYSTEM_RECURSIVE_COLLECTION_ITERATOR_HPP_INCLUDE_HEADER
