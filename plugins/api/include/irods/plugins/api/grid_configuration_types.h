#ifndef IRODS_GRID_CONFIGURATION_TYPES_H
#define IRODS_GRID_CONFIGURATION_TYPES_H

/// \file

/// \brief Input for setting or retrieving a grid configuration value.
typedef struct GridConfigurationInput
{
    char name_space[2700]; ///< Configuration namespace containing the option.
    char option_name[2700]; ///< Configuration option name.
    char option_value[2700]; ///< Configuration option value or input buffer.
} gridConfigurationInp_t;

#define GridConfigurationInp_PI "str name_space[2700]; str option_name[2700]; str option_value[2700];"

/// \brief Output for retrieving a grid configuration value.
typedef struct GridConfigurationOutput
{
    char option_value[2700]; ///< Returned configuration option value.
} gridConfigurationOut_t;

#define GridConfigurationOut_PI "str option_value[2700];"

#endif // IRODS_GRID_CONFIGURATION_TYPES_H
