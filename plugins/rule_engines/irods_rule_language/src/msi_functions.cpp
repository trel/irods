/* For copyright information please refer to files in the COPYRIGHT directory
 */

#include "irods/private/re/msi_functions.hpp"
#include "irods/private/re/msi_type_registry.hpp"
#include "irods/private/re/restructs.hpp"
#include "irods/irods_ms_plugin.hpp"
#include "irods/irods_log.hpp"

#include <cstring>
#include <sstream>

// Forward declaration of the global microservice table accessor
// Note: get_microservice_table() is C++ (returns a C++ type), so no extern "C"
extern irods::ms_table& get_microservice_table();

// Namespace alias for convenience
namespace rl = irods::rule_language;

/**
 * @brief Convert ParamSemantics to rule engine annotation character
 * 
 * The rule engine function signature format supports annotations like:
 *   'e' for expression (read-only)
 *   'f' for function argument (read-only)
 *   'a' for assignable (writable)
 *   'o' for output (written to)
 * 
 * @param semantics The parameter semantics from MSI type info
 * @return Single character annotation, or empty string for unknown
 */
static const char* semanticsToAnnotation( rl::ParamSemantics semantics ) {
    switch ( semantics ) {
        case rl::ParamSemantics::INPUT:
            return "e";    // expression (read-only)
        case rl::ParamSemantics::OUTPUT:
            return "o";    // output (written to)
        case rl::ParamSemantics::INOUT:
            return "a";    // assignable (read and write)
        case rl::ParamSemantics::DYNAMIC:
        default:
            return "";     // no annotation for unknown
    }
}

/**
 * @brief Generate type signature for an MSI function
 * 
 * Converts type metadata from the MSI type registry into a parser-friendly
 * type signature string. If the MSI is not in the registry, falls back to
 * conservative "?" (dynamic type) for all parameters.
 * 
 * Signature format includes semantic annotations:
 *   "e type1 * e type2 * o type3->return_type"
 *   ^-- 'e' means expression (read-only)
 *   ^-- 'o' means output (written)
 * 
 * Examples:
 * - "msiDataObjOpen": "e string * e string->int" (from registry)
 * - "msiDataObjRead": "e int * e int * o bytesBuf_t->int" (from registry)
 * - Unknown MSI: "? * ?->?" (conservative fallback for num_args=2)
 */
static char* getMSITypeSignature( const std::string& msi_name, unsigned int num_args, Region *r ) {
    if ( r == NULL ) {
        return NULL;
    }

    const int max_sig_len = 512;
    char *sig = (char *) region_alloc( r, max_sig_len );
    memset( sig, 0, max_sig_len );

    // Try to get type metadata from registry
    const auto* type_info = rl::getMSITypeInfo( msi_name );

    if ( type_info != NULL && type_info->num_args == num_args ) {
        // Build signature from registry metadata with semantic annotations
        // Format: "[annotation] type1 * [annotation] type2 * ... * [annotation] typeN->return_type"
        char *pos = sig;
        
        for ( unsigned int i = 0; i < type_info->arg_types.size(); ++i ) {
            const char* annot = semanticsToAnnotation( type_info->arg_semantics[i] );
            
            if ( i == 0 ) {
                if ( annot[0] != '\0' ) {
                    pos += snprintf( pos, max_sig_len - (pos - sig), "%s %s", 
                                    annot, type_info->arg_types[i].c_str() );
                }
                else {
                    pos += snprintf( pos, max_sig_len - (pos - sig), "%s", 
                                    type_info->arg_types[i].c_str() );
                }
            }
            else {
                if ( annot[0] != '\0' ) {
                    pos += snprintf( pos, max_sig_len - (pos - sig), " * %s %s", 
                                    annot, type_info->arg_types[i].c_str() );
                }
                else {
                    pos += snprintf( pos, max_sig_len - (pos - sig), " * %s", 
                                    type_info->arg_types[i].c_str() );
                }
            }
        }
        snprintf( pos, max_sig_len - (pos - sig), "->%s", 
                 type_info->return_type.c_str() );
        
        rodsLog( LOG_DEBUG, "getMSITypeSignature: [%s] using registry signature [%s]",
                 msi_name.c_str(), sig );
        return sig;
    }

    // Fallback: conservative "?" types for unknown MSIs
    // This preserves backward compatibility for unregistered microservices
    if ( num_args == 0 ) {
        snprintf( sig, max_sig_len, "->?" );
    }
    else {
        char *pos = sig;
        for ( unsigned int i = 0; i < num_args; ++i ) {
            if ( i == 0 ) {
                pos += snprintf( pos, max_sig_len - (pos - sig), "?" );
            }
            else {
                pos += snprintf( pos, max_sig_len - (pos - sig), " * ?" );
            }
        }
        snprintf( pos, max_sig_len - (pos - sig), "->?" );
    }

    rodsLog( LOG_DEBUG, "getMSITypeSignature: [%s] using fallback signature [%s] (not in registry)",
             msi_name.c_str(), sig );
    return sig;
}

int getMSIFunctionDescriptors( Hashtable *ft, Region *r ) {
    if ( ft == NULL || r == NULL ) {
        return -1;
    }

    try {
        irods::ms_table& micros_table = get_microservice_table();
        
        // Iterate through all registered microservices in the table
        // The lookup_table provides iterator support via begin() and end()
        int count = 0;
        for ( auto it = micros_table.begin(); it != micros_table.end(); ++it ) {
            const std::string& msi_name = it->first;
            irods::ms_table_entry* msi_entry = it->second;
            
            if ( msi_entry == NULL ) {
                continue;
            }

            unsigned int num_args = msi_entry->num_args();
            
            // Generate type signature: use registry if available, fallback to "?" types
            char *type_sig = getMSITypeSignature( msi_name, num_args, r );
            if ( type_sig == NULL ) {
                rodsLog( LOG_WARNING, "getMSIFunctionDescriptors: Failed to generate signature for [%s]", 
                         msi_name.c_str() );
                continue;
            }

            // Create function descriptor for this MSI
            // Note: We pass NULL for the function pointer since MSI calls are handled
            // differently at runtime (through the microservice table, not direct calls)
            FunctionDesc *fd = newFunctionFD( type_sig, NULL, r );
            if ( fd == NULL ) {
                rodsLog( LOG_WARNING, "getMSIFunctionDescriptors: Failed to create descriptor for [%s]", 
                         msi_name.c_str() );
                continue;
            }

            // Insert into function descriptor table
            insertIntoHashTable( ft, (char*) msi_name.c_str(), fd );
            count++;

            rodsLog( LOG_DEBUG, "getMSIFunctionDescriptors: Added [%s] with signature [%s]",
                     msi_name.c_str(), type_sig );
        }

        rodsLog( LOG_DEBUG, "getMSIFunctionDescriptors: Registered %d microservice descriptors", count );
        return 0;
    }
    catch ( const std::exception& e ) {
        rodsLog( LOG_ERROR, "getMSIFunctionDescriptors: Exception - %s", e.what() );
        return -1;
    }
    catch ( ... ) {
        rodsLog( LOG_ERROR, "getMSIFunctionDescriptors: Unknown exception" );
        return -1;
    }
}

size_t getMSITypeRegistrySize() {
    return rl::getMSITypeRegistrySize();
}

int isMSITypeRegistered( const char* msi_name ) {
    if ( msi_name == NULL ) {
        return 0;
    }
    const auto* type_info = rl::getMSITypeInfo( std::string( msi_name ) );
    return (type_info != NULL) ? 1 : 0;
}
