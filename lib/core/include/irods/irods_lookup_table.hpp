#ifndef PLUGIN_TABLE_HPP
#define PLUGIN_TABLE_HPP

// =-=-=-=-=-=-=-
// irods includes
#include "irods/rodsErrorTable.h"
#include "irods/irods_hash.hpp"
#include "irods/irods_error.hpp"
#include "irods/irods_stacktrace.hpp"
#include "irods/rodsLog.h"

// =-=-=-=-=-=-=-
// stl includes
#include <string>

// =-=-=-=-=-=-=-
// boost includes
#include <boost/any.hpp>

#include <fmt/format.h>

namespace irods
{

    // =-=-=-=-=-=-=-
    // class to manage tables of plugins.  employing a class in order to use
    // RAII for adding entries to the table now that it is not a static array
    template <typename ValueType, typename KeyType = std::string, typename HashType = irods_string_hash>
    class lookup_table
    {
      protected:
        using irods_hash_map = HASH_TYPE<KeyType, ValueType, HashType>;

        irods_hash_map table_;

      public:
        // clang-format off
        using key_type                     = typename irods_hash_map::key_type;
        using value_type                   = typename irods_hash_map::mapped_type;
        using size_type                    = typename irods_hash_map::size_type;
        using hasher                       = typename irods_hash_map::hasher;
        using iterator                     = typename irods_hash_map::iterator;
        using iterator_value_type          = typename irods_hash_map::value_type;
        using const_iterator               = typename irods_hash_map::const_iterator;
        using const_iterator_value_type    =    const iterator_value_type;
        // clang-format on

        /// Constructs an empty lookup table.
        lookup_table() {}

        /// Destroys the lookup table.
        virtual ~lookup_table() {}

        /// Returns the value associated with \p _k, inserting a default value if needed.
        /// \param[in] _k The key identifying the value.
        /// \return A reference to the stored value.
        value_type& operator[](const key_type& _k)
        {
            return table_[_k];
        }

        /// Returns the value associated with \p _k, inserting a default value if needed.
        /// \param[in] _k The key identifying the value.
        /// \return A reference to the stored value.
        value_type& operator[](key_type&& _k)
        {
            return table_[_k];
        }

        /// Returns the number of entries in the table.
        /// \return The number of stored entries.
        size_type size() const noexcept
        {
            return table_.size();
        }

        /// Reports whether the table contains \p _k.
        /// \param[in] _k The key to search for.
        /// \return \c true if the key exists; otherwise, \c false.
        bool has_entry(const key_type& _k) const
        {
            return table_.contains(_k);
        }

        /// Removes the entry identified by \p _k.
        /// \param[in] _k The key to erase.
        /// \return The number of erased entries.
        size_type erase(const key_type& _k)
        {
            return table_.erase(_k);
        }

        /// Removes all entries from the table.
        void clear() noexcept
        {
            table_.clear();
        }

        /// Reports whether the table is empty.
        /// \return \c true if the table contains no entries; otherwise, \c false.
        [[nodiscard]]
        bool empty() const noexcept
        {
            return table_.empty();
        }

        /// Returns an iterator to the first entry.
        /// \return An iterator to the beginning of the table.
        iterator begin() noexcept
        {
            return table_.begin();
        }

        /// Returns an iterator to the first entry.
        /// \return A const iterator to the beginning of the table.
        const_iterator begin() const noexcept
        {
            return table_.begin();
        }

        /// Returns an iterator one past the last entry.
        /// \return An iterator to the end of the table.
        iterator end() noexcept
        {
            return table_.end();
        }

        /// Returns an iterator one past the last entry.
        /// \return A const iterator to the end of the table.
        const_iterator end() const noexcept
        {
            return table_.end();
        }

        /// Returns a const iterator to the first entry.
        /// \return A const iterator to the beginning of the table.
        const_iterator cbegin() const noexcept
        {
            return table_.cbegin();
        }

        /// Returns a const iterator one past the last entry.
        /// \return A const iterator to the end of the table.
        const_iterator cend() const noexcept
        {
            return table_.cend();
        }

        /// Searches for \p _k.
        /// \param[in] _k The key to search for.
        /// \return An iterator to the matching entry or \c end().
        iterator find(const key_type& _k)
        {
            return table_.find(_k);
        }

        /// Searches for \p _k.
        /// \param[in] _k The key to search for.
        /// \return A const iterator to the matching entry or \c end().
        const_iterator find(const key_type& _k) const
        {
            return table_.find(_k);
        }

        // =-=-=-=-=-=-=-
        /// Retrieves the value stored for \p _key.
        /// \param[in] _key The key identifying the entry.
        /// \param[out] _val The retrieved value.
        /// \return An error describing success or lookup failure.
        error get(const key_type& _key, value_type& _val)
        {
            auto _val_itr = find(_key);

            if (_val_itr == end()) {
                return ERROR(KEY_NOT_FOUND, fmt::format("failed to find key [{}] in table", _key));
            }

            _val = _val_itr->second;

            return SUCCESS();
        }

        // =-=-=-=-=-=-=-
        /// Stores \p _val under \p _key.
        /// \param[in] _key The key identifying the entry.
        /// \param[in] _val The value to store.
        /// \return An error describing success.
        error set(const key_type& _key, const value_type& _val)
        {
            table_.insert_or_assign(_key, _val);
            return SUCCESS();
        }

    }; // class lookup_table


    // =-=-=-=-=-=-=-
    // partial specialization created to support templating the get/set
    // functions which need to manage exception handling etc from
    // a boost::any_cast
    template <typename KeyType, typename HashType>
    class lookup_table<boost::any, KeyType, HashType>
    {
      protected:
        using irods_hash_map = HASH_TYPE<KeyType, boost::any, HashType>;

        irods_hash_map table_;

      public:
        // clang-format off
        using key_type                     = typename irods_hash_map::key_type;
        using value_type                   = typename irods_hash_map::mapped_type;
        using size_type                    = typename irods_hash_map::size_type;
        using hasher                       = typename irods_hash_map::hasher;
        using iterator                     = typename irods_hash_map::iterator;
        using iterator_value_type          = typename irods_hash_map::value_type;
        // clang-format on

        /// Constructs an empty lookup table.
        lookup_table() = default;

        /// Destroys the lookup table.
        virtual ~lookup_table() = default;

        /// Returns the value associated with \p _k, inserting a default value if needed.
        /// \param[in] _k The key identifying the value.
        /// \return A reference to the stored value.
        value_type& operator[](key_type _k)
        {
            return table_[_k];
        }

        /// Returns the number of entries in the table.
        /// \return The number of stored entries.
        size_type size() const noexcept
        {
            return table_.size();
        }

        /// Reports whether the table contains \p _k.
        /// \param[in] _k The key to search for.
        /// \return \c true if the key exists; otherwise, \c false.
        bool has_entry(const key_type& _k) const
        {
            return table_.contains(_k);
        }

        /// Removes the entry identified by \p _k.
        /// \param[in] _k The key to erase.
        /// \return The number of erased entries.
        size_type erase(const key_type& _k)
        {
            return table_.erase(_k);
        }

        /// Removes all entries from the table.
        void clear() noexcept
        {
            table_.clear();
        }

        /// Reports whether the table is empty.
        /// \return \c true if the table contains no entries; otherwise, \c false.
        [[nodiscard]]
        bool empty() const noexcept
        {
            return table_.empty();
        }

        /// Returns an iterator to the first entry.
        /// \return An iterator to the beginning of the table.
        iterator begin() noexcept
        {
            return table_.begin();
        }

        /// Returns an iterator one past the last entry.
        /// \return An iterator to the end of the table.
        iterator end() noexcept
        {
            return table_.end();
        }

        /// Searches for \p _k.
        /// \param[in] _k The key to search for.
        /// \return An iterator to the matching entry or \c end().
        iterator find(const key_type& _k)
        {
            return table_.find(_k);
        }

        // =-=-=-=-=-=-=-
        /// Retrieves the value stored for \p _key and converts it to \p T.
        /// \param[in] _key The key identifying the entry.
        /// \param[out] _val The retrieved value.
        /// \return An error describing success, lookup failure, or type mismatch.
        template <typename T>
        error get(const key_type& _key, T& _val)
        {
            // check params
            if (_key.empty()) {
                return ERROR(KEY_NOT_FOUND, "empty key");
            }

            auto _val_itr = find(_key);

            if (_val_itr == end()) {
                return ERROR(KEY_NOT_FOUND, fmt::format("failed to find key [{}] in table", _key));
            }

            // attempt to any_cast property value to given type
            try {
                _val = boost::any_cast<T>(_val_itr->second);
                return SUCCESS();
            }
            catch (const boost::bad_any_cast&) {
                return ERROR(KEY_TYPE_MISMATCH, fmt::format("type and property key [{}] mismatch", _key));
            }

            // invalid location in the code
            return ERROR(INVALID_LOCATION, "reached unreachable code");

        } // get

        // =-=-=-=-=-=-=-
        /// Stores \p _val under \p _key.
        /// \param[in] _key The key identifying the entry.
        /// \param[in] _val The value to store.
        /// \return An error describing success or invalid input.
        template <typename T>
        error set(const key_type& _key, const T& _val)
        {
            // check params
            if (_key.empty()) {
                return ERROR(KEY_NOT_FOUND, "empty key");
            }

            // add property to map
            table_.insert_or_assign(_key, _val);

            return SUCCESS();
        }

    }; // class lookup_table

    using plugin_property_map = lookup_table<boost::any>;

}; // namespace irods

#endif // PLUGIN_TABLE_HPP
