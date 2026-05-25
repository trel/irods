#ifndef IRODS_QUERY_PROCESSOR_HPP
#define IRODS_QUERY_PROCESSOR_HPP

#include "irods/thread_pool.hpp"
#include "irods/irods_query.hpp"
#include "irods/irods_exception.hpp"

#include <string>
#include <functional>
#include <future>
#include <vector>
#include <tuple>
#include <exception>

namespace irods
{
    /// Executes query results as jobs on a thread pool.
    template <typename ConnectionType>
    class query_processor
    {
    public:
        // clang-format off
        using error        = std::tuple<int, std::string>; ///< Error code and message pair returned by a job.
        using errors       = std::vector<error>; ///< Collection of job errors.
        using result_row   = typename query<ConnectionType>::value_type; ///< Row type produced by the query.
        using job          = std::function<void (const result_row&)>; ///< Callable invoked for each query row.
        using query_type   = typename query<ConnectionType>::query_type; ///< Query type accepted by the underlying query API.
        // clang-format on

        /// Collects asynchronous job completion state.
        class future
        {
        public:
            /// Waits for all queued jobs and returns the failed ones.
            auto get() -> errors
            {
                errors errs;
                errs.reserve(promises.size());

                for (auto&& p : promises) {
                    auto&& e = p->get_future().get();
                    if (std::get<0>(e) < 0) {
                        errs.push_back(std::move(e));
                    }
                }

                return errs;
            }

            /// Returns the number of queued jobs.
            auto size() const noexcept -> std::uint32_t
            {
                return promises.size();
            }

            friend query_processor;

        private:
            /// Adds a promise representing one queued job.
            auto push_back(std::shared_ptr<std::promise<error>> p) -> void
            {
                promises.push_back(p);
            }

            /// Promises tracking all queued jobs.
            std::vector<std::shared_ptr<std::promise<error>>> promises;
        }; // class future

        /// Constructs a processor for the provided query and job.
        query_processor(const std::string& _query,
                        job _job,
                        uint32_t _limit = 0,
                        irods::query_type _type = irods::query_type::general)
            : query_{_query}
            , job_{_job}
            , limit_{_limit}
            , type_{_type}
        {
        }

        /// Constructs a processor using the deprecated nested `query_type` alias.
        [[deprecated("use irods::query_type")]]
        query_processor(const std::string& _query,
                        job _job,
                        uint32_t _limit,
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated"
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
                        query_type _type
#pragma GCC diagnostic pop
            )
            : query_processor{_query, _job, _limit, static_cast<irods::query_type>(_type)}
        {
        }

        /// Deletes copy construction.
        query_processor(const query_processor&) = delete;
        /// Deletes copy assignment.
        query_processor& operator=(const query_processor&) = delete;

        /// Executes the query and schedules one job per result row.
        ///
        /// \return A future used to wait for the scheduled jobs.
        auto execute(thread_pool& _thread_pool, ConnectionType& _conn) -> future
        {
            future f;
            query<ConnectionType> q{&_conn, query_, limit_, 0, type_};

            for (auto&& r : q) {
               auto p = std::make_shared<std::promise<error>>();
               f.push_back(p);

               thread_pool::post(_thread_pool, [this, p, r]() mutable noexcept {
                    try {
                        job_(r);
                        p->set_value({0, ""});
                    }
                    catch (const irods::exception& e) {
                        p->set_value({e.code(), e.what()});
                    }
                    catch (const std::exception& e) {
                        p->set_value({SYS_UNKNOWN_ERROR, e.what()});
                    }
                    catch (...) {
                        p->set_value({SYS_UNKNOWN_ERROR, "Unknown error occurred while processing job."});
                    }
                });

            } // for row

            return f;
        }

    private:
        std::string query_; ///< Query text to execute.
        job job_; ///< Job invoked for each result row.
        uint32_t limit_; ///< Maximum number of rows to process.
        irods::query_type type_; ///< Query flavor used for execution.
    }; // class query_processor
} // namespace irods

#endif // IRODS_QUERY_PROCESSOR_HPP
