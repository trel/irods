#ifndef RODS_TYPE_H__
#define RODS_TYPE_H__

#include <sys/types.h>

#if defined(solaris_platform) || defined(aix_platform)
    #include <strings.h>
#endif

#include "irods/rodsDef.h"

// clang-format off

#if defined(osx_platform)
/// Platform-independent signed 64-bit integer type.
typedef int64_t rodsLong_t;
/// Platform-independent unsigned 64-bit integer type.
typedef u_int64_t rodsULong_t;
#elif defined(sgi_platform)
/// Platform-independent signed 64-bit integer type.
typedef __int64_t rodsLong_t;
/// Platform-specific unsigned long long alias for SGI builds.
typedef int64_t u_longlong_t;
#elif defined(linux_platform) || defined(alpha_platform)
/// Platform-independent signed 64-bit integer type.
typedef long long rodsLong_t;
/// Platform-independent unsigned 64-bit integer type.
typedef unsigned long long rodsULong_t;
#elif defined(windows_platform)
/// Unsigned integer alias used by legacy Windows declarations.
typedef unsigned int uint;
/// Platform-independent signed 64-bit integer type.
typedef __int64 rodsLong_t;
/// Platform-independent unsigned 64-bit integer type.
typedef unsigned __int64 rodsULong_t;
#else	/* windows_platform */
/// Platform-independent signed 64-bit integer type.
typedef long long rodsLong_t;
/// Platform-independent unsigned 64-bit integer type.
typedef unsigned long long rodsULong_t;
#endif	/* windows_platform */

/// Identifies the type of object represented by a path.
typedef enum ObjectType {
    UNKNOWN_OBJ_T,  /**< Unknown object type. */
    DATA_OBJ_T,     /**< Data object. */
    COLL_OBJ_T,     /**< Collection. */
    UNKNOWN_FILE_T, /**< Unknown local filesystem object type. */
    LOCAL_FILE_T,   /**< Local filesystem file. */
    LOCAL_DIR_T,    /**< Local filesystem directory. */
    NO_INPUT_T      /**< No input object was provided. */
} objType_t;

/// Identifies whether an object exists.
typedef enum ObjectStat {
    UNKNOWN_ST,   /**< Existence is unknown. */
    NOT_EXIST_ST, /**< Object does not exist. */
    EXIST_ST      /**< Object exists. */
} objStat_t;

/// Stat-like metadata for an iRODS object.
typedef struct rodsStat {
    rodsLong_t   st_size;    /**< File size in bytes. */
    unsigned int st_dev;     /**< Device identifier. */
    unsigned int st_ino;     /**< Inode number. */
    unsigned int st_mode;    /**< Mode bits. */
    unsigned int st_nlink;   /**< Link count. */
    unsigned int st_uid;     /**< User identifier. */
    unsigned int st_gid;     /**< Group identifier. */
    unsigned int st_rdev;    /**< Special device identifier. */
    unsigned int st_atim;    /**< Time of last access. */
    unsigned int st_mtim;    /**< Time of last modification. */
    unsigned int st_ctim;    /**< Time of last status change. */
    unsigned int st_blksize; /**< Preferred filesystem block size. */
    unsigned int st_blocks;  /**< Number of allocated blocks. */
} rodsStat_t;

/// Maximum length of a directory entry name.
#define DIR_LEN 	256

/// Directory entry metadata.
typedef struct rodsDirent {
    unsigned int d_offset;       /**< Offset following this entry. */
    unsigned int d_ino;          /**< Inode number. */
    unsigned int d_reclen;       /**< Length of this record. */
    unsigned int d_namlen;       /**< Length of `d_name`. */
    char         d_name[DIR_LEN];/**< Entry name. */
} rodsDirent_t;

// clang-format on

#endif	// RODS_TYPE_H__
