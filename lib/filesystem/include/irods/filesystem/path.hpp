#ifndef IRODS_FILESYSTEM_PATH_HPP
#define IRODS_FILESYSTEM_PATH_HPP

#include "irods/filesystem/path_traits.hpp"

#include <string>
#include <utility>
#include <type_traits>
#include <istream>
#include <ostream>
#include <optional>

namespace irods::experimental::filesystem
{
    /// Represents an iRODS logical path and provides lexical path manipulation utilities.
    class path
    {
    public:
        class iterator;
        class reverse_iterator;

        // clang-format off
        using value_type                = char;
        using string_type               = std::basic_string<value_type>;
        using const_iterator            = iterator;
        using const_reverse_iterator    = reverse_iterator;
        // clang-format on

        inline static const value_type preferred_separator = '/';

    private:
        // clang-format off
        inline static const value_type dot[]     = ".";
        inline static const value_type dot_dot[] = "..";
        // clang-format on

    public:
        // Constructors and destructor

        /// Constructs an empty path.
        path() = default;

        /// Constructs a path by copying `_p`.
        ///
        /// \param[in] _p The path to copy.
        path(const path& _p) = default;

        /// Constructs a path by moving `_p`.
        ///
        /// \param[in,out] _p The path to move from.
        path(path&& _p) noexcept = default;

        /// Constructs a path from the null-terminated character sequence `_source`.
        ///
        /// \param[in] _source The source path string.
        path(const value_type* _source)
            : value_{_source}
        {
        }

        /// Constructs a path from the null-terminated character sequence `_source`.
        ///
        /// \param[in] _source The source path string.
        path(value_type* _source)
            : value_{_source}
        {
        }

        /// Constructs a path by moving the string `_source`.
        ///
        /// \param[in,out] _source The source path string.
        path(string_type&& _source) noexcept
            : value_{std::move(_source)}
        {
        }

        // Delegating constructor

        /// Constructs a path from a pathable source `_source`.
        ///
        /// \param[in] _source The source object providing path characters.
        template <typename Source,
                  typename = std::enable_if_t<path_traits::is_pathable<Source>>>
        path(const Source& _source)
            : path{std::begin(_source), std::end(_source)}
        {
        }

        /// Constructs a path from the character range [`_begin`, `_end`).
        ///
        /// \param[in] _begin The first character in the range.
        /// \param[in] _end One past the last character in the range.
        template <typename InputIterator,
                  typename ValueType = typename std::iterator_traits<InputIterator>::value_type,
                  typename = std::enable_if_t<std::is_same_v<ValueType, value_type>>>
        path(InputIterator _begin, InputIterator _end)
            : value_{_begin, _end}
        {
        }

        /// Destroys the path.
        ~path() = default;

        // Assignments

        /// Replaces the path contents by copying `_p`.
        ///
        /// \param[in] _p The path to copy.
        /// \return `*this`.
        auto operator=(const path& _p) -> path& = default;

        /// Replaces the path contents by moving `_p`.
        ///
        /// \param[in,out] _p The path to move from.
        /// \return `*this`.
        auto operator=(path&& _p) noexcept -> path& = default;

        /// Replaces the path contents with the null-terminated string `_p`.
        ///
        /// \param[in] _p The source path string.
        /// \return `*this`.
        auto operator=(const value_type* _p) -> path&
        {
            value_ = _p;
            return *this;
        }

        /// Replaces the path contents with the null-terminated string `_p`.
        ///
        /// \param[in] _p The source path string.
        /// \return `*this`.
        auto operator=(value_type* _p) -> path&
        {
            value_ = _p;
            return *this;
        }

        /// Replaces the path contents by moving `_source`.
        ///
        /// \param[in,out] _source The source path string.
        /// \return `*this`.
        auto operator=(string_type&& _source) -> path&
        {
            value_ = std::move(_source);
            return *this;
        }

        /// Replaces the path contents from a pathable source `_source`.
        ///
        /// \param[in] _source The source object providing path characters.
        /// \return `*this`.
        template <typename Source,
                  typename = std::enable_if_t<path_traits::is_pathable<Source>>>
        auto operator=(const Source& _source) -> path&
        {
            string_type new_path{std::begin(_source), std::end(_source)};
            value_ = std::move(new_path);
            return *this;
        }

        /// Replaces the path contents by moving `_source`.
        ///
        /// \param[in,out] _source The source path string.
        /// \return `*this`.
        auto assign(string_type&& _source) -> path&
        {
            return *this = std::move(_source);
        }

        /// Replaces the path contents from a pathable source `_source`.
        ///
        /// \param[in] _source The source object providing path characters.
        /// \return `*this`.
        template <typename Source,
                  typename = std::enable_if_t<path_traits::is_pathable<Source>>>
        auto assign(const Source& _source) -> path&
        {
            return *this = _source;
        }

        /// Replaces the path contents with the character range [`_begin`, `_end`).
        ///
        /// \param[in] _begin The first character in the range.
        /// \param[in] _end One past the last character in the range.
        /// \return `*this`.
        template <typename InputIterator,
                  typename ValueType = typename std::iterator_traits<InputIterator>::value_type,
                  typename = std::enable_if_t<std::is_same_v<ValueType, value_type>>>
        auto assign(InputIterator _begin, InputIterator _end) -> path&
        {
            return *this = string_type{_begin, _end};
        }

        // Appends

        /// Appends `_p` as a path element sequence.
        ///
        /// \param[in] _p The path to append.
        /// \return `*this`.
        auto operator/=(const path& _p) -> path&;

        /// Appends `_source` as a path element sequence.
        ///
        /// \param[in] _source The source path to append.
        /// \return `*this`.
        template <typename Source,
                  typename = std::enable_if_t<path_traits::is_pathable<Source>>>
        auto operator/=(const Source& _source) -> path&
        {
            return *this /= path{_source};
        }

        /// Appends `_source` as a path element sequence.
        ///
        /// \param[in] _source The source path to append.
        /// \return `*this`.
        template <typename Source,
                  typename = std::enable_if_t<path_traits::is_pathable<Source>>>
        auto append(const Source& _source) -> path&
        {
            return *this /= path{_source};
        }

        /// Appends the character range [`_begin`, `_end`) as a path element sequence.
        ///
        /// \param[in] _begin The first character in the range.
        /// \param[in] _end One past the last character in the range.
        /// \return `*this`.
        template <typename InputIterator,
                  typename ValueType = typename std::iterator_traits<InputIterator>::value_type,
                  typename = std::enable_if_t<std::is_same_v<ValueType, value_type>>>
        auto append(InputIterator _begin, InputIterator _end) -> path&
        {
            return *this /= path{_begin, _end};
        }

        // Concatenation

        // clang-format off
        auto operator+=(const path& _p) -> path&        { value_ += _p.value_; return *this; } ///< Concatenates `_p` to this path and returns `*this`.
        auto operator+=(const string_type& _p) -> path& { value_ += _p; return *this; } ///< Concatenates `_p` to this path and returns `*this`.
        auto operator+=(std::string_view _p) -> path&   { value_ += _p; return *this; } ///< Concatenates `_p` to this path and returns `*this`.
        auto operator+=(const value_type* _p) -> path&  { value_ += _p; return *this; } ///< Concatenates the null-terminated string `_p` to this path and returns `*this`.
        auto operator+=(value_type _p) -> path&         { value_ += _p; return *this; } ///< Appends the character `_p` to this path and returns `*this`.
        // clang-format on

        /// Concatenates the pathable source `_p` to this path.
        ///
        /// \param[in] _p The source path fragment to concatenate.
        /// \return `*this`.
        template <typename Source,
                  typename = std::enable_if_t<path_traits::is_pathable<Source>>>
        auto operator+=(const Source& _p) -> path&
        {
            return *this += path{_p};
        }

        // template <typename CharT>
        // auto operator+=(CharT _p) -> path&;

        /// Concatenates the character range [`_begin`, `_end`) to this path.
        ///
        /// \param[in] _begin The first character in the range.
        /// \param[in] _end One past the last character in the range.
        /// \return `*this`.
        template <typename InputIterator,
                  typename ValueType = typename std::iterator_traits<InputIterator>::value_type,
                  typename = std::enable_if_t<std::is_same_v<ValueType, value_type>>>
        auto concat(InputIterator _begin, InputIterator _end) -> path&
        {
            return *this += path{_begin, _end};
        }

        // Modifiers

        auto clear() -> void { value_.clear(); } ///< Removes all characters from the path.

        /// Removes the final object name from the path.
        ///
        /// \return `*this`.
        auto remove_object_name() -> path&;

        /// Replaces the final object name with `_replacement`.
        ///
        /// \param[in] _replacement The replacement object name.
        /// \return `*this`.
        auto replace_object_name(const path& _replacement = {}) -> path&;

        /// Replaces the extension with `_replacement`.
        ///
        /// \param[in] _replacement The replacement extension.
        /// \return `*this`.
        auto replace_extension(const path& _replacement = {}) -> path&;

        auto swap(path& _rhs) -> void { value_.swap(_rhs.value_); } ///< Exchanges the contents of this path with `_rhs`.

        // Lexical operations

        /// Returns a normalized lexical form of this path.
        auto lexically_normal() const -> path;

        /// Returns this path relative to `_base` when such a path can be formed.
        ///
        /// \param[in] _base The base path used to compute the relative path.
        /// \return The relative path, or an empty path if no relative path can be formed.
        auto lexically_relative(const path& _base) const -> path;

        /// Returns `lexically_relative(_base)` or this path if no relative path can be formed.
        ///
        /// \param[in] _base The base path used to compute the proximate path.
        /// \return A relative path when possible, otherwise this path.
        auto lexically_proximate(const path& _base) const -> path;

        // Format observers

        // clang-format off
        auto c_str() const noexcept -> const value_type* { return value_.c_str(); } ///< Returns a pointer to the null-terminated path string.
        auto string() const -> string_type               { return value_; } ///< Returns the path as a string.
        operator string_type() const                     { return value_; } ///< Converts the path to a string.
        // clang-format on

        // Compare

        /// Compares this path with `_p` lexicographically.
        ///
        /// \param[in] _p The path to compare against.
        /// \return A negative value, zero, or a positive value if this path is less than, equal to, or greater than `_p`.
        auto compare(const path& _p) const noexcept -> int;

        // Decomposition

        /// Returns the root collection component of the path.
        auto root_collection() const -> path;

        /// Returns the portion of the path after the root collection.
        auto relative_path() const -> path;

        /// Returns the parent path.
        auto parent_path() const -> path;

        /// Returns the final object name component.
        auto object_name() const -> path;

        /// Returns the object name without its extension.
        auto stem() const -> path;

        /// Returns the extension of the object name.
        auto extension() const -> path;

        // Query

        // clang-format off
        auto empty() const -> bool               { return value_.empty(); } ///< Returns true if the path is empty.
        auto has_root_collection() const -> bool { return !root_collection().empty(); } ///< Returns true if the path has a root collection.
        auto has_relative_path() const -> bool   { return !relative_path().empty(); } ///< Returns true if the path has a relative portion.
        auto has_parent_path() const -> bool     { return !parent_path().empty(); } ///< Returns true if the path has a parent path.
        auto has_object_name() const -> bool     { return !object_name().empty(); } ///< Returns true if the path has an object name.
        auto has_stem() const -> bool            { return !stem().empty(); } ///< Returns true if the object name has a stem.
        auto has_extension() const -> bool       { return !extension().empty(); } ///< Returns true if the object name has an extension.
        auto is_absolute() const -> bool         { return !empty() && preferred_separator == value_.front(); } ///< Returns true if the path begins at the root collection.
        auto is_relative() const -> bool         { return !is_absolute(); } ///< Returns true if the path does not begin at the root collection.
        // clang-format on

        // Iterators

        /// Returns an iterator to the first path component.
        auto begin() const -> iterator;

        /// Returns an iterator one past the last path component.
        auto end() const -> iterator;

        /// Returns a reverse iterator to the last path component.
        auto rbegin() const -> reverse_iterator;

        /// Returns a reverse iterator one before the first path component.
        auto rend() const -> reverse_iterator;

    private:
        void append_separator_if_needed(const path& _p);

        string_type value_;
    }; // path

    /// Iterates over the individual components of a path.
    class path::iterator
    {
    public:
        // clang-format off
        using value_type        = const path;
        using pointer           = value_type*;
        using reference         = value_type&;
        using difference_type   = std::ptrdiff_t;
        using iterator_category = std::bidirectional_iterator_tag;
        // clang-format on

        /// Constructs a default iterator.
        iterator() = default;

        /// Constructs an iterator positioned at the first component of `_p`.
        ///
        /// \param[in] _p The path to iterate over.
        explicit iterator(const path& _p);

        /// Constructs an iterator by copying `_other`.
        ///
        /// \param[in] _other The iterator to copy.
        iterator(const iterator& _other) = default;

        /// Replaces this iterator with a copy of `_other`.
        ///
        /// \param[in] _other The iterator to copy.
        /// \return `*this`.
        auto operator=(const iterator& _other) -> iterator& = default;

        /// Constructs an iterator by moving `_other`.
        ///
        /// \param[in,out] _other The iterator to move from.
        iterator(iterator&& _other) = default;

        /// Replaces this iterator by moving `_other`.
        ///
        /// \param[in,out] _other The iterator to move from.
        /// \return `*this`.
        auto operator=(iterator&& _other) -> iterator& = default;

        /// Destroys the iterator.
        ~iterator() = default;

        /// Returns true if both iterators refer to the same component.
        auto operator==(const iterator& _other) const noexcept -> bool
        {
            return _other.path_ptr_ == path_ptr_ && _other.pos_ == pos_;
        }

        auto operator!=(const iterator& _other) const noexcept -> bool { return !(*this == _other); } ///< Returns true if the iterators refer to different components.

        // clang-format off
        auto operator*() const -> reference { return element_; } ///< Returns the current path component.
        auto operator->() const -> pointer  { return &element_; } ///< Returns a pointer to the current path component.
        // clang-format on

        /// Advances the iterator to the next path component.
        ///
        /// \return A reference to the updated iterator.
        auto operator++() -> iterator&;

        auto operator++(int) -> iterator { auto it = *this; ++(*this); return it; } ///< Advances the iterator and returns the previous position.

        /// Moves the iterator to the previous path component.
        ///
        /// \return A reference to the updated iterator.
        auto operator--() -> iterator&;

        auto operator--(int) -> iterator { auto it = *this; --(*this); return it; } ///< Decrements the iterator and returns the previous position.

    private:
        friend class path;

        pointer path_ptr_;
        path element_;
        path::string_type::size_type pos_;
    }; // iterator

    /// Iterates over the individual components of a path in reverse order.
    class path::reverse_iterator
    {
    public:
        // clang-format off
        using value_type        = iterator::value_type;
        using pointer           = iterator::pointer;
        using reference         = iterator::reference;
        using difference_type   = iterator::difference_type;
        using iterator_category = iterator::iterator_category;
        // clang-format on

        /// Constructs a default reverse iterator.
        reverse_iterator() = default;

        /// Constructs a reverse iterator from the forward iterator `_it`.
        ///
        /// \param[in] _it The forward iterator designating the reverse base position.
        explicit reverse_iterator(iterator _it)
            : it_{_it}
            , element_{}
        {
            auto t = it_;
            element_ = *--t;
        }

        /// Constructs a reverse iterator by copying another reverse iterator.
        reverse_iterator(const reverse_iterator&) = default;

        /// Replaces this reverse iterator with a copy of another reverse iterator.
        ///
        /// \return `*this`.
        auto operator=(const reverse_iterator&) -> reverse_iterator& = default;

        /// Constructs a reverse iterator by moving another reverse iterator.
        reverse_iterator(reverse_iterator&&) = default;

        /// Replaces this reverse iterator by moving another reverse iterator.
        ///
        /// \return `*this`.
        auto operator=(reverse_iterator&&) -> reverse_iterator& = default;

        /// Destroys the reverse iterator.
        ~reverse_iterator() = default;

        // clang-format off
        auto operator*() const -> reference { return element_; } ///< Returns the current path element.
        auto operator->() const -> pointer  { return &element_; } ///< Returns a pointer to the current path element.

        auto operator==(const reverse_iterator& _rhs) const noexcept -> bool { return _rhs.it_ == it_; } ///< Returns true if both iterators refer to the same component.
        auto operator!=(const reverse_iterator& _rhs) const noexcept -> bool { return !(_rhs == *this); } ///< Returns true if the iterators refer to different components.
        // clang-format on

        /// Advances the iterator toward the beginning of the path.
        ///
        /// \return A reference to the updated iterator.
        auto operator++() -> reverse_iterator&
        {
            auto t = --it_;
            element_ = *--t;
            return *this;
        }

        /// Advances the iterator toward the beginning of the path and returns the previous position.
        ///
        /// \return A copy of the iterator before it was advanced.
        auto operator++(int) -> reverse_iterator
        {
            auto it = *this;
            ++(*this);
            return it;
        }

        /// Moves the iterator toward the end of the path.
        ///
        /// \return A reference to the updated iterator.
        auto operator--() -> reverse_iterator&
        {
            auto t = ++it_;
            element_ = *--t;
            return *this;
        }

        /// Moves the iterator toward the end of the path and returns the previous position.
        ///
        /// \return A copy of the iterator before it was decremented.
        auto operator--(int) -> reverse_iterator
        {
            auto it = *this;
            --(*this);
            return it;
        }

    private:
        iterator it_;
        path element_;
    }; // reverse_iterator

    // clang-format off
    /// Returns true if the first path component range compares lexicographically less than the second.
    ///
    /// \param[in] _first1 The first iterator of the left-hand range.
    /// \param[in] _last1 One past the last iterator of the left-hand range.
    /// \param[in] _first2 The first iterator of the right-hand range.
    /// \param[in] _last2 One past the last iterator of the right-hand range.
    /// \return True if the first range is lexicographically less than the second.
    auto lexicographical_compare(path::iterator _first1, path::iterator _last1,
                                 path::iterator _first2, path::iterator _last2) -> bool;

    inline auto operator==(const path& _lhs, const path& _rhs) noexcept -> bool { return _lhs.compare(_rhs) == 0; } ///< Returns true if `_lhs` and `_rhs` compare equal.
    inline auto operator!=(const path& _lhs, const path& _rhs) noexcept -> bool { return _lhs.compare(_rhs) != 0; } ///< Returns true if `_lhs` and `_rhs` compare unequal.
    inline auto operator< (const path& _lhs, const path& _rhs) noexcept -> bool { return _lhs.compare(_rhs) <  0; } ///< Returns true if `_lhs` compares less than `_rhs`.
    inline auto operator<=(const path& _lhs, const path& _rhs) noexcept -> bool { return _lhs.compare(_rhs) <= 0; } ///< Returns true if `_lhs` does not compare greater than `_rhs`.
    inline auto operator> (const path& _lhs, const path& _rhs) noexcept -> bool { return _lhs.compare(_rhs) >  0; } ///< Returns true if `_lhs` compares greater than `_rhs`.
    inline auto operator>=(const path& _lhs, const path& _rhs) noexcept -> bool { return _lhs.compare(_rhs) >= 0; } ///< Returns true if `_lhs` does not compare less than `_rhs`.

    inline auto operator/(const path& _lhs, const path& _rhs) -> path { return path{_lhs} /= _rhs; } ///< Returns a new path formed by appending `_rhs` to `_lhs`.
    // clang-format on

    /// Writes `_p` to the output stream `_os`.
    ///
    /// \param[in,out] _os The destination stream.
    /// \param[in] _p The path to write.
    /// \return `_os`.
    auto operator<<(std::ostream& _os, const path& _p) -> std::ostream&;

    /// Reads a path from the input stream `_is` into `_p`.
    ///
    /// \param[in,out] _is The source stream.
    /// \param[out] _p The path receiving the extracted value.
    /// \return `_is`.
    auto operator>>(std::istream& _is, path& _p) -> std::istream&;

    inline auto swap(path& _lhs, path& _rhs) noexcept -> void { _lhs.swap(_rhs); } ///< Exchanges the contents of `_lhs` and `_rhs`.

    /// Returns a hash value for `_p`.
    ///
    /// \param[in] _p The path to hash.
    /// \return The hash value for `_p`.
    auto hash_value(const path& _p) noexcept -> std::size_t;

    /// Returns the zone name identified by `_p`, if one can be determined.
    ///
    /// \param[in] _p The path to inspect.
    /// \return The zone name, or `std::nullopt` if none can be determined.
    auto zone_name(const path& _p) -> std::optional<std::string>;
} // namespace irods::experimental::filesystem

#endif // IRODS_FILESYSTEM_PATH_HPP
