#ifndef IRODS_METADATA_HPP
#define IRODS_METADATA_HPP

#include "irods/entity.hpp"

#include <string>
#include <vector>
#include <map>

#ifdef IRODS_METADATA_ENABLE_SERVER_SIDE_API
struct RsComm;
using rxComm_t = RsComm;
#else
struct RcComm;
using rxComm_t = RcComm;
#endif // IRODS_METADATA_ENABLE_SERVER_SIDE_API

namespace irods::experimental::metadata
{
    /// Alias for the supported metadata entity type enumeration.
    using entity_type = irods::experimental::entity::entity_type;

    /// Represents a metadata attribute-value-units triple.
    struct avu
    {
        std::string attribute; ///< Metadata attribute name.
        std::string value; ///< Metadata value.
        std::string units; ///< Metadata units string.
    };

    /// Compares two AVU triples for equality.
    ///
    /// \param[in] _lhs The first AVU.
    /// \param[in] _rhs The second AVU.
    /// \return True if all AVU fields are equal.
    bool operator==(const avu& _lhs, const avu& _rhs) noexcept;

    /// Converts an entity string token into an entity enumeration value.
    ///
    /// \param[in] _t The entity token string.
    /// \return The matching entity type.
    /// \since 4.2.9
    entity_type to_entity_type(const std::string& _t);

    /// Converts an entity type enumeration to the token expected by metadata APIs.
    ///
    /// \param[in] _t The entity type to translate.
    /// \return The metadata API token string.
    /// \since 4.2.9
    std::string to_entity_token(entity_type _t);

    /// Converts an entity type enumeration to a user-facing string.
    ///
    /// \param[in] _t The entity type to translate.
    /// \return The string representation of the entity type.
    /// \since 4.2.9
    std::string to_entity_string(entity_type _t);

    /// Sets metadata for an entity, replacing an existing matching AVU if necessary.
    ///
    /// \param[in,out] _comm The client or server connection.
    /// \param[in] _md The AVU to apply.
    /// \param[in] _et The target entity type.
    /// \param[in] _tgt The target entity identifier.
    /// \since 4.2.9
    void set(rxComm_t& _comm, const avu& _md, entity_type _et, const std::string& _tgt);

    /// Adds metadata to an entity without overwriting an existing AVU.
    ///
    /// \param[in,out] _comm The client or server connection.
    /// \param[in] _md The AVU to add.
    /// \param[in] _et The target entity type.
    /// \param[in] _tgt The target entity identifier.
    /// \since 4.2.9
    void add(rxComm_t& _comm, const avu& _md, entity_type _et, const std::string& _tgt);

    /// Removes metadata from an entity.
    ///
    /// \param[in,out] _comm The client or server connection.
    /// \param[in] _md The AVU to remove.
    /// \param[in] _et The target entity type.
    /// \param[in] _tgt The target entity identifier.
    /// \since 4.2.9
    void remove(rxComm_t& _comm, const avu& _md, entity_type _et, const std::string& _tgt);

    /// Replaces one metadata triple on an entity with another.
    ///
    /// \param[in,out] _comm The client or server connection.
    /// \param[in] _md The existing AVU to match.
    /// \param[in] _dmd The replacement AVU.
    /// \param[in] _et The target entity type.
    /// \param[in] _tgt The target entity identifier.
    /// \since 4.2.9
    void modify(rxComm_t& _comm, const avu& _md, const avu& _dmd, entity_type _et, const std::string& _tgt);

    /// Fetches all metadata attached to an entity.
    ///
    /// \param[in,out] _comm The client or server connection.
    /// \param[in] _et The target entity type.
    /// \param[in] _tgt The target entity identifier.
    /// \return The metadata triples attached to the entity.
    /// \since 4.2.9
    std::vector<avu> get(rxComm_t& _comm, entity_type _et, const std::string& _tgt);
} // namespace irods::experimental::metadata

#endif // IRODS_METADATA_HPP
