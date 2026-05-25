#ifndef IRODS_THREAD_POOL_HPP
#define IRODS_THREAD_POOL_HPP

/// \file

#include <utility>

#include <boost/asio.hpp>

#include "irods/irods_exception.hpp"

namespace irods
{
    /// Wraps a Boost.Asio thread pool for dispatching work.
    class thread_pool
    {
    public:
        /// Constructs a thread pool with the specified number of threads.
        explicit thread_pool(int _size)
            : pool_{static_cast<std::size_t>(_size)}
        {
        }

        /// Waits for all worker threads to finish.
        void join()
        {
            pool_.join();
        }

        /// Stops the thread pool.
        void stop()
        {
            pool_.stop();
        }

        /// Dispatches a function for immediate execution when possible.
        template <typename Function>
        static void dispatch(thread_pool& _pool, Function&& _func)
        {
            boost::asio::dispatch(_pool.pool_, std::forward<Function>(_func));
        }

        /// Posts a function for asynchronous execution.
        template <typename Function>
        static void post(thread_pool& _pool, Function&& _func)
        {
            boost::asio::post(_pool.pool_, std::forward<Function>(_func));
        }

        /// Defers execution of a function to the thread pool.
        template <typename Function>
        static void defer(thread_pool& _pool, Function&& _func)
        {
            boost::asio::defer(_pool.pool_, std::forward<Function>(_func));
        }

    private:
        /// Holds the underlying Boost.Asio thread pool.
        boost::asio::thread_pool pool_;
    };
} // namespace irods

#endif // IRODS_THREAD_POOL_HPP
