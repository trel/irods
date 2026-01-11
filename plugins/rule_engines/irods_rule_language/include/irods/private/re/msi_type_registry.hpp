/* For copyright information please refer to files in the COPYRIGHT directory
 */

#ifndef MSI_TYPE_REGISTRY_HPP
#define MSI_TYPE_REGISTRY_HPP

#include <string>
#include <vector>
#include <map>

/**
 * @file msi_type_registry.hpp
 * @brief Type metadata for microservice interface (MSI) functions
 * 
 * This module provides compile-time type information for all registered
 * microservices. Instead of treating all MSI parameters as dynamic type (?),
 * we can now express real type constraints for better compile-time checking.
 */

namespace irods {
namespace rule_language {

/**
 * @enum ParamSemantics
 * @brief Indicates how a parameter is used (input, output, or both)
 */
enum class ParamSemantics {
    INPUT,    ///< Parameter is read (input-only)
    OUTPUT,   ///< Parameter is written (output-only, typically modified in place)
    INOUT,    ///< Parameter is both read and written
    DYNAMIC   ///< Unknown semantics (conservative default)
};

/**
 * @enum ReturnTypeSemantics
 * @brief Indicates what the return value represents
 */
enum class ReturnTypeSemantics {
    STATUS,    ///< Standard iRODS status code (int) - semantic value is success/error
    DATA,      ///< Meaningful data returned (string, int, etc.)
    IGNORE,    ///< Return value should be ignored (typically status)
    DYNAMIC    ///< Unknown return type semantics
};

/**
 * @struct MSITypeInfo
 * @brief Type metadata for a single microservice function
 */
struct MSITypeInfo {
    std::string name;                          ///< MSI function name (e.g., "msiDataObjOpen")
    unsigned int num_args;                     ///< Number of arguments
    std::vector<std::string> arg_types;        ///< Type signature for each argument
    std::vector<ParamSemantics> arg_semantics; ///< Semantics for each argument
    std::string return_type;                   ///< Return type signature (int, string, etc.)
    ReturnTypeSemantics return_semantics;      ///< What the return value means
};

/**
 * @brief Get type information for a specific microservice
 * 
 * Looks up the registered type metadata for an MSI function by name.
 * If the MSI is not found in the registry, returns nullptr.
 * 
 * @param[in] msi_name  Name of the microservice (e.g., "msiDataObjOpen")
 * @return Pointer to MSITypeInfo if found, nullptr otherwise
 */
const MSITypeInfo* getMSITypeInfo(const std::string& msi_name);

/**
 * @brief Get all registered MSI type information
 * 
 * Returns a reference to the complete MSI type registry map.
 * 
 * @return Reference to map of MSI name -> MSITypeInfo
 */
const std::map<std::string, MSITypeInfo>& getMSITypeRegistry();

/**
 * @brief Get number of registered MSI type entries
 * 
 * @return Count of MSI functions with type metadata
 */
size_t getMSITypeRegistrySize();

/**
 * @brief Infer semantic output type for an MSI function call
 * 
 * For MSI functions that produce meaningful output (not just status codes),
 * this function returns the semantic type of the result. For example:
 *   - msiGetSystemTime returns string (the time value)
 *   - msiExecGenQuery returns GenQueryOut (query results)
 *   - msiDataObjOpen returns int (file descriptor)
 * 
 * @param[in] msi_name Name of the microservice
 * @param[in] arg_index For output-parameter MSIs, which argument index is the output
 * @return Semantic output type string, or "?" if unknown
 */
const std::string& getMSIInferredOutputType( const std::string& msi_name, unsigned int arg_index = 0 );

} // namespace rule_language
} // namespace irods

#endif /* MSI_TYPE_REGISTRY_HPP */
