#ifndef IRODS_CONNECTION_POOL_HPP
#define IRODS_CONNECTION_POOL_HPP

#include "irods/fully_qualified_username.hpp"
#include "irods/rcConnect.h"

#include <atomic>
#include <chrono>
#include <functional>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace irods
{
    /// Defines various options for changing the behavior of a connection pool.
    ///
    /// Refresh options are not mutually exclusive.
    ///
    /// It is important to understand that any of the options can trigger a connection
    /// refresh. When a refresh occurs, all state related to connection refresh is reset.
    /// Therefore, care must be taken when setting multiple options in order to obtain
    /// deterministic refresh behavior.
    ///
    /// \since 4.3.1
    struct connection_pool_options
    {
        /// Defines the number of times a connection is retrieved from the pool before it is
        /// refreshed.
        ///
        /// \since 4.3.1
        std::optional<std::int16_t> number_of_retrievals_before_connection_refresh;

        /// Defines the number of seconds to pass before a connection is refreshed.
        ///
        /// \since 4.3.1
        std::optional<std::chrono::seconds> number_of_seconds_before_connection_refresh;

        /// Instructs the connection pool to refresh a connection when a change is detected
        /// in any of the zone's resources.
        ///
        /// \since 4.3.1
        bool refresh_connections_when_resource_changes_detected = false;
    }; // struct connection_pool_options

    /// Manages multiple iRODS connections for one or more users.
    ///
    /// Instances of this class are not copyable or moveable.
    ///
    /// \since 4.2.5
    class connection_pool // NOLINT(cppcoreguidelines-special-member-functions)
    {
      public:
        /// Manages a single connection within a connection_pool.
        ///
        /// On destruction, the underlying connection is immediately returned to the pool.
        ///
        /// Instances of this class are not copyable.
        ///
        /// \since 4.2.5
        class connection_proxy // NOLINT(cppcoreguidelines-special-member-functions)
        {
          public:
            /// Allows connection_pool to construct and manage proxies directly.
            friend class connection_pool;

            /// Constructs an empty connection_proxy.
            ///
            /// On success, the following assertions will be true:
            /// - static_cast<RcComm*>(instance) will return a nullptr
            /// - static_cast<RcComm&>(instance) will throw an irods::exception
            ///
            /// \since 4.2.9
            connection_proxy();

            /// Constructs a connection_proxy by moving another instance.
            connection_proxy(connection_proxy&&) noexcept;

            /// Replaces this connection_proxy by moving another instance into it.
            connection_proxy& operator=(connection_proxy&&) noexcept;

            /// Destructs the connection_proxy and returns the underlying RcComm to the pool.
            ///
            /// \since 4.2.5
            ~connection_proxy();

            /// Returns a boolean indicating whether the connection_proxy holds a valid RcComm.
            ///
            /// \retval true  If the underlying RcComm is valid.
            /// \retval false Otherwise.
            ///
            /// \since 4.2.7
            operator bool() const noexcept; // NOLINT(google-explicit-constructor)

            /// Returns a reference to the underlying RcComm.
            ///
            /// \throws irods::exception If the underlying RcComm is not valid (i.e. nullptr).
            ///
            /// \since 4.2.5
            operator RcComm&() const; // NOLINT(google-explicit-constructor)

            /// Returns a pointer to the underlying RcComm.
            ///
            /// If the underlying RcComm is not valid, a \p nullptr is returned.
            ///
            /// \since 4.2.7
            explicit operator RcComm*() const noexcept;

            /// Transfers ownership of the underlying RcComm from the connection_proxy and
            /// connection_pool to the caller.
            ///
            /// \return The released connection pointer.
            ///
            /// \since 4.2.7
            RcComm* release();

          private:
            /// Constructs a proxy for a specific pooled connection.
            ///
            /// \param[in] _pool The owning connection pool.
            /// \param[in] _conn The connection being proxied.
            /// \param[in] _index The pool slot containing the connection.
            connection_proxy(connection_pool& _pool, RcComm& _conn, int _index) noexcept;

            static constexpr int uninitialized_index = -1; ///< Sentinel value for an invalid slot index.

            connection_pool* pool_; ///< Owning connection pool.
            RcComm* conn_; ///< Proxied connection pointer.
            int index_; ///< Pool slot index for the connection.
        }; // class connection_proxy

        /// Constructs a connection_pool.
        ///
        /// Each connection in the pool is authenticated as \p _name (pound) _zone.
        ///
        /// \param[in] _size         The number of connections to create.
        /// \param[in] _host         The hostname of the iRODS server.
        /// \param[in] _port         The port to connect to.
        /// \param[in] _name         The name of the user to connect as.
        /// \param[in] _zone         The zone of the user to connect as.
        /// \param[in] _refresh_time The number of seconds before a connection is refreshed.
        ///
        /// \throws irods::exception If an error occurs.
        ///
        /// \since 4.2.5
        [[deprecated]] connection_pool(int _size,
                                       const std::string& _host,
                                       const int _port,
                                       const std::string& _name,
                                       const std::string& _zone,
                                       const int _refresh_time);

        /// Constructs a connection_pool.
        ///
        /// Each connection in the pool is authenticated as \p _username.
        ///
        /// \param[in] _size     The number of connections to create.
        /// \param[in] _host     The hostname of the iRODS server.
        /// \param[in] _port     The port to connect to.
        /// \param[in] _username The name of the user to connect as.
        /// \param[in] _options  The connection pool options.
        ///
        /// \throws irods::exception If an error occurs.
        ///
        /// \since 4.3.1
        connection_pool(int _size,
                        const std::string& _host,
                        const int _port,
                        experimental::fully_qualified_username _username,
                        const connection_pool_options& _options = {});

        /// Constructs a connection_pool.
        ///
        /// Each connection in the pool is authenticated as \p _username.
        ///
        /// This constructor allows use of non-native authentication schemes (e.g. PAM, Kerberos, etc).
        ///
        /// \param[in] _size      The number of connections to create.
        /// \param[in] _host      The hostname of the iRODS server.
        /// \param[in] _port      The port to connect to.
        /// \param[in] _username  The name of the user to connect as.
        /// \param[in] _auth_func The callback used for authenticating each connection.
        /// \param[in] _options   The connection pool options.
        ///
        /// \throws irods::exception If an error occurs.
        ///
        /// \since 4.3.1
        connection_pool(int _size,
                        const std::string& _host,
                        const int _port,
                        experimental::fully_qualified_username _username,
                        std::function<void(RcComm&)> _auth_func,
                        const connection_pool_options& _options = {});

        /// Constructs a connection_pool.
        ///
        /// Each connection in the pool is authenticated as \p _proxy_username if provided.
        /// Otherwise, the connections are authenticated as \p _username. API operations are
        /// always executed as \p _username.
        ///
        /// This constructor allows use of non-native authentication schemes (e.g. PAM, Kerberos, etc).
        ///
        /// \param[in] _size           The number of connections to create.
        /// \param[in] _host           The hostname of the iRODS server.
        /// \param[in] _port           The port to connect to.
        /// \param[in] _proxy_username The name of the user acting as the proxy.
        /// \param[in] _username       The name of the user being proxied.
        /// \param[in] _auth_func      The callback used for authenticating each connection.
        /// \param[in] _options        The connection pool options.
        ///
        /// \throws irods::exception If an error occurs.
        ///
        /// \since 4.3.1
        connection_pool(int _size,
                        std::string_view _host,
                        const int _port,
                        std::optional<experimental::fully_qualified_username> _proxy_username,
                        experimental::fully_qualified_username _username,
                        std::function<void(RcComm&)> _auth_func,
                        const connection_pool_options& _options = {});

        /// Deletes copy construction.
        connection_pool(const connection_pool&) = delete;

        /// Deletes copy assignment.
        connection_pool& operator=(const connection_pool&) = delete;

        /// Returns a connection from the pool.
        ///
        /// This function will block if all connections are in use.
        ///
        /// \return A proxy managing the retrieved connection.
        ///
        /// \since 4.2.5
        connection_proxy get_connection();

      private:
        /// Unique pointer type used to manage pooled connections.
        using connection_pointer = std::unique_ptr<RcComm, int (*)(RcComm*)>;

        /// Tracks state associated with a single pooled connection.
        struct connection_context
        {
            std::mutex mutex{}; ///< Protects access to this connection context.
            std::atomic<bool> in_use{}; ///< Indicates whether the connection is checked out.
            bool refresh{}; ///< Indicates whether the connection should be refreshed.
            connection_pointer conn{nullptr, rcDisconnect}; ///< Managed connection handle.
            std::chrono::steady_clock::time_point creation_time; ///< Time the connection was created.
            std::string latest_resc_mtime; ///< Latest observed resource modification timestamp.
            std::int32_t resc_count{}; ///< Latest observed resource count.
            std::int16_t retrieval_count{}; ///< Number of times the connection was retrieved.
        }; // struct connection_context

        /// Creates or recreates the connection at the specified index.
        ///
        /// \param[in] _index Pool slot to populate.
        /// \param[in] _on_connect_error Callback invoked on connection failure.
        /// \param[in] _on_login_error Callback invoked on authentication failure.
        void create_connection(int _index,
                               const std::function<void()>& _on_connect_error,
                               const std::function<void()>& _on_login_error);

        /// Refreshes the connection stored at the specified index.
        ///
        /// \param[in] _index Pool slot to refresh.
        /// \return Pointer to the refreshed connection.
        RcComm* refresh_connection(int _index);

        /// Verifies that the connection at the specified index is still usable.
        ///
        /// \param[in] _index Pool slot to inspect.
        /// \retval true If the connection is valid.
        /// \retval false Otherwise.
        bool verify_connection(int _index);

        /// Returns a checked-out connection to the pool.
        ///
        /// \param[in] _index Pool slot to release.
        void return_connection(int _index);

        /// Releases pool ownership of the connection at the specified index.
        ///
        /// \param[in] _index Pool slot to detach.
        void release_connection(int _index);

        const std::string host_; ///< Hostname for new pooled connections.
        const int port_; ///< Server port for new pooled connections.
        const std::optional<experimental::fully_qualified_username> proxy_username_; ///< Optional proxy identity.
        const experimental::fully_qualified_username username_; ///< User identity for API execution.
        std::function<void(RcComm&)> auth_func_; ///< Authentication callback for each connection.
        std::vector<connection_context> conn_ctxs_; ///< State for all pooled connections.
        connection_pool_options options_; ///< Active connection refresh options.
    }; // class connection_pool

    /// Constructs a connection_pool on the heap.
    ///
    /// The connection_pool is constructed using client credentials defined in
    /// irods_environment.json.
    ///
    /// \param[in] _size The number of connections to create.
    ///
    /// \returns A shared pointer to a connection_pool.
    ///
    /// \since 4.2.8
    std::shared_ptr<connection_pool> make_connection_pool(int _size = 1);
} // namespace irods

#endif // IRODS_CONNECTION_POOL_HPP
