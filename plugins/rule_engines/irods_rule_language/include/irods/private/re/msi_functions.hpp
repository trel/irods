/* For copyright information please refer to files in the COPYRIGHT directory
 */

#ifndef MSI_FUNCTIONS_HPP
#define MSI_FUNCTIONS_HPP

#include "irods/irods_hashtable.h"
#include "irods/region.h"

/**
 * @brief Populate function descriptor table with microservice type signatures
 * 
 * Scans the global microservice registry and creates function descriptors
 * for all registered microservices. Uses type metadata from the MSI type registry
 * to generate accurate type signatures with semantic annotations.
 * 
 * Type signatures include parameter semantics:
 *   'e' for expression/input (read-only)
 *   'o' for output (written to, typically modified in place)
 *   'a' for assignable/inout (read and write)
 *   '?' for unknown/dynamic type
 * 
 * Example signatures generated:
 *   "msiDataObjOpen": "e string * e string->int"
 *   "msiDataObjRead": "e int * e int * o bytesBuf_t->int"
 *   "msiExecGenQuery": "e GenQueryInp * o GenQueryOut->int"
 * 
 * Unregistered MSIs fall back to conservative "?" types for all parameters.
 *
 * @param[in,out] ft  Function descriptor hashtable to populate
 * @param[in]     r   Region allocator for function descriptors
 * @return 0 on success, non-zero on error
 */
int getMSIFunctionDescriptors( Hashtable *ft, Region *r );

/**
 * @brief Get the number of MSI functions with registered type information
 * 
 * Returns the count of microservices in the type registry.
 * Can be used to assess coverage of type-safe MSI definitions.
 * 
 * @return Number of MSIs with type metadata
 */
size_t getMSITypeRegistrySize();

/**
 * @brief Check if an MSI has type information in the registry
 * 
 * Useful for debugging and validation to see which MSIs are type-safe.
 * 
 * @param[in] msi_name Name of the microservice
 * @return True if type info is registered, false otherwise
 */
int isMSITypeRegistered( const char* msi_name );

#endif /* MSI_FUNCTIONS_HPP */
