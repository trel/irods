#undef IRODS_IO_DEFAULT_TRANSPORT_HPP_INCLUDE_HEADER

#if defined(IRODS_IO_TRANSPORT_ENABLE_SERVER_SIDE_API)
#  if !defined(IRODS_IO_DEFAULT_TRANSPORT_HPP_FOR_SERVER)
#    define IRODS_IO_DEFAULT_TRANSPORT_HPP_FOR_SERVER
#    define IRODS_IO_DEFAULT_TRANSPORT_HPP_INCLUDE_HEADER
#  endif
#elif !defined(IRODS_IO_DEFAULT_TRANSPORT_HPP_FOR_CLIENT)
#  define IRODS_IO_DEFAULT_TRANSPORT_HPP_FOR_CLIENT
#  define IRODS_IO_DEFAULT_TRANSPORT_HPP_INCLUDE_HEADER
#endif

#ifdef IRODS_IO_DEFAULT_TRANSPORT_HPP_INCLUDE_HEADER

#undef NAMESPACE_IMPL
#undef rxComm
#undef rxDataObjRead
#undef rxDataObjWrite
#undef rxDataObjLseek
#undef rx_replica_close
#undef rx_replica_open

// clang-format off
#ifdef IRODS_IO_TRANSPORT_ENABLE_SERVER_SIDE_API
#  include "irods/rs_replica_open.hpp"
#  include "irods/rs_replica_close.hpp"
#  include "irods/rsDataObjRead.hpp"
#  include "irods/rsDataObjWrite.hpp"
#  include "irods/rsDataObjLseek.hpp"

#  define NAMESPACE_IMPL                  server

#  define rxComm                          rsComm_t

#  define rxDataObjRead                   rsDataObjRead
#  define rxDataObjWrite                  rsDataObjWrite
#  define rxDataObjLseek                  rsDataObjLseek
#  define rx_replica_open                 rs_replica_open
#  define rx_replica_close                rs_replica_close
#else
#  include "irods/replica_open.h"
#  include "irods/replica_close.h"
#  include "irods/dataObjRead.h"
#  include "irods/dataObjWrite.h"
#  include "irods/dataObjLseek.h"

#  define NAMESPACE_IMPL                  client

#  define rxComm                          rcComm_t

#  define rxDataObjRead                   rcDataObjRead
#  define rxDataObjWrite                  rcDataObjWrite
#  define rxDataObjLseek                  rcDataObjLseek
#  define rx_replica_open                 rc_replica_open
#  define rx_replica_close                rc_replica_close
#endif // IRODS_IO_TRANSPORT_ENABLE_SERVER_SIDE_API
// clang-format on

#include "irods/rcMisc.h"
#include "irods/transport/transport.hpp"
#include "irods/irods_at_scope_exit.hpp"
#include "irods/irods_hierarchy_parser.hpp"

#include <nlohmann/json.hpp>

#include <cstdlib>
#include <string>
#include <vector>

namespace irods::experimental::io::NAMESPACE_IMPL
{
    /// Default transport implementation backed by the native iRODS data object APIs.
    template <typename CharT>
    class basic_transport : public transport<CharT>
    {
    public:
        // clang-format off
        using char_type   = typename transport<CharT>::char_type;
        using traits_type = typename transport<CharT>::traits_type;
        using int_type    = typename traits_type::int_type;
        using pos_type    = typename traits_type::pos_type;
        using off_type    = typename traits_type::off_type;
        // clang-format on

    private:
        // clang-format off
        inline static constexpr auto uninitialized_file_descriptor = -1; ///< Sentinel for a closed transport.
        inline static constexpr auto minimum_valid_file_descriptor = 3; ///< Lowest valid descriptor expected from iRODS.

        // Errors
        inline static constexpr auto translation_error             = -1; ///< Indicates open-mode translation failure.
        inline static const     auto seek_error                    = pos_type{off_type{-1}}; ///< Indicates seek failure.
        // clang-format on

    public:
        /// Constructs a transport bound to an existing iRODS connection.
        ///
        /// \param[in] _comm The connection object used for replica operations.
        explicit basic_transport(rxComm& _comm)
            : transport<CharT>{}
            , comm_{&_comm}
            , fd_{uninitialized_file_descriptor}
            , root_resc_name_{}
            , leaf_resc_name_{}
            , replica_number_{}
            , replica_token_{}
        {
        }

        /// Opens a data object using server-side policy for replica selection.
        bool open(const irods::experimental::filesystem::path& _path,
                  std::ios_base::openmode _mode) override
        {
            return open_impl(_path, _mode, [](auto&) {});
        }

        /// Opens an existing replica identified by replica number.
        bool open(const irods::experimental::filesystem::path& _path,
                  const replica_number& _replica_number,
                  std::ios_base::openmode _mode) override
        {
            return open_impl(_path, _mode, [_replica_number](auto& _input) {
                const auto replica = std::to_string(_replica_number.value);
                addKeyVal(&_input.condInput, REPL_NUM_KW, replica.c_str());

                // Providing a replica number implies that the replica already exists.
                // This constructor does not support creation of new replicas.
                _input.openFlags &= ~O_CREAT;
            });
        }

        /// Opens or creates a replica identified by root resource.
        bool open(const irods::experimental::filesystem::path& _path,
                  const root_resource_name& _root_resource_name,
                  std::ios_base::openmode _mode) override
        {
            // This leaves the decision making to the server (i.e. the policy).
            return open_impl(_path, _mode, [&_root_resource_name](auto& _input) {
                addKeyVal(&_input.condInput, RESC_NAME_KW, _root_resource_name.value.c_str());
            });
        }

        /// Opens or creates a replica identified by leaf resource.
        bool open(const irods::experimental::filesystem::path& _path,
                  const leaf_resource_name& _leaf_resource_name,
                  std::ios_base::openmode _mode) override
        {
            // This is when the client knows exactly where the replica should reside.
            return open_impl(_path, _mode, [&_leaf_resource_name](auto& _input) {
                addKeyVal(&_input.condInput, LEAF_RESOURCE_NAME_KW, _leaf_resource_name.value.c_str());
            });
        }

        /// Reopens an existing replica using a replica token and replica number.
        bool open(const replica_token& _replica_token,
                  const irods::experimental::filesystem::path& _path,
                  const replica_number& _replica_number,
                  std::ios_base::openmode _mode) override
        {
            return open_impl(_path, _mode, [_replica_token, _replica_number](auto& _input) {
                const auto replica = std::to_string(_replica_number.value);
                addKeyVal(&_input.condInput, REPLICA_TOKEN_KW, _replica_token.value.data());
                addKeyVal(&_input.condInput, REPL_NUM_KW, replica.c_str());

                // Providing a replica number implies that the replica already exists.
                // This constructor does not support creation of new replicas.
                _input.openFlags &= ~O_CREAT;
            });
        }

        /// Reopens an existing replica using a replica token and leaf resource.
        bool open(const replica_token& _replica_token,
                  const irods::experimental::filesystem::path& _path,
                  const leaf_resource_name& _leaf_resource_name,
                  std::ios_base::openmode _mode) override
        {
            return open_impl(_path, _mode, [_replica_token, &_leaf_resource_name](auto& _input) {
                addKeyVal(&_input.condInput, REPLICA_TOKEN_KW, _replica_token.value.data());
                addKeyVal(&_input.condInput, LEAF_RESOURCE_NAME_KW, _leaf_resource_name.value.c_str());
            });
        }

        /// Closes the currently open replica.
        bool close(const on_close_success* _on_close_success = nullptr) override
        {
            using json = nlohmann::json;

            json json_input{{"fd", fd_}};

            if (_on_close_success) {
                json_input["update_size"] = _on_close_success->update_size;
                json_input["update_status"] = _on_close_success->update_status;
                json_input["compute_checksum"] = _on_close_success->compute_checksum;
                json_input["send_notifications"] = _on_close_success->send_notifications;
                json_input["preserve_replica_state_table"] = _on_close_success->preserve_replica_state_table;
            }

            const auto json_string = json_input.dump();

            if (const auto ec = rx_replica_close(comm_, json_string.c_str()); ec != 0) {
                return false;
            }

            fd_ = uninitialized_file_descriptor;

            return true;
        }

        /// Receives bytes from the server into the caller-provided buffer.
        std::streamsize receive(char_type* _buffer, std::streamsize _buffer_size) override
        {
            openedDataObjInp_t input{};
            input.l1descInx = fd_;
            input.len = _buffer_size;

            bytesBuf_t output{};
            output.len = input.len;
            output.buf = _buffer;

            return rxDataObjRead(comm_, &input, &output);
        }

        /// Sends bytes from the caller-provided buffer to the server.
        std::streamsize send(const char_type* _buffer, std::streamsize _buffer_size) override
        {
            openedDataObjInp_t input{};
            input.l1descInx = fd_;
            input.len = _buffer_size;

            bytesBuf_t input_buffer{};
            input_buffer.len = input.len;
            input_buffer.buf = const_cast<char_type*>(_buffer);

            return rxDataObjWrite(comm_, &input, &input_buffer);
        }

        /// Repositions the underlying replica offset.
        pos_type seekpos(off_type _offset, std::ios_base::seekdir _dir) override
        {
            openedDataObjInp_t input{};
            input.l1descInx = fd_;
            input.offset = _offset;

            switch (_dir) {
                case std::ios_base::beg:
                    input.whence = SEEK_SET;
                    break;

                case std::ios_base::cur:
                    input.whence = SEEK_CUR;
                    break;

                case std::ios_base::end:
                    input.whence = SEEK_END;
                    break;

                default:
                    return seek_error;
            }

            fileLseekOut_t* output{};

            at_scope_exit free_memory{[&output] {
                if (output) { // NOLINT(readability-implicit-bool-conversion)
                    std::free(output); // NOLINTNEXTLINE(cppcoreguidelines-owning-memory, cppcoreguidelines-no-malloc)
                }
            }};

            if (const auto ec = rxDataObjLseek(comm_, &input, &output); ec < 0) {
                return seek_error;
            }

            return output->offset;
        }

        /// Indicates whether the transport currently owns an open descriptor.
        bool is_open() const noexcept override
        {
            return fd_ >= minimum_valid_file_descriptor;
        }

        /// Returns the transport's current file descriptor.
        int file_descriptor() const noexcept override
        {
            return fd_;
        }

        /// Returns the root resource name for the open replica.
        const root_resource_name& root_resource_name() const override
        {
            return root_resc_name_;
        }

        /// Returns the leaf resource name for the open replica.
        const leaf_resource_name& leaf_resource_name() const override
        {
            return leaf_resc_name_;
        }

        /// Returns the replica number for the open replica.
        const replica_number& replica_number() const override
        {
            return replica_number_;
        }

        /// Returns the replica token for the open replica.
        const replica_token& replica_token() const override
        {
            return replica_token_;
        }

    private:
        /// Translates standard stream open modes to native iRODS flags.
        int make_open_flags(std::ios_base::openmode _mode) noexcept
        {
            using std::ios_base;

            const auto m = _mode & ~(ios_base::ate | ios_base::binary);

            if (ios_base::in == m) {
                return O_RDONLY;
            }
            else if (ios_base::out == m || (ios_base::out | ios_base::trunc) == m) {
                return O_CREAT | O_WRONLY | O_TRUNC;
            }
            else if (ios_base::app == m || (ios_base::out | ios_base::app) == m) {
                return O_CREAT | O_WRONLY | O_APPEND;
            }
            else if ((ios_base::out | ios_base::in) == m) {
                return O_RDWR;
            }
            else if ((ios_base::out | ios_base::in | ios_base::trunc) == m) {
                return O_CREAT | O_RDWR | O_TRUNC;
            }
            else if ((ios_base::out | ios_base::in | ios_base::app) == m ||
                     (ios_base::in | ios_base::app) == m)
            {
                return O_CREAT | O_RDWR | O_APPEND;
            }

            return translation_error;
        }

        /// Seeks to end when the open mode requires it.
        bool seek_to_end_if_required(std::ios_base::openmode _mode)
        {
            if (std::ios_base::ate & _mode) {
                if (seek_error == seekpos(0, std::ios_base::end)) {
                    return false;
                }
            }

            return true;
        }

        /// Applies common open logic and fills replica metadata on success.
        template <typename Function>
        bool open_impl(const filesystem::path& _path, std::ios_base::openmode _mode, Function _func)
        {
            const auto flags = make_open_flags(_mode);

            if (flags == translation_error) {
                return false;
            }

            dataObjInp_t input{};
            at_scope_exit free_memory{[&input] { clearKeyVal(&input.condInput); }};

            // Set dataSize to invalid size to signal unknown size for streaming.
            input.dataSize = -1;
            input.createMode = 0600;
            input.openFlags = flags;
            rstrcpy(input.objPath, _path.c_str(), sizeof(input.objPath));

            _func(input);

            char* json_output{}; 
            at_scope_exit free_json_output{[&json_output] {
                if (json_output) {
                    std::free(json_output);
                }
            }};

            const auto fd = rx_replica_open(comm_, &input, &json_output);

            if (fd < minimum_valid_file_descriptor) {
                return false;
            }

            fd_ = fd;

            try {
                const auto fd_info = nlohmann::json::parse(json_output);

                const auto& dobj_info = fd_info.at("data_object_info");
                const hierarchy_parser hp{dobj_info.at("resource_hierarchy").get_ref<const std::string&>()};

                root_resc_name_.value = hp.first_resc();
                leaf_resc_name_.value = hp.last_resc();
                replica_number_.value = dobj_info.at("replica_number").get<int>();
                replica_token_.value = fd_info.at("replica_token").get_ref<const std::string&>();
            }
            catch (const nlohmann::json::parse_error& e) {
                close();
                return false;
            }

            if (!seek_to_end_if_required(_mode)) {
                close();
                return false;
            }

            return true;
        }

        rxComm* comm_; ///< Connection used for all transport operations.
        int fd_; ///< Native iRODS descriptor for the open replica.
        struct root_resource_name root_resc_name_; ///< Root resource name for the open replica.
        struct leaf_resource_name leaf_resc_name_; ///< Leaf resource name for the open replica.
        struct replica_number replica_number_; ///< Replica number for the open replica.
        struct replica_token replica_token_; ///< Replica token for the open replica.
    }; // basic_transport

    // clang-format off
    using default_transport = basic_transport<char>; ///< Default native transport specialization.
    using native_transport  = basic_transport<char>; ///< Alias for the native transport specialization.
    // clang-format on
} // irods::experimental::io::NAMESPACE_IMPL

#endif // IRODS_IO_DEFAULT_TRANSPORT_HPP_INCLUDE_HEADER
