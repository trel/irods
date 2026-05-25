#ifndef IRODS_THREADS_HPP
#define IRODS_THREADS_HPP

#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>

/// =-=-=-=-=-=-=-
/// @brief C / C++ wrapper for client side threads
struct thread_context {
    boost::thread*              reconnThr; ///< Background thread used for reconnection handling.
    boost::mutex*               lock; ///< Mutex protecting the thread context state.
    boost::condition_variable*  cond; ///< Condition variable used to coordinate reconnection work.
};

#endif // IRODS_THREADS_HPP


