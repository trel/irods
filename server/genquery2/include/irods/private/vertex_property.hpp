#ifndef IRODS_GENQUERY2_VERTEX_PROPERTY_HPP
#define IRODS_GENQUERY2_VERTEX_PROPERTY_HPP

#include <string_view>

namespace irods::experimental::genquery
{
    /// \brief Describes a graph vertex used when building joins.
    struct vertex_property
    {
        std::string_view table_name; ///< The table name represented by the vertex.
    }; // struct vertex_property
} // namespace irods::experimental::genquery

#endif // IRODS_GENQUERY2_VERTEX_PROPERTY_HPP
