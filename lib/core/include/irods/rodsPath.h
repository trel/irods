#ifndef RODS_PATH_H__
#define RODS_PATH_H__

#include "irods/rodsDef.h"
#include "irods/rods.h"
#include "irods/getRodsEnv.h"
#include "irods/rodsType.h"
#include "irods/objStat.h"

#ifdef __cplusplus
extern "C" {
#endif

/// Special path string indicating standard output.
#define STDOUT_FILE_NAME	"-"

/// Represents a parsed local or iRODS path.
typedef struct RodsPath {
    objType_t objType;          /**< Object type. */
    objStat_t objState;         /**< Object existence state. */
    rodsLong_t size;            /**< Object size in bytes. */
    uint objMode;               /**< Object mode bits. */
    char inPath[MAX_NAME_LEN];  /**< Input path from the command line. */
    char outPath[MAX_NAME_LEN]; /**< Normalized path after parsing. */
    char dataId[NAME_LEN];      /**< Data object identifier string. */
    char chksum[CHKSUM_LEN];    /**< Data object checksum. */
    rodsObjStat_t *rodsObjStat; /**< Cached object status information. */
} rodsPath_t;

/// Holds parsed source and destination command-line paths.
typedef struct RodsPathInp {
    int numSrc;            /**< Number of source paths. */
    rodsPath_t *srcPath;   /**< Array of source paths. */
    rodsPath_t *destPath;  /**< Optional destination path. */
    rodsPath_t *targPath;  /**< Target path for source/destination commands. */
    int resolved;          /**< Indicates whether paths have been resolved. */
} rodsPathInp_t;

/// Allows command-line parsing with no source path.
#define	ALLOW_NO_SRC_FLAG	0x1

/// Parses a path into a `rodsPath_t`.
int
parseRodsPath( rodsPath_t *rodsPath, rodsEnv *myRodsEnv );
/// Converts an input path string into a parsed output path string.
int
parseRodsPathStr( const char *inPath, rodsEnv *myRodsEnv, char *outPath );
/// Appends a source path to a `rodsPathInp_t`.
int
addSrcInPath( rodsPathInp_t *rodsPathInp, const char *inPath );
/// Parses a local filesystem path.
int
parseLocalPath( rodsPath_t *rodsPath );
/// Parses a local filesystem path using command-line arguments.
int parse_local_path(const struct RodsArguments* _args, struct RodsPath* _path);
/// Parses command-line source and destination paths.
int
parseCmdLinePath( int argc, char **argv, int optInd, rodsEnv *myRodsEnv,
                  int srcFileType, int destFileType, int flag, rodsPathInp_t *rodsPathInp );
/// Parses command-line source and destination paths using extended arguments.
int parse_command_line_path(int argc,
                             char** argv,
                             int optInd,
                            struct RodsEnvironment* myRodsEnv,
                            int srcFileType,
                            int destFileType,
                            int flag,
                             struct RodsPathInp* rodsPathInp,
                             const struct RodsArguments* _rods_args);
/// Extracts the final path component from `inPath`.
int
getLastPathElement( char *inPath, char *lastElement );

/// Determines the type of object described by `rodsPath`.
int
getFileType( rodsPath_t *rodsPath );
/// Clears the contents of a `rodsPath_t`.
void
clearRodsPath( rodsPath_t *rodsPath );
/// Frees dynamically allocated members of a `rodsPathInp_t`.
void freeRodsPathInpMembers(rodsPathInp_t* path);

/// Returns a newly allocated copy of `_path` with special characters escaped.
char* escape_path(const char* _path);

/// Returns non-zero if `path` ends with a trailing path separator.
int has_trailing_path_separator(const char* path);

/// Removes trailing path separators from `path` in place.
void remove_trailing_path_separators(char* path);

/// Returns whether \p path starts with \p prefix.
///
/// \p path and \p prefix are expected to be null-terminated strings.
/// The behavior is undefined if either string is null or not null-terminated.
///
/// \since 4.2.8
///
/// \param[in] path   The path to search.
/// \param[in] prefix The path to look for. Trailing slashes are ignored.
///
/// \return An integer value.
/// \retval non-zero If \p path starts with \p prefix.
/// \retval 0        If \p path does not start with \p prefix or \p prefix is an empty string.
int has_prefix(const char* path, const char* prefix);
#ifdef __cplusplus
}
#endif

#endif	// RODS_PATH_H__
