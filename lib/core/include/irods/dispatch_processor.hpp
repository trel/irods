#ifndef IRODS_DISPATCH_PROCESSOR_HPP
#define IRODS_DISPATCH_PROCESSOR_HPP

/// \file

#include "irods/thread_pool.hpp"
#include "irods/connection_pool.hpp"
#include "irods/irods_exception.hpp"
#include "irods/future.hpp"
#include "irods/rodsErrorTable.h"

#include <string>
#include <functional>
#include <vector>
#include <tuple>
#include <exception>

namespace irods::experimental
{
    /// Dispatches a job across items from an iterator-backed range.
    template <typename IteratorType>
    class dispatch_processor
    {
    public:
        /// Function type invoked for each item in the range.
        using job = std::function<void (const typename IteratorType::value_type)>;

        /// Constructs a dispatch processor over the given range and job.
        dispatch_processor(std::atomic_bool& stop_flag, const IteratorType& _i, job _j)
            : stop_flag_(stop_flag)
            , iterator_{_i}
            , job_{_j}
        {
        }

        /// Copy construction is disabled.
        dispatch_processor(const dispatch_processor&) = delete;

        /// Copy assignment is disabled.
        dispatch_processor& operator=(const dispatch_processor&) = delete;

        /// Dispatches work items to the provided thread pool.
        auto execute(thread_pool& _tp) -> future
        {
            future f{stop_flag_};

            try {
                for (auto r : iterator_) {
                    if(stop_flag_) {
                        break;
                    }
                    auto p = std::make_shared<future::promise_type>();

                    f.push_back(p);

                    thread_pool::post(_tp, [this, p, r] () mutable {
                        try {
                            job_(r);
                            try {p->set_value({0, ""});} catch(...){}
                        }
                        catch (const irods::exception& e) {
                            try{p->set_value({e.code(), e.what()});} catch(...) {}
                        }
                        catch (const std::exception& e) {
                            try{p->set_value({SYS_UNKNOWN_ERROR, e.what()});} catch(...) {}
                        }
                        catch (...) {
                            try{p->set_value({SYS_UNKNOWN_ERROR, "Unknown error occurred while processing job."});} catch(...) {}
                        }
                    });

                } // for row
            }
            catch(const irods::exception& e) {
                rodsLog(LOG_ERROR, "exception caught in dispatch_processor");
            }

            return f;
        }

    private:
        /// Shared stop flag observed by dispatched work.
        std::atomic_bool& stop_flag_;

        /// Range of items to process.
        IteratorType iterator_;

        /// Job invoked for each item in the range.
        job job_;
    };
} // namespace irods::experimental

#endif // IRODS_DISPATCH_PROCESSOR_HPP
