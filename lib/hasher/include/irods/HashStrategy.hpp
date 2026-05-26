#ifndef _HASH_STRATEGY_HPP_
#define _HASH_STRATEGY_HPP_

/// \file

#include "irods/irods_error.hpp"
#include "irods/hash_types.hpp"

#include <cstdint>
#include <string>
#include <string_view>

#include <boost/any.hpp>

namespace irods {

    /// Defines the interface implemented by hashing strategies.
    class HashStrategy {
        public:

            /// Destroys the hashing strategy.
            virtual ~HashStrategy() {};

            /// Returns the strategy name.
            virtual std::string name() const = 0;
            /// Initializes the hashing context.
            virtual error init( boost::any& context ) const = 0;
            /// Feeds data into the hashing context.
            virtual error update( const std::string&, boost::any& context ) const = 0;
            /// Produces the digest using default formatting.
            virtual error digest( std::string& messageDigest, boost::any& context ) const = 0;
            /// Reports whether the provided string matches this strategy's checksum format.
            virtual bool isChecksum( const std::string& ) const = 0;
            /// Releases resources owned by the hashing context.
            virtual void free_context(boost::any& _context) const = 0;

            /// Produce a digest in string form based on the provided options.
            ///
            /// \param[in] _options \p irods::hash::options specifying how the output string should be formatted.
            /// \param[in] _context context used to store information about the hash as it is updated.
            /// \param[out] _out The output string.
            ///
            /// \return irods::error Any errors that are encountered are held in the returned \p irods::error.
            ///
            /// \since 5.1.0
            virtual auto digest(const hash::options& _options, boost::any& _context, std::string& _out) const
                -> irods::error = 0;

            /// Return the checksum prefix for this strategy. Usually this is the strategy name followed by a colon.
            ///
            /// \return std::string_view The checksum prefix.
            ///
            /// \since 5.1.0
            [[nodiscard]] virtual auto checksum_prefix() const -> std::string_view = 0;
    };
} // namespace irods

#endif // _HASH_STRATEGY_HPP_
