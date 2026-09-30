/* For copyright information please refer to files in the COPYRIGHT directory
 */
#ifndef CACHE_PROTO_HPP
#define CACHE_PROTO_HPP

typedef const void * ( CacheCopyFuncType )( unsigned char *, unsigned char **, unsigned char **, const void *, Hashtable *, int );

// Generic cache copies use a type-erased function pointer. Wrap each concrete
// copy function rather than calling it through an incompatible function type.
template <typename T,
          T* (*Copy)(unsigned char*, unsigned char**, unsigned char**, T*, Hashtable*, int)>
const void* cache_copy_adapter(unsigned char* buffer,
                               unsigned char** position,
                               unsigned char** pointers,
                               const void* source,
                               Hashtable* object_map,
                               int generate_pointer_description)
{
    return Copy(buffer, position, pointers,
                static_cast<T*>(const_cast<void*>(source)),
                object_map, generate_pointer_description);
}

#endif

#define RE_STRUCT_FUNC(T) CONCAT(copy, T)

#define RE_STRUCT_FUNC_TYPE CacheCopyFuncType

#define RE_STRUCT_FUNC_PROTO(T) \
		T* RE_STRUCT_FUNC(T)(unsigned char *buf, unsigned char **p, unsigned char **pointers, T *ptr, Hashtable *objectMap, int generatePtrDesc)

#define RE_STRUCT_FUNC_PROTO_NO_BUF(T) \
		T* RE_STRUCT_FUNC(T)(unsigned char *, unsigned char **p, unsigned char **pointers, T *ptr, Hashtable *objectMap, int generatePtrDesc)

#define RE_STRUCT_FUNC_PROTO_NO_BUF_PTR_DESC(T) \
		T* RE_STRUCT_FUNC(T)(unsigned char *, unsigned char **p, unsigned char **pointers, T *ptr, Hashtable *objectMap, int)

/* #define COPY_FUNC_BEGIN(T) \
	 COPY_FUNC_PROTO(T) {  \
		  allocateInBuffer(T, ecopy, e); */

#define RE_STRUCT_GENERIC_FUNC_PROTO(T, cpfn) \
		T* RE_STRUCT_FUNC(T)(unsigned char *buf, unsigned char **p, unsigned char **pointers, T *ptr, RE_STRUCT_FUNC_TYPE *cpfn, Hashtable *objectMap, int generatePtrDesc)
