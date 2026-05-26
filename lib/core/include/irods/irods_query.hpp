#ifndef IRODS_QUERY_HPP
#define IRODS_QUERY_HPP

/// \file

#include "irods/specificQuery.h"

#ifdef IRODS_QUERY_ENABLE_SERVER_SIDE_API
    #include "irods/rsGenQuery.hpp"
    #include "irods/rsSpecificQuery.hpp"
#else
    #include "irods/genQuery.h"
#endif // IRODS_QUERY_ENABLE_SERVER_SIDE_API

#include "irods/irods_log.hpp"
#include "irods/rcMisc.h"

#include <fmt/format.h>

#include <algorithm>
#include <cctype>
#include <string>
#include <type_traits>
#include <vector>

namespace irods
{
    /// Identifies which query API to use.
    enum class query_type
    {
        general = 0,  ///< Execute a GenQuery request.
        specific = 1  ///< Execute a specific query request.
    };
} //namespace irods

/// \cond IRODS_DOXYGEN_INTERNAL
/// Formats `irods::query_type` values for `fmt` output.
template <>
struct fmt::formatter<irods::query_type> : fmt::formatter<std::underlying_type_t<irods::query_type>>
{
    /// Formats a query type as its underlying integer value.
    /// \param e query type to format.
    /// \param ctx formatting context receiving output.
    /// \return iterator positioned after the formatted value.
    constexpr auto format(const irods::query_type& e, format_context& ctx) const
    {
        return fmt::formatter<std::underlying_type_t<irods::query_type>>::format(
            static_cast<std::underlying_type_t<irods::query_type>>(e), ctx);
    }
};
/// \endcond

namespace irods
{
    /// Iterates over rows returned by a general or specific query.
    /// \tparam connection_type Connection type used to execute query requests.
    template <typename connection_type>
    class query {
    public:
        /// Row type returned by the iterator.
        using value_type = std::vector<std::string>;

        /// Deprecated query type enum retained for compatibility.
        enum [[deprecated("use irods::query_type")]] query_type
        {
            GENERAL = 0,  ///< Execute a GenQuery request.
            SPECIFIC = 1  ///< Execute a specific query request.
        };

        /// Converts a query type string to an enum value.
        /// \param _str query type name; empty selects `general`.
        /// \return matching query type.
        /// \throws irods::exception if the string is not recognized.
        static irods::query_type string_to_query_type(const std::string& _str)
        {
            // default option
            if(_str.empty()) {
                return irods::query_type::general;
            }

            const std::string GEN_STR{"general"};
            const std::string SPEC_STR{"specific"};

            std::string lowered{_str};
            std::transform(
                lowered.begin(), lowered.end(), lowered.begin(), [](unsigned char _ch) { return std::tolower(_ch); });

            if(GEN_STR == lowered) {
                return irods::query_type::general;
            }
            else if(SPEC_STR == lowered) {
                return irods::query_type::specific;
            }
            else {
                THROW(
                    SYS_INVALID_INPUT_PARAM,
                    _str + " - is not a query type");
            }
        } // string_to_query_type

        /// Converts a query type string to the deprecated enum.
        /// \param _str query type name; empty selects `GENERAL`.
        /// \return matching deprecated query type.
        [[deprecated("use string_to_query_type")]]
        static
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated"
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
            query_type
#pragma GCC diagnostic pop
            convert_string_to_query_type(const std::string& _str)
        {
            return static_cast<
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated"
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
                query_type
#pragma GCC diagnostic pop
                >(string_to_query_type(_str));
        }

        /// Base class shared by general and specific query implementations.
        class query_impl_base
        {
        public:
            /// Initializes shared state for a query implementation.
            /// \param _comm connection used to execute the query.
            /// \param _query_limit maximum rows to expose to callers.
            /// \param _row_offset starting row offset for the query.
            /// \param _query_string query text or specific query name.
            query_impl_base(connection_type*   _comm,
                            const uint32_t     _query_limit,
                            const uint32_t     _row_offset,
                            const std::string& _query_string)
                : comm_{_comm}
                , query_limit_{_query_limit}
                , row_offset_{_row_offset}
                , query_string_{_query_string}
                , gen_output_{}
            {
            }

            /// Releases any stored query results.
            virtual ~query_impl_base() {
                freeGenQueryOut(&this->gen_output_);
            }

            /// Returns the number of rows in the current page.
            /// \return row count for the loaded page.
            size_t size() {
                if(!gen_output_) {
                    return 0;
                }
                return gen_output_->rowCnt;
            }

            /// Returns the continuation index for the current query page.
            /// \return continuation index from the query output.
            int cont_idx() {
                return gen_output_->continueInx;
            }

            /// Returns the number of rows in the current page.
            /// \return current page row count.
            int row_cnt() {
                return gen_output_->rowCnt;
            }

            /// Returns the original query string.
            /// \return query text or specific query name.
            std::string query_string() {
                return query_string_;
            }

            /// Reports whether the configured row limit has been reached.
            /// \param _count rows processed so far.
            /// \return `true` if iteration should stop because of the limit.
            bool query_limit_exceeded(const uint32_t _count) {
                return query_limit_ && _count >= query_limit_;
            }

            /// Reports whether the requested row is in the current page.
            /// \param row_idx_ zero-based row index within iteration.
            /// \return `true` if the page already holds that row.
            bool page_in_flight(const int row_idx_) {
                return (row_idx_ < row_cnt());
            }

            /// Reports whether the query has no more pages.
            /// \return `true` if no continuation index remains.
            bool query_complete() {
                // finished page, and out of pages
                return cont_idx() <= 0;
            }

            /// Captures one row from the current page.
            /// \param _row_idx zero-based row index inside the page.
            /// \return column values for the requested row.
            value_type capture_results(int _row_idx) {
                value_type res;
                for(int attr_idx = 0; attr_idx < gen_output_->attriCnt; ++attr_idx) {
                    uint32_t offset = gen_output_->sqlResult[attr_idx].len * _row_idx;
                    std::string str{&gen_output_->sqlResult[attr_idx].value[offset]};
                    res.push_back(str);
                }
                return res;
            }

            /// Reports whether the current page contains rows.
            /// \return `true` if query output exists and has at least one row.
            bool results_valid() {
                if(gen_output_) {
                    return (gen_output_->rowCnt > 0);
                }
                else {
                    return false;
                }
            }

            /// Fetches the next page of results.
            /// \return irods error code from the query API.
            virtual int fetch_page() = 0;

            /// Releases the current page before loading the next one.
            virtual void reset_for_page_boundary() = 0;

            /// Returns the total row count reported by the server.
            /// \return total row count when available, otherwise zero.
            virtual int total_row_count() noexcept
            {
                return 0;
            } // total_row_count

        protected:
            /// Connection used to execute the query.
            connection_type* comm_;
            /// Maximum number of rows to expose.
            const uint32_t query_limit_;
            /// Starting row offset for the query.
            const uint32_t row_offset_;
            /// Original query text or specific query name.
            const std::string query_string_;
            /// Current page of query output.
            genQueryOut_t* gen_output_;
        }; // class query_impl_base

        /// Implements general query execution.
        class gen_query_impl : public query_impl_base
        {
        public:
            /// Prepares a general query for execution.
            /// \param _comm connection used to execute the query.
            /// \param _query_limit maximum rows to expose to callers.
            /// \param _row_offset starting row offset.
            /// \param _query_string general query string.
            /// \param _zone_hint zone hint applied to the request.
            /// \param _options extra general query options.
            gen_query_impl(connection_type*   _comm,
                           int                _query_limit,
                           int                _row_offset,
                           const std::string& _query_string,
                           const std::string& _zone_hint,
                           int                _options)
                : query_impl_base(_comm, _query_limit, _row_offset, _query_string)
            {
                memset(&gen_input_, 0, sizeof(gen_input_));
                gen_input_.maxRows = MAX_SQL_ROWS;
                gen_input_.rowOffset = _row_offset;

                if (!_zone_hint.empty()) {
                    addKeyVal(&gen_input_.condInput, ZONE_KW, _zone_hint.c_str());
                }

                const int fill_err = parse_genquery1_string(_query_string.c_str(), &gen_input_);
                if(fill_err < 0) {
                    THROW(fill_err, fmt::format("query fill failed for [{}]", _query_string));
                }

                gen_input_.options = _options;
            } // ctor

            /// Closes any open general query statement.
            virtual ~gen_query_impl() {
                if(this->gen_output_ && this->gen_output_->continueInx) {
                    rodsLog(LOG_NOTICE, "[%s] - continueInx is not 0", __FUNCTION__);
                    // Close statements for this query
                    gen_input_.continueInx = this->gen_output_->continueInx;
                    freeGenQueryOut(&this->gen_output_);
                    gen_input_.maxRows = 0;
                    auto err = gen_query_fcn(
                                   this->comm_,
                                   &gen_input_,
                                   &this->gen_output_);
                    if (CAT_NO_ROWS_FOUND != err && err < 0) {
                        irods::log(ERROR(err, fmt::format("[{}] - Failed to close statement with continueInx [{}]",
                                    __FUNCTION__, gen_input_.continueInx)));
                    }
                }

                clearGenQueryInp(&gen_input_);
            }

            /// Clears the current page and preserves the continuation index.
            void reset_for_page_boundary() override {
                if(this->gen_output_) {
                    gen_input_.continueInx = this->gen_output_->continueInx;
                    freeGenQueryOut(&this->gen_output_);
                }
            }

            /// Requests the next page of general query results.
            /// \return irods error code from the general query API.
            int fetch_page() override {
                return gen_query_fcn(
                           this->comm_,
                           &gen_input_,
                           &this->gen_output_);
            } // fetch_page

            /// Returns the server-reported total row count.
            /// \return total matching row count, or zero if unavailable.
            int total_row_count() noexcept override
            {
                if (!this->gen_output_) {
                    return 0;
                }

                return this->gen_output_->totalRowCount;
            } // total_row_count

        private:
            /// General query input passed to the server.
            genQueryInp_t gen_input_;
#ifdef IRODS_QUERY_ENABLE_SERVER_SIDE_API
            /// Function object used to execute general queries on the server side.
            const std::function<
                int(connection_type*,
                    genQueryInp_t*,
                    genQueryOut_t**)>
                        gen_query_fcn{rsGenQuery};
#else
            /// Function object used to execute general queries on the client side.
            const std::function<
                int(connection_type*,
                    genQueryInp_t*,
                    genQueryOut_t**)>
                        gen_query_fcn{rcGenQuery};
#endif // IRODS_QUERY_ENABLE_SERVER_SIDE_API
        }; // class gen_query_impl

        /// Implements specific query execution.
        class spec_query_impl : public query_impl_base
        {
        public:
            /// Prepares a specific query for execution.
            /// \param _comm connection used to execute the query.
            /// \param _query_limit maximum rows to expose to callers.
            /// \param _row_offset starting row offset.
            /// \param _query_string specific query name or SQL alias.
            /// \param _zone_hint zone hint applied to the request.
            /// \param _args optional bound arguments for the specific query.
            spec_query_impl(connection_type*                _comm,
                            int                             _query_limit,
                            int                             _row_offset,
                            const std::string&              _query_string,
                            const std::string&              _zone_hint,
                            const std::vector<std::string>* _args)
                : query_impl_base(_comm, _query_limit, _row_offset, _query_string)
            {
                memset(&spec_input_, 0, sizeof(spec_input_));
                spec_input_.maxRows = MAX_SQL_ROWS;
                spec_input_.sql = const_cast<char*>(_query_string.c_str());

                if (!_zone_hint.empty()) {
                    addKeyVal(&spec_input_.condInput, ZONE_KW, _zone_hint.c_str());
                }

                if (_args) {
                    for (decltype(_args->size()) i = 0; i < _args->size(); ++i) {
                        spec_input_.args[i] = const_cast<char*>((*_args)[i].data());
                    }
                }
            } // ctor

            /// Closes any open specific query statement.
            virtual ~spec_query_impl() {
                if(this->gen_output_ && this->gen_output_->continueInx) {
                    // Close statement for this query
                    spec_input_.continueInx = this->gen_output_->continueInx;
                    freeGenQueryOut(&this->gen_output_);
                    spec_input_.maxRows = 0;
                    auto err = spec_query_fcn(
                                   this->comm_,
                                   &spec_input_,
                                   &this->gen_output_);
                    if (CAT_NO_ROWS_FOUND != err && err < 0) {
                        irods::log(ERROR(err, fmt::format("[{}] - Failed to close statement with continueInx [{}]",
                                    __FUNCTION__, spec_input_.continueInx)));
                    }
                }

                clearKeyVal(&spec_input_.condInput);
            }

            /// Clears the current page and preserves the continuation index.
            void reset_for_page_boundary() override {
                if(this->gen_output_) {
                    spec_input_.continueInx = this->gen_output_->continueInx;
                    freeGenQueryOut(&this->gen_output_);
                }
            }

            /// Requests the next page of specific query results.
            /// \return irods error code from the specific query API.
            int fetch_page() override {
                return spec_query_fcn(
                           this->comm_,
                           &spec_input_,
                           &this->gen_output_);
            } // fetch_page

        private:
            /// Specific query input passed to the server.
            specificQueryInp_t spec_input_;
#ifdef IRODS_QUERY_ENABLE_SERVER_SIDE_API
            /// Function object used to execute specific queries on the server side.
            const std::function<
                int(connection_type*,
                    specificQueryInp_t*,
                    genQueryOut_t**)>
                        spec_query_fcn{rsSpecificQuery};
#else
            /// Function object used to execute specific queries on the client side.
            const std::function<
                int(connection_type*,
                    specificQueryInp_t*,
                    genQueryOut_t**)>
                        spec_query_fcn{rcSpecificQuery};
#endif // IRODS_QUERY_ENABLE_SERVER_SIDE_API
        }; // class spec_query_impl

        /// Forward iterator over query result rows.
        class iterator {
            /// Query text used for diagnostics.
            const std::string query_string_;
            /// Zero-based row index within the current page.
            uint32_t row_idx_;
            /// Total number of rows processed so far.
            uint32_t total_rows_processed_;
            /// Query input used for continuation diagnostics.
            genQueryInp_t* gen_input_;
            /// Indicates whether the iterator represents the end position.
            bool end_iteration_state_;

            /// Shared query implementation backing iteration.
            std::shared_ptr<query_impl_base> query_impl_;

            public:
            /// Value returned when dereferencing the iterator.
            using value_type        = value_type;
            /// Pointer type for iterator traits.
            using pointer           = const value_type*;
            /// Reference type for iterator traits.
            using reference         = value_type;
            /// Difference type for iterator traits.
            using difference_type   = value_type;
            /// Iterator category tag.
            using iterator_category = std::forward_iterator_tag;

            /// Constructs an end iterator.
            explicit iterator() :
                query_string_{},
                row_idx_{},
                total_rows_processed_{},
                gen_input_{},
                end_iteration_state_{true},
                query_impl_{} {
            }

            /// Constructs an iterator over a prepared query implementation.
            /// \param _qimp shared query implementation backing iteration.
            explicit iterator(std::shared_ptr<query_impl_base> _qimp) :
                query_string_{},
                row_idx_{},
                total_rows_processed_{},
                gen_input_{},
                end_iteration_state_{false},
                query_impl_(_qimp) {
            }

            /// Constructs an iterator from query state.
            /// \param _query_string query text used for error reporting.
            /// \param _gen_input query input carrying the continuation index.
            explicit iterator(
                const std::string&       _query_string,
                genQueryInp_t*           _gen_input) :
                query_string_{_query_string},
                row_idx_{},
                total_rows_processed_{},
                gen_input_{_gen_input},
                end_iteration_state_{false},
                query_impl_{} {
            } // ctor

            /// Advances to the next row.
            /// \return iterator positioned at the next row or end.
            iterator operator++() {
                advance_query();
                return *this;
            }

            /// Advances to the next row and returns the previous iterator state.
            /// The integer parameter selects postfix-increment semantics.
            /// \return iterator state before advancing.
            iterator operator++(int) {
                iterator ret = *this;
                ++(*this);
                return ret;
            }

            /// Compares two iterators for equality.
            /// \param _rhs iterator to compare against.
            /// \return `true` if both iterators represent the same end state.
            bool operator==(const iterator& _rhs) const {
                if(end_iteration_state_ && _rhs.end_iteration_state_) {
                    return true;
                }

                return (query_impl_->query_string() == _rhs.query_string_);
            }

            /// Compares two iterators for inequality.
            /// \param _rhs iterator to compare against.
            /// \return `true` if the iterators differ.
            bool operator!=(const iterator& _rhs) const {
                return !(*this == _rhs);
            }

            /// Returns the current row.
            /// \return column values for the current row.
            value_type operator*() {
                return capture_results();
            }

            /// Resets row state before loading the next page.
            void reset_for_page_boundary() {
                row_idx_ = 0;
                query_impl_->reset_for_page_boundary();
            }

            /// Moves iteration to the next available row.
            void advance_query() {
                total_rows_processed_++;
                if(query_impl_->query_limit_exceeded(total_rows_processed_)) {
                    end_iteration_state_ = true;
                    return;
                }

                row_idx_++;
                if(query_impl_->page_in_flight(row_idx_)) {
                    return;
                }

                if(query_impl_->query_complete()) {
                    end_iteration_state_ = true;
                    return;
                }

                reset_for_page_boundary();
                const int query_err = query_impl_->fetch_page();
                if(query_err < 0) {
                    if(CAT_NO_ROWS_FOUND != query_err) {
                        THROW(query_err, fmt::format("gen query failed for [{}] on idx {}",
                            query_string_, gen_input_->continueInx));
                    }

                   end_iteration_state_ = true;

                } // if

            } // advance_query 

            /// Captures the current row from the active page.
            /// \return column values for the current row.
            value_type capture_results() {
                return query_impl_->capture_results(row_idx_);
            }
        }; // class iterator

        /// Executes a query with optional arguments and configuration.
        /// \param _comm connection used to execute the query.
        /// \param _query_string general query text or specific query name.
        /// \param _specific_query_args optional bound arguments for specific queries.
        /// \param _zone_hint zone hint applied to the request.
        /// \param _query_limit maximum rows to expose to callers.
        /// \param _row_offset starting row offset.
        /// \param _query_type query API to use.
        /// \param _options extra query options.
        query(connection_type*                _comm,
              const std::string&              _query_string,
              const std::vector<std::string>* _specific_query_args,
              const std::string&              _zone_hint,
              uintmax_t                       _query_limit,
              uintmax_t                       _row_offset,
              irods::query_type               _query_type,
              int                             _options)
            : iter_{}
            , query_impl_{}
        {
            if (_query_type == irods::query_type::general) {
                query_impl_ = std::make_shared<gen_query_impl>(
                                  _comm,
                                  _query_limit,
                                  _row_offset,
                                  _query_string,
                                  _zone_hint,
                                  _options);
            }
            else if (_query_type == irods::query_type::specific) {
                query_impl_ = std::make_shared<spec_query_impl>(
                                  _comm,
                                  _query_limit,
                                  _row_offset,
                                  _query_string,
                                  _zone_hint,
                                  _specific_query_args);
            }

            const int fetch_err = query_impl_->fetch_page();
            if(fetch_err < 0) {
                if(CAT_NO_ROWS_FOUND == fetch_err) {
                    iter_ = std::make_unique<iterator>();
                }
                else {
                    THROW(fetch_err, fmt::format("query failed for [{}] type [{}]", _query_string, _query_type));
                }
            }

            if(query_impl_->results_valid()) {
                iter_ = std::make_unique<iterator>(query_impl_);
            }
            else {
                iter_ = std::make_unique<iterator>();
            }
        } // ctor

        /// Executes a query without bound specific-query arguments.
        /// \param _comm connection used to execute the query.
        /// \param _query_string general query text or specific query name.
        /// \param _query_limit maximum rows to expose to callers.
        /// \param _row_offset starting row offset.
        /// \param _query_type query API to use.
        /// \param _options extra query options.
        query(connection_type*   _comm,
              const std::string& _query_string,
              uintmax_t          _query_limit = 0,
              uintmax_t          _row_offset  = 0,
              irods::query_type  _query_type  = irods::query_type::general,
              int                _options     = 0)
            : query{_comm, _query_string, nullptr, {}, _query_limit, _row_offset, _query_type, _options}
        {
        } // ctor

        /// Executes a query using the deprecated query type enum.
        /// \param _comm connection used to execute the query.
        /// \param _query_string general query text or specific query name.
        /// \param _specific_query_args optional bound arguments for specific queries.
        /// \param _zone_hint zone hint applied to the request.
        /// \param _query_limit maximum rows to expose to callers.
        /// \param _row_offset starting row offset.
        /// \param _query_type deprecated query API selector.
        /// \param _options extra query options.
        [[deprecated("use irods::query_type")]]
        query(connection_type*                _comm,
              const std::string&              _query_string,
              const std::vector<std::string>* _specific_query_args,
              const std::string&              _zone_hint,
              uintmax_t                       _query_limit,
              uintmax_t                       _row_offset,
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated"
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
              query_type                      _query_type,
#pragma GCC diagnostic pop
              int                             _options)
            : query{_comm, _query_string, _specific_query_args, _zone_hint, _query_limit, _row_offset, static_cast<irods::query_type>(_query_type), _options}
        {
        } // ctor

        /// Executes a query using the deprecated query type enum.
        /// \param _comm connection used to execute the query.
        /// \param _query_string general query text or specific query name.
        /// \param _query_limit maximum rows to expose to callers.
        /// \param _row_offset starting row offset.
        /// \param _query_type deprecated query API selector.
        /// \param _options extra query options.
        [[deprecated("irods::query_type")]]
        query(connection_type*   _comm,
              const std::string& _query_string,
              uintmax_t          _query_limit,
              uintmax_t          _row_offset,
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated"
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
              query_type         _query_type,
#pragma GCC diagnostic pop
              int                _options     = 0)
            : query{_comm, _query_string, _query_limit, _row_offset, static_cast<irods::query_type>(_query_type), _options}
        {
        } // ctor

        /// Moves query state from another instance.
        /// \param[in] other source query object.
        query(query&& other) = default;

        /// Replaces this query with another query's state.
        /// \param[in] other source query object.
        /// \return reference to this query.
        query& operator=(query&& other) = default;

        /// Destroys the query wrapper.
        ~query() {}

        /// Returns an iterator to the first row.
        /// \return iterator positioned at the first available row.
        iterator   begin() { return *iter_; }

        /// Returns the end iterator.
        /// \return iterator representing the end of results.
        iterator   end()   { return iterator(); }

        /// Returns the first row.
        /// \return column values for the first available row.
        value_type front() { return (*(*iter_)); }

        /// Returns the first row.
        /// \return column values for the first available row.
        value_type front() const { return (*(*iter_)); }

        /// Returns the row count in the current page.
        /// \return number of rows currently loaded.
        size_t size()  { return query_impl_->size(); }

        /// Returns the row count in the current page.
        /// \return number of rows currently loaded.
        size_t size() const { return query_impl_->size(); }

        /// Reports whether the current page is empty.
        /// \return `true` if no rows are currently loaded.
        size_t empty() { return 0 == query_impl_->size(); }

        /// Reports whether the current page is empty.
        /// \return `true` if no rows are currently loaded.
        size_t empty() const { return 0 == query_impl_->size(); }

        /// Returns the server-reported total row count.
        /// \return total matching row count, or zero if unavailable.
        int total_row_count()
        {
            return query_impl_->total_row_count();
        }

    private:
        /// Iterator positioned at the first row when results are available.
        std::unique_ptr<iterator>        iter_;
        /// Shared implementation that owns query state and pages.
        std::shared_ptr<query_impl_base> query_impl_;
    }; // class query
} // namespace irods

#endif // IRODS_QUERY_HPP
