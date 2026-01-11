/* For copyright information please refer to files in the COPYRIGHT directory
 */

#include "irods/private/re/msi_type_checking.hpp"
#include "irods/irods_log.hpp"
#include <atomic>

/**
 * @brief Global MSI type checking configuration
 * 
 * Thread-safe storage of the current type checking mode.
 * Defaults to PERMISSIVE (backward compatible).
 */
static std::atomic<MSITypeCheckingMode> g_msi_type_checking_mode(MSITypeCheckingMode::PERMISSIVE);

/**
 * @brief Global counter of type checking errors
 * 
 * Incremented each time a type mismatch is detected.
 * Used for reporting and statistics.
 */
static std::atomic<unsigned int> g_msi_type_check_error_count(0);

int initMSITypeChecking() {
    // Read from configuration file if available
    // For now, default to PERMISSIVE mode
    // Future: integrate with rule engine configuration system
    
    g_msi_type_checking_mode = MSITypeCheckingMode::PERMISSIVE;
    g_msi_type_check_error_count = 0;
    
    rodsLog( LOG_DEBUG, "MSI type checking initialized: mode=%s",
             getMSITypeCheckingModeName( g_msi_type_checking_mode ) );
    
    return 0;
}

MSITypeCheckingMode getMSITypeCheckingMode() {
    return g_msi_type_checking_mode;
}

void setMSITypeCheckingMode( MSITypeCheckingMode mode ) {
    MSITypeCheckingMode old_mode = g_msi_type_checking_mode;
    g_msi_type_checking_mode = mode;
    
    rodsLog( LOG_DEBUG, "MSI type checking mode changed: %s -> %s",
             getMSITypeCheckingModeName( old_mode ),
             getMSITypeCheckingModeName( mode ) );
}

int isMSITypeCheckingStrict() {
    return (g_msi_type_checking_mode == MSITypeCheckingMode::STRICT) ? 1 : 0;
}

int isMSITypeCheckingDisabled() {
    return (g_msi_type_checking_mode == MSITypeCheckingMode::DISABLED) ? 1 : 0;
}

const char* getMSITypeCheckingModeName( MSITypeCheckingMode mode ) {
    switch ( mode ) {
        case MSITypeCheckingMode::PERMISSIVE:
            return "PERMISSIVE";
        case MSITypeCheckingMode::STRICT:
            return "STRICT";
        case MSITypeCheckingMode::WARN_ONLY:
            return "WARN_ONLY";
        case MSITypeCheckingMode::DISABLED:
            return "DISABLED";
        default:
            return "UNKNOWN";
    }
}

unsigned int getMSITypeCheckErrorCount() {
    return g_msi_type_check_error_count;
}

void resetMSITypeCheckErrorCount() {
    g_msi_type_check_error_count = 0;
}

void incrementMSITypeCheckErrorCount() {
    ++g_msi_type_check_error_count;
}
