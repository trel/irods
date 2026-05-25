#ifndef DATA_OBJ_INP_OUT_H__
#define DATA_OBJ_INP_OUT_H__

#include "irods/rodsDef.h"
#include "irods/rodsType.h"
#include "irods/objInfo.h"

#if defined(aix_platform)
#ifndef _AIX_PTHREADS_D7
#define pthread_mutexattr_default NULL
#define pthread_condattr_default NULL
#define pthread_attr_default NULL
#endif  /* _AIX_PTHREADS_D7 */
#else   /* aix_platform */
#define pthread_mutexattr_default NULL
#define pthread_condattr_default NULL
#define pthread_attr_default NULL
#endif  /* aix_platform */

/** Network connection details for portal-based transfers. */
typedef struct PortList {
    int portNum;                  ///< TCP port number.
    int cookie;                   ///< Transfer cookie.
    int sock;                     ///< Server socket descriptor when applicable.
    int windowSize;               ///< Requested TCP window size.
    char hostAddr[LONG_NAME_LEN]; ///< Host address for the transfer portal.
} portList_t;

/**
 * \var dataObjInp_t
 * \brief Input struct for Data object operation
 * \since 1.0
 *
 * \ingroup capi_input_data_structures
 *
 * \remark none
 *
 * \note
 * Elements of dataObjInp_t:
 * \li char objPath[MAX_NAME_LEN] - full path of the data object.
 * \li int createMode - the file mode of the data object.
 * \li int openFlags - the flags for the I/O operation,
 *      Valid flags are O_RDONLY, O_WRONLY, O_RDWR and O_TRUNC.
 *      Also used for specCollInx in rcQuerySpecColl.
 * \li rodsLong_t offset - the offset.
 * \li rodsLong_t dataSize - the size of the data object.
 * \li int numThreads - the number of threads to use.
 * \li int oprType - the type of operation.
 * \li specColl_t *specColl - a pointer to a specColl_t if this path is
 *      in a special collection (e.g. mounted collection).
 * \li keyValPair_t condInput -  keyword/value pair input. Valid keywords
 *      depend on the API.
 *
 * \sa none
 */

typedef struct DataObjInp {
    char objPath[MAX_NAME_LEN]; ///< Full path to the data object.
    int createMode;            ///< Mode used when creating a new object.
    int openFlags;             ///< Open flags or special-collection index.
    rodsLong_t offset;         ///< Byte offset for the operation.
    rodsLong_t dataSize;       ///< Data size in bytes.
    int numThreads;            ///< Requested number of transfer threads.
    int oprType;               ///< Operation type constant.
    specColl_t *specColl;      ///< Special collection context, if any.
    keyValPair_t condInput;    ///< Operation-specific keyword/value pairs.
} dataObjInp_t;

/**
 * \var openedDataObjInp_t
 * \brief Input struct for Opened data object operation
 * \since 1.0
 *
 * \ingroup capi_input_data_structures
 *
 * \remark none
 *
 * \note
 * Elements of openedDataObjInp_t:
 * \li int l1descInx - the opened data object descriptor from rcDataObjOpen
 *        or rcDataObjCreate
 * \li int len - the length (number of bytes) to read/write.
 * \li int whence - valid only for rcDataObjLseek (similar to lseek of UNIX).
 *      Valid values are SEEK_SET, SEEK_CUR and SEEK_END.
 * \li int oprType - The operation type. Valid values are - PUT_OPR, GET_OPR,
 *      REPLICATE_OPR, ... See dataObjInpOut.h for more.
 * \li rodsLong_t offset - the offset.
 * \li rodsLong_t bytesWritten - Number of bytes written (valid for
 *      rcDataObjClose).
 * \li keyValPair_t condInput -  keyword/value pair input. Valid keywords
 *      depend on the API.
 *
 * \sa none
 */

typedef struct OpenedDataObjInp {
    int l1descInx;             ///< Opened descriptor index.
    int len;                   ///< Length of the read or write operation.
    int whence;                ///< Seek origin for lseek operations.
    int oprType;               ///< Operation type constant.
    rodsLong_t offset;         ///< Byte offset for the operation.
    rodsLong_t bytesWritten;   ///< Number of bytes written on close.
    keyValPair_t condInput;    ///< Operation-specific keyword/value pairs.
} openedDataObjInp_t;

/** Output from opening a transfer portal. */
typedef struct portalOprOut {
    int status;               ///< API status code.
    int l1descInx;            ///< Opened descriptor index.
    int numThreads;           ///< Number of transfer threads to use.
    char chksum[CHKSUM_LEN];  ///< Checksum returned for the transfer.
    portList_t portList;      ///< Portal connection information.
} portalOprOut_t;

/** Input describing a low-level data transfer operation. */
typedef struct DataOprInp {
    int oprType;          ///< Operation type constant.
    int numThreads;       ///< Number of transfer threads.
    int srcL3descInx;     ///< Source level-3 descriptor index.
    int destL3descInx;    ///< Destination level-3 descriptor index.
    int srcRescTypeInx;   ///< Source resource type index.
    int destRescTypeInx;  ///< Destination resource type index.
    /* XXXXXXX offset and dataSize moved to here because of problem with
     * 64 bit susue linux that condInput has pointer's in it which
     * cause condInput to be aligned at 64 the beginning and end of condInput */
    rodsLong_t offset;    ///< Byte offset for the transfer.
    rodsLong_t dataSize;  ///< Total transfer size in bytes.
    keyValPair_t condInput; ///< Transfer-specific keyword/value pairs.
} dataOprInp_t;

/**
 * \var collInp_t
 * \brief Input struct for collection operation
 * \since 1.0
 *
 * \ingroup capi_input_data_structures
 *
 * \remark none
 *
 * \note
 * Elements of collInp_t:
 * \li char collName[MAX_NAME_LEN] - full path of the collection.
 * \li int flags - Flags. Used only by rcOpenCollection.
 * \li int oprType - operation type. not used.
 * \li keyValPair_t condInput -  keyword/value pair input. Valid keywords
 *      depend on the API.
 *
 * \sa none
 */

typedef struct CollInp {
    char collName[MAX_NAME_LEN]; ///< Full collection path.
    int flags;                   ///< Collection operation flags.
    int oprType;                 ///< Operation type constant.
    keyValPair_t condInput;      ///< Operation-specific keyword/value pairs.
} collInp_t;

/* definition for oprType in dataObjInp_t, portalOpr_t and l1desc_t */

/** Sentinel indicating that a portal operation has completed. */
#define DONE_OPR                9999
/** Data object put operation. */
#define PUT_OPR                 1
/** Data object get operation. */
#define GET_OPR                 2
/** Copy operation on the same host. */
#define SAME_HOST_COPY_OPR      3
/** Copy from iRODS to the local host. */
#define COPY_TO_LOCAL_OPR       4
/** Copy from the local host to a remote target. */
#define COPY_TO_REM_OPR         5
/** Replica creation operation. */
#define REPLICATE_OPR           6
/** Destination side of a replication operation. */
#define REPLICATE_DEST          7
/** Source side of a replication operation. */
#define REPLICATE_SRC           8
/** Destination side of a copy operation. */
#define COPY_DEST               9
/** Source side of a copy operation. */
#define COPY_SRC                10
/** Rename a data object. */
#define RENAME_DATA_OBJ         11
/** Rename a collection. */
#define RENAME_COLL             12
/** Move operation. */
#define MOVE_OPR                13
/** Rsync operation. */
#define RSYNC_OPR               14
/** Physical move operation. */
#define PHYMV_OPR               15
/** Source side of a physical move operation. */
#define PHYMV_SRC               16
/** Destination side of a physical move operation. */
#define PHYMV_DEST              17
/** Query data objects. */
#define QUERY_DATA_OBJ          18
/** Query data objects recursively. */
#define QUERY_DATA_OBJ_RECUR    19
/** Query collections. */
#define QUERY_COLL_OBJ          20
/** Query collections recursively. */
#define QUERY_COLL_OBJ_RECUR    21
/** Rename an object of unknown type. */
#define RENAME_UNKNOWN_TYPE     22
/** Operation targets a remote zone. */
#define REMOTE_ZONE_OPR         24
/** Unregister operation. */
#define UNREG_OPR               26

/* definition for openType in l1desc_t */
/** Open type for object creation. */
#define CREATE_TYPE             1
/** Open type for read-only access. */
#define OPEN_FOR_READ_TYPE      2
/** Open type for write access. */
#define OPEN_FOR_WRITE_TYPE     3

/** Portal transfer request sent between peers. */
typedef struct PortalOpr {
    int oprType;                    ///< Operation type constant.
    dataOprInp_t dataOprInp;        ///< Low-level data operation input.
    portList_t portList;            ///< Portal connection information.
    char shared_secret[ NAME_LEN ]; ///< Shared secret used for encryption.
} portalOpr_t;

/* definition for flags */
/** Enables streaming transfer behavior. */
#define STREAMING_FLAG          0x1
/** Disables copy-length verification. */
#define NO_CHK_COPY_LEN_FLAG    0x2

/** Header describing a ranged transfer request. */
typedef struct TransferHeader {
    int oprType;          ///< Operation type constant.
    int flags;            ///< Transfer flags.
    rodsLong_t offset;    ///< Starting byte offset.
    rodsLong_t length;    ///< Number of bytes to transfer.
} transferHeader_t;

#endif  // DATA_OBJ_INP_OUT_H__
