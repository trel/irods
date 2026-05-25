#ifndef IRODS_GET_MISC_SVR_INFO_H
#define IRODS_GET_MISC_SVR_INFO_H

#include "irods/rcConnect.h"

// there is no input struct. Therefore, the inPackInstruct is NULL
// miscSvrInfo_t is the output struct

// definition for server type
#define RCAT_NOT_ENABLED        0
#define RCAT_ENABLED            1

/**
 * \brief Miscellaneous server information returned by `rcGetMiscSvrInfo`.
 */
typedef struct MiscSvrInfo {
    int serverType; ///< RCAT_ENABLED or RCAT_NOT_ENABLED.
    uint serverBootTime; ///< Server boot timestamp.
    char relVersion[NAME_LEN]; ///< Release version number.
    char apiVersion[NAME_LEN]; ///< API version number.
    char rodsZone[NAME_LEN]; ///< Zone served by this server.
    bytesBuf_t certinfo; ///< Certificate information returned by the server.
} miscSvrInfo_t;

#define MiscSvrInfo_PI                                                                                               \
  "int serverType; int serverBootTime; str relVersion[NAME_LEN]; str apiVersion[NAME_LEN]; str rodsZone[NAME_LEN]; " \
  "struct BinBytesBuf_PI;"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Query miscellaneous server information.
 *
 * @param[in] conn Client connection handle.
 * @param[out] outSvrInfo Output structure describing the target server.
 *
 * @return Operation status.
 */
int rcGetMiscSvrInfo(rcComm_t* conn, miscSvrInfo_t** outSvrInfo);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // IRODS_GET_MISC_SVR_INFO_H
