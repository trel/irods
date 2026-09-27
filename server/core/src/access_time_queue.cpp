#include "irods/access_time_queue.hpp"

#include "irods/irods_exception.hpp"
#include "irods/rodsErrorTable.h"

#ifndef IRODS_ACCESS_TIME_QUEUE_FILE_BACKEND
#include <boost/interprocess/ipc/message_queue.hpp>
#endif

#include <fmt/format.h>

#include <fcntl.h>
#include <dirent.h>
#include <signal.h>
#include <sys/stat.h>
#include <unistd.h>

#include <algorithm>
#include <cerrno>
#include <cctype>
#include <chrono>
#include <cstdlib>
#include <memory>
#include <string>

namespace
{
    // On initialization, holds the PID of the process that initialized the access time queue.
    // This ensures that only the process that initialized the system can deinitialize it.
    pid_t g_owner_pid; // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

    // Holds the name of the message queue which will exist in shared memory.
    std::string g_mq_name; // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

#ifndef IRODS_ACCESS_TIME_QUEUE_FILE_BACKEND
    // The pointer to the message queue which will exist in shared memory.
    // Allocating on the heap allows us to know when the message queue is constructed/destructed.
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
    std::unique_ptr<boost::interprocess::message_queue> g_mq;
#else
    std::string g_marker_path;
    std::string g_queue_path;
    std::int32_t g_queue_size;
    off_t g_read_offset;
#endif

    // Returns true if the string starts with an alphabetic character or underscore, optionally followed
    // by one or more alphanumeric characters and/or underscore. The string is expected to contain ASCII
    // characters only. Anything else is considered undefined behavior.
    auto is_queue_name_prefix_valid(const std::string_view _s) -> bool
    {
        if (const auto first_ch = _s[0]; std::isalpha(static_cast<unsigned char>(first_ch)) == 0 && first_ch != '_') {
            return false;
        }

        return std::all_of(std::next(std::begin(_s)), std::end(_s), [](const unsigned char _ch) {
            return _ch == '_' || std::isalnum(_ch);
        });
    } // is_queue_name_prefix_valid

#ifdef IRODS_ACCESS_TIME_QUEUE_FILE_BACKEND
    auto has_inactive_owner_pid(const std::string_view _name, const std::string_view _prefix) noexcept -> bool
    {
        if (!_name.starts_with(_prefix)) {
            return false;
        }

        auto suffix = _name.substr(_prefix.size());
        if (suffix.ends_with(".queue")) {
            suffix.remove_suffix(std::string_view{".queue"}.size());
        }

        const auto separator = suffix.find('_');
        if (separator == std::string_view::npos || separator == 0 || separator == suffix.size() - 1) {
            return false;
        }

        const auto pid_string = suffix.substr(0, separator);
        const auto epoch_string = suffix.substr(separator + 1);
        if (!std::all_of(pid_string.begin(), pid_string.end(), [](const unsigned char _ch) { return std::isdigit(_ch); }) ||
            !std::all_of(epoch_string.begin(), epoch_string.end(), [](const unsigned char _ch) { return std::isdigit(_ch); })) {
            return false;
        }

        const auto pid = static_cast<pid_t>(std::strtol(std::string{pid_string}.c_str(), nullptr, 10));
        if (pid <= 0) {
            return false;
        }

        errno = 0;
        if (::kill(pid, 0) == 0 || errno == EPERM) {
            return false;
        }

        return errno == ESRCH;
    }

    auto remove_access_time_queue_files_in_directory(
        const std::string& _directory, const std::string_view _prefix, const bool _queue_data_files_only) noexcept -> void
    {
        DIR* dir = ::opendir(_directory.c_str());
        if (!dir) {
            return;
        }

        while (const auto* entry = ::readdir(dir)) {
            const std::string_view name{entry->d_name};
            if (!name.starts_with(_prefix)) {
                continue;
            }

            if (_queue_data_files_only && !name.ends_with(".queue")) {
                continue;
            }

            if (!has_inactive_owner_pid(name, _prefix)) {
                continue;
            }

            ::unlink((_directory + '/' + entry->d_name).c_str());
        }

        ::closedir(dir);
    }

    auto remove_stale_access_time_queue_files(const std::string_view _queue_name_prefix) noexcept -> void
    {
        remove_access_time_queue_files_in_directory("/dev/shm", _queue_name_prefix, false);
        remove_access_time_queue_files_in_directory("/tmp", _queue_name_prefix, true);

        if (_queue_name_prefix != "irods_access_time_queue_") {
            remove_access_time_queue_files_in_directory("/dev/shm", "irods_access_time_queue_", false);
            remove_access_time_queue_files_in_directory("/tmp", "irods_access_time_queue_", true);
        }
    }

    auto create_file(const std::string& _path, off_t _size = 0) -> void
    {
        const int fd = ::open(_path.c_str(), O_CREAT | O_TRUNC | O_RDWR, 0600);
        if (fd < 0) {
            THROW(SYS_INTERNAL_ERR, fmt::format("Failed to create access time queue file [{}].", _path));
        }

        if (_size > 0) {
            ::ftruncate(fd, _size);
        }

        ::close(fd);
    }
#endif
} // anonymous namespace

namespace irods::access_time_queue
{
    auto init(const std::string_view _queue_name_prefix, std::int32_t _queue_size) -> void
    {
        if (getpid() == g_owner_pid) {
            return;
        }

        if (_queue_name_prefix.empty()) {
            THROW(CONFIGURATION_ERROR, fmt::format("{}: Access time queue name prefix is empty.", __func__));
        }

        if (!is_queue_name_prefix_valid(_queue_name_prefix)) {
            THROW(CONFIGURATION_ERROR,
                  fmt::format(
                      "{}: Access time queue name prefix violates name requirement: [_a-zA-Z][_a-zA-Z0-9]*", __func__));
        }

        if (_queue_size < 0) {
            THROW(CONFIGURATION_ERROR,
                  fmt::format("{}: Access time queue size [{}] is less than 0.", __func__, _queue_size));
        }

        if (constexpr std::int32_t max = 500'000; _queue_size > max) {
            THROW(CONFIGURATION_ERROR,
                  fmt::format("{}: Access time queue size [{}] is greater than {}.", __func__, _queue_size, max));
        }

        using clock_type = std::chrono::system_clock;
        using seconds = std::chrono::seconds;

        const auto epoch = duration_cast<seconds>(clock_type::now().time_since_epoch()).count();
        g_mq_name = fmt::format("{}{}_{}", _queue_name_prefix, getpid(), epoch);

#ifndef IRODS_ACCESS_TIME_QUEUE_FILE_BACKEND
        boost::interprocess::message_queue::remove(g_mq_name.c_str());

        g_owner_pid = getpid();
        g_mq = std::make_unique<boost::interprocess::message_queue>(
            boost::interprocess::create_only, g_mq_name.data(), _queue_size, sizeof(access_time_data));
#else
        g_owner_pid = getpid();
        g_queue_size = _queue_size;
        g_read_offset = 0;
        g_marker_path = "/dev/shm/" + g_mq_name;
        g_queue_path = "/tmp/" + g_mq_name + ".queue";

        remove_stale_access_time_queue_files(_queue_name_prefix);
        create_file(g_marker_path, static_cast<off_t>(_queue_size) * static_cast<off_t>(sizeof(access_time_data)));
        create_file(g_queue_path);
#endif
    } // init

    auto init_no_create(const std::string_view _queue_name) -> void
    {
        if (_queue_name.empty()) {
            THROW(CONFIGURATION_ERROR, fmt::format("{}: Access time queue name is empty.", __func__));
        }

        g_owner_pid = 0;
        g_mq_name = std::string{_queue_name};
#ifndef IRODS_ACCESS_TIME_QUEUE_FILE_BACKEND
        g_mq = std::make_unique<boost::interprocess::message_queue>(boost::interprocess::open_only, g_mq_name.data());
#else
        g_marker_path = "/dev/shm/" + g_mq_name;
        g_queue_path = "/tmp/" + g_mq_name + ".queue";
        g_read_offset = 0;
#endif
    } // init_no_create

    auto deinit() noexcept -> void
    {
        if (getpid() != g_owner_pid) {
            return;
        }

        try {
            g_owner_pid = 0;

#ifndef IRODS_ACCESS_TIME_QUEUE_FILE_BACKEND
            if (g_mq) {
                g_mq.reset();
            }

            boost::interprocess::message_queue::remove(g_mq_name.c_str());
#endif
        }
        catch (...) {
        }
    } // deinit

    auto shared_memory_name() -> std::string_view
    {
        return g_mq_name;
    } // shared_memory_name

    auto try_enqueue(const access_time_data& _data) -> bool
    {
#ifndef IRODS_ACCESS_TIME_QUEUE_FILE_BACKEND
        return g_mq->try_send(&_data, sizeof(access_time_data), 0);
#else
        if (g_queue_size > 0 && number_of_queued_updates() >= static_cast<std::size_t>(g_queue_size)) {
            return false;
        }

        const int fd = ::open(g_queue_path.c_str(), O_WRONLY | O_APPEND);
        if (fd < 0) {
            return false;
        }

        const auto bytes_written = ::write(fd, &_data, sizeof(_data));
        ::close(fd);

        return bytes_written == static_cast<ssize_t>(sizeof(_data));
#endif
    } // try_enqueue

    auto try_dequeue(access_time_data& _data) -> bool
    {
#ifndef IRODS_ACCESS_TIME_QUEUE_FILE_BACKEND
        // NOLINTNEXTLINE(cppcoreguidelines-init-variables)
        [[maybe_unused]] boost::interprocess::message_queue::size_type recv_size;
        [[maybe_unused]] unsigned int priority; // NOLINT(cppcoreguidelines-init-variables)
        return g_mq->try_receive(&_data, sizeof(access_time_data), recv_size, priority);
#else
        const int fd = ::open(g_queue_path.c_str(), O_RDONLY);
        if (fd < 0) {
            return false;
        }

        if (::lseek(fd, g_read_offset, SEEK_SET) < 0) {
            ::close(fd);
            return false;
        }

        const auto bytes_read = ::read(fd, &_data, sizeof(_data));
        ::close(fd);

        if (bytes_read != static_cast<ssize_t>(sizeof(_data))) {
            return false;
        }

        g_read_offset += sizeof(_data);
        return true;
#endif
    } // try_dequeue

    auto number_of_queued_updates() -> std::size_t
    {
#ifndef IRODS_ACCESS_TIME_QUEUE_FILE_BACKEND
        return g_mq->get_num_msg();
#else
        struct stat st {};
        if (::stat(g_queue_path.c_str(), &st) < 0 || st.st_size <= g_read_offset) {
            return 0;
        }

        return static_cast<std::size_t>(st.st_size - g_read_offset) / sizeof(access_time_data);
#endif
    } // number_of_queued_updates
} // namespace irods::access_time_queue
