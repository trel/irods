/* For copyright information please refer to files in the COPYRIGHT directory
 */

#include "irods/private/re/msi_type_registry.hpp"
#include <unordered_map>

namespace irods {
namespace rule_language {

/**
 * @brief Global MSI type registry
 * 
 * This registry maps MSI function names to their type metadata.
 * Entries are populated statically during module initialization.
 * 
 * Type Notation:
 *   - string: text data
 *   - int: integer value
 *   - float/double: floating point
 *   - KeyValPair: key-value pair structure
 *   - GenQueryInp: genQuery input structure
 *   - GenQueryOut: genQuery output structure
 *   - dataObjInfo_t: data object metadata
 *   - rodsObjStat_t: iRODS object stat structure
 *   - bytesBuf_t: byte buffer
 *   - T_*: iRODS type constants
 *   - ?: dynamic/unknown type (conservative fallback)
 *   - type?: nullable variant
 *   - (type1|type2): union type
 */
static const std::map<std::string, MSITypeInfo> msi_type_registry = {
    // ==============================================
    // Core Microservices with Known Type Signatures
    // ==============================================
    
    {
        "msiDataObjOpen",
        {
            "msiDataObjOpen",
            2,
            {"string", "string"},           // path, open_flags
            {ParamSemantics::INPUT, ParamSemantics::INPUT},
            "int",                          // returns file descriptor
            ReturnTypeSemantics::DATA       // Return value is meaningful data (fd)
        }
    },
    
    {
        "msiDataObjClose",
        {
            "msiDataObjClose",
            2,
            {"int", "int"},                 // fd (from msiDataObjOpen), unknown flags
            {ParamSemantics::INPUT, ParamSemantics::INPUT},
            "int",
            ReturnTypeSemantics::STATUS
        }
    },
    
    {
        "msiDataObjRead",
        {
            "msiDataObjRead",
            3,
            {"int", "int", "bytesBuf_t"},  // fd, size, buffer (output)
            {ParamSemantics::INPUT, ParamSemantics::INPUT, ParamSemantics::OUTPUT},
            "int",
            ReturnTypeSemantics::STATUS
        }
    },
    
    {
        "msiDataObjWrite",
        {
            "msiDataObjWrite",
            3,
            {"int", "bytesBuf_t", "int"},  // fd, buffer, size
            {ParamSemantics::INPUT, ParamSemantics::INPUT, ParamSemantics::INPUT},
            "int",
            ReturnTypeSemantics::STATUS
        }
    },
    
    {
        "msiDataObjCreate",
        {
            "msiDataObjCreate",
            3,
            {"string", "string", "string"}, // path, type, resource_name
            {ParamSemantics::INPUT, ParamSemantics::INPUT, ParamSemantics::INPUT},
            "int",
            ReturnTypeSemantics::STATUS
        }
    },
    
    {
        "msiDataObjUnlink",
        {
            "msiDataObjUnlink",
            2,
            {"string", "string"},           // path, flags
            {ParamSemantics::INPUT, ParamSemantics::INPUT},
            "int",
            ReturnTypeSemantics::STATUS
        }
    },
    
    {
        "msiExecGenQuery",
        {
            "msiExecGenQuery",
            2,
            {"GenQueryInp", "GenQueryOut"},  // query_input, query_output (output)
            {ParamSemantics::INPUT, ParamSemantics::OUTPUT},
            "int",
            ReturnTypeSemantics::STATUS
        }
    },
    
    {
        "msiMakeGenQuery",
        {
            "msiMakeGenQuery",
            3,
            {"string", "string", "GenQueryInp"}, // select_list, conditions, query_input (output)
            {ParamSemantics::INPUT, ParamSemantics::INPUT, ParamSemantics::OUTPUT},
            "int",
            ReturnTypeSemantics::STATUS
        }
    },
    
    {
        "msiGetMoreRows",
        {
            "msiGetMoreRows",
            3,
            {"GenQueryInp", "GenQueryOut", "int"}, // query_input, query_output (output), cont_index (output)
            {ParamSemantics::INOUT, ParamSemantics::OUTPUT, ParamSemantics::OUTPUT},
            "int",
            ReturnTypeSemantics::STATUS
        }
    },
    
    {
        "msiCloseGenQuery",
        {
            "msiCloseGenQuery",
            2,
            {"GenQueryInp", "GenQueryOut"},  // query_input, query_output
            {ParamSemantics::INPUT, ParamSemantics::INPUT},
            "int",
            ReturnTypeSemantics::STATUS
        }
    },
    
    {
        "msiAddKeyVal",
        {
            "msiAddKeyVal",
            3,
            {"KeyValPair", "string", "string"}, // kvp, key, value
            {ParamSemantics::INOUT, ParamSemantics::INPUT, ParamSemantics::INPUT},
            "int",
            ReturnTypeSemantics::STATUS
        }
    },
    
    {
        "msiGetValByKey",
        {
            "msiGetValByKey",
            3,
            {"KeyValPair", "string", "string"}, // kvp, key, value (output)
            {ParamSemantics::INPUT, ParamSemantics::INPUT, ParamSemantics::OUTPUT},
            "int",
            ReturnTypeSemantics::STATUS
        }
    },
    
    {
        "msiObjStat",
        {
            "msiObjStat",
            2,
            {"string", "rodsObjStat_t"},    // path, stat_output (output)
            {ParamSemantics::INPUT, ParamSemantics::OUTPUT},
            "int",
            ReturnTypeSemantics::STATUS
        }
    },
    
    {
        "msiGetObjType",
        {
            "msiGetObjType",
            2,
            {"string", "string"},           // path, type (output: 'dataObj' or 'collection')
            {ParamSemantics::INPUT, ParamSemantics::OUTPUT},
            "int",
            ReturnTypeSemantics::STATUS
        }
    },
    
    {
        "msiSetACL",
        {
            "msiSetACL",
            4,
            {"string", "string", "string", "string"}, // recursive_flag, access_level, user_name, path
            {ParamSemantics::INPUT, ParamSemantics::INPUT, ParamSemantics::INPUT, ParamSemantics::INPUT},
            "int",
            ReturnTypeSemantics::STATUS
        }
    },
    
    {
        "msiCheckPermission",
        {
            "msiCheckPermission",
            1,
            {"string"},                     // permission
            {ParamSemantics::INPUT},
            "int",
            ReturnTypeSemantics::STATUS
        }
    },
    
    {
        "msiSleep",
        {
            "msiSleep",
            2,
            {"int", "int"},                 // seconds, microseconds
            {ParamSemantics::INPUT, ParamSemantics::INPUT},
            "int",
            ReturnTypeSemantics::STATUS
        }
    },
    
    {
        "msiGetSystemTime",
        {
            "msiGetSystemTime",
            2,
            {"string", "string"},           // timestamp (output), format
            {ParamSemantics::OUTPUT, ParamSemantics::INPUT},
            "int",
            ReturnTypeSemantics::STATUS
        }
    },
    
    {
        "msiGetFormattedSystemTime",
        {
            "msiGetFormattedSystemTime",
            3,
            {"string", "string", "string"}, // timestamp (output), format_string, timezone
            {ParamSemantics::OUTPUT, ParamSemantics::INPUT, ParamSemantics::INPUT},
            "int",
            ReturnTypeSemantics::STATUS
        }
    },
    
    {
        "msiModAVUMetadata",
        {
            "msiModAVUMetadata",
            6,
            {"string", "string", "string", "string", "string", "string"}, // item_type, item_name, op, attr, value, unit
            {ParamSemantics::INPUT, ParamSemantics::INPUT, ParamSemantics::INPUT,
             ParamSemantics::INPUT, ParamSemantics::INPUT, ParamSemantics::INPUT},
            "int",
            ReturnTypeSemantics::STATUS
        }
    },
    
    // ==============================================
    // Microservices with Partially Known Types
    // (Use ? for unknown parameter types)
    // ==============================================
    
    {
        "msiExecCmd",
        {
            "msiExecCmd",
            6,
            {"string", "string", "string", "?", "?", "?"}, // cmd, args, stdin, ... other params
            {ParamSemantics::INPUT, ParamSemantics::INPUT, ParamSemantics::INPUT,
             ParamSemantics::DYNAMIC, ParamSemantics::DYNAMIC, ParamSemantics::DYNAMIC},
            "int",
            ReturnTypeSemantics::STATUS
        }
    },
    
    {
        "msiApplyDCMetadataTemplate",
        {
            "msiApplyDCMetadataTemplate",
            2,
            {"?", "?"},                     // template_input, output
            {ParamSemantics::INPUT, ParamSemantics::OUTPUT},
            "int",
            ReturnTypeSemantics::STATUS
        }
    },
    
    // ==============================================
    // Collection Operations
    // ==============================================
    
    {
        "msiCollCreate",
        {
            "msiCollCreate",
            3,
            {"string", "string", "string"},  // parent_path, coll_name, options
            {ParamSemantics::INPUT, ParamSemantics::INPUT, ParamSemantics::INPUT},
            "int",
            ReturnTypeSemantics::STATUS
        }
    },
    
    {
        "msiRmColl",
        {
            "msiRmColl",
            3,
            {"string", "?", "int"},          // path, force, null
            {ParamSemantics::INPUT, ParamSemantics::DYNAMIC, ParamSemantics::INPUT},
            "int",
            ReturnTypeSemantics::STATUS
        }
    },
    
    // ==============================================
    // Replication & Sync
    // ==============================================
    
    {
        "msiDataObjRepl",
        {
            "msiDataObjRepl",
            3,
            {"string", "string", "string"},  // path, dest_resc, copy_mode
            {ParamSemantics::INPUT, ParamSemantics::INPUT, ParamSemantics::INPUT},
            "int",
            ReturnTypeSemantics::STATUS
        }
    },
    
    {
        "msiDataObjRsync",
        {
            "msiDataObjRsync",
            5,
            {"string", "string", "string", "?", "?"}, // from_path, to_path, mode, ...
            {ParamSemantics::INPUT, ParamSemantics::INPUT, ParamSemantics::INPUT,
             ParamSemantics::DYNAMIC, ParamSemantics::DYNAMIC},
            "int",
            ReturnTypeSemantics::STATUS
        }
    },
    
    {
        "msiCollRsync",
        {
            "msiCollRsync",
            5,
            {"string", "string", "string", "?", "?"}, // from_coll, to_coll, mode, ...
            {ParamSemantics::INPUT, ParamSemantics::INPUT, ParamSemantics::INPUT,
             ParamSemantics::DYNAMIC, ParamSemantics::DYNAMIC},
            "int",
            ReturnTypeSemantics::STATUS
        }
    },
    
    // ==============================================
    // Time Operations
    // ==============================================
    
    {
        "msiGetDiffTime",
        {
            "msiGetDiffTime",
            4,
            {"string", "string", "string", "int"},  // time1, time2, format, diff (output)
            {ParamSemantics::INPUT, ParamSemantics::INPUT, ParamSemantics::INPUT, ParamSemantics::OUTPUT},
            "int",
            ReturnTypeSemantics::STATUS
        }
    },
    
    {
        "msiHumanToSystemTime",
        {
            "msiHumanToSystemTime",
            2,
            {"string", "string"},           // human_readable, sys_time (output)
            {ParamSemantics::INPUT, ParamSemantics::OUTPUT},
            "int",
            ReturnTypeSemantics::STATUS
        }
    },
    
    // ==============================================
    // Access Control & Permissions
    // ==============================================
    
    {
        "msiCheckAccess",
        {
            "msiCheckAccess",
            3,
            {"string", "string", "int"},    // path, operation, result (output: 0=no, 1=yes)
            {ParamSemantics::INPUT, ParamSemantics::INPUT, ParamSemantics::OUTPUT},
            "int",
            ReturnTypeSemantics::STATUS
        }
    },
    
    {
        "msiSetDefaultResc",
        {
            "msiSetDefaultResc",
            2,
            {"string", "?"},                // resource_name, unknown
            {ParamSemantics::INPUT, ParamSemantics::DYNAMIC},
            "int",
            ReturnTypeSemantics::STATUS
        }
    },
    
    // ==============================================
    // String/Buffer Operations
    // ==============================================
    
    {
        "writeBytesBuf",
        {
            "writeBytesBuf",
            2,
            {"?", "bytesBuf_t"},            // output_stream, buffer
            {ParamSemantics::DYNAMIC, ParamSemantics::INPUT},
            "int",
            ReturnTypeSemantics::STATUS
        }
    },
    
    {
        "msiStrToBytesBuf",
        {
            "msiStrToBytesBuf",
            2,
            {"string", "bytesBuf_t"},       // input_str, output_buf
            {ParamSemantics::INPUT, ParamSemantics::OUTPUT},
            "int",
            ReturnTypeSemantics::STATUS
        }
    },
    
    {
        "msiBytesBufToStr",
        {
            "msiBytesBufToStr",
            2,
            {"bytesBuf_t", "string"},       // input_buf, output_str
            {ParamSemantics::INPUT, ParamSemantics::OUTPUT},
            "int",
            ReturnTypeSemantics::STATUS
        }
    },
    
    {
        "msiString2KeyValPair",
        {
            "msiString2KeyValPair",
            2,
            {"string", "KeyValPair"},       // input_str, kvp (output)
            {ParamSemantics::INPUT, ParamSemantics::OUTPUT},
            "int",
            ReturnTypeSemantics::STATUS
        }
    },
    
    // ==============================================
    // Execution & System
    // ==============================================
    
    {
        "delayExec",
        {
            "delayExec",
            3,
            {"string", "string", "string"}, // delay_condition, action, recovery
            {ParamSemantics::INPUT, ParamSemantics::INPUT, ParamSemantics::INPUT},
            "int",
            ReturnTypeSemantics::STATUS
        }
    },
    
    {
        "remoteExec",
        {
            "remoteExec",
            4,
            {"string", "string", "string", "string"}, // host, condition, action, recovery
            {ParamSemantics::INPUT, ParamSemantics::INPUT, ParamSemantics::INPUT, ParamSemantics::INPUT},
            "int",
            ReturnTypeSemantics::STATUS
        }
    },
    
    {
        "msiExecCmd",
        {
            "msiExecCmd",
            6,
            {"string", "string", "string", "?", "?", "?"}, // cmd, args, stdin, ...
            {ParamSemantics::INPUT, ParamSemantics::INPUT, ParamSemantics::INPUT,
             ParamSemantics::DYNAMIC, ParamSemantics::DYNAMIC, ParamSemantics::DYNAMIC},
            "int",
            ReturnTypeSemantics::STATUS
        }
    },
    
    // ==============================================
    // Query Extension
    // ==============================================
    
    {
        "msiAddSelectFieldToGenQuery",
        {
            "msiAddSelectFieldToGenQuery",
            3,
            {"string", "string", "GenQueryInp"},  // select_field, function, query_input
            {ParamSemantics::INPUT, ParamSemantics::INPUT, ParamSemantics::INOUT},
            "int",
            ReturnTypeSemantics::STATUS
        }
    },
    
    {
        "msiAddConditionToGenQuery",
        {
            "msiAddConditionToGenQuery",
            4,
            {"string", "string", "string", "GenQueryInp"},  // attribute, operator, value, query_input
            {ParamSemantics::INPUT, ParamSemantics::INPUT, ParamSemantics::INPUT, ParamSemantics::INOUT},
            "int",
            ReturnTypeSemantics::STATUS
        }
    },
    
    {
        "msiPrintGenQueryOutToBuffer",
        {
            "msiPrintGenQueryOutToBuffer",
            3,
            {"GenQueryOut", "string", "?"},  // query_output, format, buffer
            {ParamSemantics::INPUT, ParamSemantics::INPUT, ParamSemantics::OUTPUT},
            "int",
            ReturnTypeSemantics::STATUS
        }
    },
    
    {
        "msiGetContInxFromGenQueryOut",
        {
            "msiGetContInxFromGenQueryOut",
            2,
            {"GenQueryOut", "int"},         // query_output, cont_index (output)
            {ParamSemantics::INPUT, ParamSemantics::OUTPUT},
            "int",
            ReturnTypeSemantics::STATUS
        }
    },
    
    // ==============================================
    // TODO: Add remaining MSIs with conservative '?' types
    // Complete coverage would require analyzing each MSI's semantics
    // ==============================================
};

/**
 * @brief Mapping of MSI names to their inferred output types
 * 
 * For MSIs that produce specific semantic output types (not just status codes),
 * this map captures what type the result should be inferred as.
 */
static const std::unordered_map<std::string, std::string> msi_inferred_output_types = {
    // Query operations return GenQueryOut in arg 1
    {"msiExecGenQuery", "GenQueryOut"},
    {"msiMakeGenQuery", "GenQueryInp"},
    
    // Time operations return string in arg 0
    {"msiGetSystemTime", "string"},
    {"msiGetFormattedSystemTime", "string"},
    {"msiHumanToSystemTime", "string"},
    
    // Key-value operations return KeyValPair/string
    {"msiString2KeyValPair", "KeyValPair"},
    {"msiGetValByKey", "string"},
    
    // Object stat operations return rodsObjStat_t
    {"msiObjStat", "rodsObjStat_t"},
    
    // Object type returns string
    {"msiGetObjType", "string"},
    
    // Conversion operations
    {"msiBytesBufToStr", "string"},
    {"msiStrToBytesBuf", "bytesBuf_t"},
    
    // Default for all others: int (status code)
};

static const std::string UNKNOWN_TYPE = "?";

const MSITypeInfo* getMSITypeInfo(const std::string& msi_name) {
    auto it = msi_type_registry.find(msi_name);
    if (it != msi_type_registry.end()) {
        return &it->second;
    }
    return nullptr;
}

const std::map<std::string, MSITypeInfo>& getMSITypeRegistry() {
    return msi_type_registry;
}

size_t getMSITypeRegistrySize() {
    return msi_type_registry.size();
}

const std::string& getMSIInferredOutputType( const std::string& msi_name, unsigned int arg_index ) {
    auto it = msi_inferred_output_types.find( msi_name );
    if ( it != msi_inferred_output_types.end() ) {
        return it->second;
    }
    return UNKNOWN_TYPE;
}

} // namespace rule_language
} // namespace irods
