/* For copyright information please refer to files in the COPYRIGHT directory
 */
#ifndef _REGION_H__
#define _REGION_H__

#include <stdio.h>
#include <setjmp.h>

/// Default size, in bytes, of the first region block.
#define DEFAULT_BLOCK_SIZE 1024
/// Alignment, in bytes, used for region allocations.
#define REGION_ALIGNMENT 8
/// Rounds a size up to the region allocation alignment.
#define roundToAlignment(x) ((x)%REGION_ALIGNMENT == 0?(x):(((x)/REGION_ALIGNMENT)+1)*REGION_ALIGNMENT)
/// Returns the address of element `i` of type `t` starting at `p`.
#define CACHE_ELEM(t, p, i) (((t)p)+i)
/// Returns the aligned cache size for `l` elements of type `t`.
#define CACHE_SIZE(t, l) roundToAlignment(sizeof(t)*l)

/// Describes an error raised while working with a region.
struct region_error {
    int code;        /**< Error code. */
    char msg[1024];  /**< Human-readable error message. */
    void *obj;       /**< Object associated with the error. */
};

/* #define REGION_MALLOC */
#ifdef REGION_MALLOC
/// Tracks an allocation in malloc-backed region mode.
struct region_node {
    size_t size;              /**< Allocation size in bytes. */
    void *ptr;                /**< Allocated memory block. */
    struct region_node *next; /**< Next tracked allocation. */
};

/// Region allocator state for malloc-backed mode.
typedef struct region {
    struct region_node *head, *tail; /**< First and last allocation nodes. */
} Region;
#else
/// Tracks a block in a region allocator.
struct region_node {
    unsigned char *block;     /**< Pointer to the memory block. */
    size_t size;              /**< Size of the memory block in bytes. */
    size_t used;              /**< Number of bytes already allocated. */
    struct region_node *next; /**< Next block in the region. */

};


/// Region allocator state.
typedef struct region {
    struct region_node *head, *active; /**< First and active blocks. */
    jmp_buf *label;                    /**< Jump target for fatal allocation errors. */
    struct region_error error;         /**< Last recorded region error. */
} Region;
#endif
/// Metadata stored ahead of values allocated from a region.
typedef struct region_desc {
    Region *region; /**< Owning region. */
    size_t size;    /**< Size of the user allocation. */
    int del;        /**< Delete marker. */
} RegionDesc;

/// Returns whether `x` belongs to region `r`.
#define IN_REGION(x,r) (((RegionDesc *)(((const unsigned char*)(x))-CACHE_SIZE(RegionDesc, 1)))->region == (r))
/// Marks an allocation as deleted.
#define SET_DELETE(x) (((RegionDesc *)(((const unsigned char*)(x))-CACHE_SIZE(RegionDesc, 1)))->del=1)
/// Returns whether an allocation is marked as deleted.
#define DELETED(x) (((RegionDesc *)(((const unsigned char*)(x))-CACHE_SIZE(RegionDesc, 1)))->del)
/// Returns the recorded size of an allocation.
#define SIZE(x) (((RegionDesc *)(((const unsigned char*)(x))-CACHE_SIZE(RegionDesc, 1)))->size)

#ifdef __cplusplus
extern "C" {
#endif

/// Creates a region using `is` bytes for the initial block when non-zero.
///
/// If `is` is zero, the default block size is used. Returns null on allocation failure.
Region *make_region( size_t is, jmp_buf *label );
/// Allocates `s` bytes from region `r`.
///
/// Returns null on allocation failure.
void *region_alloc( Region *r, size_t s );
/// Frees all memory owned by region `r`.
void region_free( Region *r );
/// Returns the total size of memory managed by region `r`.
size_t region_size( Region *r );

#ifdef __cplusplus
}
#endif

#endif // REGION_H__
