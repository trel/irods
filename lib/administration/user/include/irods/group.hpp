#ifndef IRODS_USER_ADMINISTRTION_GROUP_HPP
#define IRODS_USER_ADMINISTRTION_GROUP_HPP

/// \file

#include <string>
#include <ostream>

/// Namespace containing administrative user and group types.
namespace irods::experimental::administration
{
    /// Inline namespace for the current user administration ABI.
    inline namespace v1
    {
        /// Represents an iRODS group.
        struct group
        {
            /// Constructs a group from its name.
            ///
            /// \param[in] name The group name.
            explicit group(std::string name);

            /// Compares two groups for equality.
            ///
            /// \param[in] other The group to compare against.
            ///
            /// \retval true The groups are equal.
            /// \retval false Otherwise.
            auto operator==(const group& other) const noexcept -> bool;
            /// Compares two groups for inequality.
            ///
            /// \param[in] other The group to compare against.
            ///
            /// \retval true The groups are not equal.
            /// \retval false Otherwise.
            auto operator!=(const group& other) const noexcept -> bool;
            /// Orders groups lexicographically.
            ///
            /// \param[in] other The group to compare against.
            ///
            /// \retval true This group is ordered before \p other.
            /// \retval false Otherwise.
            auto operator< (const group& other) const noexcept -> bool;

            /// The group name.
            std::string name;
        }; // group

        /// Writes a group to an output stream.
        ///
        /// \param[in,out] out The destination stream.
        /// \param[in] user The group to write.
        ///
        /// \return \p out.
        auto operator<<(std::ostream& out, const group& user) -> std::ostream&;
    } // namespace v1
} // namespace irods::experimental::administration

#endif // IRODS_USER_ADMINISTRTION_GROUP_HPP
