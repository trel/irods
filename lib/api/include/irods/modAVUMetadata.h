#ifndef IRODS_MOD_AVU_METADATA_H
#define IRODS_MOD_AVU_METADATA_H

/// \file
/// \brief Declares the AVU metadata modification API.

#include "irods/objInfo.h"

struct RcComm;

/// Input describing an AVU metadata operation.
typedef struct ModifyAVUMetadataInput {
    char *arg0;                 ///< Operation keyword such as add, rm, mod, or set.
    char *arg1;                 ///< Target item type selector.
    char *arg2;                 ///< Target item name.
    char *arg3;                 ///< Attribute name.
    char *arg4;                 ///< Attribute value.
    char *arg5;                 ///< Attribute unit.
    char *arg6;                 ///< Replacement attribute name for modify and set operations.
    char *arg7;                 ///< Replacement attribute value for modify and set operations.
    char *arg8;                 ///< Replacement attribute unit for modify and set operations.
    char *arg9;                 ///< Reserved argument slot.
    struct KeyValPair condInput; ///< Additional keyword options.
} modAVUMetadataInp_t;

/// Packing instruction for `modAVUMetadataInp_t`.
#define ModAVUMetadataInp_PI "str *arg0; str *arg1; str *arg2; str *arg3; str *arg4; str *arg5; str *arg6; str *arg7;  str *arg8;  str *arg9; struct KeyValPair_PI;"

#ifdef __cplusplus
extern "C" {
#endif

/// Applies an AVU metadata operation described by the input structure.
int rcModAVUMetadata(struct RcComm* comm, struct ModifyAVUMetadataInput* modAVUMetadataInp);

/// Releases dynamic memory owned by a `modAVUMetadataInp_t` value.
void clearModAVUMetadataInp(void* voidInp);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // IRODS_MOD_AVU_METADATA_H
