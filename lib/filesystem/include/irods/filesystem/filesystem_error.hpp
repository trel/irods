#ifndef IRODS_FILESYSTEM_ERROR_HPP
#define IRODS_FILESYSTEM_ERROR_HPP

#include "irods/filesystem/path.hpp"

#include <cerrno>
#include <system_error>
#include <string>

namespace irods::experimental::filesystem
{
    /// \brief Forward declaration of the logical path type.
    class path;

    /// \brief Reports a filesystem operation failure along with related paths.
    // NOLINTNEXTLINE(cppcoreguidelines-special-member-functions)
    class filesystem_error
        : public std::system_error
    {
      public:
        /// \brief Constructs an error with a message and error code.
        explicit filesystem_error(const std::string& _msg, std::error_code _ec)
            : std::system_error{_ec, _msg}
        {
        }

        /// \brief Constructs an error associated with one path.
        // NOLINTNEXTLINE(modernize-pass-by-value)
        filesystem_error(const std::string& _msg, const path& _p1, std::error_code _ec)
            : std::system_error{_ec, _msg}
            , p1_{_p1}
        {
        }

        /// \brief Constructs an error associated with two paths.
        // NOLINTNEXTLINE(modernize-pass-by-value)
        filesystem_error(const std::string& _msg, const path& _p1, const path& _p2, std::error_code _ec)
            : std::system_error{_ec, _msg}
            , p1_{_p1}
            , p2_{_p2}
        {
        }

        /// \brief Constructs an error by copying another instance.
        filesystem_error(const filesystem_error& _other) = default;
        /// \brief Replaces this error with another instance.
        auto operator=(const filesystem_error& _other) -> filesystem_error& = default;

        /// \brief Destroys the error object.
        // NOLINTNEXTLINE(modernize-use-override)
        ~filesystem_error() = default;

        /// \brief Returns the first path associated with the error.
        [[nodiscard]] auto path1() const noexcept -> const path&
        {
            return p1_;
        }

        /// \brief Returns the second path associated with the error.
        [[nodiscard]] auto path2() const noexcept -> const path&
        {
            return p2_;
        }

      private:
        path p1_; ///< The first related path.
        path p2_; ///< The second related path.
    };
} // namespace irods::experimental::filesystem

#endif // IRODS_FILESYSTEM_ERROR_HPP
