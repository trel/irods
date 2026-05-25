#ifndef IRODS_HASH_HPP
#define IRODS_HASH_HPP

/// \file

#include "irods/irods_stacktrace.hpp"
#include "irods/rodsLog.h"

#include <unordered_map>

/// \def HASH_TYPE
/// \brief Alias macro for the hash container type used by iRODS.
#define HASH_TYPE std::unordered_map

#include <string>
#include <iostream>

namespace irods
{
    /// Provides hashing and comparison support for iRODS string keys.
    struct irods_string_hash
    {
        /// Compile-time hash table configuration values.
        enum
        {
            /// Minimum number of elements per bucket.
            bucket_size = 4, // 0 < bucket_size

            /// Minimum number of buckets.
            min_buckets = 8
        }; // min_buckets = 2 ^^ N, 0 < N

        /// Destructor.
        ~irods_string_hash() = default;

        /// Computes a hash value for the provided string.
        std::size_t operator()(const std::string& s1) const
        {
            if (s1.empty()) {
                std::cerr << irods::stacktrace().dump();
                rodsLog(LOG_NOTICE, "irods_string_hash - empty string value");
                return 0;
            }

            const unsigned char *p = reinterpret_cast<const unsigned char*>(s1.c_str());
            std::size_t hashval = 0;

            while (*p != '\0') {
                hashval = 31 * hashval + *p++;
            }

            return hashval;
        }

        /// Compares two strings for ordering.
        bool operator()(const std::string& s1, const std::string& s2) const
        {
            return s1 < s2;
        }
    }; // struct irods_string_hash
} // namespace irods

#endif // IRODS_HASH_HPP
