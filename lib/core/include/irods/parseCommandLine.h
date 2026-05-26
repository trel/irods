#ifndef RODS_PARSE_COMMAND_LINE_H__
#define RODS_PARSE_COMMAND_LINE_H__

#include "irods/rodsType.h"

/** Boolean true value used by legacy command-line parsing code. */
#define True  1
/** Boolean false value used by legacy command-line parsing code. */
#define False 0

/** Stores recognized command-line options and their parsed values. */
typedef struct RodsArguments {
    int add;                       /**< Nonzero if the add option was specified. */
    int age;                       /**< Nonzero if an age filter was specified. */
    int agevalue;                  /**< Parsed age value. */
    int all;                       /**< Nonzero if the all option was specified. */
    int accessControl;             /**< Nonzero if access control output was requested. */
    int admin;                     /**< Nonzero if admin mode was requested. */
    int agginfo;                   /**< Nonzero if aggregate information was requested. */
    int ascii;                     /**< Nonzero if ASCII mode was requested. */
    int ascitime;                  /**< Nonzero if ASCII timestamps were requested. */
    int attr;                      /**< Nonzero if an attribute filter was specified. */
    int noattr;                    /**< Nonzero if attributes should be omitted. */
    char *attrStr;                 /**< Attribute string value. */
    int bulk;                      /**< Nonzero if bulk mode was requested. */
    int condition;                 /**< Nonzero if a condition string was specified. */
    char *conditionString;         /**< Condition string value. */
    int collection;                /**< Nonzero if a collection option was specified. */
    char *collectionString;        /**< Collection string value. */
    int dataObjects;               /**< Nonzero if data object mode was requested. */
    int dim;                       /**< Nonzero if dimension-related output was requested. */
    int dryrun;                    /**< Nonzero if the command should not perform changes. */
    int echo;                      /**< Nonzero if command echoing was requested. */
    int empty;                     /**< Nonzero if empty collections or values were requested. */
    int force;                     /**< Nonzero if force mode was requested. */
    int file;                      /**< Nonzero if a file argument was specified. */
    char *fileString;              /**< File argument value. */
    int global;                    /**< Nonzero if global scope was requested. */
    int rescGroup;                 /**< Nonzero if a resource group was specified. */
    char *rescGroupString;         /**< Resource group name. */
    int hash;                      /**< Nonzero if a hash value was specified. */
    char *hashValue;               /**< Hash value string. */
    int header;                    /**< Nonzero if header output was requested. */
    int help;                      /**< Nonzero if help output was requested. */
    int hostAddr;                  /**< Nonzero if a host address was specified. */
    char *hostAddrString;          /**< Host address string. */
    int input;                     /**< Nonzero if input mode was requested. */
    int redirectConn;              /**< Nonzero if connection redirection was requested. */
    int checksum;                  /**< Nonzero if checksum calculation was requested. */
    int verifyChecksum;            /**< Nonzero if checksum verification was requested. */
    int dataType;                  /**< Nonzero if a data type was specified. */
    char *dataTypeString;          /**< Data type string value. */
    int longOption;                /**< Nonzero if long output was requested. */
    int link;                      /**< Nonzero if link handling was requested. */
    int veryLongOption;            /**< Nonzero if very long output was requested. */
    int mountCollection;           /**< Nonzero if a mount collection was specified. */
    char *mountType;               /**< Mount type string. */
    int replNum;                   /**< Nonzero if a replica number was specified. */
    char *replNumValue;            /**< Replica number string value. */
    int newFlag;                   /**< Nonzero if new-only behavior was requested. */
    char *startTimeInxStr;         /**< Start time index string. */
    int noPage;                    /**< Nonzero if paging should be disabled. */
    int number;                    /**< Nonzero if a numeric limit was specified. */
    int numberValue;               /**< Parsed numeric limit. */
    int physicalPath;              /**< Nonzero if a physical path was specified. */
    char *physicalPathString;      /**< Physical path value. */
    int logicalPath;               /**< Nonzero if a logical path was specified. */
    char *logicalPathString;       /**< Logical path value. */
    int progressFlag;              /**< Nonzero if progress reporting was requested. */
    int option;                    /**< Nonzero if an option string was specified. */
    char *optionString;            /**< Option string value. */
    int orphan;                    /**< Nonzero if orphaned entries were requested. */
    int purgeCache;                /**< Nonzero if cache purging was requested. */
    int bundle;                    /**< Nonzero if bundle mode was requested. */
    int prompt;                    /**< Nonzero if interactive prompting was requested. */
    int query;                     /**< Nonzero if a query string was specified. */
    char* queryStr;                /**< Query string value. */
    int reg;                       /**< Nonzero if registration mode was requested. */
    int recursive;                 /**< Nonzero if recursive behavior was requested. */
    int resource;                  /**< Nonzero if a resource was specified. */
    char *resourceString;          /**< Resource name value. */
    int remove;                    /**< Nonzero if removal mode was requested. */
    int showFirstLine;             /**< Nonzero if only the first line should be shown. */
    int sizeFlag;                  /**< Nonzero if a size filter was specified. */
    rodsLong_t size;               /**< Parsed size value. */
    int srcResc;                   /**< Nonzero if a source resource was specified. */
    char *srcRescString;           /**< Source resource name value. */
    int subset;                    /**< Nonzero if subset mode was requested. */
    int subsetByVal;               /**< Nonzero if subsetting by value was requested. */
    char *subsetStr;               /**< Subset expression value. */
    int test;                      /**< Nonzero if test mode was requested. */
    int ticket;                    /**< Nonzero if a ticket string was specified. */
    char *ticketString;            /**< Ticket string value. */
    int tree;                      /**< Nonzero if tree output was requested. */
    int ttl;                       /**< Nonzero if a TTL value was specified. */
    int ttlValue;                  /**< Parsed TTL value. */
    int reconnect;                 /**< Nonzero if reconnect behavior was requested. */
    int user;                      /**< Nonzero if a user name was specified. */
    char *userString;              /**< User name value. */
    int unmount;                   /**< Nonzero if unmount mode was requested. */
    int verbose;                   /**< Nonzero if verbose output was requested. */
    int veryVerbose;               /**< Nonzero if very verbose output was requested. */
    int writeFlag;                 /**< Nonzero if write access was requested. */
    int zone;                      /**< Nonzero if a zone was specified. */
    char *zoneName;                /**< Zone name value. */
    int verify;                    /**< Nonzero if verification was requested. */
    int var;                       /**< Nonzero if a variable string was specified. */
    char *varStr;                  /**< Variable string value. */
    int extract;                   /**< Nonzero if extraction was requested. */
    int restart;                   /**< Nonzero if a restart file was specified. */
    char *restartFileString;       /**< Restart file path. */
    int lfrestart;                 /**< Nonzero if a large-file restart file was specified. */
    char *lfrestartFileString;     /**< Large-file restart file path. */
    int version;                   /**< Nonzero if version output was requested. */
    int retries;                   /**< Nonzero if a retry count was specified. */
    int retriesValue;              /**< Parsed retry count. */
    int regRepl;                   /**< Nonzero if replica registration was requested. */
    int noCompute;                 /**< Nonzero if compute-intensive work should be skipped. */

    int parallel;                  /**< Nonzero if parallel transfer mode was requested. */
    int serial;                    /**< Nonzero if serial transfer mode was requested. */
    int primaryIcat;               /**< Nonzero if the primary iCAT should be targeted. */
    int silent;                    /**< Nonzero if output should be suppressed. */
    int sql;                       /**< Nonzero if SQL output or input was requested. */
    int optind;                    /**< Index of the first unrecognized argument in `argv`. */

    int excludeFile;               /**< Nonzero if an exclude file was specified. */
    char *excludeFileString;       /**< Exclude file path. */

    // =-=-=-=-=-=-=-
    // atomic metadata put &
    // kv pass through
    char* metadata_string;         /**< Atomic metadata input string. */
    char* acl_string;              /**< ACL input string. */
    int kv_pass;                   /**< Nonzero if key/value passthrough was requested. */
    char* kv_pass_string;          /**< Key/value passthrough string. */
} rodsArguments_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Parses command-line options into a `rodsArguments_t` structure.
 *
 * The function examines `argc` and `argv` using `optString` and, when
 * `includeLong` is nonzero, also recognizes the supported long options.
 * Parsed flags and option values are written into `rodsArgs`.
 */
int
parseCmdLineOpt( int argc, char **argv, const char *optString, int includeLong,
                 rodsArguments_t *rodsArgs );

#ifdef __cplusplus
} // extern "C"
#endif

#endif // RODS_PARSE_COMMAND_LINE_H__
