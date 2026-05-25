#include "irods/ADLER32Strategy.hpp"

#include "irods/base64.hpp"
#include "irods/checksum.h"
#include "irods/rodsErrorTable.h"

#include <cstring>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>

#include <boost/algorithm/string/predicate.hpp>

static constexpr std::uint32_t ADLER32_DIGEST_LENGTH = 4;

namespace irods {

    const std::string ADLER32_NAME( "adler32" );

    /// @brief Stores the rolling Adler-32 state.
    struct adler32_parts {
        /// The low-order Adler-32 accumulator.
        uint32_t a;

        /// The high-order Adler-32 accumulator.
        uint32_t b;
    };

    /// @brief Returns the initial Adler-32 state.
    adler32_parts adler32_init() {
        return adler32_parts{1, 0};
    }

    /// @brief Updates the Adler-32 state with additional bytes.
    static adler32_parts adler32_update(const adler32_parts& parts, const unsigned char *data, size_t len) {

        const uint32_t MOD_ADLER = 65521;

        uint32_t a = parts.a, b = parts.b;

        // Process each byte of the data in order
        for (size_t index = 0; index < len; ++index)
        {
            a = (a + data[index]) % MOD_ADLER;
            b = (b + a) % MOD_ADLER;
        }

        return adler32_parts{a, b};
    }

    /// @brief Converts the rolling Adler-32 state into the final checksum value.
    static uint32_t adler32_final(const adler32_parts& parts) {
        return (parts.b << 16) | parts.a;
    }


    /// @brief Initializes the hashing context.
    error
    ADLER32Strategy::init( boost::any& _context ) const {
        _context = adler32_init();
        return SUCCESS();
    }

    /// @brief Updates the hashing context with input data.
    error
    ADLER32Strategy::update( const std::string& data, boost::any& _context ) const {

        _context = adler32_update(boost::any_cast<adler32_parts>(_context), reinterpret_cast<const unsigned char*>(data.c_str()), data.size());
        return SUCCESS();
    }

    /// @brief Produces the default checksum string for the context.
    error
    ADLER32Strategy::digest( std::string& _messageDigest, boost::any& _context ) const {
        return digest(
            irods::hash::options{.output_mode = irods::hash::output_mode::hex_string, .include_checksum_prefix = true},
            _context,
            _messageDigest);
    }

    /// @brief Reports whether the string has the Adler-32 checksum prefix.
    bool ADLER32Strategy::isChecksum(const std::string& _chksum) const
    {
        return _chksum.starts_with(ADLER32_CHKSUM_PREFIX);
    }

    /// @brief Produces the checksum string according to the requested options.
    auto ADLER32Strategy::digest(const hash::options& _options, boost::any& _context, std::string& _out) const
        -> irods::error
    {
        if (irods::hash::output_mode::hex_string != _options.output_mode) {
            return ERROR(SYS_NOT_IMPLEMENTED, "Only hex string output is supported for this strategy.");
        }

        const std::uint32_t result = adler32_final(boost::any_cast<adler32_parts>(_context));

        std::stringstream ss;
        if (_options.include_checksum_prefix) {
            ss << checksum_prefix();
        }
        ss << std::setfill('0') << std::hex << std::setw(ADLER32_DIGEST_LENGTH * 2) << result;

        _out += ss.str();

        return SUCCESS();
    } // ADLER32Strategy::digest

    /// @brief Returns the checksum prefix for Adler-32 digests.
    auto ADLER32Strategy::checksum_prefix() const -> std::string_view
    {
        return ADLER32_CHKSUM_PREFIX;
    } // ADLER32Strategy::checksum_prefix

    /// @brief Releases resources associated with the hashing context.
    void ADLER32Strategy::free_context(boost::any& context) const {}
}; // namespace irods
