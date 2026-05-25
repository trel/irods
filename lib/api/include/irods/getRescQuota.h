#ifndef GET_RESC_QUOTA_H__
#define GET_RESC_QUOTA_H__

/// \file
/// \brief Declares resource quota query types and functions.

#include "irods/rcConnect.h"
#include "irods/objInfo.h"
#include "irods/rodsType.h"

/// Resource quota policy has not been initialized.
#define RESC_QUOTA_UNINIT       -1      // RescQuotaPolicy not initialized
/// Resource quota policy is disabled.
#define RESC_QUOTA_OFF          0       // RescQuotaPolicy disabled
/// Resource quota policy is enabled.
#define RESC_QUOTA_ON           1       // RescQuotaPolicy enabled

/// Selects the global quota when updating or querying overrun state.
#define GLB_QUOTA       0x1     // update the global quota
/// Selects the resource-specific quota when updating or querying overrun state.
#define RESC_QUOTA      0x2     // update the resource quota
/// Selects both global and resource-specific quotas.
#define ALL_QUOTA       GLB_QUOTA|RESC_QUOTA

/// Input for retrieving quota information for a user and resource.
typedef struct getRescQuotaInp {
    char rescName[NAME_LEN];            ///< Resource name.
    char userName[NAME_LEN];            ///< User name, optionally qualified by zone.
    char zoneHint[MAX_NAME_LEN];        ///< Zone hint used during lookup.
    int flags;                          ///< Reserved flags field.
    int dummy;                          ///< Padding for packing alignment.
    keyValPair_t condInput;             ///< Additional keyword options.
} getRescQuotaInp_t;
/// Packing instruction for `getRescQuotaInp_t`.
#define getRescQuotaInp_PI "str rescName[NAME_LEN]; str userName[NAME_LEN]; str zoneHint[MAX_NAME_LEN]; int flags; int dummy; struct KeyValPair_PI;"

/// Linked-list node describing quota usage for a resource.
typedef struct rescQuota {
    char rescName[NAME_LEN];            ///< Resource name.
    char userId[NAME_LEN];              ///< User identifier associated with the quota.
    int flags;                          ///< Quota flags.
    int dummy;                          ///< Padding for packing alignment.
    rodsLong_t quotaLimit;              ///< Configured quota limit.
    rodsLong_t quotaOverrun;            ///< Amount by which usage exceeds the limit.
    struct rescQuota *next;             ///< Next quota entry in the result list.
} rescQuota_t;
/// Packing instruction for `rescQuota_t`.
#define rescQuota_PI "str rescName[NAME_LEN]; str userId[NAME_LEN]; int flags; int dummy; double quotaLimit; double quotaOverrun; struct *rescQuota_PI;"

/// Indicates that a quota entry represents a global quota.
#define GLOBAL_QUOTA    0x1     // the quota is global

/// Retrieves resource quota information for the supplied input.
int rcGetRescQuota( rcComm_t *conn, getRescQuotaInp_t *getRescQuotaInp, rescQuota_t **rescQuota );
/// Frees a linked list of `rescQuota_t` entries.
int freeAllRescQuota( rescQuota_t *rescQuotaHead );

#endif  // GET_RESC_QUOTA_H__
