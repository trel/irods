#ifndef IRODS_GENQUERY2_EDGE_PROPERTY_HPP
#define IRODS_GENQUERY2_EDGE_PROPERTY_HPP

#include <string_view>

namespace irods::experimental::genquery
{
    /// \brief Describes a graph edge used when building join relationships.
    struct edge_property
    {
        std::string_view join_condition; ///< The SQL join predicate for the edge.
        std::uint8_t position;           ///< The relative join position.
    }; // struct edge_property
} // namespace irods::experimental::genquery

#endif // IRODS_GENQUERY2_EDGE_PROPERTY_HPP
