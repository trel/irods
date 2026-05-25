#ifndef IRODS_API_HANDLER_HPP
#define IRODS_API_HANDLER_HPP

#include "irods/rods.h"
#include "irods/packStruct.h"
#include "irods/irods_lookup_table.hpp"
#include "irods/irods_plugin_base.hpp"
#include "irods/irods_re_ruleexistshelper.hpp"
#include "irods/irods_stacktrace.hpp"
#include "irods/irods_pack_table.hpp"
#include "irods/irods_at_scope_exit.hpp"
#include "irods/irods_re_namespaceshelper.hpp"
#include "irods/irods_re_plugin.hpp"
#include "irods/irods_re_ruleexistshelper.hpp"
#include "irods/irods_logger.hpp"
#include "irods/rcMisc.h"

#include <boost/shared_ptr.hpp>
#include <boost/any.hpp>

#include <typeinfo>
#include <functional>
#include <utility>
#include <type_traits>
#include <vector>
#include <string>
#include <string_view>

namespace irods
{
    /// @brief Describes an API entry and its marshaling metadata.
    struct apidef_t {
        int            apiNumber;      ///< API number identifying the endpoint.
        char*          apiVersion;     ///< API version string expected by this endpoint.
        int            clientUserAuth; ///< Required client authentication level.
        int            proxyUserAuth;  ///< Required proxy authentication level.
        const char*    inPackInstruct; ///< Packing instruction string for the input structure.
        int            inBsFlag;       ///< Indicates whether the API consumes an input byte stream.
        const char*    outPackInstruct;///< Packing instruction string for the output structure.
        int            outBsFlag;      ///< Indicates whether the API produces an output byte stream.
        boost::any     svrHandler;     ///< Server handler callable or null for client-side entries.
        const char*    operation_name; ///< Operation name used for plugin dispatch and PEP lookup.
        std::function<void(void*)> clearInStruct;  ///< Releases an input structure instance.
        std::function<void(void*)> clearOutStruct; ///< Releases an output structure instance.
        int(*call_wrapper)(...);       ///< Wrapper used to adapt the API handler signature.
    }; // struct apidef_t

    /// @brief Reports whether an error code should bypass exception PEP handling.
    /// @tparam Integer Integral error-code type.
    /// @param[in] error_code Error code returned by an API operation.
    /// @return `true` if the error code is treated as acceptable, otherwise `false`.
    template <typename Integer,
              typename std::enable_if_t<std::is_integral<Integer>::value, int> = 0>
    bool is_acceptable_error(Integer error_code) {
        static const std::vector<Integer> acceptable_errors {
            SYS_NO_HANDLER_REPLY_MSG //... others as necessary
        };
        return std::any_of( acceptable_errors.begin(), acceptable_errors.end(),
            [error_code](auto listed)->bool { return listed == error_code; }
        );
    }

    /// @brief Adapts legacy API handlers to the plugin error interface.
    /// @tparam types_t API argument types following `rsComm_t*`.
    template <typename... types_t>
    class api_call_adaptor
    {
      private:
        std::function<int(rsComm_t*, types_t...)> fcn_;

      public:
        /// @brief Constructs an adaptor around a legacy API handler.
        /// @param[in] _fcn Function to invoke when the adaptor is called.
        api_call_adaptor( std::function<int(rsComm_t*, types_t...)> _fcn )
            : fcn_(_fcn)
        {
        }

        /// @brief Executes the adapted API handler.
        /// The plugin context is accepted for interface compatibility and the remaining arguments are
        /// forwarded to the legacy API handler.
        /// @param[in] _comm Server communication handle for the request.
        /// @param[in] _t Arguments forwarded to the legacy API handler.
        /// @return An iRODS error object built from the handler return code.
        irods::error operator()( irods::plugin_context&, rsComm_t* _comm, types_t... _t )
        {
#ifdef IRODS_ENABLE_SYSLOG
            bool logger_updated = false;

            const auto update_logger = [&logger_updated](auto&& _arg)
            {
                // If there exists multiple objects containing verbose flags,
                // then only allow one update.  What happens if subsequent calls
                // are made (hopefully, the condition objects are passed through
                // unchanged)?
                if (logger_updated)
                {
                    return;
                }

                // _arg's type will be deduced as <type>& if it's an lvalue.
                // For pointers, this means the type will we T*&.  Therefore, we
                // must either remove the reference or add a reference so that
                // std::is_same compares the correct types.
                using T = std::remove_reference_t<decltype(_arg)>;

                if constexpr (std::is_same_v<dataObjInp_t*, T>)
                {
                    logger_updated = true;
                    const auto* value = getValByKey(&_arg->condInput, VERY_VERBOSE_KW);
                    irods::experimental::log::write_to_error_object(value); 
                }
                else if constexpr (std::is_same_v<dataObjCopyInp_t*, T>)
                {
                    logger_updated = true;
                    const auto* flag_0 = getValByKey(&_arg->srcDataObjInp.condInput, VERY_VERBOSE_KW);
                    const auto* flag_1 = getValByKey(&_arg->destDataObjInp.condInput, VERY_VERBOSE_KW);
                    irods::experimental::log::write_to_error_object(flag_0 && flag_1); 
                }
            };

            (update_logger(std::forward<types_t>(_t)), ...);
#endif // IRODS_ENABLE_SYSLOG

            int ret = fcn_( _comm, _t... );
            if( ret >= 0 ) {
                return CODE( ret );
            }
            else {
                return ERROR( ret, "fail" );
            }
        }

    }; // class api_call_adaptor

    /// @brief Plugin-backed representation of an API table entry.
    class api_entry : public irods::plugin_base
    {
      private:
        using rule_engine_context_manager_type = rule_engine_context_manager<unit, ruleExecInfo_t*, AUDIT_RULE>;

      public:
        /// @brief Registers a legacy API handler under the provided operation name.
        /// @tparam types_t Argument types accepted by the handler.
        /// @param[in] _op Operation name used for lookup.
        /// @param[in] _f Legacy handler callable to register.
        /// @return An error indicating success or invalid input.
        template<typename... types_t>
        error add_operation(const std::string& _op, std::function<int(types_t...)> _f)
        {
            if ( _op.empty() ) {
                std::stringstream msg;
                msg << "empty operation key [" << _op << "]";
                return ERROR(SYS_INVALID_INPUT_PARAM, msg.str());
            }

            operation_name = _op;
            operations_[operation_name] = _f;

            return SUCCESS();
        } // add_operation

        /// @brief Calls the registered API handler and associated policy enforcement points.
        /// @param[in] _comm Server communication handle for the request.
        /// @param[in] _t Arguments forwarded to the registered handler and PEPs.
        /// @return Integer status code returned by the operation or rule engine.
        template<typename... types_t>
        int call_handler(rsComm_t* _comm, types_t... _t)
        {
            if( !operations_.has_entry(operation_name) ) {
                rodsLog(LOG_ERROR, "missing api operation [%s]", operation_name.c_str());
                return SYS_INVALID_INPUT_PARAM;
            }

            try {
                using fcn_t = std::function<int(rsComm_t*, types_t...)>;
                fcn_t fcn = boost::any_cast<fcn_t>( operations_[ operation_name ] );
#ifdef ENABLE_RE
                irods::plugin_property_map prop_map;
                irods::plugin_context ctx(_comm,prop_map);
                ruleExecInfo_t rei;
                memset( &rei, 0, sizeof( rei ) );
                if (_comm) {
                    rei.rsComm      = _comm;
                    rei.uoic        = &_comm->clientUser;
                    rei.uoip        = &_comm->proxyUser;
                }

                rule_engine_context_manager_type re_ctx_mgr(re_plugin_globals->global_re_mgr, &rei);

                // Always run the finally-PEP at scope exit.
                at_scope_exit invoke_finally_pep{[&] {
                    error finally_err = invoke_policy_enforcement_point(re_ctx_mgr,
                                                                        ctx,
                                                                        operation_name,
                                                                        "finally",
                                                                        std::forward<types_t>(_t)...);

                    if (!finally_err.ok()) {
                        irods::log(PASS(finally_err));
                    }
                }};

                error op_err = SUCCESS();

                // invoke the pre-pep for this operation
                error pre_err = invoke_policy_enforcement_point(
                                    re_ctx_mgr,
                                    ctx,
                                    operation_name,
                                    "pre",
                                    std::forward<types_t>(_t)...);

                if (pre_err.code() != RULE_ENGINE_SKIP_OPERATION) {
                    if (!pre_err.ok()) {
                        // if the pre-pep fails, invoke the exception pep
                        error except_err = invoke_policy_enforcement_point(
                                           re_ctx_mgr,
                                           ctx,
                                           operation_name,
                                           "except",
                                           std::forward<types_t>(_t)...);

                        if (!except_err.ok()) {
                            irods::log(PASS(except_err));
                        }

                        return pre_err.code();
                    }

                    using adapted_func_type = std::function<error(irods::plugin_context&, rsComm_t*, types_t...)>;
                    adapted_func_type adapted_fcn{api_call_adaptor<types_t...>(fcn)};
                    op_err = adapted_fcn(ctx, _comm, std::forward<types_t>(_t)...);

                    if (!op_err.ok() && !is_acceptable_error(op_err.code())) {
                        // if the operation fails, invoke the exception pep
                        error except_err = invoke_policy_enforcement_point(
                                               re_ctx_mgr,
                                               ctx,
                                               operation_name,
                                               "except",
                                               std::forward<types_t>(_t)...);

                        if (!except_err.ok()) {
                            irods::log(PASS(except_err));
                        }

                        return op_err.code();
                    }
                } // error_code != RULE_ENGINE_SKIP_OPERATION

                // invoke the post-pep for this operation
                error post_err = invoke_policy_enforcement_point(
                                     re_ctx_mgr,
                                     ctx,
                                     operation_name,
                                     "post",
                                     std::forward<types_t>(_t)...);

                if (!post_err.ok()) {
                    // if the post-pep fails, invoke the exception pep
                    error except_err = invoke_policy_enforcement_point(
                                           re_ctx_mgr,
                                           ctx,
                                           operation_name,
                                           "except",
                                           std::forward<types_t>(_t)...);

                    if (!except_err.ok()) {
                        irods::log(PASS(except_err));
                    }

                    return post_err.code();
                }

                return op_err.code();
#else // ENABLE_RE
                return fcn(_comm, _t...);
#endif // ENABLE_RE
            }
            catch (const boost::bad_any_cast&) {
                std::string msg( "failed for call - " );
                msg += operation_name;
                irods::log(ERROR(INVALID_ANY_CAST, msg));
                return INVALID_ANY_CAST;
            }

            return 0;
        } // call_handler

        /// @brief Calls the registered API handler without invoking policy enforcement points.
        /// @param[in] _comm Server communication handle for the request.
        /// @param[in] _args Arguments forwarded directly to the registered handler.
        /// @return Integer status code returned by the handler.
        template <typename ...Args>
        int call_handler_without_policy(rsComm_t* _comm, Args... _args)
        {
            if (!operations_.has_entry(operation_name)) {
                rodsLog(LOG_ERROR, "missing api operation [%s]", operation_name.c_str());
                return SYS_INVALID_INPUT_PARAM;
            }

            try {
                using fcn_t = std::function<int(rsComm_t*, Args...)>;
                fcn_t fcn = boost::any_cast<fcn_t>(operations_[operation_name]);
                return fcn(_comm, _args...);
            }
            catch (const boost::bad_any_cast&) {
                std::string msg = "failed for call - ";
                msg += operation_name;
                irods::log(ERROR(INVALID_ANY_CAST, msg));
                return INVALID_ANY_CAST;
            }

            return 0;
        } // call_handler_without_policy

        /// @brief Constructs an API entry from a static API definition.
        api_entry( apidef_t& );

        /// @brief Copies an API entry.
        api_entry( const api_entry& );

        /// @brief Assigns state from another API entry.
        /// @return Reference to `*this`.
        api_entry& operator=( const api_entry& );

        int            apiNumber;      ///< API number identifying the endpoint.
        char*          apiVersion;     ///< API version string expected by this endpoint.
        int            clientUserAuth; ///< Required client authentication level.
        int            proxyUserAuth;  ///< Required proxy authentication level.
        const char*    inPackInstruct; ///< Packing instruction string for the input structure.
        int            inBsFlag;       ///< Indicates whether the API consumes an input byte stream.
        const char*    outPackInstruct;///< Packing instruction string for the output structure.
        int            outBsFlag;      ///< Indicates whether the API produces an output byte stream.
        funcPtr        call_wrapper;   ///< Wrapper used to adapt the API handler signature.
        std::string    in_pack_key;    ///< Pack-table key for the input structure.
        std::string    out_pack_key;   ///< Pack-table key for the output structure.
        std::string    in_pack_value;  ///< Pack-table value for the input structure.
        std::string    out_pack_value; ///< Pack-table value for the output structure.
        std::string    operation_name; ///< Operation name used for dispatch and PEP lookup.

        lookup_table< std::string>   extra_pack_struct; ///< Additional pack-structure definitions owned by the entry.

        std::function<void(void*)> clearInStruct;  ///< Releases an input structure instance.
        std::function<void(void*)> clearOutStruct; ///< Releases an output structure instance.

      private:
#ifdef ENABLE_RE
        template<typename... types_t>
        error invoke_policy_enforcement_point(
            rule_engine_context_manager_type _re_ctx_mgr,
            plugin_context&                  _ctx,
            const std::string&               _operation_name,
            const std::string&               _class,
            types_t...                       _t)
        {
            using log = irods::experimental::log::rule_engine;

            bool ret = false;
            error saved_op_err = SUCCESS();
            error skip_op_err = SUCCESS();

            for (auto& ns : NamespacesHelper::Instance()->getNamespaces()) {
                std::string rule_name = ns + "pep_" + _operation_name + "_" + _class;

                if (RuleExistsHelper::Instance()->checkOperation( rule_name ) ) {
                    if (_re_ctx_mgr.rule_exists(rule_name, ret).ok() && ret) {
                        error op_err = _re_ctx_mgr.exec_rule(rule_name, instance_name_, _ctx, std::forward<types_t>(_t)...);

                        if (!op_err.ok()) {
                            log::debug("{}-pep rule [{}] failed with error code [{}]", _class, rule_name, op_err.code());
                            saved_op_err = op_err;
                        }
                        else if (op_err.code() == RULE_ENGINE_SKIP_OPERATION) {
                            skip_op_err = op_err;

                            if (_class != "pre") {
                                log::warn("RULE_ENGINE_SKIP_OPERATION ({}) incorrectly returned from PEP [{}]! "
                                          "RULE_ENGINE_SKIP_OPERATION should only be returned from pre-PEPs!",
                                          RULE_ENGINE_SKIP_OPERATION, rule_name);
                            }
                        }
                    }
                    else {
                        log::trace("Rule [{}] passes regex test, but does not exist", rule_name);
                    }
                }
            }

            if (!saved_op_err.ok()) {
                return saved_op_err;
            }

            if (skip_op_err.code() == RULE_ENGINE_SKIP_OPERATION) {
                return skip_op_err;
            }

            return saved_op_err;
        } // invoke_policy_enforcement_point
#endif // ENABLE_RE
    }; // class api_entry

    /// @brief Shared pointer type for `api_entry` instances.
    typedef boost::shared_ptr< api_entry > api_entry_ptr;

    /// =-=-=-=-=-=-=-
    /// @brief class which will hold statically compiled and dynamically loaded api handles
    class api_entry_table
        : public lookup_table<api_entry_ptr, size_t, boost::hash<size_t>>
    {
      public:
        /// @brief Constructs an API entry table from static definitions.
        /// @param[in] defs Array of API definitions to load.
        /// @param[in] size Number of elements in `defs`.
        api_entry_table(apidef_t defs[], size_t size);

        /// @brief Reports whether an API plugin has already been loaded.
        /// @param[in] plugin_name Name of the plugin to query.
        /// @return `true` if the plugin has been marked as loaded.
        auto is_plugin_loaded(std::string_view plugin_name) -> bool;

        /// @brief Marks an API plugin as loaded.
        /// @param[in] plugin_name Name of the plugin to record.
        auto mark_plugin_as_loaded(std::string_view plugin_name) -> void;

      private:
        std::vector<std::string> loaded_plugins_;
    }; // class api_entry_table

    /// @brief Loads statically compiled and dynamically discovered API entries.
    /// @param[in,out] _api_tbl Table receiving API entries.
    /// @param[in,out] _pack_tbl Table receiving related pack-structure definitions.
    /// @param[in] _cli_flg `true` when loading client-side entries, otherwise server-side entries.
    /// @return An error describing the result of initialization.
    error init_api_table(
        api_entry_table&  _api_tbl,    // table holding api entries
        pack_entry_table& _pack_tbl,   // table for pack struct ref
        bool              _cli_flg = true ); // default to client
} // namespace irods

#endif // IRODS_API_HANDLER_HPP
