#ifndef IRODS_DELAY_QUEUE_HPP
#define IRODS_DELAY_QUEUE_HPP

#include <boost/version.hpp>

#if BOOST_VERSION < 107200
#  include "irods/capped_memory_resource.hpp"
#else // BOOST_VERSION < 107200
#  include "irods/fixed_buffer_resource.hpp"
#endif // BOOST_VERSION < 107200

#include <boost/container/pmr/global_resource.hpp>
#include <boost/container/pmr/vector.hpp>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#include <boost/container/pmr/string.hpp>
#pragma GCC diagnostic pop

#include <cstddef>
#include <algorithm>
#include <mutex>
#include <vector>
#include <memory>

namespace irods
{
    /// @brief Tracks queued delay rule identifiers.
    class delay_queue // NOLINT(cppcoreguidelines-special-member-functions)
    {
      public:
        /// @brief Constructs a delay queue with an optional bounded memory pool.
        explicit delay_queue(std::int64_t _pool_size_in_bytes)
        {
            namespace bpmr = boost::container::pmr;
            namespace ipmr = experimental::pmr;

            if (_pool_size_in_bytes > 0) {
#if BOOST_VERSION < 107200
                allocator_.reset(new ipmr::capped_memory_resource(_pool_size_in_bytes));
#else // BOOST_VERSION < 107200
                buffer_.resize(_pool_size_in_bytes);

                // DO NOT wrap this memory resource in a pool resource (e.g. unsynchronized_pool_resource). Doing
                // so has shown that it can lead to the delay server not making progress on delay rule execution.
                // The goal of this memory resource is to limit memory usage. Wrapping it adds no additional value.
                allocator_ = std::make_unique<ipmr::fixed_buffer_resource<std::byte>>(buffer_.data(), buffer_.size());
#endif // BOOST_VERSION < 107200
                queued_rules_ = std::make_unique<bpmr::vector<boost::container::pmr::string>>(allocator_.get());
            }
            else {
                queued_rules_ = std::make_unique<bpmr::vector<bpmr::string>>(bpmr::new_delete_resource());
            }
        }

        /// @brief Copy construction is disabled.
        delay_queue(const delay_queue&) = delete;

        /// @brief Copy assignment is disabled.
        delay_queue& operator=(const delay_queue&) = delete;

        /// @brief Returns whether the queue contains the provided rule identifier.
        bool contains_rule_id(const std::string& _rule_id)
        {
            std::lock_guard rules_lock{rules_mutex_};
            return std::any_of(queued_rules_->begin(), queued_rules_->end(), [&](const auto& q_rule) {
                return q_rule.data() == _rule_id;
            });
        }

        /// @brief Adds a rule identifier to the queue.
        void enqueue_rule(const std::string& rule_id)
        {
            std::lock_guard rules_lock{rules_mutex_};
            queued_rules_->emplace_back(rule_id.data());
        }

        /// @brief Removes a rule identifier from the queue.
        void dequeue_rule(const std::string& rule_id)
        {
            std::lock_guard rules_lock{rules_mutex_};
            for (auto it = queued_rules_->begin(); it != queued_rules_->end(); ++it) {
                if (*it == rule_id.data()) {
                    queued_rules_->erase(it);
                    break;
                }
            }
        }

      private:
        /// @brief Protects access to the queued rule list.
        std::mutex rules_mutex_;
#if BOOST_VERSION >= 107200
        /// @brief Owns storage used by the fixed buffer memory resource.
        std::vector<std::byte> buffer_;
#endif // BOOST_VERSION >= 107200

        /// @brief Allocator backing the queue storage.
        std::unique_ptr<boost::container::pmr::memory_resource> allocator_;

        /// @brief The queued rule identifiers.
        std::unique_ptr<boost::container::pmr::vector<boost::container::pmr::string>> queued_rules_;
    }; // delay_queue
} // namespace irods

#endif // IRODS_DELAY_QUEUE_HPP
