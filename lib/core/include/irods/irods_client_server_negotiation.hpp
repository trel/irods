#ifndef __IRODS_CLIENT_SERVER_NEGOTIATION_HPP__
#define __IRODS_CLIENT_SERVER_NEGOTIATION_HPP__

// =-=-=-=-=-=-=-
#include "irods/irods_log.hpp"
#include "irods/irods_network_object.hpp"
#include "irods/irods_server_properties.hpp"

// =-=-=-=-=-=-=-
// boost includes
#include <boost/shared_ptr.hpp>

// =-=-=-=-=-=-=-
// irods includes
#include "irods/rcConnect.h"

// =-=-=-=-=-=-=-
// stl includes
#include <string>
#include <string_view>

namespace irods
{
    /// Environment variable controlling client-server negotiation behavior.
    inline const char RODS_CS_NEG[]       = {"RODS_CS_NEG"};
    /// Keyword indicating that SSL was selected during negotiation.
    inline const char CS_NEG_USE_SSL_KW[] = {"cs_neg_ssl_kw"};

    /// Status code indicating successful negotiation.
    inline const int CS_NEG_STATUS_SUCCESS = 1;
    /// Status code indicating failed negotiation.
    inline const int CS_NEG_STATUS_FAILURE = 0;

    /// \brief Constant for the length of the negotiation_key in bytes
    inline const std::size_t negotiation_key_length_in_bytes = 32;

    /// Holds a serialized client-server negotiation message.
    struct cs_neg_t {
        int  status_; ///< Negotiation status code.
        char result_[MAX_NAME_LEN]; ///< Negotiation result string.
    };

    /// Negotiation result indicating failure.
    inline const std::string CS_NEG_FAILURE( "CS_NEG_FAILURE" );
    /// Negotiation result requesting SSL.
    inline const std::string CS_NEG_USE_SSL( "CS_NEG_USE_SSL" );
    /// Negotiation result requesting TCP.
    inline const std::string CS_NEG_USE_TCP( "CS_NEG_USE_TCP" );

    /// Negotiation policy requiring protection.
    inline const std::string CS_NEG_REQUIRE( "CS_NEG_REQUIRE" );     // index 0
    /// Negotiation policy refusing protection.
    inline const std::string CS_NEG_REFUSE( "CS_NEG_REFUSE" );       // index 1
    /// Negotiation policy expressing no preference.
    inline const std::string CS_NEG_DONT_CARE( "CS_NEG_DONT_CARE" ); // index 2

    /// Keyword storing the signed zone key.
    inline const std::string CS_NEG_SID_KW( "cs_neg_sid_kw" );
    /// Keyword storing the negotiation result.
    inline const std::string CS_NEG_RESULT_KW( "cs_neg_result_kw" );

    /// Returns whether client-server negotiation is required on the server side.
    bool do_client_server_negotiation_for_server(const char* _neg);

    /// Returns whether client-server negotiation is required on the client side.
    bool do_client_server_negotiation_for_client();

    /// Performs TLS and authentication negotiation for a server-side connection.
    error client_server_negotiation_for_server(irods::network_object_ptr, // server connection handle
                                               std::string&, // results of negotiation
                                               bool, // boolean indicating whether client-server negotiation is needed
                                               RsComm&); // RsComm initialized during agent startup

    /// Performs TLS and authentication negotiation for a client-side connection.
    error client_server_negotiation_for_client(
        irods::network_object_ptr, // client connection handle
        const std::string&,        // host name
        std::string& );            // results of the negotiation

    /// Sends a client-server negotiation message over the provided socket.
    error send_client_server_negotiation_message(
        irods::network_object_ptr,  // socket
        cs_neg_t& );                // message payload

    /// Reads a client-server negotiation message from the provided socket.
    error read_client_server_negotiation_message(
        irods::network_object_ptr,        // socket
        boost::shared_ptr< cs_neg_t >& ); // message payload

    /// \brief Given a buffer, encrypt and hash it for negotiation.
    ///
    /// \param[in] _zone_key The zone key to sign.
    /// \param[in] _encryption_key The encryption key to use for signing the zone key.
    /// \param[out] _signed_zone_key The encrypted and hashed zone key.
    ///
    /// \deprecated Deprecated in 5.1.0. Use \p sign_zone_key instead.
    [[deprecated("Use sign_zone_key instead.")]] auto sign_server_sid(const std::string& _zone_key,
                                                                      const std::string& _encryption_key,
                                                                      std::string& _signed_zone_key) -> irods::error;

    /// \brief Sign the zone key using the provided encryption key and hash scheme.
    ///
    /// \param[in] _zone_key The zone key to sign.
    /// \param[in] _encryption_key The encryption key to use for signing the zone key.
    /// \param[in] _zone_key_signing_hash_scheme The \p irods::Hasher scheme name to use to hash the encrypted buffer.
    /// \param[out] _signed_zone_key The encrypted and hashed zone key.
    ///
    /// \return irods::error Describing any errors which occurred while signing the zone key.
    ///
    /// \since 5.1.0
    auto sign_zone_key(const std::string& _zone_key,
                       const std::string& _encryption_key,
                       const std::string& _zone_key_signing_hash_scheme,
                       std::string& _signed_zone_key) -> irods::error;

    /// \brief check the incoming signed zone_key against local and remote zone_keys
    ///
    /// \param[in] _zone_key The signed zone key to validate.
    /// \return An error object describing the validation result.
    ///
    /// \deprecated Deprecated in 5.1.0.
    [[deprecated]] auto check_sent_sid(const std::string& _zone_key) -> irods::error;

    /// \brief Return whether the configured negotiation_key meets a set of requirements.
    ///
    /// \param[in] _key negotiation_key to check.
    ///
    /// \retval true If the negotiation_key has exactly 32 valid characters.
    /// \retval false If the negotiation_key does not have exactly 32 valid characters.
    ///
    /// \since 4.2.11
    auto negotiation_key_is_valid(const std::string_view _key) -> bool;
} // namespace irods

#endif // __IRODS_CLIENT_SERVER_NEGOTIATION_HPP__
