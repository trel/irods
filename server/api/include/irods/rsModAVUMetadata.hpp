#ifndef IRODS_RS_MOD_A_V_U_METADATA_HPP
#define IRODS_RS_MOD_A_V_U_METADATA_HPP

/// \file

#include "irods/modAVUMetadata.h"

struct RsComm;
struct ModifyAVUMetadataInput;

/// Modifies AVU metadata on behalf of a server API call.
int rsModAVUMetadata(RsComm* rsComm, ModifyAVUMetadataInput* modAVUMetadataInp);
/// Internal implementation of `rsModAVUMetadata`.
int _rsModAVUMetadata(RsComm* rsComm, ModifyAVUMetadataInput* modAVUMetadataInp);

#endif // IRODS_RS_MOD_A_V_U_METADATA_HPP
