#ifndef GET_HIER_FROM_LEAF_ID_HPP
#define GET_HIER_FROM_LEAF_ID_HPP

/// \file

#include "irods/rodsDef.h"
#include "irods/rodsType.h"
#include "irods/rcConnect.h"

/// \brief Input for requesting a hierarchy from a leaf resource ID.
typedef struct {
    rodsLong_t  resc_id_; ///< Leaf resource identifier.
} get_hier_inp_t;

/// \brief Output containing the hierarchy for a leaf resource ID.
typedef struct {
    char hier_[MAX_NAME_LEN]; ///< Hierarchy string associated with the leaf resource.
} get_hier_out_t;

#define GetHierInp_PI "double resc_id_;"
#define GetHierOut_PI "str hier_[MAX_NAME_LEN];"

/**
 * Resolve a resource hierarchy from a leaf resource ID.
 *
 * @param[in] Client connection handle.
 * @param[in] get_hier_inp Hierarchy lookup request.
 * @param[out] get_hier_out Output containing the resolved hierarchy.
 *
 * @return Operation status.
 */
int rcGetHierFromLeafId(rcComm_t*,get_hier_inp_t*,get_hier_out_t**);

#endif
