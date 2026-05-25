#ifndef OBJ_INFO_H__
#define OBJ_INFO_H__

#include "irods/rodsType.h"
#include "irods/rodsUser.h"

/* dataObjInfo_t is for info about a data object.
   intKeyStrVal_t is a generic (integer keyword)/(string value) pair.
   It can be used for many things. For example, for the input "condition",
   the condKeywd_t is used for keyword. But it can also be used to
   input parameters for rcat registration.

   The routine addIntKeywdStrVal() in rcMisc.c can be used to add a
   keyword/value pair and getValByIntKeywd() can be used to get the
   value string based on a keyword.
*/

/** Select all replicas when specifying a copy count. */
#define ALL_COPIES              -1      /* "all" */

/** Generic data type name. */
#define GENERIC_DT_STR          "generic"
/** Tar archive data type name. */
#define TAR_DT_STR              "tar file"
/** Gzip-compressed tar archive data type name. */
#define GZIP_TAR_DT_STR         "gzipTar"  // JMC - backport 4632
/** Bzip2-compressed tar archive data type name. */
#define BZIP2_TAR_DT_STR        "bzip2Tar" // JMC - backport 4632
/** Zip archive data type name. */
#define ZIP_DT_STR              "zipFile"  // JMC - backport 4633
/** MSSO data type name. */
#define MSSO_DT_STR             "msso file"

/** Tar bundle type used for internal physical bundles. */
#define TAR_BUNDLE_DT_STR       "tar bundle"       // JMC - backport 4658
/** Gzip tar bundle type used for internal physical bundles. */
#define GZIP_TAR_BUNDLE_DT_STR  "gzipTar bundle"   // JMC - backport 4658
/** Bzip2 tar bundle type used for internal physical bundles. */
#define BZIP2_TAR_BUNDLE_DT_STR "bzip2Tar bundle"  // JMC - backport 4658
/** Zip bundle type used for internal physical bundles. */
#define ZIP_BUNDLE_DT_STR       "zipFile bundle"   // JMC - backport 4658

/** HAAW data type name. */
#define HAAW_DT_STR             "haaw file"
/** Maximum number of soft links allowed in a path. */
#define MAX_LINK_CNT            20      /* max number soft link in a path */

/* special collection */

/** Identifies the type of special collection represented by `specColl_t`. */
typedef enum SpecialCollClass {
    NO_SPEC_COLL,      /**< The collection is not special. */
    STRUCT_FILE_COLL,  /**< The collection is backed by a structured file. */
    MOUNTED_COLL,      /**< The collection is mounted from physical storage. */
    LINKED_COLL        /**< The collection is a logical link to another collection. */
} specCollClass_t;

/** Identifies the type of structured file. */
typedef enum StructFileType {
    NONE_STRUCT_FILE_T = 0,  /**< No known structured file type. */
    HAAW_STRUCT_FILE_T = 1,  /**< A UK eScience HAAW structured file. */
    TAR_STRUCT_FILE_T  = 2,  /**< A tar structured file. */
    MSSO_STRUCT_FILE_T = 3,  /**< A workflow MSSO structured file. */
} structFileType_t;

/** Describes how an operation interacts with a special collection. */
typedef enum StructFileOprType {
    NOT_SPEC_COLL_OPR,               /**< The operation does not target a special collection. */
    NON_STRUCT_FILE_SPEC_COLL_OPR,   /**< The operation targets a non-structured special collection. */
    STRUCT_FILE_SPEC_COLL_OPR,       /**< The operation targets a structured-file special collection. */
    NORMAL_OPR_ON_STRUCT_FILE_COLL   /**< The operation is a normal operation within a structured-file collection. */
} structFileOprType_t;

/** String form of `HAAW_STRUCT_FILE_T`. */
#define HAAW_STRUCT_FILE_STR      "haawStructFile"
/** String form of `TAR_STRUCT_FILE_T`. */
#define TAR_STRUCT_FILE_STR       "tarStructFile"
/** Keyword identifying a mount point collection. */
#define MOUNT_POINT_STR           "mountPoint"
/** Keyword identifying a linked collection. */
#define LINK_POINT_STR            "linkPoint"
/** Keyword indicating inheritance from the parent special collection. */
#define INHERIT_PAR_SPEC_COLL_STR "inheritParentSpecColl"
/** String form of `MSSO_STRUCT_FILE_T`. */
#define MSSO_STRUCT_FILE_STR      "mssoStructFile"
/** Keyword identifying an MSO object. */
#define MSO_STR                   "mso"

/** Keyword identifying an unmount operation. */
#define UNMOUNT_STR               "unmount"

/** Describes a special collection and its backing storage. */
typedef struct SpecColl {
    specCollClass_t collClass;       /**< Class of special collection. */
    structFileType_t type;           /**< Structured file type, if applicable. */
    char collection[MAX_NAME_LEN];   /**< Structured file path or mounted collection path. */
    char objPath[MAX_NAME_LEN];      /**< Logical path of the structured file, or not applicable for mounted collections. */
    char resource[NAME_LEN];         /**< Resource associated with the collection. */
    char rescHier[MAX_NAME_LEN];     /**< Resource hierarchy associated with the collection. */
    char phyPath[MAX_NAME_LEN];      /**< Physical path of the structured file or mounted directory. */
    char cacheDir[MAX_NAME_LEN];     /**< Cache directory used for structured file extraction. */
    int cacheDirty;                  /**< Nonzero if the cache tree has been modified. */
    int replNum;                     /**< Replica number associated with the collection. */
} specColl_t;

/** Permission level cached for a special collection. */
typedef enum SpecialCollPerm {
    UNKNOWN_COLL_PERM,  /**< The permission level is unknown. */
    READ_COLL_PERM,     /**< Read permission is available. */
    WRITE_COLL_PERM     /**< Write permission is available. */
} specCollPerm_t;

/** Caches metadata about a special collection. */
typedef struct SpecCollCache {
    specCollPerm_t perm;             /**< Cached permission level. */
    specColl_t specColl;             /**< Cached special collection description. */
    char collId[NAME_LEN];           /**< Collection identifier. */
    char ownerName[NAME_LEN];        /**< Collection owner name. */
    char ownerZone[NAME_LEN];        /**< Collection owner zone. */
    char createTime[NAME_LEN];       /**< Creation timestamp string. */
    char modifyTime[NAME_LEN];       /**< Modification timestamp string. */
    struct SpecCollCache *next;      /**< Next cache entry in the linked list. */
} specCollCache_t;

/** Integer representation of replica state values. */
typedef int repl_status_t;
/** Replica state values used by `repl_status_t`. */
enum {
    STALE_REPLICA,         /**< Replica contents are stale. */
    GOOD_REPLICA,          /**< Replica contents are current. */
    INTERMEDIATE_REPLICA,  /**< Replica update is in progress. */
    READ_LOCKED,           /**< Replica is locked for reading. */
    WRITE_LOCKED           /**< Replica is locked for writing. */
};

/** Stores parallel arrays of string keys and string values. */
typedef struct KeyValPair {
    int len;             /**< Number of key/value pairs. */
    char **keyWord;      /**< Array of keys. */
    char **value;        /**< Array of values. */
} keyValPair_t;

/** Flag indicating metadata changes should not be committed immediately. */
#define NO_COMMIT_FLAG  0x1  /* used in chlModDataObjMeta and chlRegDataObj */

/** Describes a single data object replica and its catalog metadata. */
typedef struct DataObjInfo {
    char objPath[MAX_NAME_LEN];     /**< Logical path of the data object. */
    char rescName[NAME_LEN];        /**< Resource name hosting the replica. */
    char rescHier[MAX_NAME_LEN];    /**< Resource hierarchy containing the replica. */
    char dataType[NAME_LEN];        /**< Data type string. */
    rodsLong_t dataSize;            /**< Size of the data object in bytes. */
    char chksum[CHKSUM_LEN];        /**< Checksum string for the replica. */
    char version[NAME_LEN];         /**< Version string. */
    char filePath[MAX_NAME_LEN];    /**< Physical path to the replica. */
    char dataOwnerName[NAME_LEN];   /**< Owner user name. */
    char dataOwnerZone[NAME_LEN];   /**< Owner zone name. */
    int  replNum;                   /**< Replica number. */
    int  replStatus;                /**< Replica status value. */
    char statusString[NAME_LEN];    /**< Status text from the catalog. */
    rodsLong_t  dataId;             /**< Data object identifier. */
    rodsLong_t  collId;             /**< Parent collection identifier. */
    int  dataMapId;                 /**< Data map identifier. */
    int flags;                      /**< Flags used by catalog registration operations. */
    char dataComments[LONG_NAME_LEN]; /**< User-visible comments. */
    char dataMode[SHORT_STR_LEN];   /**< Mode string for the data object. */
    char dataExpiry[TIME_LEN];      /**< Expiration timestamp string. */
    char dataCreate[TIME_LEN];      /**< Creation timestamp string. */
    char dataModify[TIME_LEN];      /**< Modification timestamp string. */
    char dataAccess[NAME_LEN];      /**< Access control string. */
    int  dataAccessInx;             /**< Access index value. */
    int  writeFlag;                 /**< Nonzero if the replica is open for writing. */
    char destRescName[NAME_LEN];    /**< Destination resource name for replication or movement. */
    // The backupResc feature has been removed. This struct member is not deleted in iRODS 5
    // because this would break federation between iRODS 4 and iRODS 5.
    char backupRescName[NAME_LEN];  /**< Legacy backup resource name retained for compatibility. */
    char subPath[MAX_NAME_LEN];     /**< Subpath within a structured file or special collection. */
    specColl_t *specColl;           /**< Associated special collection, if any. */
    int regUid;                     /**< UNIX uid of the registering user. */
    int otherFlags;                 /**< Additional flags reserved for future use. */
    keyValPair_t condInput;         /**< Conditional input keywords associated with the object. */
    char in_pdmo[MAX_NAME_LEN];     /**< Indicates the current PDMO context in the hierarchy. */
    struct DataObjInfo *next;       /**< Next replica description in the linked list. */
    rodsLong_t rescId;              /**< Resource identifier. */
    char dataAccessTime[TIME_LEN];  /**< Last access timestamp string. */
} dataObjInfo_t ;

/**
 * Additional semantics for `collInfo_t` type-specific fields.
 *
 * `collInfo1` stores the physical directory path for `MOUNTED_COLL`, the linked
 * logical path for `LINKED_COLL`, and the logical path of the tar file for
 * `TAR_STRUCT_FILE_T`.
 *
 * `collInfo2` stores the resource for `MOUNTED_COLL`, nothing for
 * `LINKED_COLL`, and `cacheDirPath;;;resource;;;cacheDirty` for
 * `TAR_STRUCT_FILE_T`.
 */
/** Describes a collection and its catalog metadata. */
typedef struct CollInfo {
    rodsLong_t collId;              /**< Collection identifier. */
    char collName[MAX_NAME_LEN];    /**< Logical collection path. */
    char collParentName[MAX_NAME_LEN]; /**< Parent collection path. */
    char collOwnerName[NAME_LEN];   /**< Owner user name. */
    char collOwnerZone[NAME_LEN];   /**< Owner zone name. */
    int  collMapId;                 /**< Collection map identifier. */
    int  collAccessInx;             /**< Access index value. */
    char collComments[LONG_NAME_LEN]; /**< User-visible comments. */
    char collInheritance[LONG_NAME_LEN]; /**< Inheritance setting string. */
    char collExpiry[TIME_LEN];      /**< Expiration timestamp string. */
    char collCreate[TIME_LEN];      /**< Creation timestamp string. */
    char collModify[TIME_LEN];      /**< Modification timestamp string. */
    char collAccess[NAME_LEN];      /**< Access control string. */
    char collType[NAME_LEN];        /**< Collection type string. */
    char collInfo1[MAX_NAME_LEN];   /**< Type-specific collection metadata field 1. */
    char collInfo2[MAX_NAME_LEN];   /**< Type-specific collection metadata field 2. */
    keyValPair_t condInput;         /**< Conditional input keywords associated with the collection. */

    struct CollInfo *next;          /**< Next collection description in the linked list. */
} collInfo_t;

/** Placeholder rule metadata structure. */
typedef struct RuleInfo {
    int TDB;                        /**< Reserved field. */
} ruleInfo_t;

/** Stores parallel arrays of integer indices and integer values. */
typedef struct InxIvalPair {
    int len;            /**< Number of index/value pairs. */
    int *inx;           /**< Array of indices. */
    int *value;         /**< Array of values. */
} inxIvalPair_t;

/** Stores parallel arrays of integer indices and string values. */
typedef struct InxValPair {
    int len;            /**< Number of index/value pairs. */
    int *inx;           /**< Array of indices. */
    char **value;       /**< Array of string values. */
} inxValPair_t;

/** Stores a flat character buffer interpreted as a fixed-width string array. */
typedef struct StrArray {
    int len;            /**< Number of elements in the array. */
    int size;           /**< Maximum size of each element in characters. */
    char* value;        /**< Character buffer laid out as `[len][size]`. */
} strArray_t;

/** Stores an array of integers. */
typedef struct IntArray {
    int len;            /**< Number of elements in the array. */
    int *value;         /**< Integer array of length `len`. */
} intArray_t;

/** Identifies a resource category. */
typedef enum ResourceCategory {
    FILE_CAT,  /**< File-system-backed resource category. */
    DB_CAT     /**< Database-backed resource category. */
} rescCat_t;

/** Default mode used when creating files. */
#define DEFAULT_FILE_MODE       0600
/** Default mode used when creating directories. */
#define DEFAULT_DIR_MODE        0750

/** Disallow registration of the path. */
#define DISALLOW_PATH_REG       0 // disallow path registration // JMC - backport 4774
/** Skip path permission checks. */
#define NO_CHK_PATH_PERM        1 // JMC - backport 4758
/** Require path permission checks. */
#define DO_CHK_PATH_PERM        2 // JMC - backport 4774
/** Allow registration of a user's vault path. */
#define CHK_NON_VAULT_PATH_PERM 3 // allow reg of user's vault path // JMC - backport 4774

/** String form of `DISALLOW_PATH_REG`. */
#define DISALLOW_PATH_REG_STR       "disallowPathReg"     // JMC - backport 4774
/** String form of `NO_CHK_PATH_PERM`. */
#define NO_CHK_PATH_PERM_STR        "noChkPathPerm"       // JMC - backport 4774
/** String form of `DO_CHK_PATH_PERM`. */
#define DO_CHK_PATH_PERM_STR        "doChkPathPerm"       // JMC - backport 4774
/** String form of `CHK_NON_VAULT_PATH_PERM`. */
#define CHK_NON_VAULT_PATH_PERM_STR "chkNonVaultPathPerm" // JMC - backport 4774

/** No staging action is required. */
#define NO_STAGING        0
/** Stage the source object before use. */
#define STAGE_SRC         1
/** Synchronize the destination object. */
#define SYNC_DEST         2

/** Move removed data into the trash. */
#define DO_TRASH_CAN      0
/** Remove data without using the trash. */
#define NO_TRASH_CAN      1

/** Controls whether missing paths may be created. */
typedef enum CreatePath {
    NO_CREATE_PATH,  /**< Do not create missing paths. */
    CREATE_PATH      /**< Create missing paths as needed. */
} createPath_t;

/** Resource class identifier for cache resources. */
#define CACHE_CL        0
/** Resource class identifier for archival resources. */
#define ARCHIVAL_CL     1
/** Resource class identifier for bundle resources. */
#define BUNDLE_CL       2
/** Resource class identifier for compound resources. */
#define COMPOUND_CL     3
/** Resource class identifier for database resources. */
#define DATABASE_CL     4

/** Bit flag marking a primary resource class. */
#define PRIMARY_FLAG    0x8000          /* primary class when this bit is set */
// JMC - legacy resource - typedef struct RescClass {
//    char *className;
//    int classType;
//} rescClass_t;

/** Legacy transfer statistics structure. */
typedef struct TransStat {
    int numThreads;            /**< Number of transfer threads used. */
    rodsLong_t bytesWritten;   /**< Total bytes written. */
} transStat_t;

/** Transfer statistics structure with explicit padding field. */
typedef struct TransferStat {
    int numThreads;            /**< Number of transfer threads used. */
    int flags;                 /**< Reserved flags and padding. */
    rodsLong_t bytesWritten;   /**< Total bytes written. */
} transferStat_t;

/** Default number of files reported per collection operation status update. */
#define FILE_CNT_PER_STAT_OUT   10      /* the default file count per collOprStat output */
/** Summarizes progress for a collection operation. */
typedef struct CollectionOperationStat {
    int filesCnt;                    /**< Number of files processed in the current batch. */
    int totalFileCnt;                /**< Total number of files processed. */
    rodsLong_t bytesWritten;         /**< Total bytes written. */
    char lastObjPath[MAX_NAME_LEN];  /**< Path of the most recently processed object. */
} collOprStat_t;

/** Maps tagged substrings to keywords using parallel tag arrays. */
typedef struct TagStruct {
    int len;             /**< Number of tag definitions. */
    char **preTag;       /**< Array of regexes marking the start of values. */
    char **postTag;      /**< Array of regexes marking the end of values. */
    char **keyWord;      /**< Array of keywords associated with the tags. */
} tagStruct_t;

/** Identifies a subfile within a structured file or special collection. */
typedef struct Subfile {
    rodsHostAddr_t addr;             /**< Host address of the backing storage. */
    char subFilePath[MAX_NAME_LEN];  /**< Physical or logical path to the subfile. */
    int mode;                        /**< Open mode for the subfile. */
    int flags;                       /**< Operation flags for the subfile. */
    rodsLong_t offset;               /**< Byte offset within the subfile. */
    specColl_t *specColl;            /**< Associated special collection. */
} subFile_t;

/** Maps a structured file type name to its enum value. */
typedef struct StructFileTypeDef {
    char *typeName;                  /**< Structured file type name. */
    structFileType_t type;           /**< Structured file type value. */
} structFileTypeDef_t;

#endif  // OBJ_INFO_H__
