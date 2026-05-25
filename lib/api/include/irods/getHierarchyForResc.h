#ifndef GET_HIERARCHY_FOR_RESC_H__
#define GET_HIERARCHY_FOR_RESC_H__


#include "irods/rcConnect.h"

/** Input for resolving a full hierarchy from a resource name. */
typedef struct GetHierarchyForRescInp {
    char resc_name_[ MAX_NAME_LEN ]; ///< Resource name whose hierarchy is requested.
} getHierarchyForRescInp_t;
#define getHierarchyForRescInp_PI "str resc_name_[MAX_NAME_LEN];"

/** Output containing the resolved resource hierarchy. */
typedef struct GetHierarchyForRescOut {
    char resc_hier_[ MAX_NAME_LEN ]; ///< Resolved hierarchy string for the requested resource.
} getHierarchyForRescOut_t;
#define getHierarchyForRescOut_PI "str resc_hier_[MAX_NAME_LEN];"

#ifdef __cplusplus
extern "C"
#endif
/**
 * Resolve the full resource hierarchy for a named resource.
 *
 * @param[in] server_comm_ptr Client connection handle.
 * @param[in] incoming_resc_name Input naming the target resource.
 * @param[out] full_hier_to_resc Output containing the resolved hierarchy string.
 *
 * @return Operation status.
 */
int rcGetHierarchyForResc(rcComm_t* server_comm_ptr, getHierarchyForRescInp_t* incoming_resc_name, getHierarchyForRescOut_t** full_hier_to_resc);

#endif
