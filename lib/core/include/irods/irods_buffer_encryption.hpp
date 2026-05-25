#ifndef __IRODS_BUFFER_ENCRYPTION_HPP__
#define __IRODS_BUFFER_ENCRYPTION_HPP__

// =-=-=-=-=-=-=-
// irods includes
#include "irods/rodsDef.h"

// =-=-=-=-=-=-=-
#include "irods/irods_error.hpp"

// =-=-=-=-=-=-=-
// boost includes
#include <boost/numeric/ublas/storage.hpp>

// =-=-=-=-=-=-=-
// ssl includes
#include <openssl/evp.h>

namespace irods {

/// @brief Encrypts and decrypts transfer buffers using OpenSSL EVP.
    class buffer_crypt {

        public:
            /// Byte buffer type used for keys, IVs, and payloads.
            typedef std::vector< unsigned char > array_t;

            /// Constructs a buffer encryptor using default settings.
            buffer_crypt();

            /// Constructs a buffer encryptor using the provided algorithm settings.
            buffer_crypt(
                int,           // key size in bytes
                int,           // salt size in bytes
                int,           // num hash rounds
                const char* ); // algorithm

            /// Destroys the buffer encryptor.
            ~buffer_crypt();

            /// Encrypts a plaintext buffer using the provided key material.
            irods::error encrypt(
                const array_t&, // key
                const array_t&, // initialization vector
                const array_t&, // plaintext buffer
                array_t& );     // encrypted buffer

            /// Decrypts a ciphertext buffer using the provided key material.
            irods::error decrypt(
                const array_t&, // key
                const array_t&, // initialization vector
                const array_t&, // encrypted buffer
                array_t& );     // plaintext buffer

            /// Generates an initialization vector for the current configuration.
            irods::error initialization_vector(
                array_t& );     // initialization vector

            /// Generates a random byte key.
            static irods::error generate_key(
                array_t&,       // random byte key
                int );          // key size in bytes

            /// Encodes a byte buffer as hexadecimal text.
            static irods::error hex_encode(
                const array_t&,     // bytes to encode
                std::string& );     // hex encoded bytes

            /// Returns the configured key size in bytes.
            int         key_size()        {
                return key_size_;
            };

            /// Returns the configured salt size in bytes.
            int         salt_size()       {
                return salt_size_;
            };

            /// Returns the configured number of hash rounds.
            int         num_hash_rounds() {
                return num_hash_rounds_;
            };

            /// Returns the configured cipher algorithm name.
            std::string algorithm()       {
                return algorithm_;
            };

        private:
            int         key_size_; ///< Configured key size in bytes.
            int         salt_size_; ///< Configured salt size in bytes.
            int         num_hash_rounds_; ///< Configured number of hash rounds.
            std::string algorithm_; ///< Configured cipher algorithm name.

    }; // class buffer_crypt

}; // namespace irods

#endif // __IRODS_BUFFER_ENCRYPTION_HPP__


