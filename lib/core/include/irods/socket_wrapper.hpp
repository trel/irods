#ifndef SOCKET_WRAPPER_HPP__
#define SOCKET_WRAPPER_HPP__

// =-=-=-=-=-=-=-
// Boost Includes
#include <boost/asio.hpp>

namespace irods {

    /// @brief Abstract wrapper around Boost.Asio socket types.
    class socket_wrapper {

        public:
            /// @brief Constructs a socket wrapper.
            socket_wrapper() {}

            /// @brief Destroys the socket wrapper.
            virtual ~socket_wrapper() {}

            /// @brief Opens the socket around an existing native descriptor.
            virtual bool open( int, const char* ) = 0;

    }; // class socket_wrapper

    /// @brief Socket wrapper implementation for TCP sockets.
    class socket_wrapper_tcp : public socket_wrapper {
            /// @brief Underlying Boost.Asio TCP socket.
            boost::asio::ip::tcp::socket* sock_;
        public:
            /// @brief Constructs a TCP socket wrapper.
            socket_wrapper_tcp();

            /// @brief Destroys the TCP socket wrapper.
            ~socket_wrapper_tcp();

            /// @brief Opens the TCP socket around an existing native descriptor.
            virtual bool open( int, const char* );

    }; // class socket_wrapper_tcp

} // namespace irods

#endif //  SOCKET_WRAPPER_HPP__
