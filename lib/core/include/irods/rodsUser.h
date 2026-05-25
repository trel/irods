#ifndef RODS_USER_H__
#define RODS_USER_H__

#include "irods/rodsDef.h"

/// Authentication scheme value for password-based authentication.
#define PASSWORD        0

/// Keyword naming the password authentication scheme.
#define PASSWORD_AUTH_KEYWD     "PASSWORD"

/// Keyword used to override the client user name.
#define CLIENT_USER_NAME_KEYWD  "clientUserName"
/// Keyword used to override the client zone name.
#define CLIENT_RODS_ZONE_KEYWD  "clientRodsZone"

/// Authentication has not completed.
#define NO_USER_AUTH            0
/// Deprecated public authentication state.
#define PUBLIC_USER_AUTH        1
/// Authenticated as a remote-zone user.
#define REMOTE_USER_AUTH        2
/// Authenticated as a local-zone user.
#define LOCAL_USER_AUTH         3
/// Authenticated as a privileged remote-zone user.
#define REMOTE_PRIV_USER_AUTH   4
/// Authenticated as a privileged local-zone user.
#define LOCAL_PRIV_USER_AUTH    5

/// Authentication details associated with a user.
typedef struct AuthInfo {
    char authScheme[NAME_LEN];     /**< Authentication scheme name. */
    int authFlag;                  /**< Authentication status and privilege level. */
    int flag;                      /**< Additional authentication flags. */
    int ppid;                      /**< Session parent process identifier. */
    char host[NAME_LEN];           /**< Session host name. */
    char authStr[NAME_LEN];        /**< Authentication string, such as a GSI DN. */
} authInfo_t;

/// Auxiliary metadata recorded for a user.
typedef struct UserOtherInfo {
    char userInfo[NAME_LEN];       /**< User information string. */
    char userComments[NAME_LEN];   /**< User comment string. */
    char userCreate[TIME_LEN];     /**< User creation timestamp. */
    char userModify[TIME_LEN];     /**< User modification timestamp. */
} userOtherInfo_t;

/// Indicates that `authStr` refers to data stored in a file.
#define AUTH_IN_FILE    0x1

/// Regular user privilege level.
#define REG_USER        0
/// Privileged user in the local zone.
#define LOC_PRIV_USER   1
/// Privileged user from a remote zone.
#define REM_PRIV_USER   2

/// Core user account information.
typedef struct UserInfo {
    char userName[NAME_LEN];       /**< User name. */
    char rodsZone[NAME_LEN];       /**< User zone name. */
    char userType[NAME_LEN];       /**< User type string. */
    int sysUid;                    /**< Local system user identifier. */
    authInfo_t authInfo;           /**< Authentication state for the user. */
    userOtherInfo_t userOtherInfo; /**< Additional user metadata. */
} userInfo_t;

#endif /* RODS_USER_H__ */
