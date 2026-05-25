#ifndef IRODS_LOGGER_HPP
#define IRODS_LOGGER_HPP

/// \file
/// \brief The iRODS Logging library
///
/// This library is designed for server-side use only. It is tightly coupled to the server and
/// makes use of features such as syslog and shared memory. Attempting to use this library in client-side
/// code is highly discouraged.
///
/// To use it, the following requirements must be satisfied.
/// - Define the macro, \p IRODS_ENABLE_SYSLOG, before including the header file
/// - Link against \p libirods_common.so
/// - Link against \p libfmt.so (must be from the same externals package used to compile \p libirods_common.so)
///
/// If you're implementing plugins for the iRODS server, then the requirements just mentioned can be skipped.

#ifdef IRODS_ENABLE_SYSLOG
#  define SPDLOG_ENABLE_SYSLOG
#  include <spdlog/spdlog.h>
#  undef SPDLOG_ENABLE_SYSLOG
#endif // IRODS_ENABLE_SYSLOG

#include "irods/apiNumberMap.h"
#include "irods/chrono.hpp"
#include "irods/rodsError.h"
#include "irods/rcMisc.h"
#include "irods/rcConnect.h"

#include <boost/range/iterator_range_core.hpp>
#include <fmt/format.h>
#include <fmt/compile.h>
#include <nlohmann/json.hpp>

#include <unistd.h>

#include <chrono>
#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <initializer_list>
#include <type_traits>
#include <iterator>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <concepts>
#include <tuple>

#if FMT_VERSION >= 100000
#  include <fmt/chrono.h>
#  define IRODS_CHRONO_FORMATTER_FMT 1
#else
#  include <ctime>
#  define IRODS_CHRONO_FORMATTER_FMT 0
#endif

/// Defines all things related to the logging API.
///
/// \since 4.3.0
namespace irods::experimental::log
{
    /// A type alias used to represent a single JSON property in a log message.
    ///
    /// \since 4.3.0
    using key_value = std::pair<const std::string, std::string>;

    /// An enumeration representing the list of supported log levels.
    ///
    /// The levels are ordered from most noisy to least noisy.
    ///
    /// \since 4.3.0
    enum class level
    {
        trace,    ///< Most verbose diagnostic output.
        debug,    ///< Diagnostic output intended for development and troubleshooting.
        info,     ///< Informational messages describing normal operation.
        warn,     ///< Warnings about unusual but non-fatal conditions.
        error,    ///< Errors indicating a failed operation.
        critical  ///< Severe failures requiring immediate attention.
    }; // enum class level

    /// Pre-defined log categories.
    ///
    /// The types defined within this namespace serve as tags. They are used to tie various logging
    /// components together.
    ///
    /// \since 4.3.0
    namespace category
    {
        // clang-format off
        struct legacy {};         ///< Tag type for legacy log messages.
        struct server {};         ///< Tag type for server log messages.
        struct agent_factory {};  ///< Tag type for agent factory log messages.
        struct agent {};          ///< Tag type for agent log messages.
        struct delay_server {};   ///< Tag type for delay server log messages.
        struct genquery1 {};      ///< Tag type for GenQuery1 log messages.
        struct genquery2 {};      ///< Tag type for GenQuery2 log messages.
        struct resource {};       ///< Tag type for resource log messages.
        struct database {};       ///< Tag type for database log messages.
        struct authentication {}; ///< Tag type for authentication log messages.
        struct api {};            ///< Tag type for API log messages.
        struct microservice {};   ///< Tag type for microservice log messages.
        struct network {};        ///< Tag type for network log messages.
        struct rule_engine {};    ///< Tag type for rule engine log messages.
        struct sql {};            ///< Tag type for SQL log messages.
        // clang-format on
    } // namespace category

    // clang-format off
    /// A class template forward declaration that allows users to introduce new log categories.
    ///
    /// This class template is meant to be specialized based on a log category. Pre-defined
    /// specializations have been provided for the log categories defined in the category namespace.
    ///
    /// \tparam Category A tag type used to associate state to a particular log category.
    ///
    /// Below is an example that demonstrates how to specialize this class template.
    /// \code{.cpp}
    /// struct my_log_category; // My fancy new log category.
    ///
    /// template <>
    /// class logger_config<my_log_category>
    /// {
    ///     static constexpr const char* name = "my_log_category"; // The name you want to display in the log message.
    ///     inline static level level = level::info;               // The default log level for the log category.
    ///
    ///     friend class logger<my_log_category>;
    /// }; // class logger_config<category::legacy>
    /// \endcode
    ///
    /// \since 4.3.0
    template <typename Category> class logger_config;

    /// @brief Logger class template used to emit messages for a category.
    /// @tparam Category Tag type identifying the log category.
    template <typename Category> class logger;

    /// \name Log Categories
    ///
    /// Pre-defined type alias representing loggers that are meant to be used in various components
    /// throughout the iRODS codebase.
    ///
    /// The names of each type alias act as a hint for where they should be used. For example, the
    /// \p microservice logger should be used in microservice specific code. Choosing not to follow
    /// this recommendation will result in confusing log messages.
    ///@{
    using legacy         = logger<category::legacy>; ///< Logger type for legacy log messages.
    using server         = logger<category::server>; ///< Logger type for server log messages.
    using agent_factory  = logger<category::agent_factory>; ///< Logger type for agent factory log messages.
    using agent          = logger<category::agent>; ///< Logger type for agent log messages.
    using delay_server   = logger<category::delay_server>; ///< Logger type for delay server log messages.
    using genquery1      = logger<category::genquery1>; ///< Logger type for GenQuery1 log messages.
    using genquery2      = logger<category::genquery2>; ///< Logger type for GenQuery2 log messages.
    using resource       = logger<category::resource>; ///< Logger type for resource log messages.
    using database       = logger<category::database>; ///< Logger type for database log messages.
    using authentication = logger<category::authentication>; ///< Logger type for authentication log messages.
    using api            = logger<category::api>; ///< Logger type for API log messages.
    using microservice   = logger<category::microservice>; ///< Logger type for microservice log messages.
    using network        = logger<category::network>; ///< Logger type for network log messages.
    using rule_engine    = logger<category::rule_engine>; ///< Logger type for rule engine log messages.
    using sql            = logger<category::sql>; ///< Logger type for SQL log messages.
    ///@}
    // clang-format on

    /// Initializes logging facilities.
    ///
    /// This function must be called before invoking other logging API operations.
    /// This function is not thread-safe.
    ///
    /// \param[in] _pid              The PID of the main iRODS server process.
    /// \param[in] _write_to_stdout  Configures the logging API to write all messages to stdout.
    /// \param[in] _enable_test_mode Configures the logging API to also write messages to a special
    ///                              file that flushes its output on every write. This is good for
    ///                              tests that need to search the log file for specific messages.
    ///
    /// \since 4.3.0
    auto init(pid_t _pid, bool _write_to_stdout = false, bool _enable_test_mode = false) noexcept -> void;

    /// Converts a string to a specific log level.
    ///
    /// \param[in] _level The string representation of a supported log level.
    ///
    /// \return A \p level enumeration if the string is valid.
    /// \retval level::info If \p _level could not be converted to a supported log level.
    ///
    /// \since 4.3.0
    auto to_level(const std::string_view _level) noexcept -> level;

    /// Loads the log level for a specific log category from server_config.json.
    ///
    /// \param[in] _category The log category to fetch the log level for.
    ///
    /// \return A \p level enumeration if the log category is defined.
    /// \retval level::info If the log level cannot be retrieved for \p _category.
    ///
    /// \since 4.3.0
    auto get_level_from_config(const std::string_view _category) noexcept -> level;

    /// Associates or disassociates an ErrorStack object with all loggers.
    ///
    /// This function is not thread-safe.
    ///
    /// \param[in] _error A pointer to an ErrorStack object. Passing \p nullptr disassociates any
    ///                   error objects from the loggers.
    ///
    /// \since 4.3.0
    auto set_error_object(ErrorStack* _error) noexcept -> void;

    /// Returns the error object currently used by all loggers, if available.
    ///
    /// This function is not thread-safe.
    ///
    /// \since 4.3.1
    auto get_error_object() noexcept -> ErrorStack*;

    /// Instructs the logging library to write messages to an ErrorStack object if available.
    ///
    /// This function is not thread-safe.
    ///
    /// \param[in] _value A boolean which enables or disables writing messages to the attached
    ///                   ErrorStack object.
    ///
    /// \see set_error_object()
    ///
    /// \since 4.3.0
    auto write_to_error_object(bool _value) noexcept -> void;

    /// Returns whether all loggers should write error messages to the error object.
    ///
    /// This function is not thread-safe.
    ///
    /// \since 4.3.1
    auto should_write_to_error_object() noexcept -> bool;

    /// Sets the API number for subsequent log messages.
    ///
    /// The value for this should be updated every time the connected client invokes an API endpoint.
    /// This function is not thread-safe.
    ///
    /// \param[in] _api_number The integer value mapped to a specific API endpoint.
    ///
    /// \since 4.3.0
    auto set_request_api_number(int _api_number) noexcept -> void;

    /// Instructs the logging library to not include the API number in subsequent log messages.
    ///
    /// This function is not thread-safe.
    ///
    /// \since 4.3.0
    auto clear_request_api_number() noexcept -> void;

    /// Returns the API number currently used by all loggers, if available.
    ///
    /// This function is not thread-safe.
    ///
    /// \since 4.3.1
    auto get_request_api_number() noexcept -> std::optional<int>;

    /// Sets the version information of the connected client for subsequent log messages.
    ///
    /// This function is not thread-safe.
    ///
    /// \param[in] _client_version A pointer to the version information of the connected client.
    ///                            Passing \p nullptr will cause all loggers to not include version
    ///                            information in subsequent log messages.
    ///
    /// \since 4.3.0
    auto set_request_client_version(const Version* _client_version) noexcept -> void;

    /// Returns the version information of the connected client currently used by all loggers.
    ///
    /// This function is not thread-safe.
    ///
    /// \since 4.3.1
    auto get_request_client_version() noexcept -> const Version*;

    /// Sets the hostname/IP of the connected client for subsequent log messages.
    ///
    /// This function is not thread-safe.
    ///
    /// \param[in] _hostname A string representing the hostname or IP of the connected client.
    ///
    /// \since 4.3.0
    auto set_request_client_hostname(std::string _hostname) noexcept -> void;

    /// Returns the client hostname of the connected client currently used by all loggers.
    ///
    /// This function is not thread-safe.
    ///
    /// \since 4.3.1
    auto get_request_client_hostname() noexcept -> std::string_view;

    /// Sets the username of the connected client for subsequent log messages.
    ///
    /// This function is not thread-safe.
    ///
    /// \param[in] _username A string containing the iRODS username of the connected client.
    ///
    /// \since 4.3.0
    auto set_request_client_username(std::string _username) noexcept -> void;

    /// Returns the client username of the connected client currently used by all loggers.
    ///
    /// This function is not thread-safe.
    ///
    /// \since 4.3.1
    auto get_request_client_username() noexcept -> std::string_view;

    /// Sets the proxy username of the connected client for subsequent log messages.
    ///
    /// This function is not thread-safe.
    ///
    /// \param[in] _username A string containing the iRODS proxy username of the connected client.
    ///
    /// \since 4.3.0
    auto set_request_proxy_username(std::string _username) noexcept -> void;

    /// Returns the proxy username of the connected client currently used by all loggers.
    ///
    /// This function is not thread-safe.
    ///
    /// \since 4.3.1
    auto get_request_proxy_username() noexcept -> std::string_view;

    /// Sets the server type for subsequent log messages.
    ///
    /// This function is not thread-safe.
    ///
    /// \param[in] _type A string representing the type of the server.
    ///
    /// \since 4.3.0
    auto set_server_type(std::string _type) noexcept -> void;

    /// Returns the server type currently used by all loggers.
    ///
    /// This function is not thread-safe.
    ///
    /// \since 4.3.1
    auto get_server_type() noexcept -> std::string_view;

    /// Sets the hostname of the iRODS server servicing the API request for subsequent log messages.
    ///
    /// This function is not thread-safe.
    ///
    /// \param[in] _hostname A string containing the hostname of the iRODS server.
    ///
    /// \since 4.3.0
    auto set_server_hostname(std::string _hostname) noexcept -> void;

    /// Returns the hostname of the iRODS server currently used by all loggers.
    ///
    /// This function is not thread-safe.
    ///
    /// \since 4.3.1
    auto get_server_hostname() noexcept -> std::string_view;

    /// Sets the server name for subsequent log messages.
    ///
    /// This function is not thread-safe.
    ///
    /// \param[in] _name A string representing the server name.
    ///
    /// \since 4.3.0
    auto set_server_name(std::string _name) noexcept -> void;

    /// Returns the server name currently used by all loggers.
    ///
    /// This function is not thread-safe.
    ///
    /// \since 4.3.1
    auto get_server_name() noexcept -> std::string_view;

    /// Sets the name of the zone for subsequent log messages.
    ///
    /// This function is not thread-safe.
    ///
    /// \param[in] _name A string representing the server zone.
    ///
    /// \since 4.3.1
    auto set_server_zone(std::string _name) noexcept -> void;

    /// Returns the name of the zone currently used by all loggers.
    ///
    /// This function is not thread-safe.
    ///
    /// \since 4.3.1
    auto get_server_zone() noexcept -> std::string_view;

    /// Defines types and functions that are private to the implementation of the library.
    ///
    /// Users of this library MUST NOT use anything defined in this namespace.
    ///
    /// \since 4.3.1
    namespace detail
    {
#ifdef IRODS_ENABLE_SYSLOG
        /// @brief Returns the shared backend logger used by the logging API.
        /// @return A shared pointer to the configured spdlog logger.
        auto get_logger() noexcept -> std::shared_ptr<spdlog::logger>;
#endif // IRODS_ENABLE_SYSLOG
    } // namespace detail

    /// Defines various property names used to construct a log message.
    ///
    /// \since 4.3.0
    namespace tag
    {
        /// Defines various property names used to construct a log message.
        ///
        /// \since 4.3.0
        namespace log
        {
            // clang-format off
            inline constexpr const char* category = "log_category"; ///< JSON key storing the log category name.
            inline constexpr const char* message  = "log_message";  ///< JSON key storing the rendered log message.
            inline constexpr const char* level    = "log_level";    ///< JSON key storing the log level.
            // clang-format on
        } // namespace log

        /// Defines various property names used to construct a log message.
        ///
        /// \since 4.3.0
        namespace request
        {
            // clang-format off
            inline constexpr const char* release_version = "request_release_version"; ///< JSON key storing the client release version.
            inline constexpr const char* api_version     = "request_api_version";     ///< JSON key storing the client API version.
            inline constexpr const char* host            = "request_host";            ///< JSON key storing the client hostname or IP.
            inline constexpr const char* client_user     = "request_client_user";     ///< JSON key storing the client username.
            inline constexpr const char* proxy_user      = "request_proxy_user";      ///< JSON key storing the proxy username.
            inline constexpr const char* api_number      = "request_api_number";      ///< JSON key storing the API number.
            inline constexpr const char* api_name        = "request_api_name";        ///< JSON key storing the API name.
            // clang-forexprmat on
        } // namespace request

        /// Defines various property names used to construct a log message.
        ///
        /// \since 4.3.0
        namespace server
        {
            // clang-format off
            inline constexpr const char* type      = "server_type";      ///< JSON key storing the server type.
            inline constexpr const char* host      = "server_host";      ///< JSON key storing the server hostname.
            inline constexpr const char* pid       = "server_pid";       ///< JSON key storing the server process ID.
            inline constexpr const char* name      = "server_name";      ///< JSON key storing the configured server name.
            inline constexpr const char* timestamp = "server_timestamp"; ///< JSON key storing the message timestamp.
            inline constexpr const char* zone      = "server_zone";      ///< JSON key storing the server zone.
            // clang-format on
        } // namespace server
    } // namespace tag

    //
    // Logger Interface Definition
    //

    /// A class template which defines the primary functionality of all loggers.
    ///
    /// Instantiations of this class template are not copyable or moveable.
    ///
    /// \tparam Category A tag type used to associate a logger to a particular log category.
    ///
    /// \since 4.3.0
    template <typename Category>
    class logger // NOLINT(cppcoreguidelines-special-member-functions)
    {
      public:
        /// A class template that provides the implementation for logging messages in various ways.
        ///
        /// Instantiations of this class template are not copyable or moveable. Users of the logging
        /// library MUST NOT instantiate this template directly. Future versions of this library are
        /// free to change the implementation of this class template.
        ///
        /// \tparam Level The level to specialize the implementation on.
        ///
        /// \since 4.3.0
        template <level Level>
        class impl // NOLINT(cppcoreguidelines-special-member-functions)
        {
          public:
            /// Grants the enclosing logger access to implementation internals.
            friend class logger<Category>;

            /// @brief Disabled copy construction for logger implementations.
            impl(const impl&) = delete;
            /// @brief Disabled copy assignment for logger implementations.
            auto operator=(const impl&) -> impl& = delete;

#ifdef IRODS_ENABLE_SYSLOG
            template <typename T>
            /// @brief Detects whether a type supports iteration.
            using is_iterable = decltype(std::begin(std::declval<std::decay_t<T>>()));

            /// Writes a formatted string to the log file.
            ///
            /// \tparam Args The list of types matching the arguments passed.
            ///
            /// \param[in] _format The format string to write. It can include any number of placeholders.
            /// \param[in] _args   The format arguments to inject into the format string.
            ///
            /// \since 4.3.0
            template <typename... Args>
            auto operator()(fmt::format_string<Args...> _format, Args&&... _args) const -> void
#if __clang__ == 0 || __clang_major__ > 15
                // Clang 15 and later (and GCC 11 and later) consider this overload ambiguous
                // with the std::invocable overload. This constraint sorts that out.
                requires (sizeof...(Args) != 1 || !std::is_invocable_v<std::tuple_element_t<0, std::tuple<Args...>>>)
#elif __clang_major__ == 15
                // Clang 15 can't parse the above statement properly, so we have special overloads for 15.
                // This overload handles the 1-argument case.
                requires (sizeof...(Args) == 0)
#endif
            {
                if (should_log()) {
                    const auto msg = {key_value{tag::log::message, fmt::format(_format, std::forward<Args>(_args)...)}};
                    log_message(std::begin(msg), std::end(msg));
                }
            } // operator()

#if __clang_major__ == 15
            // This overload covers the 2-argument cases
            template <typename Arg>
            auto operator()(fmt::format_string<Arg> _format, Arg&& _arg) const -> void
                requires (!std::is_invocable_v<Arg>)
            {
                if (should_log()) {
                    const auto msg = {key_value{tag::log::message, fmt::format(_format, std::forward<Args>(_arg))}};
                    log_message(std::begin(msg), std::end(msg));
                }
            } // operator()

            // This overload covers the 3+-argument cases
            template <typename Arg1, typename... Args>
            auto operator()(fmt::format_string<Arg1, Args...> _format, Arg1&& _arg1, Args&&... _args) const -> void
                requires (sizeof...(Args) > 0) // to eliminate ambiguity with previous overload
            {
                if (should_log()) {
                    const auto msg = {
                        key_value{tag::log::message,
                                  fmt::format(_format, std::forward<Args>(_arg1), std::forward<Args>(_args)...)}};
                    log_message(std::begin(msg), std::end(msg));
                }
            } // operator()
#endif

            /// Writes a string to the log file.
            ///
            /// \param[in] _msg The string to write to the log file.
            ///
            /// \since 4.3.0
            auto operator()(const std::string& _msg) const -> void
            {
                (*this)({{tag::log::message, _msg}});
            } // operator()

            /// Writes a list of ::key_value objects to the log file.
            ///
            /// This member function allows developers to inject new JSON properties into the log
            /// message. Keys become JSON property names that sit at the same level as ones automatically
            /// included by the logging API (e.g. "log_message" and "server_timestamp").
            ///
            /// This member function does not format any arguments passed to it.
            ///
            /// \param[in] _list The list of key-value pairs to write.
            ///
            /// \since 4.3.0
            auto operator()(std::initializer_list<key_value> _list) const -> void
            {
                (*this)(std::begin(_list), std::end(_list));
            } // operator()

            /// Writes the contents of a container to the log file.
            ///
            /// The container is required to hold a list of ::key_value objects.
            ///
            /// \tparam Container A container type that supports iteration of its elements.
            /// \tparam ValueType Must match ::key_value.
            ///
            /// \param[in] _container The container holding the list of ::key_value objects to write.
            ///
            /// \since 4.3.0
            template <
                typename Container,
                typename ValueType = typename Container::value_type,
                typename = is_iterable<Container>,
                typename = std::enable_if_t<std::is_same_v<ValueType, key_value>>>
            auto operator()(const Container& _container) const -> void
            {
                (*this)(std::begin(_container), std::end(_container));
            } // operator()

            /// Writes the values denoted by the range [_first, _last) to the log file.
            ///
            /// \tparam ForwardIt An iterator type that satifies the forward iterator requirements.
            /// \tparam ValueType Must match ::key_value.
            ///
            /// \param[in] _first The iterator representing the start of the range.
            /// \param[in] _last  The iterator representing the end of the range.
            ///
            /// \since 4.3.0
            template <
                typename ForwardIt,
                typename ValueType = typename std::iterator_traits<ForwardIt>::value_type,
                typename = std::enable_if_t<std::is_same_v<ValueType, key_value>>>
            auto operator()(ForwardIt _first, ForwardIt _last) const -> void
            {
                if (should_log()) {
                    log_message(_first, _last);
                }
            } // operator()

            /// Writes a formatted string to the log file.
            ///
            /// This member function gives developers a way to defer evaluation of format arguments. For
            /// those situations where it is expensive to construct objects and the logger decides to not
            /// write the results to the log file, this overload should be used.
            ///
            /// Below is an example demonstrating usage.
            /// \code{.cpp}
            /// using log_db = irods::experimental::log::database;
            ///
            /// log_db::info("This value, {}, is too expensive to construct for the logger to ignore.", [] {
            ///     return std::make_tuple(very_expensive_result_to_compute_and_ignore());
            /// });
            /// \endcode
            ///
            /// \param[in] _format    The format string to write.
            /// \param[in] _invocable An invocable object that returns a tuple-like object containing the
            ///                       arguments that will be injected into the format string.
            ///
            /// \since 4.3.1
            auto operator()(const std::string_view _format, std::invocable auto&& _invocable) const -> void
            {
                if (should_log()) {
                    std::apply(
                        [this, _format]<typename... Ts>(Ts&&... _args) {
                            const auto msg = {key_value{
                                tag::log::message, fmt::format(fmt::runtime(_format), std::forward<Ts>(_args)...)}};
                            log_message(std::begin(msg), std::end(msg));
                        },
                        _invocable());
                }
            } // operator()
#else // IRODS_ENABLE_SYSLOG
            // clang-format off

            // Clients should not have access to the logger, therefore we provide a
            // different implementation to allow inclusion of the logger without
            // breaking any existing implementation.

            // clang-format on

            /// @brief No-op overload retained for client-side builds.
            /// @param[in] _list Ignored key-value list.
            constexpr auto operator()(std::initializer_list<key_value> _list) const noexcept -> void
            {
            } // operator()

            template <typename... Args>
            /// @brief No-op overload retained for client-side builds.
            /// Accepts arbitrary arguments and ignores them.
            constexpr auto operator()(Args&&...) const noexcept -> void
            {
            } // operator()
#endif // IRODS_ENABLE_SYSLOG

          private:
            /// Constructs a logger implementation.
            impl() = default;

#ifdef IRODS_ENABLE_SYSLOG
            /// Reports whether the logger should emit messages at this level.
            [[nodiscard]] auto should_log() const noexcept -> bool
            {
                return Level >= logger_config<Category>::level;
            } // should_log

            /// Returns the current log level as a string literal.
            static constexpr auto log_level_as_string() noexcept -> const char*
            {
                // clang-format off
                if      constexpr (Level == level::trace)    { return "trace"; }
                else if constexpr (Level == level::debug)    { return "debug"; }
                else if constexpr (Level == level::info)     { return "info"; }
                else if constexpr (Level == level::warn)     { return "warn"; }
                else if constexpr (Level == level::error)    { return "error"; }
                else if constexpr (Level == level::critical) { return "critical"; }
                // clang-format on

                return "?";
            } // log_level_as_string

            /// Renders log properties and ambient request metadata as a JSON string.
            template <typename ForwardIt>
            auto to_json_string(ForwardIt _first, ForwardIt _last) const -> std::string
            {
                using json = nlohmann::json;
                using container = std::unordered_map<std::string, std::string>;

                json object = container(_first, _last);

                object[tag::log::category] = logger_config<Category>::name;
                object[tag::log::level] = log_level_as_string();

                if (const auto api_num = get_request_api_number(); api_num) {
                    object[tag::request::api_number] = *api_num;
                    auto iter = irods::api_number_names.find(*api_num);

                    if (std::end(irods::api_number_names) != iter) {
                        object[tag::request::api_name] = iter->second;
                    }
                    else {
                        object[tag::request::api_name] = "";
                    }
                }

                if (const auto* vers = get_request_client_version(); vers) {
                    object[tag::request::release_version] = vers->relVersion;
                    object[tag::request::api_version] = vers->apiVersion;
                }

                if (const auto hostname = get_request_client_hostname(); !hostname.empty()) {
                    object[tag::request::host] = hostname;
                }

                if (const auto username = get_request_client_username(); !username.empty()) {
                    object[tag::request::client_user] = username;
                }

                if (const auto username = get_request_proxy_username(); !username.empty()) {
                    object[tag::request::proxy_user] = username;
                }

                object[tag::server::type] = get_server_type();
                object[tag::server::host] = get_server_hostname();
                object[tag::server::pid] = getpid();
                object[tag::server::timestamp] = utc_timestamp();
                object[tag::server::zone] = get_server_zone();

                return object.dump(-1, ' ', false, json::error_handler_t::replace);
            } // to_json_string

            /// Writes a prepared JSON log message and mirrors it to the error stack when configured.
            template <
                typename ForwardIt,
                typename ValueType = typename std::iterator_traits<ForwardIt>::value_type,
                typename = std::enable_if_t<std::is_same_v<ValueType, log::key_value>>>
            constexpr auto log_message(ForwardIt _first, ForwardIt _last) const -> void
            {
                const auto msg = to_json_string(_first, _last);

                if constexpr (Level == level::trace) {
                    detail::get_logger()->trace(msg);
                }
                else if constexpr (Level == level::debug) {
                    detail::get_logger()->debug(msg);
                }
                else if constexpr (Level == level::info) {
                    detail::get_logger()->info(msg);
                }
                else if constexpr (Level == level::warn) {
                    detail::get_logger()->warn(msg);
                }
                else if constexpr (Level == level::error) {
                    detail::get_logger()->error(msg);
                }
                else if constexpr (Level == level::critical) {
                    detail::get_logger()->critical(msg);
                }

                append_to_r_error_stack(_first, _last);
            } // log_message

            /// Appends the emitted message fields to the associated error stack.
            template <typename ForwardIt>
            constexpr auto append_to_r_error_stack(ForwardIt _first, ForwardIt _last) const -> void
            {
                if (!should_write_to_error_object()) {
                    return;
                }

                if (auto* const error = get_error_object(); error) {
                    for (const auto& [k, v] : boost::make_iterator_range(_first, _last)) {
                        const auto msg = fmt::format("{}: {}", k, v);

                        // Force addRErrorMsg() to skip prefixing the message with 'Level N:'.
                        // When the output is printed on the client-side, it will be very close
                        // to the log file.
                        constexpr int disable_level_prefix = STDOUT_STATUS;
                        addRErrorMsg(error, disable_level_prefix, msg.c_str());
                    }
                }
            } // append_to_r_error_stack
#endif // IRODS_ENABLE_SYSLOG
        }; // class impl

        /// @brief Loggers cannot be instantiated directly.
        logger() = delete;

        /// @brief Loggers are not copyable.
        logger(const logger&) = delete;
        /// @brief Loggers are not copy-assignable.
        auto operator=(const logger&) -> logger& = delete;

        /// Sets the log level for a specific logger.
        ///
        /// This member function is not thread-safe.
        ///
        /// \param[in] _level The log level that will be used by the logger.
        ///
        /// \since 4.3.0
        static constexpr auto set_level(level _level) noexcept -> void
        {
            logger_config<Category>::level = _level;
        } // set_level

        // clang-format off
        inline static const auto trace    = impl<level::trace>{};    ///< Logger entry point for trace-level messages.
        inline static const auto debug    = impl<level::debug>{};    ///< Logger entry point for debug-level messages.
        inline static const auto info     = impl<level::info>{};     ///< Logger entry point for info-level messages.
        inline static const auto warn     = impl<level::warn>{};     ///< Logger entry point for warning-level messages.
        inline static const auto error    = impl<level::error>{};    ///< Logger entry point for error-level messages.
        inline static const auto critical = impl<level::critical>{}; ///< Logger entry point for critical-level messages.
        // clang-format on
    }; // class logger

    //
    // Pre-defined Logger Category Configurations
    //

    /// @brief Logger configuration specialization for the legacy category.
    template <>
    class logger_config<category::legacy>
    {
        /// Name written into log messages for this category.
        static constexpr const char* const name = "legacy";
        /// Default minimum level for this category.
        inline static level level = level::info; // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

        /// Grants the logger access to configuration internals.
        friend class logger<category::legacy>;
    }; // class logger_config<category::legacy>

    /// @brief Logger configuration specialization for the server category.
    template <>
    class logger_config<category::server>
    {
        /// Name written into log messages for this category.
        static constexpr const char* const name = "server";
        /// Default minimum level for this category.
        inline static level level = level::info; // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

        /// Grants the logger access to configuration internals.
        friend class logger<category::server>;
    }; // class logger_config<category::server>

    /// @brief Logger configuration specialization for the agent factory category.
    template <>
    class logger_config<category::agent_factory>
    {
        /// Name written into log messages for this category.
        static constexpr const char* const name = "agent_factory";
        /// Default minimum level for this category.
        inline static level level = level::info; // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

        /// Grants the logger access to configuration internals.
        friend class logger<category::agent_factory>;
    }; // class logger_config<category::agent_factory>

    /// @brief Logger configuration specialization for the agent category.
    template <>
    class logger_config<category::agent>
    {
        /// Name written into log messages for this category.
        static constexpr const char* const name = "agent";
        /// Default minimum level for this category.
        inline static level level = level::info; // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

        /// Grants the logger access to configuration internals.
        friend class logger<category::agent>;
    }; // class logger_config<category::agent>

    /// @brief Logger configuration specialization for the delay server category.
    template <>
    class logger_config<category::delay_server>
    {
        /// Name written into log messages for this category.
        static constexpr const char* const name = "delay_server";
        /// Default minimum level for this category.
        inline static level level = level::info; // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

        /// Grants the logger access to configuration internals.
        friend class logger<category::delay_server>;
    }; // class logger_config<category::delay_server>

    /// @brief Logger configuration specialization for the GenQuery1 category.
    template <>
    class logger_config<category::genquery1>
    {
        /// Name written into log messages for this category.
        static constexpr const char* const name = "genquery1";
        /// Default minimum level for this category.
        inline static level level = level::info; // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

        /// Grants the logger access to configuration internals.
        friend class logger<category::genquery1>;
    }; // class logger_config<category::genquery1>

    /// @brief Logger configuration specialization for the GenQuery2 category.
    template <>
    class logger_config<category::genquery2>
    {
        /// Name written into log messages for this category.
        static constexpr const char* const name = "genquery2";
        /// Default minimum level for this category.
        inline static level level = level::info; // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

        /// Grants the logger access to configuration internals.
        friend class logger<category::genquery2>;
    }; // class logger_config<category::genquery2>

    /// @brief Logger configuration specialization for the resource category.
    template <>
    class logger_config<category::resource>
    {
        /// Name written into log messages for this category.
        static constexpr const char* const name = "resource";
        /// Default minimum level for this category.
        inline static level level = level::info; // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

        /// Grants the logger access to configuration internals.
        friend class logger<category::resource>;
    }; // class logger_config<category::resource>

    /// @brief Logger configuration specialization for the database category.
    template <>
    class logger_config<category::database>
    {
        /// Name written into log messages for this category.
        static constexpr const char* const name = "database";
        /// Default minimum level for this category.
        inline static level level = level::info; // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

        /// Grants the logger access to configuration internals.
        friend class logger<category::database>;
    }; // class logger_config<category::database>

    /// @brief Logger configuration specialization for the authentication category.
    template <>
    class logger_config<category::authentication>
    {
        /// Name written into log messages for this category.
        static constexpr const char* const name = "authentication";
        /// Default minimum level for this category.
        inline static level level = level::info; // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

        /// Grants the logger access to configuration internals.
        friend class logger<category::authentication>;
    }; // class logger_config<category::authentication>

    /// @brief Logger configuration specialization for the API category.
    template <>
    class logger_config<category::api>
    {
        /// Name written into log messages for this category.
        static constexpr const char* const name = "api";
        /// Default minimum level for this category.
        inline static level level = level::info; // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

        /// Grants the logger access to configuration internals.
        friend class logger<category::api>;
    }; // class logger_config<category::api>

    /// @brief Logger configuration specialization for the microservice category.
    template <>
    class logger_config<category::microservice>
    {
        /// Name written into log messages for this category.
        static constexpr const char* const name = "microservice";
        /// Default minimum level for this category.
        inline static level level = level::info; // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

        /// Grants the logger access to configuration internals.
        friend class logger<category::microservice>;
    }; // class logger_config<category::microservice>

    /// @brief Logger configuration specialization for the network category.
    template <>
    class logger_config<category::network>
    {
        /// Name written into log messages for this category.
        static constexpr const char* const name = "network";
        /// Default minimum level for this category.
        inline static level level = level::info; // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

        /// Grants the logger access to configuration internals.
        friend class logger<category::network>;
    }; // class logger_config<category::network>

    /// @brief Logger configuration specialization for the rule engine category.
    template <>
    class logger_config<category::rule_engine>
    {
        /// Name written into log messages for this category.
        static constexpr const char* const name = "rule_engine";
        /// Default minimum level for this category.
        inline static level level = level::info; // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

        /// Grants the logger access to configuration internals.
        friend class logger<category::rule_engine>;
    }; // class logger_config<category::rule_engine>

    /// @brief Logger configuration specialization for the SQL category.
    template <>
    class logger_config<category::sql>
    {
        /// Name written into log messages for this category.
        static constexpr const char* const name = "sql";
        /// Default minimum level for this category.
        inline static level level = level::info; // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

        /// Grants the logger access to configuration internals.
        friend class logger<category::sql>;
    }; // class logger_config<category::sql>
} // namespace irods::experimental::log

#endif // IRODS_LOGGER_HPP
