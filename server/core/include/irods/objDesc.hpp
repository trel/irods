#ifndef IRODS_OBJ_DESC_HPP
#define IRODS_OBJ_DESC_HPP

/// \file

#include "irods/rods.h"
#include "irods/objInfo.h"
#include "irods/dataObjInpOut.h"
#include "irods/fileRename.h"
#include "irods/miscUtil.h"
#include "irods/structFileSync.h"
#include "irods/structFileExtAndReg.h"
#include "irods/dataObjOpenAndStat.h"
#include "irods/rodsConnect.h"

#include <boost/any.hpp>

#include <string>

/// \brief Number of available L1 descriptor slots.
#define NUM_L1_DESC     1026    /* number of L1Desc */

/// \brief Maximum number of failed orphan checks before stopping.
#define CHK_ORPHAN_CNT_LIMIT  20  /* number of failed check before stopping */
/* definition for getNumThreads */

/// \brief Register a checksum for the replica.
#define REG_CHKSUM      1
/// \brief Verify the checksum for the replica.
#define VERIFY_CHKSUM   2

// Values in l1desc are the desired values.
// Values in dataObjInfo are the values in the catalog.
//
// If you modify this data type, you must also update init_l1desc().
//
// DO NOT call std::memset() on this data type. Use init_l1desc().
// DO NOT call std::memcpy() on this data type. Use the assignment operator.
/// \brief Describes an open data object replica and its operation state.
struct l1desc
{
    /// \brief Constructs an empty L1 descriptor.
    l1desc() = default;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
    /// \brief Copies an L1 descriptor.
    l1desc(const l1desc&) = default;
    /// \brief Assigns one L1 descriptor to another.
    auto operator=(const l1desc&) -> l1desc& = default;

    /// \brief Moves an L1 descriptor.
    l1desc(l1desc&&) = default;
    /// \brief Move-assigns an L1 descriptor.
    auto operator=(l1desc&&) -> l1desc& = default;
#pragma GCC diagnostic pop

    /// \brief Destroys the L1 descriptor.
    ~l1desc() = default;

    int l3descInx; ///< Index of the associated low-level file descriptor.
    int inuseFlag; ///< Indicates whether this descriptor slot is in use.
    int oprType; ///< Operation type associated with the descriptor.
    int openType; ///< Open mode for the data object.
    int oprStatus; ///< Status of the current operation.
    int dataObjInpReplFlag; ///< Replica-related flag copied from the input.
    dataObjInp_t *dataObjInp; ///< Pointer to the request input structure.
    dataObjInfo_t *dataObjInfo; ///< Pointer to the target replica information.
    // NOLINTNEXTLINE(misc-non-private-member-variables-in-classes)
    [[deprecated]] dataObjInfo_t* otherDataObjInfo; ///< Deprecated pointer to secondary replica information.
    int copiesNeeded; ///< Number of additional copies required.
    rodsLong_t bytesWritten; ///< Number of bytes written through this descriptor.
    rodsLong_t dataSize; ///< Target size expected at close.
    int replStatus; ///< Replica status to apply on close.
    int chksumFlag; /* parsed from condition */ ///< Checksum operation requested for the replica.
    int srcL1descInx; ///< Source L1 descriptor index for related operations.
    char chksum[CHKSUM_LEN]; ///< Input checksum value.
    int remoteL1descInx; ///< Remote L1 descriptor index for redirected operations.
    int stageFlag; ///< Indicates whether staging is required.
    int purgeCacheFlag; ///< Indicates whether the cache copy should be purged.
    int lockFd; ///< File descriptor for the associated lock.
    // NOLINTBEGIN(misc-non-private-member-variables-in-classes)
    [[deprecated]] boost::any pluginData; ///< Deprecated plugin-specific state.
    [[deprecated]] dataObjInfo_t* replDataObjInfo; ///< Deprecated destination replica information used on close.
    // NOLINTEND(misc-non-private-member-variables-in-classes)
    rodsServerHost_t *remoteZoneHost; ///< Remote zone host handling the operation.
    char in_pdmo[MAX_NAME_LEN]; ///< Requested physical data movement operation string.

    std::string replica_token; ///< Replica access token used for coordinated updates.
};

/// \brief Alias for the L1 descriptor type.
using l1desc_t = l1desc;

/// \brief Initializes the global L1 descriptor table.
int initL1Desc();

/// \brief Resets an L1 descriptor to its default state.
auto init_l1desc(l1desc& _l1d) -> void;

/// \brief Copies the contents of one L1 descriptor into another.
auto copy_l1desc(l1desc& _dst, const l1desc& _src) -> void;

/// \brief Allocates an available L1 descriptor slot.
int allocL1Desc();

/// \brief Releases resources held by an L1 descriptor structure.
int freeL1desc_struct(l1desc& _l1desc);

/// \brief Frees the L1 descriptor at the given index.
int freeL1desc(const int l1descInx);

/// \brief Populates an L1 descriptor using replica operation inputs.
int fillL1desc(
    int l1descInx,
    dataObjInp_t* dataObjInp,
    dataObjInfo_t* dataObjInfo,
    int replStatus,
    rodsLong_t dataSize);

/// \brief Returns the L1 descriptor index matching the given replica info.
int getL1descIndexByDataObjInfo(const dataObjInfo_t* dataObjInfo);

/// \brief Calculates the number of transfer threads to use for an operation.
int getNumThreads(
    rsComm_t* rsComm,
    rodsLong_t dataSize,
    int inpNumThr,
    keyValPair_t* condInput,
    char* destRescName,
    char* srcRescName,
    int oprType);

/// \brief Initializes a data operation input structure from an L1 descriptor.
int initDataOprInp(dataOprInp_t* dataOprInp, int l1descInx, int oprType);

/// \brief Converts a low-level descriptor index into an external descriptor index.
int convL3descInx(int l3descInx);

/// \brief Allocates an available L1 descriptor slot.
int allocL1desc();

/// \brief Closes all active L1 descriptors for the server connection.
int closeAllL1desc(rsComm_t* rsComm);

/// \brief Initializes the special collection descriptor table.
int initSpecCollDesc();

/// \brief Allocates an available special collection descriptor slot.
int allocSpecCollDesc();

/// \brief Frees the special collection descriptor at the given index.
int freeSpecCollDesc(int specCollInx);

/// \brief Initializes the global L1 descriptor table.
int initL1desc();

/// \brief Allocates an available collection handle slot.
int allocCollHandle();

/// \brief Frees the collection handle at the given index.
int freeCollHandle(int handleInx);

/// \brief Initializes a server-side query handle.
int rsInitQueryHandle(queryHandle_t* queryHandle, rsComm_t* rsComm);

/// \brief Allocates and fills an L1 descriptor for a remote zone operation.
int allocAndSetL1descForZoneOpr(
    int l3descInx,
    dataObjInp_t* dataObjInp,
    rodsServerHost_t* remoteZoneHost,
    openStat_t* openStat);

/// \brief Returns whether any L1 descriptor is currently in use.
int isL1descInuse();

namespace irods
{
    /// \brief Allocates and populates an L1 descriptor based on the provided inputs.
    ///
    /// \param[in] _inp The data object input used to seed the descriptor.
    /// \param[in] _info The replica information describing the opened replica.
    /// \param[in] _data_size The expected size at close of the replica which is being opened.
    ///
    /// \returns Generated L1 descriptor index
    ///
    /// \since 4.2.9
    auto populate_L1desc_with_inp(
        DataObjInp& _inp,
        DataObjInfo& _info,
        const rodsLong_t _data_size) -> int;
} // namespace irods

#endif // IRODS_OBJ_DESC_HPP
