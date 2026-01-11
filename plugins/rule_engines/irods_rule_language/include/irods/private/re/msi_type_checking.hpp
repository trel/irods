/* For copyright information please refer to files in the COPYRIGHT directory
 */

#ifndef MSI_TYPE_CHECKING_HPP
#define MSI_TYPE_CHECKING_HPP

/**
 * @file msi_type_checking.hpp
 * @brief Configuration and control for MSI type safety checking mode
 * 
 * Provides a flexible system for controlling how strictly MSI argument types
 * are validated during rule execution.
 */

/**
 * @enum MSITypeCheckingMode
 * @brief Controls behavior of MSI type validation
 */
enum class MSITypeCheckingMode {
    PERMISSIVE,    ///< Log warnings but allow execution (default)
    STRICT,        ///< Reject type mismatches, prevent execution
    WARN_ONLY,     ///< Log warnings, allow execution (explicit)
    DISABLED       ///< Skip type checking entirely (performance mode)
};

/**
 * @brief Initialize MSI type checking configuration
 * 
 * Reads configuration from rule engine settings and sets up
 * the global type checking mode.
 * 
 * @return 0 on success, non-zero on error
 */
int initMSITypeChecking();

/**
 * @brief Get current MSI type checking mode
 * 
 * @return Current checking mode
 */
MSITypeCheckingMode getMSITypeCheckingMode();

/**
 * @brief Set MSI type checking mode
 * 
 * @param[in] mode The new checking mode to use
 */
void setMSITypeCheckingMode( MSITypeCheckingMode mode );

/**
 * @brief Check if strict MSI type checking is enabled
 * 
 * @return 1 if strict mode, 0 otherwise
 */
int isMSITypeCheckingStrict();

/**
 * @brief Check if MSI type checking is disabled
 * 
 * @return 1 if disabled, 0 otherwise
 */
int isMSITypeCheckingDisabled();

/**
 * @brief Get descriptive name for a type checking mode
 * 
 * @param[in] mode The mode to get the name for
 * @return String name of the mode (e.g., "PERMISSIVE")
 */
const char* getMSITypeCheckingModeName( MSITypeCheckingMode mode );

/**
 * @brief Get total count of type checking errors encountered
 * 
 * Useful for reporting statistics about type safety during compilation.
 * 
 * @return Number of type mismatches detected
 */
unsigned int getMSITypeCheckErrorCount();

/**
 * @brief Reset type checking error counter
 * 
 * Called at the start of rule compilation.
 */
void resetMSITypeCheckErrorCount();

/**
 * @brief Increment type checking error counter
 * 
 * Called when a type mismatch is detected.
 */
void incrementMSITypeCheckErrorCount();

#endif /* MSI_TYPE_CHECKING_HPP */
