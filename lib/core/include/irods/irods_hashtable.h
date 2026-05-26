/* For copyright information please refer to files in the COPYRIGHT directory
 */
#ifndef HASHTABLE_H__
#define HASHTABLE_H__

/// \file
/// \brief Declares a simple string-keyed hash table API.

#include <string.h>
#include <stdio.h>
#include "irods/region.h"

/// \brief Seed used by the default hash function.
#define HASH_BASE 5381
/// \brief Computes the hash value for a string key.
#define myhash(x) B_hash((unsigned char*)(x))

/// \brief Represents a single hash table entry.
struct bucket {
    char* key;              ///< Key associated with the entry.
    const void* value;      ///< Value stored for the key.
    struct bucket *next;    ///< Next entry in the collision chain.
};

/// \brief Represents a hash table of string keys to opaque values.
typedef struct hashtable {
    struct bucket **buckets; ///< Bucket array.
    int size;                 ///< Total bucket capacity.
    int len;                  ///< Number of stored entries.
    int dynamic;              ///< Indicates whether storage is dynamically managed.
    Region *bucketRegion;     ///< Region used for bucket allocation.
} Hashtable;

#ifdef __cplusplus
extern "C" {
#endif

/// \brief Creates a new bucket for a key-value pair.
struct bucket *newBucket( const char* key, const void* value );
/// \brief Returns the hash value for a string.
unsigned long B_hash( unsigned char* string );
/// \brief Allocates a new hash table with the requested capacity.
Hashtable *newHashTable( int size );
/// \brief Allocates a new hash table using the provided region.
Hashtable *newHashTable2( int size, Region *r );
/// \brief Inserts a key-value pair into the hash table.
int insertIntoHashTable( Hashtable *h, const char* key, const void *value );
/// \brief Replaces the value associated with a key.
const void* updateInHashTable( Hashtable *h, const char* key, const void *value );
/// \brief Removes the key-value pair for a key.
const void* deleteFromHashTable( Hashtable *h, const char* key );
/// \brief Looks up the value associated with a key.
const void* lookupFromHashTable( Hashtable *h, const char* key );
/// \brief Deletes a hash table and optionally frees stored values.
void deleteHashTable( Hashtable *h, void ( *f )( const void * ) );
/// \brief Deletes a bucket chain and optionally frees stored values.
void deleteBucket( struct bucket *h, void ( *f )( const void * ) );
/// \brief Returns the bucket containing the requested key.
struct bucket* lookupBucketFromHashTable( Hashtable *h, const char* key );
/// \brief Returns the next bucket after the bucket for the requested key.
struct bucket* nextBucket( struct bucket *h, const char* key );
/// \brief No-op deleter for values which do not require cleanup.
void nop( const void *a );
/// \brief Frees a value stored as a const pointer.
void free_const( const void *a );

#ifdef __cplusplus
}
#endif

#endif // HASHTABLE_H__
