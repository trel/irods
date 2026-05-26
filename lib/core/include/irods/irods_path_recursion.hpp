#ifndef IRODS_PATH_RECURSION_HPP
#define IRODS_PATH_RECURSION_HPP

#include "irods/rodsPath.h"
#include "irods/parseCommandLine.h"

#include <string>
#include <sstream>
#include <map>
#include <iomanip>
#include <chrono>

#include <boost/filesystem.hpp>

namespace irods
{
    /// Maps canonical paths to the original user-supplied paths seen during recursion.
    typedef std::map< std::string, std::string > recursion_map_t;

    /// Checks whether a user path is eligible for recursive traversal.
    ///
    /// Returns `true` if the path names a directory that has not already been
    /// examined. Throws `irods::exception` if a loop or filesystem error is detected.
    bool is_path_valid_for_recursion(boost::filesystem::path const &, recursion_map_t &, bool);

    /// Checks whether a command-line path is eligible for recursive traversal.
    ///
    /// This overload is used when filesystem loop tracking is not needed and only
    /// validates the path, directory status, and symlink handling flags.
    bool is_path_valid_for_recursion( rodsArguments_t const * const, const char * );

    /// Throws `irods::exception` if a filesystem loop is detected.
    void check_for_filesystem_loop(boost::filesystem::path const &,    // Canonical path
                                   boost::filesystem::path const &,    // user path (just for emitting decent exception messages)
                                   recursion_map_t &);

    /// Recursively scans a directory tree for symlinks and filesystem loops.
    int check_directories_for_loops( boost::filesystem::path const &, irods::recursion_map_t &, bool);

    /// Scans all source directories for loops before transferring data.
    ///
    /// Returns `0` on success or a negative rodsError code on failure.
    int scan_all_source_directories_for_loops(irods::recursion_map_t &, const std::vector<std::string>&, bool);

    /// Performs filesystem loop and sanity checks for `irsync` and `iput`.
    int file_system_sanity_check( irods::recursion_map_t &,
                                  rodsArguments_t const * const,
                                  rodsPathInp_t const * const);

    /// Rejects mixed file and directory sources when recursive mode is enabled.
    ///
    /// \param[in] rodsArgs Command-line arguments to inspect.
    /// \param[in] rodsPathInp Input paths to validate.
    int disallow_file_dir_mix_on_command_line( rodsArguments_t const * const rodsArgs,
                                               rodsPathInp_t const * const rodsPathInp );

    /// Environment variable enabling scan-duration output.
    static const char *chrono_env = "IRODS_SCAN_TIME";

    /// Measures elapsed time for recursive path scans.
    class scantime
    {
        public:
            /// Constructs and starts the scan timer.
            explicit scantime();

            /// Destroys the scan timer.
            virtual ~scantime();

            /// Returns the elapsed scan duration as a formatted string.
            /// @return Human-readable duration string.
            std::string get_duration_string() const;
        private:
            std::chrono::time_point<std::chrono::high_resolution_clock> start_; ///< Scan start time.
    };
} // namespace irods

#endif // IRODS_PATH_RECURSION_HPP
