
#include <future>

/// \file

namespace irods {
    /// Holds promises for asynchronously collected iRODS errors.
    class future
    {
    public:
        // clang-format off
        /// Represents an error code and message pair.
        using error_type   = std::tuple<int, std::string>;
        /// Represents a collection of error code and message pairs.
        using errors_type  = std::vector<error_type>;
        /// Represents the promise type used to publish job results.
        using promise_type = std::promise<error_type>;
        // clang-format on

        /// Constructs a future bound to the shared stop flag.
        future(std::atomic_bool& s) : stop_flag_{s}
        {
        }

        /// Adds a promise to the collection of tracked promises.
        void push_back(std::shared_ptr<promise_type> p) {
            promises.push_back(p);
        }

        /// Collects all failing results from the tracked promises.
        auto get() const {
            errors_type x{};
            x.reserve(promises.size());

            for(auto&& p : promises) {
                if(stop_flag_) {
                    break;
                }

                auto&& e = p->get_future().get();
                if(std::get<0>(e) < 0) {
                    x.emplace_back(std::move(e));
                }
            }

            return x;
        } // get

    private:
        /// Indicates whether pending work should stop early.
        std::atomic_bool& stop_flag_;
        /// Stores the promises associated with dispatched jobs.
        std::vector<std::shared_ptr<promise_type>> promises;
    }; // class future

} // irods
