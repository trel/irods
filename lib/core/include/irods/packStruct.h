#ifndef PACK_STRUCT_H__
#define PACK_STRUCT_H__

#include "irods/rodsDef.h"

/** Maximum packing-instruction string length. */
#define MAX_PI_LEN                      1024
/** Flag indicating a trailing semicolon was parsed. */
#define SEMI_COL_FLAG                   0x2
/** Initial allocation size for packed output buffers. */
#define PACKED_OUT_ALLOC_SZ             (16 * 1024)
/** Initial allocation size when unpacking a sub-structure. */
#define SUB_STRUCT_ALLOC_SZ             1024
/** Maximum packed output allocation size. */
#define MAX_PACKED_OUT_ALLOC_SZ         (1024 * 1024)
/** Sentinel string representing a packed null pointer. */
#define NULL_PTR_PACK_STR               "%@#ANULLSTR$%"

// definition for the flag in packXmlTag()
/** Emit a start tag while packing XML. */
#define START_TAG_FL                    0
/** Emit an end tag while packing XML. */
#define END_TAG_FL                      1
/** Emit a line feed while packing XML. */
#define LF_FL                           2

// indicate the end of packing table
#define PACK_TABLE_END_PI               "PACK_TABLE_END_PI" /**< Sentinel marking the end of a pack table. */

#define XML_TAG                         "iRODSStruct" /**< Top-level XML tag for packed structures. */

/** Entry describing a named packing instruction. */
typedef struct PackingInstruction {
    const char *name;                 ///< Name of the instruction.
    const char *packInstruct;         ///< Packing instruction string.
    void (*clearInStruct)(void*);     ///< Cleanup callback for unpacked structs.
} packInstruct_t;

/** Named integer constant referenced by pack instructions. */
typedef struct PackingConstant {
    char *name; ///< Constant name.
    int value;  ///< Constant value.
} packConstant_t;

/** Primitive and composite packing type identifiers. */
typedef enum PackingTypeIndex {
    PACK_CHAR_TYPE,          ///< Character data.
    PACK_BIN_TYPE,           ///< Binary data.
    PACK_STR_TYPE,           ///< Null-terminated string data.
    PACK_PI_STR_TYPE,        ///< Packing-instruction string data.
    PACK_INT_TYPE,           ///< Integer data.
    PACK_DOUBLE_TYPE,        ///< Double-precision floating-point data.
    PACK_STRUCT_TYPE,        ///< Nested structure data.
    PACK_DEPENDENT_TYPE,     ///< Type selected by a dependency.
    PACK_INT_DEPENDENT_TYPE, ///< Integer-selected dependent type.
    PACK_INT16_TYPE          ///< 16-bit integer data.
} packTypeInx_t;

/** Operation mode used by packing helpers. */
typedef enum PackingOperation {
    PACK_OPR,   ///< Serialize a structure.
    UNPACK_OPR  ///< Deserialize a structure.
} packOpr_t;

/** Metadata describing a supported packed type. */
typedef struct PackingType {
    char *name;                ///< Name of the type.
    packTypeInx_t  number;     ///< Numeric type identifier.
    int size;                  ///< Size of the type in bytes.
} packType_t;

/** Maximum number of dimensions supported by packed arrays. */
#define MAX_PACK_DIM    20

// definition for pointerType
/** Item is not represented as a pointer. */
#define NON_POINTER     0
/** Item is represented as a pointer. */
#define A_POINTER       1
/** Item is a pointer that must not be freed. */
#define NO_FREE_POINTER 2
/** Item is a pointer that must not be packed. */
#define NO_PACK_POINTER 3

// definition for packFlag
/** Free pointer-valued input after packing. */
#define FREE_POINTER    0x1

/** Parsed representation of a single packing instruction item. */
typedef struct packItem {
    packTypeInx_t typeInx;          ///< Type of the packed item.
    char *name;                     ///< Instruction fragment for the item.
    int pointerType;                ///< Pointer classification flag.
    const void *pointer;            ///< Pointer value when applicable.
    int intValue;                   ///< Integer value for integer items.
    char strValue[NAME_LEN];        ///< String value for string items.
    int dim;                        ///< Number of array dimensions.
    int dimSize[MAX_PACK_DIM];      ///< Size of each array dimension.
    int hintDim;                    ///< Number of hint dimensions.
    int hintDimSize[MAX_PACK_DIM];  ///< Size of each hint dimension.
    const struct packItem *parent;  ///< Parent item in the parse tree.
    struct packItem *prev;          ///< Previous sibling item.
    struct packItem *next;          ///< Next sibling item.
} packItem_t;

/** Array of byte buffers used for unpacked passthrough content. */
typedef struct BytesBufferArray {
    int numBuf;             ///< Number of buffers in the array.
    bytesBuf_t* bBufArray;  ///< Pointer to an array of `bytesBuf_t` values.
} bytesBufArray_t;

/** Aggregate output produced by structure packing. */
typedef struct PackedOutput {
    bytesBuf_t bBuf;                ///< Packed output buffer.
    int bufSize;                    ///< Capacity of `bBuf` in bytes.
    bytesBufArray_t nopackBufArray; ///< Byte buffers carried without packing.
} packedOutput_t;

#ifdef __cplusplus
extern "C" {
#endif

__attribute__((deprecated("Use pack_struct")))
/** Packs a structure using the legacy packing API. */
int packStruct(const void *inStruct,
               bytesBuf_t **packedResult,
               const char *packInstName,
               const packInstruct_t *myPackTable,
               int packFlag,
               irodsProt_t irodsProt);

__attribute__((deprecated("Use unpack_struct")))
/** Unpacks a structure using the legacy packing API. */
int unpackStruct(const void *inPackStr,
                 void **outStruct,
                 const char *packInstName,
                 const packInstruct_t *myPackTable,
                 irodsProt_t irodsProt);

/** Packs a structure according to a named packing instruction. */
int pack_struct(const void *inStruct,
                 bytesBuf_t **packedResult,
                 const char *packInstName,
                 const packInstruct_t *myPackTable,
                 int packFlag,
                 irodsProt_t irodsProt,
                 const char* peer_version);

/** Unpacks a structure according to a named packing instruction. */
int unpack_struct(const void *inPackStr,
                   void **outStruct,
                   const char *packInstName,
                   const packInstruct_t *myPackTable,
                   irodsProt_t irodsProt,
                   const char* peer_version);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // PACK_STRUCT_H__
