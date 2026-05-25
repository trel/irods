#ifndef IRODS_IO_DSTREAM_HPP
#define IRODS_IO_DSTREAM_HPP

#include "irods/filesystem/path.hpp"
#include "irods/transport/transport.hpp"

#include <streambuf>
#include <type_traits>
#include <array>
#include <string>
#include <stdexcept>
#include <algorithm>
#include <utility>
#include <cstring>

namespace irods::experimental::io
{
    // Details about what each virtual function in this template are required to
    // do can be found at the following link:
    //
    //      https://en.cppreference.com/w/cpp/io/basic_streambuf
    //
    /// Stream buffer implementation for iRODS data object I/O.
    template <typename CharT,
              typename Traits = std::char_traits<CharT>>
    class basic_data_object_buf final
        : public std::basic_streambuf<CharT, Traits>
    {
    public:
        // clang-format off
        using char_type   = CharT; ///< Character type handled by the buffer.
        using traits_type = Traits; ///< Character traits type.
        using int_type    = typename traits_type::int_type; ///< Integer type used by the traits.
        using pos_type    = typename traits_type::pos_type; ///< Absolute stream position type.
        using off_type    = typename traits_type::off_type; ///< Relative stream offset type.
        // clang-format on

        static_assert(std::is_same_v<char_type, char>, R"(character type must be "char")");

    private:
        using base_type = std::basic_streambuf<CharT, Traits>; ///< Base stream buffer type.

        // clang-format off
        inline static constexpr auto buffer_size          = 4096; ///< Size of the internal transfer buffer in characters.

        // Errors
        inline static constexpr auto external_write_error = -1; ///< Sentinel returned when a transport write fails.
        inline static const     auto seek_error           = pos_type{off_type{-1}}; ///< Sentinel returned when a seek fails.
        // clang-format on

    public:
        /// Constructs a closed stream buffer.
        basic_data_object_buf()
            : base_type{}
            , buf_{}
            , transport_{}
        {
        }

        /// Move-constructs a stream buffer from another stream buffer.
        ///
        /// \param[in] _other The stream buffer to move from.
        basic_data_object_buf(basic_data_object_buf&& _other)
            : basic_data_object_buf{}
        {
            swap(_other);
        }

        /// Move-assigns the state of another stream buffer.
        ///
        /// \param[in] _other The stream buffer to move from.
        /// \return A reference to this stream buffer.
        basic_data_object_buf& operator=(basic_data_object_buf&& _other)
        {
            close();
            swap(_other);
            return *this;
        }

        /// Destroys the stream buffer after attempting to close it.
        ~basic_data_object_buf()
        {
            close();
        }

        /// Exchanges the state of this stream buffer with another.
        ///
        /// \param[in,out] _other The stream buffer to exchange state with.
        void swap(basic_data_object_buf& _other)
        {
            using std::swap;

            base_type::swap(_other);
            swap(transport_, _other.transport_);
            swap(buf_, _other.buf_);
        }

        /// Exchanges the state of two stream buffers.
        ///
        /// \param[in,out] _lhs The first stream buffer.
        /// \param[in,out] _rhs The second stream buffer.
        friend void swap(basic_data_object_buf& _lhs, basic_data_object_buf& _rhs)
        {
            _lhs.swap(_rhs);
        }

        /// Indicates whether the underlying transport has an open replica.
        ///
        /// \return True if the underlying transport is open.
        bool is_open() const noexcept
        {
            return transport_ && transport_->is_open();
        }
	
        /// Opens a data object using the provided transport and path.
        ///
        /// \param[in,out] _transport The transport used for I/O.
        /// \param[in] _path The logical path to the data object.
        /// \param[in] _mode The stream open mode.
        /// \return This stream buffer on success, or null on failure.
        basic_data_object_buf* open(transport<char_type>& _transport,
                                    const filesystem::path& _path,
                                    std::ios_base::openmode _mode)
        {
            transport_ = &_transport;

            if (!transport_->open(_path, _mode)) {
                return nullptr;
            }

            init_get_or_put_area(_mode);

            return this;
        }

        /// Opens a specific replica of a data object.
        ///
        /// \param[in,out] _transport The transport used for I/O.
        /// \param[in] _path The logical path to the data object.
        /// \param[in] _replica_number The replica number to open.
        /// \param[in] _mode The stream open mode.
        /// \return This stream buffer on success, or null on failure.
        basic_data_object_buf* open(transport<char_type>& _transport,
                                    const filesystem::path& _path,
                                    const replica_number& _replica_number,
                                    std::ios_base::openmode _mode)
        {
            transport_ = &_transport;

            if (!transport_->open(_path, _replica_number, _mode)) {
                return nullptr;
            }

            init_get_or_put_area(_mode);

            return this;
        }

        /// Opens a data object replica selected by root resource.
        ///
        /// \param[in,out] _transport The transport used for I/O.
        /// \param[in] _path The logical path to the data object.
        /// \param[in] _root_resource_name The root resource identifying the replica.
        /// \param[in] _mode The stream open mode.
        /// \return This stream buffer on success, or null on failure.
        basic_data_object_buf* open(transport<char_type>& _transport,
                                    const filesystem::path& _path,
                                    const root_resource_name& _root_resource_name,
                                    std::ios_base::openmode _mode)
        {
            transport_ = &_transport;

            if (!transport_->open(_path, _root_resource_name, _mode)) {
                return nullptr;
            }

            init_get_or_put_area(_mode);

            return this;
        }

        /// Opens a data object replica selected by leaf resource.
        ///
        /// \param[in,out] _transport The transport used for I/O.
        /// \param[in] _path The logical path to the data object.
        /// \param[in] _leaf_resource_name The leaf resource identifying the replica.
        /// \param[in] _mode The stream open mode.
        /// \return This stream buffer on success, or null on failure.
        basic_data_object_buf* open(transport<char_type>& _transport,
                                    const filesystem::path& _path,
                                    const leaf_resource_name& _leaf_resource_name,
                                    std::ios_base::openmode _mode)
        {
            transport_ = &_transport;

            if (!transport_->open(_path, _leaf_resource_name, _mode)) {
                return nullptr;
            }

            init_get_or_put_area(_mode);

            return this;
        }

        /// Reopens a specific replica using a replica token.
        ///
        /// \param[in,out] _transport The transport used for I/O.
        /// \param[in] _replica_token The token authorizing access to the replica.
        /// \param[in] _path The logical path to the data object.
        /// \param[in] _replica_number The replica number to open.
        /// \param[in] _mode The stream open mode.
        /// \return This stream buffer on success, or null on failure.
        basic_data_object_buf* open(transport<char_type>& _transport,
                                    const replica_token& _replica_token,
                                    const filesystem::path& _path,
                                    const replica_number& _replica_number,
                                    std::ios_base::openmode _mode)
        {
            transport_ = &_transport;

            if (!transport_->open(_replica_token, _path, _replica_number, _mode)) {
                return nullptr;
            }

            init_get_or_put_area(_mode);

            return this;
        }

        /// Reopens a replica on a specific leaf resource using a replica token.
        ///
        /// \param[in,out] _transport The transport used for I/O.
        /// \param[in] _replica_token The token authorizing access to the replica.
        /// \param[in] _path The logical path to the data object.
        /// \param[in] _leaf_resource_name The leaf resource identifying the replica.
        /// \param[in] _mode The stream open mode.
        /// \return This stream buffer on success, or null on failure.
        basic_data_object_buf* open(transport<char_type>& _transport,
                                    const replica_token& _replica_token,
                                    const filesystem::path& _path,
                                    const leaf_resource_name& _leaf_resource_name,
                                    std::ios_base::openmode _mode)
        {
            transport_ = &_transport;

            if (!transport_->open(_replica_token, _path, _leaf_resource_name, _mode)) {
                return nullptr;
            }

            init_get_or_put_area(_mode);

            return this;
        }

        /// Closes the currently open replica.
        ///
        /// \param[in] _on_close_success Optional close directives for the server.
        /// \return This stream buffer on success, or null on failure.
        basic_data_object_buf* close(const on_close_success* _on_close_success = nullptr)
        {
            if (!transport_ || !transport_->is_open()) {
                return nullptr;
            }

            auto* sb = this;

            if (this->sync()) {
                sb = nullptr;
            }

            if (!transport_->close(_on_close_success)) {
                sb = nullptr;
            }

            return sb;
        }

        /// Returns the file descriptor exposed by the transport.
        ///
        /// \return The transport-specific file descriptor.
        int file_descriptor() const noexcept
        {
            return transport_->file_descriptor();;
        }

        /// Returns the root resource name for the open replica.
        ///
        /// \return The root resource name associated with the open replica.
        const root_resource_name& root_resource_name() const
        {
            return transport_->root_resource_name();
        }

        /// Returns the leaf resource name for the open replica.
        ///
        /// \return The leaf resource name associated with the open replica.
        const leaf_resource_name& leaf_resource_name() const
        {
            return transport_->leaf_resource_name();
        }

        /// Returns the replica number for the open replica.
        ///
        /// \return The replica number associated with the open replica.
        const replica_number& replica_number() const
        {
            return transport_->replica_number();
        }

        /// Returns the replica token for the open replica.
        ///
        /// \return The replica token associated with the open replica.
        const replica_token& replica_token() const
        {
            return transport_->replica_token();
        }

    protected:
        /// Refills the get area when more input is required.
        ///
        /// \return The next available character, or EOF on failure or end-of-input.
        int_type underflow() override
        {
            prepare_for_input();

            // If the "Get" area has not been fully consumed, then return the
            // current character.
            if (this->gptr() < this->egptr()) {
                return traits_type::to_int_type(*this->gptr());
            }

            // The "Get" area has been consumed. Fill the internal buffer with
            // new data from the data object.

            const auto bytes_read = transport_->receive(buf_.data(), buf_.size() * sizeof(char_type));

            if (bytes_read <= 0) {
                return traits_type::eof();
            }

            auto* pbase = buf_.data();
            this->setg(pbase, pbase, pbase + bytes_read);

            return traits_type::to_int_type(*this->gptr());
        }

        /// Flushes buffered output and optionally writes one additional character.
        ///
        /// \param[in] _c The optional character to write after flushing.
        /// \return EOF on failure, or a non-EOF value on success.
        int_type overflow(int_type _c = traits_type::eof()) override
        {
            prepare_for_output();

            if (flush_buffer() == external_write_error) {
                return traits_type::eof();
            }

            if (!traits_type::eq_int_type(_c, traits_type::eof())) {
                return this->sputc(_c);
            }

            return traits_type::not_eof(_c);
        }

        /// Reads a sequence of characters into the caller-provided buffer.
        ///
        /// \param[out] _buffer The destination buffer.
        /// \param[in] _buffer_size The number of characters requested.
        /// \return The number of characters read from the underlying transport.
        std::streamsize xsgetn(char_type* _buffer, std::streamsize _buffer_size) override
        {
            prepare_for_input();

            const auto bytes_to_copy = this->egptr() - this->gptr(); 

            // If there are bytes in the internal buffer that haven't been consumed,
            // then copy those bytes from the internal buffer into "_buffer".
            if (bytes_to_copy > 0) {
                std::memcpy(_buffer, this->gptr(), bytes_to_copy * sizeof(char_type));
                this->gbump(bytes_to_copy);
            }

            return transport_->receive(_buffer + bytes_to_copy, (_buffer_size - bytes_to_copy) * sizeof(char_type));
        }

        /// Writes a sequence of characters from the caller-provided buffer.
        ///
        /// \param[in] _buffer The source buffer.
        /// \param[in] _buffer_size The number of characters to write.
        /// \return The number of characters written, or a negative value on failure.
        std::streamsize xsputn(const char_type* _buffer, std::streamsize _buffer_size) override
        {
            prepare_for_output();

            if (flush_buffer() == external_write_error) {
                return external_write_error;
            }

            return transport_->send(_buffer, _buffer_size * sizeof(char_type));
        }

        /// Flushes any pending output.
        ///
        /// \return Zero on success, or a non-zero value on failure.
        int sync() override
        {
            if (this->pptr()) {
                return flush_buffer();
            }

            return 0;
        }

        /// Pushes a character back into the get area.
        ///
        /// \param[in] _c The character to push back, or EOF to only move the get pointer.
        /// \return EOF on failure, or a non-EOF value on success.
        int_type pbackfail(int_type _c = traits_type::eof()) override
        {
            // If the "next" pointer of the "Get" area points to the beginning of the
            // "Get" area, then return immediately.
            if (this->gptr() == this->eback()) {
                return traits_type::eof();
            }

            // Decrement the "next" pointer.
            this->gbump(-1);

            // If the "next" pointer is not pointing to "EOF", then store "_c" at the location
            // pointed at by the "next" pointer.
            if (!traits_type::eq_int_type(_c, traits_type::eof())) {
                *this->gptr() = traits_type::to_char_type(_c);
            }

            return traits_type::not_eof(_c);
        }

        /// Repositions the stream by an offset relative to a direction.
        ///
        /// \param[in] _off The offset to apply.
        /// \param[in] _dir The base position used to interpret \p _off.
        /// \param[in] _which The active stream sequences being repositioned.
        /// \return The resulting position, or an error position on failure.
        pos_type seekoff(off_type _off,
                         std::ios_base::seekdir _dir,
                         std::ios_base::openmode _which = std::ios_base::in | std::ios_base::out) override
        {
            if (this->sync() != 0) {
                return seek_error;
            }

            return transport_->seekpos(_off, _dir);
        }

        /// Repositions the stream to an absolute position.
        ///
        /// \param[in] _pos The absolute position to seek to.
        /// \param[in] _which The active stream sequences being repositioned.
        /// \return The resulting position, or an error position on failure.
        pos_type seekpos(pos_type _pos, std::ios_base::openmode _which = std::ios_base::in | std::ios_base::out) override
        {
            if (this->sync() != 0) {
                return seek_error;
            }

            return transport_->seekpos(_pos, std::ios_base::beg);
        }

    private:
        /// Prepares the get area for reading.
        void prepare_for_input()
        {
            // Return if the pointers of "Get" area have already been set up.
            if (this->gptr()) {
                return;
            }

            // Flush and clear the contents of the "Put" area.
            this->sync();
            this->setp(nullptr, nullptr);

            // Setup the "Get" area.
            auto* pbase = buf_.data();
            this->setg(pbase, pbase, pbase);
        }

        /// Prepares the put area for writing.
        void prepare_for_output()
        {
            // Return if the pointers of "Put" area have already been set up.
            if (this->pptr()) {
                return;
            }

            // Clear the contents of the "Get" area.
            this->setg(nullptr, nullptr, nullptr);

            // Setup the "Put" area.
            auto* pbase = buf_.data();
            this->setp(pbase, pbase + buf_.size());
        }

        /// Initializes the active stream area based on `_mode`.
        void init_get_or_put_area(std::ios_base::openmode _mode) noexcept
        {
            using std::ios_base;

            const auto m = ios_base::in | ios_base::out;

            if ((_mode & m) == m || _mode & ios_base::in) {
                prepare_for_input();
            }
            else if (_mode & ios_base::out) {
                prepare_for_output();
            }
        }

        /// Flushes buffered output through the transport.
        int flush_buffer()
        {
            const auto bytes_to_send = this->pptr() - this->pbase();

            if (bytes_to_send == 0) {
                return 0;
            }

            const auto bytes_written = transport_->send(buf_.data(), bytes_to_send * sizeof(char_type));

            if (bytes_written < 0) {
                return external_write_error;
            }

            this->pbump(-bytes_written);

            return 0;
        }

        std::array<char_type, buffer_size> buf_; ///< Internal transfer buffer.
        transport<char_type>* transport_; ///< Transport used to reach the server.
    }; // basic_data_object_buf

    // Provides a default openmode for basic_dstream constructors and open()
    // member functions based on the type "T". "T" must be one of the general
    // stream types defined by the C++ standard (i.e. basic_istream, basic_ostream,
    // or basic_iostream).
    /// Default open mode selected for a given standard stream type.
    template <typename>
    inline static constexpr std::ios_base::openmode default_openmode{};

    /// Default open mode for input streams.
    template <typename T>
    inline static constexpr std::ios_base::openmode default_openmode<std::basic_istream<T>> = std::ios_base::in;

    /// Default open mode for output streams.
    template <typename T>
    inline static constexpr std::ios_base::openmode default_openmode<std::basic_ostream<T>> = std::ios_base::out;

    /// Default open mode for input/output streams.
    template <typename T>
    inline static constexpr std::ios_base::openmode default_openmode<std::basic_iostream<T>> = std::ios_base::in | std::ios_base::out;

    // mandatory_openmode used to be private members of basic_dstream, but due
    // to a bug in GCC (https://gcc.gnu.org/bugzilla/show_bug.cgi?id=90031),
    // it was moved outside of the class.
    /// Details used by `basic_dstream` implementation.
    namespace detail {
        /// Mandatory open mode bits for a given stream category.
        template <typename>
        inline static constexpr std::ios_base::openmode mandatory_openmode{};

        /// Mandatory open mode bits for input streams.
        template <typename T>
        inline static constexpr std::ios_base::openmode mandatory_openmode<std::basic_istream<T>> = std::ios_base::in;

        /// Mandatory open mode bits for output streams.
        template <typename T>
        inline static constexpr std::ios_base::openmode mandatory_openmode<std::basic_ostream<T>> = std::ios_base::out;
    }

    // A concrete stream class template that wraps a basic_data_object_buf object.
    // The general stream used to instantiate this type must use "char" for the underlying
    // character type. Using wchar_t or anything else to instantiate this template is undefined.
    // This class is modeled after the C++ standard file stream classes.
    /// Standard stream wrapper backed by an iRODS data object stream buffer.
    template <typename GeneralStream>
    class basic_dstream final
        : public GeneralStream
    {
    public:
        // clang-format off
        using char_type   = typename GeneralStream::char_type; ///< Character type handled by the stream.
        using traits_type = typename GeneralStream::traits_type; ///< Character traits type.
        using int_type    = typename traits_type::int_type; ///< Integer type used by the traits.
        using pos_type    = typename traits_type::pos_type; ///< Absolute stream position type.
        using off_type    = typename traits_type::off_type; ///< Relative stream offset type.
        // clang-format on

        static_assert(std::is_same_v<char_type, char>, R"(character type must be "char")");

        /// Constructs a closed stream.
        basic_dstream()
            : GeneralStream{&buf_}
            , buf_{}
        {
        }

        /// Constructs and opens a stream for the given data object path.
        ///
        /// \param[in,out] _transport The transport used for I/O.
        /// \param[in] _path The logical path to the data object.
        /// \param[in] _mode The stream open mode.
        basic_dstream(transport<char_type>& _transport,
                      const filesystem::path& _path,
                      std::ios_base::openmode _mode = default_openmode<GeneralStream>)
            : basic_dstream{}
        {
            open(_transport, _path, _mode);
        }

        /// Constructs and opens a stream for a specific replica number.
        ///
        /// \param[in,out] _transport The transport used for I/O.
        /// \param[in] _path The logical path to the data object.
        /// \param[in] _replica_number The replica number to open.
        /// \param[in] _mode The stream open mode.
        basic_dstream(transport<char_type>& _transport,
                      const filesystem::path& _path,
                      const replica_number& _replica_number,
                      std::ios_base::openmode _mode = default_openmode<GeneralStream>)
            : basic_dstream{}
        {
            open(_transport, _path, _replica_number, _mode);
        }

        /// Constructs and opens a stream for a replica selected by root resource.
        ///
        /// \param[in,out] _transport The transport used for I/O.
        /// \param[in] _path The logical path to the data object.
        /// \param[in] _root_resource_name The root resource identifying the replica.
        /// \param[in] _mode The stream open mode.
        basic_dstream(transport<char_type>& _transport,
                      const filesystem::path& _path,
                      const root_resource_name& _root_resource_name,
                      std::ios_base::openmode _mode = default_openmode<GeneralStream>)
            : basic_dstream{}
        {
            open(_transport, _path, _root_resource_name, _mode);
        }

        /// Constructs and opens a stream for a replica selected by leaf resource.
        ///
        /// \param[in,out] _transport The transport used for I/O.
        /// \param[in] _path The logical path to the data object.
        /// \param[in] _leaf_resource_name The leaf resource identifying the replica.
        /// \param[in] _mode The stream open mode.
        basic_dstream(transport<char_type>& _transport,
                      const filesystem::path& _path,
                      const leaf_resource_name& _leaf_resource_name,
                      std::ios_base::openmode _mode = default_openmode<GeneralStream>)
            : basic_dstream{}
        {
            open(_transport, _path, _leaf_resource_name, _mode);
        }

        /// Constructs and opens a stream for a replica number using a replica token.
        ///
        /// \param[in,out] _transport The transport used for I/O.
        /// \param[in] _replica_token The token authorizing access to the replica.
        /// \param[in] _path The logical path to the data object.
        /// \param[in] _replica_number The replica number to open.
        /// \param[in] _mode The stream open mode.
        basic_dstream(transport<char_type>& _transport,
                      const replica_token& _replica_token,
                      const filesystem::path& _path,
                      const replica_number& _replica_number,
                      std::ios_base::openmode _mode = default_openmode<GeneralStream>)
            : basic_dstream{}
        {
            open(_transport, _replica_token, _path, _replica_number, _mode);
        }

        /// Constructs and opens a stream for a leaf-resource replica using a replica token.
        ///
        /// \param[in,out] _transport The transport used for I/O.
        /// \param[in] _replica_token The token authorizing access to the replica.
        /// \param[in] _path The logical path to the data object.
        /// \param[in] _leaf_resource_name The leaf resource identifying the replica.
        /// \param[in] _mode The stream open mode.
        basic_dstream(transport<char_type>& _transport,
                      const replica_token& _replica_token,
                      const filesystem::path& _path,
                      const leaf_resource_name& _leaf_resource_name,
                      std::ios_base::openmode _mode = default_openmode<GeneralStream>)
            : basic_dstream{}
        {
            open(_transport, _replica_token, _path, _leaf_resource_name, _mode);
        }

        /// Move-constructs a stream from another stream.
        ///
        /// \param[in] _other The stream to move from.
        basic_dstream(basic_dstream&& _other)
            : GeneralStream{std::move(_other)}
            , buf_{std::move(_other.buf_)}
        {
            this->set_rdbuf(&buf_);
        }

        /// Move-assigns the state of another stream.
        ///
        /// \param[in] _other The stream to move from.
        /// \return A reference to this stream.
        basic_dstream& operator=(basic_dstream&& _other)
        {
            GeneralStream::operator=(std::move(_other));
            buf_ = std::move(_other.buf_);
            return *this;
        }

        /// Destroys the stream.
        ~basic_dstream() = default;

        /// Exchanges the state of this stream with another stream.
        ///
        /// \param[in,out] _other The stream to exchange state with.
        void swap(basic_dstream& _other)
        {
            GeneralStream::swap(_other);
            buf_.swap(_other.buf_);
        }

        /// Exchanges the state of two streams.
        ///
        /// \param[in,out] _lhs The first stream.
        /// \param[in,out] _rhs The second stream.
        friend void swap(basic_dstream& _lhs, basic_dstream& _rhs)
        {
            _lhs.swap(_rhs);
        }

        /// Returns the underlying stream buffer.
        ///
        /// \return A pointer to the underlying data object stream buffer.
        basic_data_object_buf<char_type, traits_type>* rdbuf() const
        {
            return const_cast<basic_data_object_buf<char_type, traits_type>*>(&buf_);
        }

        /// Indicates whether the underlying stream buffer is open.
        ///
        /// \return True if the stream buffer is open.
        bool is_open() const noexcept
        {
            return buf_.is_open();
        }

        /// Opens a data object using the provided transport and path.
        ///
        /// \param[in,out] _transport The transport used for I/O.
        /// \param[in] _path The logical path to the data object.
        /// \param[in] _mode The stream open mode.
        void open(transport<char_type>& _transport,
                  const filesystem::path& _path,
                  std::ios_base::openmode _mode = default_openmode<GeneralStream>)
        {
            if (!buf_.open(_transport, _path, _mode | detail::mandatory_openmode<GeneralStream>)) {
                this->setstate(std::ios_base::failbit);
            }
            else {
                this->clear();
            }
        }

        /// Opens a specific replica of a data object.
        ///
        /// \param[in,out] _transport The transport used for I/O.
        /// \param[in] _path The logical path to the data object.
        /// \param[in] _replica_number The replica number to open.
        /// \param[in] _mode The stream open mode.
        void open(transport<char_type>& _transport,
                  const filesystem::path& _path,
                  const replica_number& _replica_number,
                  std::ios_base::openmode _mode = default_openmode<GeneralStream>)
        {
            if (!buf_.open(_transport, _path, _replica_number, _mode | detail::mandatory_openmode<GeneralStream>)) {
                this->setstate(std::ios_base::failbit);
            }
            else {
                this->clear();
            }
        }

        /// Opens a data object replica selected by root resource.
        ///
        /// \param[in,out] _transport The transport used for I/O.
        /// \param[in] _path The logical path to the data object.
        /// \param[in] _root_resource_name The root resource identifying the replica.
        /// \param[in] _mode The stream open mode.
        void open(transport<char_type>& _transport,
                  const filesystem::path& _path,
                  const root_resource_name& _root_resource_name,
                  std::ios_base::openmode _mode = default_openmode<GeneralStream>)
        {
            if (!buf_.open(_transport, _path, _root_resource_name, _mode | detail::mandatory_openmode<GeneralStream>)) {
                this->setstate(std::ios_base::failbit);
            }
            else {
                this->clear();
            }
        }

        /// Opens a data object replica selected by leaf resource.
        ///
        /// \param[in,out] _transport The transport used for I/O.
        /// \param[in] _path The logical path to the data object.
        /// \param[in] _leaf_resource_name The leaf resource identifying the replica.
        /// \param[in] _mode The stream open mode.
        void open(transport<char_type>& _transport,
                  const filesystem::path& _path,
                  const leaf_resource_name& _leaf_resource_name,
                  std::ios_base::openmode _mode = default_openmode<GeneralStream>)
        {
            if (!buf_.open(_transport, _path, _leaf_resource_name, _mode | detail::mandatory_openmode<GeneralStream>)) {
                this->setstate(std::ios_base::failbit);
            }
            else {
                this->clear();
            }
        }

        /// Opens a specific replica using a replica token.
        ///
        /// \param[in,out] _transport The transport used for I/O.
        /// \param[in] _replica_token The token authorizing access to the replica.
        /// \param[in] _path The logical path to the data object.
        /// \param[in] _replica_number The replica number to open.
        /// \param[in] _mode The stream open mode.
        void open(transport<char_type>& _transport,
                  const replica_token& _replica_token,
                  const filesystem::path& _path,
                  const replica_number& _replica_number,
                  std::ios_base::openmode _mode = default_openmode<GeneralStream>)
        {
            if (!buf_.open(_transport, _replica_token, _path, _replica_number, _mode | detail::mandatory_openmode<GeneralStream>)) {
                this->setstate(std::ios_base::failbit);
            }
            else {
                this->clear();
            }
        }

        /// Opens a leaf-resource replica using a replica token.
        ///
        /// \param[in,out] _transport The transport used for I/O.
        /// \param[in] _replica_token The token authorizing access to the replica.
        /// \param[in] _path The logical path to the data object.
        /// \param[in] _leaf_resource_name The leaf resource identifying the replica.
        /// \param[in] _mode The stream open mode.
        void open(transport<char_type>& _transport,
                  const replica_token& _replica_token,
                  const filesystem::path& _path,
                  const leaf_resource_name& _leaf_resource_name,
                  std::ios_base::openmode _mode = default_openmode<GeneralStream>)
        {
            if (!buf_.open(_transport, _replica_token, _path, _leaf_resource_name, _mode | detail::mandatory_openmode<GeneralStream>)) {
                this->setstate(std::ios_base::failbit);
            }
            else {
                this->clear();
            }
        }

        /// Closes the currently open replica.
        ///
        /// \param[in] _on_close_success Optional close directives for the server.
        void close(const on_close_success* _on_close_success = nullptr)
        {
            if (!buf_.close(_on_close_success)) {
                this->setstate(std::ios_base::failbit);
            }
            else {
                this->clear();
            }
        }

        /// Returns the file descriptor exposed by the stream buffer.
        ///
        /// \return The transport-specific file descriptor.
        int file_descriptor() const noexcept
        {
            return buf_.file_descriptor();
        }

        /// Returns the root resource name for the open replica.
        ///
        /// \return The root resource name associated with the open replica.
        const root_resource_name& root_resource_name() const
        {
            return buf_.root_resource_name();
        }

        /// Returns the leaf resource name for the open replica.
        ///
        /// \return The leaf resource name associated with the open replica.
        const leaf_resource_name& leaf_resource_name() const
        {
            return buf_.leaf_resource_name();
        }

        /// Returns the replica number for the open replica.
        ///
        /// \return The replica number associated with the open replica.
        const replica_number& replica_number() const
        {
            return buf_.replica_number();
        }

        /// Returns the replica token for the open replica.
        ///
        /// \return The replica token associated with the open replica.
        const replica_token& replica_token() const
        {
            return buf_.replica_token();
        }

    private:
        basic_data_object_buf<char_type, traits_type> buf_; ///< Underlying data object stream buffer.
    }; // basic_dstream

    // clang-format off
    //
    // The concrete stream types.
    // For details on how to use these types, refer to the following links:
    //
    //    https://en.cppreference.com/w/cpp/io/basic_fstream
    //    https://en.cppreference.com/w/cpp/io/basic_ifstream
    //    https://en.cppreference.com/w/cpp/io/basic_ofstream
    //
    using data_object_buf = basic_data_object_buf<char>; ///< Stream buffer type for iRODS data objects.
    using idstream        = basic_dstream<std::basic_istream<char>>; ///< Input stream type for iRODS data objects.
    using odstream        = basic_dstream<std::basic_ostream<char>>; ///< Output stream type for iRODS data objects.
    using dstream         = basic_dstream<std::basic_iostream<char>>; ///< Input/output stream type for iRODS data objects.
    // clang-format on
} // namespace irods::experimental::io

#endif // IRODS_IO_DSTREAM_HPP
