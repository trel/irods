#ifndef IRODS_GET_RODS_ENV_H
#define IRODS_GET_RODS_ENV_H

#include "irods/rodsDef.h"

// NOLINTNEXTLINE(modernize-use-using)
/** Client environment settings loaded from iRODS configuration files. */
typedef struct RodsEnvironment {
    char rodsUserName[NAME_LEN];                    ///< User name for the session.
    char rodsHost[LONG_NAME_LEN];                   ///< Host name of the iRODS server.
    int  rodsPort;                                  ///< Server port number.
    char rodsHome[MAX_NAME_LEN];                    ///< User home collection.
    char rodsCwd[MAX_NAME_LEN];                     ///< Current working collection.
    char rodsAuthScheme[NAME_LEN];                  ///< Authentication scheme name.
    char rodsDefResource[NAME_LEN];                 ///< Default resource name.
    char rodsZone[NAME_LEN];                        ///< Default zone name.
    int rodsLogLevel;                               ///< Client log verbosity.
    char rodsAuthFile[LONG_NAME_LEN];               ///< Authentication file path.
    char rodsClientServerPolicy[ LONG_NAME_LEN ];   ///< Negotiation policy.
    char rodsClientServerNegotiation[ LONG_NAME_LEN ]; ///< Negotiation mode.

    // client side options for encryption
    int  rodsEncryptionKeySize;                     ///< Encryption key size in bytes.
    int  rodsEncryptionSaltSize;                    ///< Encryption salt size in bytes.
    int  rodsEncryptionNumHashRounds;               ///< Number of hashing rounds.
    char rodsEncryptionAlgorithm[ HEADER_TYPE_LEN ];///< Encryption algorithm name.

    // client side options for hashing
    char rodsDefaultHashScheme[ NAME_LEN ];         ///< Default hash scheme name.
    char rodsMatchHashPolicy[ NAME_LEN ];           ///< Hash match policy name.

    // legacy ssl environment variables
    char irodsSSLCACertificatePath[MAX_NAME_LEN];  ///< Path to CA certificates.
    char irodsSSLCACertificateFile[MAX_NAME_LEN];  ///< File containing CA certificates.
    char irodsSSLVerifyServer[MAX_NAME_LEN];       ///< Whether to verify the server certificate.

    // advanced settings
    int irodsMaxSizeForSingleBuffer;               ///< Maximum single-buffer transfer size.
    int irodsDefaultNumberTransferThreads;         ///< Default transfer thread count.
    int irodsTransBufferSizeForParaTrans;          ///< Parallel transfer buffer size.
    int irodsConnectionPoolRefreshTime;            ///< Connection pool refresh interval.

    // override of plugin installation directory
    char irodsPluginDirectory[MAX_NAME_LEN];       ///< Plugin installation directory override.

    // TCP keepalive configurations
    int tcp_keepalive_intvl;                       ///< TCP keepalive probe interval.
    int tcp_keepalive_probes;                      ///< TCP keepalive probe count.
    int tcp_keepalive_time;                        ///< TCP keepalive idle time.
} rodsEnv;

#ifdef __cplusplus
extern "C" {
#endif

/** Loads the current process environment into `myRodsEnv`. */
int getRodsEnv( rodsEnv *myRodsEnv );

/** Returns the path to the active `irods_environment.json` file. */
char *getRodsEnvFileName();
/** Returns the path to the active authentication file. */
char *getRodsEnvAuthFileName();

/** Prints the loaded environment to the provided stream. */
int printRodsEnv( FILE* );

/** Captures a string configuration property into the provided buffer. */
int capture_string_property(const char* _key, char* _val, size_t _val_size);

#ifdef __cplusplus

/** C++ helper that loads the current process environment into `myRodsEnv`. */
void _getRodsEnv( rodsEnv &myRodsEnv );
/** C++ helper that reloads the environment into `myRodsEnv`. */
void _reloadRodsEnv( rodsEnv &myRodsEnv );

}
#endif

#endif // IRODS_GET_RODS_ENV_H
