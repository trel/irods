#ifndef STRUCT_FILE_EXT_AND_REG_H__
#define STRUCT_FILE_EXT_AND_REG_H__

#include "irods/rcConnect.h"
#include "irods/objInfo.h"

/// \brief Input for extracting a structured file and registering its contents.
typedef struct StructFileExtAndRegInp {
    char objPath[MAX_NAME_LEN];    ///< Logical path to the structured file data object.
    char collection[MAX_NAME_LEN]; ///< Collection under which extracted entries are registered.
    int oprType;                   ///< Operation type defined by syncMountedColl.h.
    int flags;                     ///< Reserved for future use.
    keyValPair_t condInput;        ///< Optional keyword-value options such as checksum control.
} structFileExtAndRegInp_t;
/// \brief Packing instruction for \ref structFileExtAndRegInp_t.
#define StructFileExtAndRegInp_PI "str objPath[MAX_NAME_LEN]; str collection[MAX_NAME_LEN]; int oprType; int flags; struct KeyValPair_PI;"


#ifdef __cplusplus
extern "C"
#endif
/// \brief Extracts a structured file and registers the extracted entries.
/// \param[in] conn Client connection handle.
/// \param[in] structFileExtAndRegInp Operation input.
/// \return Integer error code.
int rcStructFileExtAndReg( rcComm_t *conn, structFileExtAndRegInp_t *structFileExtAndRegInp );

#endif
