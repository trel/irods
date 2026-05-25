#ifndef IRODS_USER_ADMINISTRTION_USER_HPP
#define IRODS_USER_ADMINISTRTION_USER_HPP

/// \file

#include <string>
#include <optional>
#include <ostream>

/// Namespace containing administrative user and group types.
namespace irods::experimental::administration
{
    /// Inline namespace for the current user administration ABI.
    inline namespace v1
    {
        /// Represents an iRODS user.
        struct user
        {
            /// Constructs a user from a name and optional zone.
            ///
            /// \param[in] name The user name.
            /// \param[in] zone The optional zone name.
            explicit user(std::string name, std::optional<std::string> zone = std::nullopt);

            /// Compares two users for equality.
            ///
            /// \param[in] other The user to compare against.
            ///
            /// \retval true The users are equal.
            /// \retval false Otherwise.
            auto operator==(const user& other) const noexcept -> bool;
            /// Compares two users for inequality.
            ///
            /// \param[in] other The user to compare against.
            ///
            /// \retval true The users are not equal.
            /// \retval false Otherwise.
            auto operator!=(const user& other) const noexcept -> bool;
            /// Orders users lexicographically.
            ///
            /// \param[in] other The user to compare against.
            ///
            /// \retval true This user is ordered before \p other.
            /// \retval false Otherwise.
            auto operator< (const user& other) const noexcept -> bool;

            /// The user name.
            std::string name;
            /// The user zone.
            std::string zone;
        }; // user

        /// Writes a user to an output stream.
        ///
        /// \param[in,out] out The destination stream.
        /// \param[in] user The user to write.
        ///
        /// \return \p out.
        auto operator<<(std::ostream& out, const user& user) -> std::ostream&;
    } // namespace v1
} // namespace irods::experimental::administration

#endif // IRODS_USER_ADMINISTRTION_USER_HPP
