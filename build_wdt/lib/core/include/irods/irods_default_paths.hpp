#ifndef IRODS_DEFAULT_PATHS_HPP
#define IRODS_DEFAULT_PATHS_HPP

#include <boost/filesystem/path.hpp>

#define IRODS_DEFAULT_PATH_SBINDIR       "usr/sbin"
#define IRODS_DEFAULT_PATH_LIBDIR        "usr/lib"
#define IRODS_DEFAULT_PATH_SYSCONFDIR    "etc"
#define IRODS_DEFAULT_PATH_RUNSTATEDIR   "var/run"
#define IRODS_DEFAULT_PATH_HOMEDIR       "var/lib/irods"
#define IRODS_DEFAULT_PATH_PLUGINDIR     "usr/lib/irods/plugins"

namespace irods
{
    boost::filesystem::path get_irods_root_directory();

    boost::filesystem::path get_irods_sbin_directory();

    boost::filesystem::path get_irods_lib_directory();

    boost::filesystem::path get_irods_config_directory();

    boost::filesystem::path get_irods_runstate_directory();

    boost::filesystem::path get_irods_home_directory();

    boost::filesystem::path get_irods_default_plugin_directory();

    boost::filesystem::path get_irods_stacktrace_directory();

    boost::filesystem::path get_irods_proc_directory();

    boost::filesystem::path get_irods_msiExecCmd_bin_directory();
} // namespace irods

#endif // IRODS_DEFAULT_PATHS_HPP
