#ifndef IRODS_GENQUERY2_SQL_HPP
#define IRODS_GENQUERY2_SQL_HPP

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace irods::experimental::genquery2
{
    /// Forward declaration of the GenQuery2 select AST node.
    struct select;

    /// Options controlling SQL generation for a GenQuery2 AST.
    struct options
    {
        std::string_view user_name; ///< User name used for access filtering.
        std::string_view user_zone; ///< User zone used for access filtering.
        std::string_view database; ///< Database vendor identifier.
        std::uint16_t default_number_of_rows = 16; ///< Default row limit when none is provided.
        bool admin_mode = false; ///< Indicates whether administrative visibility rules apply.
    }; // struct options

    /// Converts a GenQuery2 AST into SQL text and bind arguments.
    ///
    /// \param[in] _select The parsed GenQuery2 select expression.
    /// \param[in] _opts Options controlling SQL generation.
    /// \return A tuple containing the SQL statement and its bind arguments.
    auto to_sql(const select& _select, const options& _opts) -> std::tuple<std::string, std::vector<std::string>>;
} // namespace irods::experimental::genquery2

#endif // IRODS_GENQUERY2_SQL_HPP
