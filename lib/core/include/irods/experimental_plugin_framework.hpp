#ifndef IRODS_EXPERIMENTAL_PLUGIN_FRAMEWORK
#define IRODS_EXPERIMENTAL_PLUGIN_FRAMEWORK

// =-=-=-=-=-=-=-
// irods includes
#include "irods/rodsDef.h"
#include "irods/msParam.h"
#include "irods/rcConnect.h"
#include "irods/irods_plugin_base.hpp"

#include "irods/thread_pool.hpp"
#include "irods/connection_pool.hpp"
#include "irods/dispatch_processor.hpp"
#include "irods/query_builder.hpp"

// =-=-=-=-=-=-=-
// stl includes
#include <cctype>
#include <sstream>
#include <string>
#include <iostream>
#include <termios.h>
#include <unistd.h>
#include <thread>

#include <nlohmann/json.hpp>
/// @brief JSON type used by the experimental API framework.
using json = nlohmann::json;

namespace irods::experimental::api {
    /// @brief Atomic flag type used to coordinate long-running operations.
    using flag_type = std::atomic_bool;

    /// @brief Status strings published by experimental API operations.
    namespace states {
        /// @brief Operation is paused.
        const std::string paused{"paused"};
        /// @brief Operation failed.
        const std::string failed{"failed"};
        /// @brief Operation is currently running.
        const std::string running{"running"};
        /// @brief Operation state is not yet known.
        const std::string unknown{"unknown"};
        /// @brief Operation completed successfully.
        const std::string complete{"complete"};
    }; // states

    /// @brief Command strings accepted by experimental API operations.
    namespace commands {
        /// @brief Requests that an operation pause.
        const std::string pause{"pause"};
        /// @brief Requests that an operation resume.
        const std::string resume{"resume"};
        /// @brief Requests that an operation cancel.
        const std::string cancel{"cancel"};
        /// @brief Key naming the requested action.
        const std::string request{"request"};
        /// @brief Requests the latest progress update.
        const std::string progress{"progress"};
    }; // commands

    /// @brief Endpoint names exposed by experimental API plugins.
    namespace endpoints {
        /// @brief Command-handling endpoint name.
        const std::string command{"command"};
        /// @brief Primary operation endpoint name.
        const std::string operation{"operation"};
    }; // endpoints

    /// @brief Common JSON field names used by the framework.
    namespace constants {
        /// @brief Error-code field name.
        const std::string code{"code"};
        /// @brief Operation-status field name.
        const std::string status{"status"};
        /// @brief Plugin-name field name.
        const std::string plugin{"plugin"};
        /// @brief Error-payload field name.
        const std::string errors{"errors"};
        /// @brief Command field name.
        const std::string command{"command"};
        /// @brief Error-message field name.
        const std::string message{"message"};
        /// @brief Progress field name.
        const std::string progress{"progress"};
    }; // constants

    /// @brief Forward declaration for the experimental API plugin base class.
    class base;

    namespace {
        /// @brief Sleeps briefly between polling attempts.
        void sleep()
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }

        /// @brief Sends a JSON request to the experimental API adapter.
        /// @param[in] comm Connected client communication handle.
        /// @param[in] msg JSON payload to serialize and submit.
        /// @return Parsed JSON response from the adapter.
        auto invoke(rcComm_t& comm, const json& msg)
        {
            const auto ADAPTER_APN{120000};

            auto str = msg.dump();

            bytesBuf_t inp{};
            inp.buf = (void*)str.c_str();
            inp.len = str.size();

            bytesBuf_t* resp{};

            auto err = procApiRequest(&comm, ADAPTER_APN, (void*)&inp, NULL, (void**)&resp, NULL );

            if (err < 0) {
                THROW(err, "failed to perform the invocation");
            }

            const auto* buf = static_cast<char*>(resp->buf);

            return json::parse(buf, buf + resp->len);
        } // invoke

        /// @brief Returns a required value from a JSON document.
        /// @tparam T Value type to extract.
        /// @param[in] n Name of the JSON field to read.
        /// @param[in] p JSON document containing the field.
        /// @return Value converted to type `T`.
        template<typename T>
        auto get(const std::string& n, const json& p)
        {
            if(!p.contains(n)) {
                THROW(
                    SYS_INVALID_INPUT_PARAM,
                    boost::format("missing [%s] parameter")
                    % n);
            }

            return p.at(n).get<T>();
        } // get

        /// @brief Returns an optional value from a JSON document.
        /// @tparam T Value type to extract.
        /// @param[in] n Name of the JSON field to read.
        /// @param[in] p JSON document containing the field.
        /// @param[in] d Default value returned when the field is missing.
        /// @return Value converted to type `T` or `d`.
        template<typename T>
        auto get(const std::string& n, const json& p, T d)
        {
            if(!p.contains(n)) {
                return d;
            }

            return p.at(n).get<T>();
        } // get

        /// @brief Converts a completed future into blackboard status data.
        /// @param[in] f Future containing reported error tuples.
        /// @return Blackboard JSON describing completion or failure.
        auto to_blackboard(const irods::future& f) -> json
        {
            auto bb = json{{constants::status, states::complete}};

            try {
                auto xx = f.get();

                if(xx.size() > 0) {
                    auto arr = json::array();
                    for(auto&& e : xx) {
                        arr.push_back({{constants::code,    std::get<0>(e)},
                                       {constants::message, std::get<1>(e)}});
                    }

                    bb[constants::errors] = arr;
                    bb.update({{constants::status, states::failed}});
                }
            }
            catch(...) {
                return json{{constants::errors, {
                            {constants::code, SYS_INTERNAL_ERR},
                            {constants::message, "unknown error in to_blackboard"}}}};
            }

            return bb;

        } // to_blackboard

#ifdef RODS_SERVER
        /// @brief Loads an experimental API plugin by operation name.
        /// @param[in] operation Name of the plugin operation to load.
        /// @param[in] type Unused plugin type string.
        /// @return Pointer to the loaded plugin instance.
        static auto resolve_api_plugin(const std::string& operation, const std::string& type)
        {
            std::string lower{operation};
            std::transform(
                lower.begin(), lower.end(), lower.begin(), [](unsigned char _ch) { return std::tolower(_ch); });

            base* plugin{};
            auto err = irods::load_plugin<base>(
                            plugin,
                            operation,
                            "experimental",
                            "irods::experimental::api::base",
                            "empty_context" );
            if(!err.ok()) {
                THROW(err.code(), err.result());
            }

            return plugin;

        } // resolve_api_plugin
#endif

    } // namespace

    /// @brief Thread-safe wrapper around a JSON document.
    class locking_json
    {
    public:
        /// @brief Constructs the wrapper with an initial JSON value.
        /// @param[in] j Initial JSON content.
        locking_json(const json& j) : c_{j} {}

        /// @brief Replaces the stored JSON value.
        /// @param[in] j New JSON content.
        void set(const json& j)
        {
            std::scoped_lock l(m_);
            c_ = j;
        }

        /// @brief Returns a copy of the stored JSON value.
        /// @return The current JSON content.
        json get()
        {
            std::scoped_lock l(m_);
            return c_;
        }

        /// @brief Merges fields into the stored JSON value.
        /// @param[in] j JSON fields to merge.
        /// @return Updated JSON content.
        json update(const json& j)
        {
            std::scoped_lock l(m_);
            c_.update(j);
            return c_;
        }

    private:
        /// @brief Stored JSON content.
        json       c_;

        /// @brief Mutex protecting access to the stored JSON content.
        std::mutex m_;

    }; // locking_json

    /// @brief Callback invoked with serialized progress updates.
    using progress_handler_type = std::function<void(const std::string&)>;

    /// @brief Client-side helper for invoking experimental API plugins.
    class client
    {
    public:
        /// @brief Executes an experimental API request and polls for progress.
        /// @param[in] conn Connected client communication handle.
        /// @param[in] exit_flag Cancellation flag checked between polls.
        /// @param[in] progress_handler Callback receiving progress strings.
        /// @param[in] options Request options merged into the initial payload.
        /// @param[in] endpoint Target plugin endpoint.
        /// @return Final JSON response or serialized error information.
        json operator()(
            rcComm_t&             conn
          , flag_type&            exit_flag
          , progress_handler_type progress_handler
          , const json&           options
          , const std::string&    endpoint)
        {
            json req{}, rep{};

            try {
                req = {{commands::request, endpoints::operation},
                       {constants::plugin, endpoint}};

                req.update(options);

                rep = invoke(conn, req);

                std::string status{}, progress{}, command{};

                while(states::complete != status &&
                      states::failed   != status) {

                    sleep();

                    command = exit_flag ? commands::cancel
                                        : commands::progress;

                    req = {{commands::request,  endpoints::command},
                           {constants::command, command},
                           {constants::plugin,  endpoint}};

                    rep = invoke(conn, req);

                    status   = get<std::string>(constants::status,   rep, "");
                    progress = get<std::string>(constants::progress, rep, "");

                    progress_handler(progress);

                } // while
            }
            catch (const irods::exception& e) {
                return json{{constants::errors, {
                            {constants::code, e.code()},
                            {constants::message, e.what()}}}};
            }

            return rep;

        } // operator()

    }; // class client

    /// @brief Tracks progress and publishes percent complete.
    class progress_handler
    {
    public:
        /// @brief Starts asynchronous progress tracking.
        /// @param[in] comm Communication handle associated with the operation.
        /// @param[in] total Total number of work items expected.
        /// @param[in,out] b_board Shared blackboard updated with progress.
        /// @param[in,out] t_pool Thread pool used for background tracking.
        /// @param[in,out] e_flag Cancellation flag observed by the tracker.
        progress_handler(
            rcComm_t&          comm
          , uint64_t           total
          , locking_json&      b_board
          , thread_pool&       t_pool
          , flag_type&         e_flag) :
            exit_flag_{e_flag}
          , count_{}
          , total_{total}
          , blackboard_{b_board}
        {
            try {
                // launch the tracking thread
                thread_pool::post(t_pool, [&]() {
                    while(!exit_flag_ && !complete()) {
                        sleep();
                        auto t = static_cast<uint64_t>(100*(double)count_/(double)total_);
                        blackboard_.update({{constants::progress, std::to_string(t)}});
                    }
                });
            }
            catch(const irods::exception& e) {
                rodsLog(LOG_ERROR,
                        "exception caught in progress_handler %d:%s",
                        e.code(), e.what());
            }
        } // ctor

        /// @brief Increments the completed work-item count.
        void operator++(int) { count_++; }

        /// @brief Reports whether all work items have been processed.
        /// @return `true` if the completed count reached the total.
        bool complete() const { return count_ >= total_; }

    private:
        /// @brief Shared exit flag observed by the tracking task.
        flag_type&           exit_flag_;

        /// @brief Number of completed work items.
        std::atomic_uint64_t count_;

        /// @brief Total number of work items expected.
        std::atomic_uint64_t total_;

        /// @brief Blackboard updated with percent-complete information.
        locking_json&        blackboard_;
    }; // class progress_handler

    /// @brief Watches the blackboard for cancellation requests.
    class cancellation_handler
    {
    public:
        /// @brief Starts asynchronous cancellation monitoring.
        /// @param[in] p_hdlr Progress handler used to detect completion.
        /// @param[in,out] b_board Shared blackboard containing commands.
        /// @param[in,out] t_pool Thread pool used for background monitoring.
        /// @param[in,out] e_flag Cancellation flag set when cancel is requested.
        cancellation_handler(
            const progress_handler& p_hdlr
          , locking_json&           b_board
          , thread_pool&            t_pool
          , flag_type&              e_flag
          ) :
            exit_flag_{e_flag}
          , blackboard_{b_board}
          , p_handler_{p_hdlr}
        {
            thread_pool::post(t_pool, [&t_pool, this]() {
                while(!exit_flag_ && !p_handler_.complete()) {
                    sleep();

                    json bb = blackboard_.get();

                    if(bb.contains(constants::command) &&
                        bb.at(constants::command) == commands::cancel) {
                        exit_flag_ = true;
                        t_pool.stop();
                        blackboard_.update({{constants::status, states::complete}});
                        break;
                    }
                } // while
            });

        } // ctor

    private:
        /// @brief Shared exit flag set when cancellation is requested.
        flag_type&              exit_flag_;

        /// @brief Blackboard containing commands and status.
        locking_json&           blackboard_;

        /// @brief Progress handler used to determine whether work is complete.
        const progress_handler& p_handler_;
    }; // class cancellation_handler

    /// @brief Base class for experimental API plugins.
    class base : public irods::plugin_base
    {
    protected:
        /// @brief Single-thread pool used to run asynchronous operations.
        thread_pool async_pool{1};

        /// @brief Wraps a member function in the operation-table signature.
        #define WRAPPER(C, F) \
        std::function<json(C*, const json&)>([&](C* c, const json& j) -> json { \
                return F(c, j);})

        /// @brief Executes a registered operation immediately.
        /// @tparam COMM_T Communication object type.
        /// @param[in] comm Communication object passed to the operation.
        /// @param[in] n Operation table key.
        /// @param[in] req Request JSON for the operation.
        /// @return JSON response produced by the operation.
        template<typename COMM_T>
        json sync(COMM_T* comm, const std::string& n, const json& req)
        {
            if(operations_.find(n) == operations_.end()) {
                THROW(SYS_INVALID_INPUT_PARAM,
                      boost::format("call operation :: missing operation[%s]") % n);
            }

            using fcn_t = std::function<json(COMM_T*, const json&)>;

            auto op = boost::any_cast<fcn_t&>(operations_[n]);

            return op(comm, req);
        
        } // sync

        /// @brief Schedules a registered operation on the async pool.
        /// @tparam COMM_T Communication object type.
        /// @param[in] comm Communication object passed to the operation.
        /// @param[in] n Operation table key.
        /// @param[in] req Request JSON for the operation.
        /// @return JSON status indicating the request is running.
        template<typename COMM_T>
        json async(COMM_T* comm, const std::string& n, const json& req)
        {
            if(operations_.find(n) == operations_.end()) {
                THROW(SYS_INVALID_INPUT_PARAM,
                      boost::format("call operation :: missing operation[%s]") % n);
            }

            using fcn_t = std::function<json(COMM_T*, const json&)>;

            auto op = boost::any_cast<fcn_t&>(operations_[n]);

            std::function<void(void)> f = [=]() -> void {
                op(comm, req);
            };

            thread_pool::post(async_pool, f);

            return {{constants::status, states::running}};

        } // call

        /// @brief Shared state exchanged with command requests.
        locking_json blackboard{{constants::status, states::unknown}};

    public:

        /// @brief Registers standard experimental API operations.
        /// @param[in] n Plugin instance name.
        base(const std::string& n) : plugin_base(n, "empty_context_string")
        {
            operations_[endpoints::operation] = WRAPPER(rsComm_t, operation);
            operations_[endpoints::command]   = WRAPPER(rsComm_t, command);
        } // ctor

        /// @brief Waits for any outstanding asynchronous work to finish.
        virtual ~base() {
            async_pool.join();
        }

        /// @brief Dispatches a request to the named plugin operation.
        /// @tparam COMM_T Communication object type.
        /// @param[in] comm Communication object passed to the operation.
        /// @param[in] n Operation table key.
        /// @param[in] req Request JSON for the operation.
        /// @return JSON response produced synchronously or asynchronously.
        template<typename COMM_T>
        auto call(COMM_T* comm, const std::string& n, const json& req)
        {
            auto op = get<std::string>(commands::request, req);
            auto is_op = endpoints::operation == op;
            auto is_as = enable_asynchronous_operation();

            if(is_op && is_as) {
                return async(comm, n, req);
            }
            else {
                return sync(comm, n, req);
            }

        } // call

        /// @brief Indicates whether `operation()` should run asynchronously.
        /// @return `true` to schedule `operation()` on the async pool.
        virtual bool enable_asynchronous_operation() { return false; }

        /// @brief Executes the plugin's primary operation request.
        /// @param[in] comm Server communication handle.
        /// @param[in] req Request JSON payload.
        /// @return JSON response for the requested operation.
        virtual json operation(rsComm_t* comm, const json  req) = 0;

        /// @brief Handles command requests using the shared blackboard.
        /// @param[in] comm Server communication handle.
        /// @param[in] req Command JSON to merge into the blackboard.
        /// @return Updated blackboard JSON.
        virtual json   command(rsComm_t* comm, const json& req)
        {
            // using update() for bidirectional communication
            return blackboard.update(req);
        };

    }; // class base

} // namespace irods::experimental::api

#endif // IRODS_EXPERIMENTAL_PLUGIN_FRAMEWORK
