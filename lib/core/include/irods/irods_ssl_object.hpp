#ifndef __IRODS_SSL_OBJECT_HPP__
#define __IRODS_SSL_OBJECT_HPP__

/// \file

// =-=-=-=-=-=-=-
#include "irods/irods_network_object.hpp"
#include "irods/irods_buffer_encryption.hpp"

// =-=-=-=-=-=-=-
// ssl includes
#include <openssl/ssl.h>

namespace irods {

// =-=-=-=-=-=-=-
// constant key for tcp network object
    /// Name of the SSL network plugin.
    const std::string SSL_NETWORK_PLUGIN( "ssl" );

// =-=-=-=-=-=-=-
// SSL Network Object
    /// Represents a network object backed by an OpenSSL connection.
    class ssl_object : public network_object {
        public:
            // =-=-=-=-=-=-=-
            // Constructors
            /// Constructs an empty SSL object.
            ssl_object();
            /// Constructs an SSL object from a client connection.
            ssl_object( const rcComm_t& );
            /// Constructs an SSL object from a server connection.
            ssl_object( const rsComm_t& );
            /// Copy-constructs an SSL object.
            ssl_object( const ssl_object& );

            // =-=-=-=-=-=-=-
            // Destructors
            /// Destroys the SSL object.
            virtual ~ssl_object();

            // =-=-=-=-=-=-=-
            // Operators
            /// Copies another SSL object into this one.
            /// \return Reference to this object.
            virtual ssl_object& operator=( const ssl_object& );

            /// @brief Comparison operator
            /// @param[in] _rhs SSL object to compare against.
            /// @return True if the SSL objects compare equal; otherwise false.
            virtual bool operator==( const ssl_object& _rhs ) const;

            // =-=-=-=-=-=-=-
            // plugin resolution operation
            /// Resolves a plugin implementing the requested interface.
            /// \return An error describing the result of plugin resolution.
            virtual error resolve(
                const std::string&, // plugin interface name
                plugin_ptr& );      // resolved plugin

            // =-=-=-=-=-=-=-
            // conversion to client comm ptr
            /// Copies this object's state into a client communication structure.
            /// \return An error describing the conversion result.
            virtual error to_client( rcComm_t* );

            // =-=-=-=-=-=-=-
            // conversion to client comm ptr
            /// Copies this object's state into a server communication structure.
            /// \return An error describing the conversion result.
            virtual error to_server( rsComm_t* );

            // =-=-=-=-=-=-=-
            // accessor for rule engine variables
            /// Populates the rule-engine variable set for this object.
            /// \return An error describing the result of the population operation.
            virtual error get_re_vars( rule_engine_vars_t& );

            // =-=-=-=-=-=-=-
            // Accessors
            /// Returns the OpenSSL context.
            virtual SSL_CTX*              ssl_ctx()       const {
                return ssl_ctx_;
            }
            /// Returns the OpenSSL session.
            virtual SSL*                  ssl()           const {
                return ssl_;
            }
            /// Returns the remote host name.
            virtual std::string           host()          const {
                return host_;
            }
            /// Returns the negotiated shared secret.
            virtual buffer_crypt::array_t shared_secret() const {
                return shared_secret_;
            }

            /// Returns the configured encryption key size.
            virtual int         key_size()             const {
                return key_size_;
            }
            /// Returns the configured salt size.
            virtual int         salt_size()            const {
                return salt_size_;
            }
            /// Returns the configured number of hash rounds.
            virtual int         num_hash_rounds()      const {
                return num_hash_rounds_;
            }
            /// Returns the configured encryption algorithm name.
            virtual std::string encryption_algorithm() const {
                return encryption_algorithm_;
            }

            // =-=-=-=-=-=-=-
            // mutators
            /// Sets the OpenSSL context.
            virtual void ssl_ctx( SSL_CTX* _c )                 {
                ssl_ctx_       = _c;
            }
            /// Sets the OpenSSL session.
            virtual void ssl( SSL* _s )                         {
                ssl_           = _s;
            }
            /// Sets the remote host name.
            virtual void host( const std::string& _h )          {
                host_          = _h;
            }
            /// Sets the negotiated shared secret.
            virtual void shared_secret( const buffer_crypt::array_t& _s ) {
                shared_secret_ = _s;
            }

            /// Sets the encryption key size.
            virtual void key_size( int _s )                            {
                key_size_             = _s;
            }
            /// Sets the salt size.
            virtual void salt_size( int _s )                           {
                salt_size_            = _s;
            }
            /// Sets the number of hash rounds.
            virtual void num_hash_rounds( int _h )                     {
                num_hash_rounds_      = _h;
            }
            /// Sets the encryption algorithm name.
            virtual void encryption_algorithm( const std::string& _a ) {
                encryption_algorithm_ = _a;
            }

        private:
            /// OpenSSL context.
            SSL_CTX*              ssl_ctx_;
            /// OpenSSL session.
            SSL*                  ssl_;
            /// Remote host name.
            std::string           host_;
            /// Shared secret used for buffer encryption.
            buffer_crypt::array_t shared_secret_;

            /// Encryption key size.
            int         key_size_;
            /// Salt size.
            int         salt_size_;
            /// Number of hash rounds.
            int         num_hash_rounds_;
            /// Encryption algorithm name.
            std::string encryption_algorithm_;

    }; // class ssl_object

/// =-=-=-=-=-=-=-
/// @brief typedef for shared tcp object ptr
    /// Shared pointer to an SSL network object.
    typedef boost::shared_ptr< ssl_object > ssl_object_ptr;

}; // namespace irods

#endif // __IRODS_SSL_OBJECT_HPP__
