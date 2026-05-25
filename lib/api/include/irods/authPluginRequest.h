#ifndef AUTH_PLUGIN_REQUEST_H__
#define AUTH_PLUGIN_REQUEST_H__

/// \file

#include "irods/rcConnect.h"

/// \brief Input for submitting a request to an authentication plugin.
typedef struct AuthPluginReqInp {
    char auth_scheme_[ MAX_NAME_LEN ]; ///< Authentication scheme name.
    char context_    [ MAX_NAME_LEN ]; ///< Context string passed to the plugin.
} authPluginReqInp_t;
#define authPlugReqInp_PI "str auth_scheme_[MAX_NAME_LEN]; str context_[MAX_NAME_LEN];"

/// \brief Output returned by an authentication plugin request.
typedef struct AuthPluginReqOut {
    char result_[ MAX_NAME_LEN ]; ///< Result string produced by the plugin.
} authPluginReqOut_t;
#define authPlugReqOut_PI "str result_[MAX_NAME_LEN];"


#ifdef __cplusplus
extern "C"
#endif
/**
 * Submit a request to an authentication plugin.
 *
 * @param[in] server_comm_ptr Client connection handle.
 * @param[in] incoming_struct_with_scheme Input describing the scheme and context.
 * @param[out] response_from_agent Output returned by the agent.
 *
 * @return Operation status.
 */
int rcAuthPluginRequest(rcComm_t* server_comm_ptr, authPluginReqInp_t* incoming_struct_with_scheme, authPluginReqOut_t** response_from_agent);

#endif
