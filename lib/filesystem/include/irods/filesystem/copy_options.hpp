#ifndef IRODS_FILESYSTEM_COPY_OPTIONS_HPP
#define IRODS_FILESYSTEM_COPY_OPTIONS_HPP

#include <cstdint>
#include <type_traits>

namespace irods::experimental::filesystem
{
    /// Options controlling filesystem copy operations.
    enum class copy_options : std::uint32_t
    {
        // clang-format off
        none               = 0U,  ///< Do not enable any optional behavior.
        skip_existing      = 1U,  ///< Leave existing targets unchanged.
        overwrite_existing = 2U,  ///< Replace existing targets.
        update_existing    = 4U,  ///< Replace targets only when the source is newer.
        recursive          = 8U,  ///< Recurse into subcollections.
        collections_only   = 16U, ///< Copy only collection structure.
        in_recursive_copy  = 32U  ///< Internal flag used during recursive copies.
        // clang-format on
    };

    /// Combines copy option flags into the left-hand operand.
    ///
    /// \param[in,out] _lhs The option set being updated.
    /// \param[in] _rhs The option flags to combine into \p _lhs.
    /// \return A reference to \p _lhs.
    inline auto operator|=(copy_options& _lhs, copy_options _rhs) noexcept -> copy_options&
    {
        using T = std::underlying_type_t<copy_options>;
        return _lhs = static_cast<copy_options>(static_cast<T>(_lhs) | static_cast<T>(_rhs));
    }

    /// Returns the bitwise OR of two copy option sets.
    ///
    /// \param[in] _lhs The first option set.
    /// \param[in] _rhs The second option set.
    /// \return The combined option set.
    inline auto operator|(copy_options _lhs, copy_options _rhs) noexcept -> copy_options
    {
        return _lhs |= _rhs;
    }

    /// Intersects copy option flags into the left-hand operand.
    ///
    /// \param[in,out] _lhs The option set being updated.
    /// \param[in] _rhs The option flags to intersect with \p _lhs.
    /// \return A reference to \p _lhs.
    inline auto operator&=(copy_options& _lhs, copy_options _rhs) noexcept -> copy_options&
    {
        using T = std::underlying_type_t<copy_options>;
        return _lhs = static_cast<copy_options>(static_cast<T>(_lhs) & static_cast<T>(_rhs));
    }

    /// Returns the bitwise AND of two copy option sets.
    ///
    /// \param[in] _lhs The first option set.
    /// \param[in] _rhs The second option set.
    /// \return The intersected option set.
    inline auto operator&(copy_options _lhs, copy_options _rhs) noexcept -> copy_options
    {
        return _lhs &= _rhs;
    }

    /// Applies bitwise exclusive OR to the left-hand operand.
    ///
    /// \param[in,out] _lhs The option set being updated.
    /// \param[in] _rhs The option flags to toggle in \p _lhs.
    /// \return A reference to \p _lhs.
    inline auto operator^=(copy_options& _lhs, copy_options _rhs) noexcept -> copy_options&
    {
        using T = std::underlying_type_t<copy_options>;
        return _lhs = static_cast<copy_options>(static_cast<T>(_lhs) ^ static_cast<T>(_rhs));
    }

    /// Returns the bitwise exclusive OR of two copy option sets.
    ///
    /// \param[in] _lhs The first option set.
    /// \param[in] _rhs The second option set.
    /// \return The toggled option set.
    inline auto operator^(copy_options _lhs, copy_options _rhs) noexcept -> copy_options
    {
        return _lhs ^= _rhs;
    }

    /// Inverts all bits in a copy option set in place.
    ///
    /// \param[in,out] _value The option set to invert.
    /// \return A reference to \p _value.
    inline auto operator~(copy_options& _value) noexcept -> copy_options&
    {
        using T = std::underlying_type_t<copy_options>;
        return _value = static_cast<copy_options>(~static_cast<T>(_value));
    }
} // namespace irods::experimental::filesystem

#endif // IRODS_FILESYSTEM_COPY_OPTIONS_HPP
