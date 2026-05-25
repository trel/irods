#ifndef IRODS_PACK_TABLE_HPP
#define IRODS_PACK_TABLE_HPP

/// \file

#include "irods/irods_lookup_table.hpp"
#include "irods/packStruct.h"

namespace irods
{
    /// Represents a single entry in the pack table.
    class pack_entry
    {
      public:
        /// Pack instruction string associated with the entry.
        std::string packInstruct;
    }; // class pack_entry

    /// Provides lookup access to pack table entries.
    class pack_entry_table : public lookup_table<pack_entry>
    {
      public:
        // NOLINTNEXTLINE(modernize-avoid-c-arrays, cppcoreguidelines-avoid-c-arrays)
        /// Constructs the pack entry table from the provided definitions.
        explicit pack_entry_table(const packInstruct_t _defs[]);
    }; // class pack_entry_table

    // NOLINTNEXTLINE(modernize-use-trailing-return-type)
    /// Returns the global pack entry table.
    irods::pack_entry_table& get_pack_table();

    /// No-op clear function for input structures.
    void clearInStruct_noop(void*);

    /// No-op clear function for output structures.
    void clearOutStruct_noop(void*);
} // namespace irods

#endif // IRODS_PACK_TABLE_HPP
