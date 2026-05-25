#ifndef IRODS_RE_PLUGIN_HPP
#define IRODS_RE_PLUGIN_HPP

#include "irods/irods_error.hpp"
#include "irods/irods_load_plugin.hpp"
#include "irods/irods_lookup_table.hpp"
#include "irods/irods_re_structs.hpp"
#include "irods/irods_state_table.h"

#include <boost/any.hpp>
#include <boost/algorithm/string.hpp>

#include <fmt/format.h>

#include <functional>
#include <initializer_list>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <optional>
#include <utility>
#include <vector>

#ifdef IRODS_ENABLE_SYSLOG
#  define IRODS_SERVER_ONLY(x) x
#  include "irods/irods_logger.hpp"
using log_re = irods::experimental::log::rule_engine;
#else
#  define IRODS_SERVER_ONLY(x)
#endif // IRODS_ENABLE_SYSLOG

namespace irods {

// Notes about the exec_rule_* family of functions:
// - exec_rule: Executes dynamic PEPs and user-defined rules.
// - exec_rule_text: Used by irule exclusively.
// - exec_rule_expression: Used to execute delay() and remote() code.

/* How to use
 * Get rei and set necessary fields
        ruleExecInfo_t rei;
        memset( ( char* )&rei, 0, sizeof( ruleExecInfo_t ) );
        rei.rsComm        = _comm;

 * Create rule_engine_context_manager
        rule_engine_context_manager<unit, ruleExecInfo_t*, AUDIT_RULE> re_ctx_mgr = rule_engine_context_manager<unit, ruleExecInfo_t*, AUDIT_RULE>(global_re_mgr, &rei);

 * to call any operation with dynamic pep
        dynamic_operation_execution_manager<T,C,DONT_AUDIT_RULE> rex_mgr (std::shared_ptr<rule_engine_context_manager<unit,ruleExecInfo_t*,DONT_AUDIT_RULE> > (new rule_engine_context_manager<unit,ruleExecInfo_t*,DONT_AUDIT_RULE >(global_re_mgr, &rei)));
        rex_mgr.call("instance", "op", op, args...);

 * If you want to determine if rule exists (not microservices)
        bool ret;
        error err;
        if ( !(err = re_ctx_mgr.rule_exists(_rn, ret)).ok() ) {
            return err;
        } else if( !ret ) {
            return ERROR( SYS_RULE_NOT_FOUND, "no rule found" );
        }

 * If you want to execute a rule, pass input parameter using rvalue or reference, output parameter using pointer
        std::string out;
        err = re_ctx_mgr.exec_rule(_rn, &out, args...);

*/
    template<typename T>
    /// Returns a copy of the provided value.
    T clone(T& a) {

        return a;
    }

    /// Marks a packed argument list for forwarding without repacking.
    class unpack {
    public:
        /// Wraps an existing packed argument list.
        unpack(std::list<boost::any> &_l);
        std::list<boost::any> &l_; ///< Referenced argument list.
    };

    /// Appends zero or more values to a packed argument list.
    void var_arg_to_list(std::list<boost::any>& _l);

    template<typename... As>
    /// Forwards an unpack marker into the list-building overload set.
    void var_arg_to_list(std::list<boost::any>& _l, unpack &&_p, As &&... _ps) {
        var_arg_to_list(_l, _p, std::forward<As>(_ps)...);
    }

    template<typename... As>
    /// Appends the contents of an existing packed list.
    void var_arg_to_list(std::list<boost::any>& _l, unpack &_p, As &&... _ps) {
        for (auto itr = begin (_p.l_) ; itr != end(_p.l_);++itr) {
            _l.push_back(boost::any(*itr));
        }
        var_arg_to_list(_l, std::forward<As>(_ps)...);

    }

    template<typename A, typename... As>
    /// Appends one value and continues packing the remaining arguments.
    void var_arg_to_list(std::list<boost::any>& _l,A&& _p, As&&... _ps) {
        _l.push_back(boost::any(_p));
        var_arg_to_list(_l, std::forward<As>(_ps)...);
    }

    /// Extracts packed values into output arguments.
    error list_to_var_arg(std::list<boost::any>& _l);

    template<typename A, typename... As>
    /// Extracts one packed value and continues unpacking the rest.
    error list_to_var_arg(std::list<boost::any>& _l, A&& _p, As&&... _ps) {
        if(_l.empty()) {
            return ERROR(RE_TYPE_ERROR, "arg list mismatch");
        } else {
            _p = boost::any_cast<A>(_l.front());
            _l.pop_front();
            return list_to_var_arg(_l, std::forward<As>(_ps)...);

        }
    }

    template<typename... As>
    /// Packs a variadic argument list into a `std::list<boost::any>`.
    std::list<boost::any> pack(As &&... _ps) {
        std::list<boost::any> l;
        var_arg_to_list(l, std::forward<As>(_ps)...);
        return l;
    }

    /// Default microservice manager placeholder.
    template<typename C>
    class default_microservice_manager {
    };

    /// Alias for the active microservice manager implementation.
    template<typename C>
    using microservice_manager = default_microservice_manager<C>;

    /// Controls whether rule execution is audited.
    enum rule_execution_manager_pack{
        AUDIT_RULE, ///< Enable rule auditing.
        DONT_AUDIT_RULE ///< Disable rule auditing.
    };

    /// Forward declaration for the rule execution context manager.
    template<typename T, typename C, rule_execution_manager_pack Audit>
    class rule_engine_context_manager;

    /// Adapts a rule execution context manager into a callback object.
    class callback final {
    public:
        template<typename T, typename C, rule_execution_manager_pack Audit>
        /// Captures a callback target.
        callback(rule_engine_context_manager<T,C,Audit>& _callback_hdlr) : callback_hdlr_(&_callback_hdlr) {}

        template<typename... As>
        /// Invokes the captured callback target.
        error operator()(const std::string& _rn, As&&... _ps);
    protected:
        boost::any callback_hdlr_; ///< Stored callback target.
    };

    /// Type-erases a callable while preserving its expected signature.
    class serialize final {
    public:
        template<typename... As>
        /// Stores the callable and its display name.
        serialize(std::function<error(As ...)> _op, const std::string& _op_name = "<unknown>") : op_(_op), op_name_(_op_name) { }

        template<typename... Bs>
        /// Invokes the stored callable if the argument types match.
        error operator()(Bs&&... _ps) {
            if(typeid(std::function<error(Bs...)>) == op_.type()) {
                return boost::any_cast<std::function<error(Bs...)> >(op_)(std::forward<Bs>(_ps)...);
            } else {
                return ERROR(RE_TYPE_ERROR, std::string("operation ") + op_name_ + " called with wrong types of arguments");
            }
        }
    protected:
        boost::any op_; ///< Stored callable.
        std::string op_name_; ///< Display name used in errors.
    };

    template <typename T>
    /// Function pointer type exported by rule engine plugins.
    using re_plugin_operation = error(*) (T&, ...);


    /// Wraps a rule engine plugin instance and its exported operations.
    template<typename T>
    class pluggable_rule_engine final {
    public:

        /// Constructs a plugin wrapper for one instance name.
        pluggable_rule_engine(const std::string &_in, const std::string &_context) : instance_name_(_in) {
        }

        template<typename... types_t>
        /// Registers a named operation exposed by the plugin.
        error add_operation(
                const std::string& _op,
                std::function<error(types_t...)> _f ) {
            if ( _op.empty() ) {
                std::stringstream msg;
                msg << "empty operation [" << _op << "]";
                return ERROR( SYS_INVALID_INPUT_PARAM, msg.str() );
            }
            operations_[_op] = _f;
            return SUCCESS();

        }

        /// Invokes the plugin setup hook.
        error setup_operation(T& _in)
        {
            try {
                auto fcn = boost::any_cast<std::function<error(T&, const std::string&)>>(operations_["setup"]);
                return fcn(_in, instance_name_);
            }
            catch (const boost::bad_any_cast& e) {
                return ERROR(
                    INVALID_ANY_CAST,
                    fmt::format("failed to extract setup operation from instance [{}]: {}", instance_name_, e.what()));
            }
            catch (const std::exception& e) {
                return ERROR(
                    PLUGIN_ERROR,
                    fmt::format("failed to extract setup operation from instance [{}]: {}", instance_name_, e.what()));
            }
        } // setup_operation

        /// Invokes the plugin teardown hook.
        error teardown_operation(T& _in)
        {
            try {
                auto fcn = boost::any_cast<std::function<error(T&, const std::string&)>>(operations_["teardown"]);
                return fcn(_in, instance_name_);
            }
            catch (const boost::bad_any_cast& e) {
                return ERROR(
                    INVALID_ANY_CAST,
                    fmt::format(
                        "failed to extract teardown operation from instance [{}]: {}", instance_name_, e.what()));
            }
            catch (const std::exception& e) {
                return ERROR(
                    PLUGIN_ERROR,
                    fmt::format(
                        "failed to extract teardown operation from instance [{}]: {}", instance_name_, e.what()));
            }
        } // teardown_operation

        /// Invokes the plugin start hook.
        error start_operation(T& _in) {
            try {
                auto fcn = boost::any_cast<std::function<error(T&,const std::string&)>>( operations_["start"] );
                return fcn(_in, instance_name_);
            } catch (const boost::bad_any_cast& e) {
                return ERROR(INVALID_ANY_CAST, boost::format("failed to extract start operation from instance [%s] exception message [%s]") % instance_name_ % e.what());
            }
        }

        /// Invokes the plugin stop hook.
        error stop_operation(T& _in) {
            try {
                auto fcn = boost::any_cast<std::function<error(T&,const std::string&)>>( operations_["stop"] );
                return fcn(_in,instance_name_);
            } catch (const boost::bad_any_cast& e) {
                return ERROR(INVALID_ANY_CAST, boost::format("failed to extract stop operation from instance [%s] exception message [%s]") % instance_name_ % e.what());
            }
        }

        /// Checks whether the plugin defines the named rule.
        error rule_exists(const std::string& _rn, T& _re_ctx, bool& _out) {
            try {
                auto fcn = boost::any_cast<std::function<error(T&,const std::string&,bool&)>>( operations_["rule_exists"] );
                return fcn(_re_ctx, _rn, _out);
            } catch (const boost::bad_any_cast& e) {
                return ERROR(INVALID_ANY_CAST, boost::format("failed to extract rule_exists operation from instance [%s] exception message [%s]") % instance_name_ % e.what());
            }
        }

        /// Appends this plugin's rule names to the provided vector.
        error list_rules(T& _re_ctx, std::vector<std::string>& _rule_vec) {
            try {
                auto fcn = boost::any_cast<std::function<error(T&,std::vector<std::string>&)>>( operations_["list_rules"] );
                return fcn(_re_ctx, _rule_vec);
            } catch ( const boost::bad_any_cast& e ) {
                return ERROR( INVALID_ANY_CAST, boost::format( "failed to extract list_rules operation from instance [%s] exception message [%s]") % instance_name_ % e.what() );
                        }
        }

        template<typename ...As>
        /// Executes a named rule through the plugin.
        error exec_rule(const std::string& _rn, T& _re_ctx, As&&... _ps, callback _callback) {
            try {
                auto l = pack(std::forward<As>(_ps)...);
                auto fcn = boost::any_cast<std::function<error(T&, const std::string&, std::list<boost::any> &, callback)>>( operations_["exec_rule"] );
                return fcn(_re_ctx, _rn, l, _callback);
            } catch (const boost::bad_any_cast& e) {
                return ERROR(INVALID_ANY_CAST, boost::format("failed to extract exec_rule operation from instance [%s] exception message [%s]") % instance_name_ % e.what());
            }
        }

        /// Executes rule text through the plugin.
        error exec_rule_text(
                T&                 _re_ctx,
                const std::string& _rt,
                msParamArray_t*    _ms_params,
                const std::string& _out_desc,
                callback           _callback) {
            try {
                auto fcn = boost::any_cast<
                    std::function<error(T&, const std::string&, msParamArray_t*, const std::string&, callback)>>(
                        operations_["exec_rule_text"] );
                return fcn(_re_ctx, _rt, _ms_params, _out_desc, _callback);
            } catch (const boost::bad_any_cast& e) {
                return ERROR(INVALID_ANY_CAST, boost::format("failed to extract exec_rule_text operation from instance [%s] exception message [%s]") % instance_name_ % e.what());
            }
        }

        /// Executes a rule expression through the plugin.
        error exec_rule_expression(
                T&                 _re_ctx,
                const std::string& _rt,
                msParamArray_t*    _ms_params,
                callback           _callback) {
            try {
                auto fcn = boost::any_cast<
                    std::function<error(T&, const std::string&, msParamArray_t*, callback)>>(
                        operations_["exec_rule_expression"] );
                return fcn(_re_ctx, _rt, _ms_params, _callback);
            } catch (const boost::bad_any_cast& e) {
                return ERROR(INVALID_ANY_CAST, boost::format("failed to extract exec_rule_expression operation from instance [%s] exception message [%s]") % instance_name_ % e.what());
            }
        }

    private:
        /// Loads one exported operation from a shared library handle.
        error load_operation(void *handle, std::string _fcn, std::string _key);
        irods::lookup_table< boost::any > operations_; ///< Registered plugin operations.
        std::string instance_name_; ///< Plugin instance name.
    };

    /// Forward declaration for the rule execution context manager.
    template<typename T, typename C, rule_execution_manager_pack Audit>
    class rule_engine_context_manager;

    /// Executes operations while preserving rule engine context.
    template<typename T, typename C, rule_execution_manager_pack Audit>
    class dynamic_operation_execution_manager final {
    public:

        /// Stores a shared rule execution manager.
        dynamic_operation_execution_manager(
            std::shared_ptr<rule_engine_context_manager<T,C,Audit> > _re_mgr  // rule engine manager
        ) : re_mgr_{_re_mgr} { }

        /// Executes rule text against the selected plugin instance.
        error exec_rule_text(
            const std::string& _instance_name,
            const std::string& _rule_text,
            msParamArray_t*    _ms_params,
            const std::string& _out_desc)
        {
            return re_mgr_->exec_rule_text(_instance_name, _rule_text, _ms_params, _out_desc);
        }

        /// Executes a rule expression against the selected plugin instance.
        error exec_rule_expression(
            const std::string& _instance_name,
            const std::string& _rule_text,
            msParamArray_t*    _ms_params)
        {
            return re_mgr_->exec_rule_expression(_instance_name, _rule_text, _ms_params);
        }

        template<typename OP, typename... As >
        /// Executes one dynamic operation exactly once with forwarded arguments.
        error call(
            const std::string& _instance_name,
            const std::string& _operation_name,
            OP _operation,
            As&& ... _ps) {
            // =-=-=-=-=-=-=-
            // debug message for creating dynPEP rules
            rodsLog(
                LOG_DEBUG,
                "exec_op [%s]",
                _operation_name.c_str() );

            bool execOnce = false;
            std::function<error()> op = [&execOnce, &_ps..., &_operation] () {
                if(execOnce) {
                    return ERROR(RE_TYPE_ERROR, "cannot execute operation more than once with client supplied arguments");
                } else {
                    execOnce = true;
                    return _operation(std::forward<As>(_ps)...);
                }
            };

            auto err = op();

            return !err.ok() ? PASS(err) : err;
        }

    protected:

        std::shared_ptr<rule_engine_context_manager<T,C,Audit> >re_mgr_; ///< Shared execution manager.

    }; // class dynamic_operation_execution_manager

    /// Describes one configured rule engine plugin instance.
    template<typename T>
    struct re_pack_inp final {
        std::string instance_name_; ///< Rule engine instance name.
        std::string plugin_name_; ///< Shared library plugin name.
        T re_ctx_; ///< Per-plugin rule execution context.
        pluggable_rule_engine<T> *re_; ///< Resolved plugin instance.
        /// Constructs a rule engine plugin description.
        re_pack_inp(const std::string& _instance_name, const std::string& _plugin_name, T _re_ctx) : instance_name_(_instance_name), plugin_name_(_plugin_name), re_ctx_(_re_ctx) { }
    };

    /// Lazily resolves and caches pluggable rule engine instances.
    template<typename T>
    class rule_engine_plugin_manager final {
    public:
        /// Returns the plugin manager interface version.
        double interface_version() { return 1.0; }
        /// Stores the plugin directory to search.
        rule_engine_plugin_manager(const std::string& _dir) : dir_(_dir) { }

        /// Releases cached plugin instances.
        ~rule_engine_plugin_manager() {
            for (auto itr = begin(re_plugin_map_);itr != end(re_plugin_map_); ++itr) delete itr->second;
        }

        /// Resolves a plugin instance by name, loading it on first use.
        error resolve(std::string& _plugin_name, const std::string& _inst_name, pluggable_rule_engine<T> *& _re_ptr) {
            auto itr = re_plugin_map_.find(_inst_name);

            if(itr == end(re_plugin_map_)) {
                error err = load_plugin <pluggable_rule_engine<T> > (_re_ptr, _plugin_name, dir_, _inst_name, std::string("empty_context"));

                if (!err.ok()) {
                    IRODS_SERVER_ONLY((log_re::error(PASS(err).user_result())));
                    return err;
                }

                re_plugin_map_[_inst_name] = _re_ptr;
            } else {
                _re_ptr = itr->second;
            }

            return SUCCESS();
        }

        std::map<std::string, pluggable_rule_engine<T> *> re_plugin_map_; ///< Cached plugins by instance name.

    protected:
        std::string dir_; ///< Plugin search directory.

    };

    /// Owns the active set of configured rule engine instances.
    template<typename T, typename C>
    class rule_engine_manager final {
    public:
        /// Initializes all configured rule engine instances.
        rule_engine_manager(rule_engine_plugin_manager <T>& _re_plugin_mgr, std::vector<re_pack_inp<T> > &_re_packs, microservice_manager<C> &_ms_mgr) : ms_mgr_(_ms_mgr), re_plugin_mgr_(_re_plugin_mgr) {
            std::for_each(begin(_re_packs), end(_re_packs), [this](re_pack_inp<T> &_inp) {
                error err = this->init_rule_engine(_inp);
                if( !err.ok() ) {
                    IRODS_SERVER_ONLY((log_re::error(PASS(err).user_result())));
                }
            });
        }

        /// Destructor.
        ~rule_engine_manager() {
        }

        /// Adds one configured rule engine instance to the manager.
        error init_rule_engine(re_pack_inp<T>& _inp) {
            pluggable_rule_engine<T>* pre = NULL;
            error err;

            err = re_plugin_mgr_.resolve(_inp.plugin_name_, _inp.instance_name_, pre);
            if(!err.ok()) {
                return PASS( err );
            }

            _inp.re_ = pre;
            re_packs_.push_back(_inp);

            return SUCCESS();
        }

        /// Calls setup on all configured plugins.
        void call_setup_operations()
        {
            std::for_each(begin(re_packs_), end(re_packs_), [](re_pack_inp<T>& _inp) {
                _inp.re_->setup_operation(_inp.re_ctx_);
            });
        }

        /// Calls teardown on all configured plugins.
        void call_teardown_operations()
        {
            std::for_each(begin(re_packs_), end(re_packs_), [](re_pack_inp<T>& _inp) {
                _inp.re_->teardown_operation(_inp.re_ctx_);
            });
        }

        /// Calls start on all configured plugins.
        void call_start_operations() {
            std::for_each(begin(re_packs_), end(re_packs_), [](re_pack_inp<T> &_inp) {
                _inp.re_->start_operation(_inp.re_ctx_);
            });
        }

        /// Calls stop on all configured plugins.
        void call_stop_operations() {
            std::for_each(begin(re_packs_), end(re_packs_), [](re_pack_inp<T> &_inp) {
                _inp.re_->stop_operation(_inp.re_ctx_);
            });
        }

        microservice_manager<C> &ms_mgr_; ///< Microservice execution manager.
        std::list<re_pack_inp<T> > re_packs_; ///< Active rule engine instances.
    protected:
        rule_engine_plugin_manager<T> &re_plugin_mgr_; ///< Plugin loader and cache.

    };

    /// Returns true if the error code requests continued rule processing.
    inline bool is_continuation_code(int _error_code)
    {
        // Continue to the next rule engine plugin if the current REP
        // returns either of the following:
        // - SYS_NOT_SUPPORTED   : Signals to the REPF that the REP did nothing.
        // - RULE_ENGINE_CONTINUE: Signals to the REPF to continue with the next REP.
        const std::initializer_list<int> continuation_codes = {SYS_NOT_SUPPORTED, RULE_ENGINE_CONTINUE};

        return std::any_of(std::begin(continuation_codes),
                           std::end(continuation_codes),
                           [_error_code](auto _ec) { return _ec == _error_code; });
    }

    /// Logs a rule engine plugin return code at debug or error level.
    inline void log_error_code(const error& IRODS_SERVER_ONLY(e))
    {
        IRODS_SERVER_ONLY(
            // clang-format off
            if (is_continuation_code(e.code()) ||
                e.code() >= 0                  ||
                e.code() == RE_PARSER_ERROR    ||
                e.code() == RULE_ENGINE_ERROR)
            {
                log_re::debug("Rule Engine Plugin returned [{}].", e.code());
            }
            else {
                log_re::error("Rule Engine Plugin returned [{}].", e.code());
            }
            // clang-format on
        )
    }

    template <typename ER, typename EM, typename T, typename ...As>
    /// Dispatches a rule to rule engines first and microservices second.
    inline error control(std::list<re_pack_inp<T> >& _re_packs, ER _er, EM _em, const std::string& _rn, As &&... _ps) {
        // "unsafe_ms_ctx" is a special keyword that must be processed by the microservice
        // handler, "_em". If this keyword is seen, the REPs should be skipped.
        if ("unsafe_ms_ctx" != _rn) {
            // Holds the error code of the last REP that processed the rule "_rn".
            // If the rule was NOT processed by any REP, then "last_error_code" will
            // be empty and "_rn" MIGHT reference a microservice. It is VERY important that
            // "last_error_code" be instantiated in the empty state.
            std::optional<int> last_error_code;

            // Iterate over the list of REPs. If the rule exists in one of the REPs,
            // then execute that REP's rule code.
            for (auto itr = begin(_re_packs); itr != end(_re_packs); ++itr) {
                bool rule_exists = false;

                error err = itr->re_->rule_exists(_rn, itr->re_ctx_, rule_exists);

                if (!err.ok()) {
                    return err;
                }

                if (rule_exists) {
                    err = _er(*itr, _rn, std::forward<As>(_ps)...);
                    last_error_code = err.code();

                    if (!is_continuation_code(*last_error_code)) {
                        return err;
                    }
                }
            }

            // If the rule was processed by a REP and the last error code is zero or a continuation
            // code, then return the last error code. Checking for zero is equivalent to a REP returning
            // SUCCESS(). This keeps the REPF from trying to execute rules as microservices.
            if (last_error_code && (*last_error_code == 0 || is_continuation_code(*last_error_code))) {
                return CODE(*last_error_code);
            }
        }

        return _em(_rn, std::forward<As>(_ps)...);
    }

    /// Provides a shared implementation of `rule_exists` queries.
    template<typename T, typename C>
    class rule_exists_manager {
    public:
        /// Stores a reference to the owning rule engine manager.
        rule_exists_manager(rule_engine_manager<T,C>& _re_mgr) :
                re_mgr_{_re_mgr} {}

        /// Destructor.
        virtual ~rule_exists_manager() {}

        /// Returns whether a rule exists in any configured rule engine.
        error rule_exists(const std::string& _rn, bool& ret) {
            auto er = [&ret] (re_pack_inp<T>&, const std::string&) {
                ret = true;
                return SUCCESS();
            };

            auto em = [&ret] (const std::string&) {
                ret = false;
                return SUCCESS();
            };

            return control(re_mgr_.re_packs_, er, em, _rn);
        }
    protected:
        rule_engine_manager<T,C> &re_mgr_; ///< Owning rule engine manager.
    };

    /// Primary template for rule execution contexts.
    template<typename T, typename C, rule_execution_manager_pack Audit>
    class rule_engine_context_manager {};

    /// Audited rule execution context.
    template<typename T, typename C>
    class rule_engine_context_manager<T,C,AUDIT_RULE> final : public rule_exists_manager<T,C>
    {
    public:
        /// Builds an audited rule execution context.
        rule_engine_context_manager(rule_engine_manager<T,C>& _re_mgr, C _ctx)
            : rule_exists_manager<T,C>{_re_mgr}
            , ctx_{_ctx}
            , rex_mgr_{std::make_shared<rule_engine_context_manager<T,C,DONT_AUDIT_RULE>>(_re_mgr, _ctx)}
        {
        }

        template <typename ...As>
        /// Executes a named rule with auditing enabled.
        error exec_rule(const std::string& _rn, As &&... _ps) {
            auto er = [this](re_pack_inp<T>& _itr, const std::string& _rn, decltype(_ps)... _ps) {
                using func_t = std::function<error(const std::string&, re_pack_inp<T>&, decltype(_ps)...)>;

                func_t fun = [this](const std::string& _rn, re_pack_inp<T>& _itr, decltype(_ps)... _ps) {
                    return _itr.re_->template exec_rule<As...>(_rn, _itr.re_ctx_, std::forward<As >(_ps)..., callback(*this));
                };

                return this->rex_mgr_.call(_itr.instance_name_,
                                           std::string("exec_rule"),
                                           fun, 
                                           _rn, 
                                           _itr, 
                                           std::forward<As>(_ps)...);
            };

            auto em = [this](const std::string& _rn, decltype(_ps)... _ps) {
                using func_t = std::function<error(const std::string&, C&, decltype(_ps)...)>;

                func_t fun = [this](const std::string& _rn, C& _ctx, decltype(_ps)... _ps) {
                    return this->re_mgr_.ms_mgr_.exec_microservice(_rn, _ctx, std::forward<As>(_ps)...);
                };

                return this->rex_mgr_.call(std::string("microservice_manager"),
                                           std::string("exec_microservice"), 
                                           fun, 
                                           _rn, 
                                           this->ctx_, 
                                           std::forward<As>(_ps)...);
            };

            return control(this->re_mgr_.re_packs_, er, em, _rn, std::forward<As>(_ps)...);
        }

        /// Executes rule text against one or all configured plugin instances.
        error exec_rule_text(
                const std::string& _instance_name,
                const std::string& _rt,
                msParamArray_t*    _ms_params,
                const std::string& _out_desc)
        {
            if (_instance_name.empty()) {
                for (auto&& re_pack : this->re_mgr_.re_packs_) {
                    log_error_code(re_pack.re_->exec_rule_text(re_pack.re_ctx_, _rt, _ms_params, _out_desc, callback(*this)));
                }

                return SUCCESS();
            }

            for (auto&& re_pack : this->re_mgr_.re_packs_) {
                if (_instance_name == re_pack.instance_name_) {
                    return re_pack.re_->exec_rule_text(re_pack.re_ctx_, _rt, _ms_params, _out_desc, callback(*this));
                }
            }

            std::string msg = "instance not found [";
            msg += _instance_name;
            msg += ']';

            return ERROR(SYS_INVALID_INPUT_PARAM, msg);
        }

        /// Executes a rule expression against one or all configured plugin instances.
        error exec_rule_expression(
                const std::string& _instance_name,
                const std::string& _rt,
                msParamArray_t*    _ms_params)
        {
            if (_instance_name.empty()) {
                for (auto&& re_pack : this->re_mgr_.re_packs_) {
                    log_error_code(re_pack.re_->exec_rule_expression(re_pack.re_ctx_, _rt, _ms_params, callback(*this)));
                }

                return SUCCESS();
            }

            for (auto&& re_pack : this->re_mgr_.re_packs_) {
                if (_instance_name == re_pack.instance_name_) {
                    return re_pack.re_->exec_rule_expression(re_pack.re_ctx_, _rt, _ms_params, callback(*this));
                }
            }

            std::string msg = "instance not found [";
            msg += _instance_name;
            msg += ']';

            return ERROR(SYS_INVALID_INPUT_PARAM, msg);
        }

        /// Appends rule names from all configured plugins.
        error list_rules(std::vector< std::string >& rule_vec) {
            return this->rex_mgr_.list_rules( rule_vec );
        }

    protected:
        C ctx_; ///< Microservice execution context.
        dynamic_operation_execution_manager<T,C,DONT_AUDIT_RULE> rex_mgr_; ///< Helper for nested execution.
    };

    /// Non-audited rule execution context.
    template<typename T, typename C>
    class rule_engine_context_manager<T,C,DONT_AUDIT_RULE> final : public rule_exists_manager<T,C>
    {
    public:
        /// Builds a non-audited rule execution context.
        rule_engine_context_manager(rule_engine_manager<T,C>& _re_mgr, C _ctx) :
                rule_exists_manager<T,C>{_re_mgr},
                ctx_{_ctx} {}

        template <typename ...As>
        /// Executes a named rule without auditing.
        error exec_rule(const std::string& _rn, As &&... _ps) {
            auto er = [this](re_pack_inp<T>& _itr, const std::string& _rn, decltype(_ps)... _ps) {
                return _itr.re_->template exec_rule<As...>( _rn, _itr.re_ctx_, std::forward<As>(_ps)..., callback(*this));
            };

            auto em = [this](const std::string& _rn, decltype(_ps)... _ps) {
                return this->re_mgr_.ms_mgr_.exec_microservice(_rn, this->ctx_, std::forward<As>(_ps)...);
            };

            return control(this->re_mgr_.re_packs_, er, em, _rn, std::forward<As>(_ps)...);
        }

        /// Executes rule text against one or all configured plugin instances.
        error exec_rule_text(
                const std::string& _instance_name,
                const std::string& _rt,
                msParamArray_t*    _ms_params,
                const std::string& _out_desc)
        {
            if (_instance_name.empty()) {
                for (auto&& re_pack : this->re_mgr_.re_packs_) {
                    log_error_code(re_pack.re_->exec_rule_text(re_pack.re_ctx_, _rt, _ms_params, _out_desc, callback(*this)));
                }

                return SUCCESS();
            }

            for (auto&& re_pack : this->re_mgr_.re_packs_) {
                if (_instance_name == re_pack.instance_name_) {
                    return re_pack.re_->exec_rule_text(re_pack.re_ctx_, _rt, _ms_params, _out_desc, callback(*this));
                }
            }

            std::string msg = "instance not found [";
            msg += _instance_name;
            msg += ']';

            return ERROR(SYS_INVALID_INPUT_PARAM, msg);
        }

        /// Executes a rule expression against one or all configured plugin instances.
        error exec_rule_expression(
                const std::string& _instance_name,
                const std::string& _rt,
                msParamArray_t* _ms_params)
        {
            if (_instance_name.empty()) {
                for (auto&& re_pack : this->re_mgr_.re_packs_) {
                    log_error_code(re_pack.re_->exec_rule_expression(re_pack.re_ctx_, _rt, _ms_params, callback(*this)));
                }

                return SUCCESS();
            }

            for (auto&& re_pack : this->re_mgr_.re_packs_) {
                if (_instance_name == re_pack.instance_name_) {
                    return re_pack.re_->exec_rule_expression(re_pack.re_ctx_, _rt, _ms_params, callback(*this));
                }
            }

            std::string msg = "instance not found [";
            msg += _instance_name;
            msg += ']';

            return ERROR(SYS_INVALID_INPUT_PARAM, msg);
        }

        /// Appends rule names from all configured plugins.
        error list_rules(std::vector< std::string >& rule_vec) {
            for( auto itr  = begin(this->re_mgr_.re_packs_);
                      itr != end(this->re_mgr_.re_packs_);
                            ++itr ) {
                error ret = itr->re_->list_rules( itr->re_ctx_, rule_vec );
                if ( !ret.ok() ) {
                    rodsLog( LOG_ERROR, "Error in list_rules for instance [%s]", itr->instance_name_.c_str() );
                    return ret;
                }
            }

            return SUCCESS();
        }

    protected:
        C ctx_; ///< Microservice execution context.
    };

    template<typename T>
    /// Resolves and stores one plugin operation by name.
    error pluggable_rule_engine<T>::load_operation(void *_handle, std::string _fcn, std::string _key) {
        dlerror();
        re_plugin_operation<T> plugin_op_ptr = reinterpret_cast< re_plugin_operation<T> > (dlsym(_handle, _fcn.c_str()));
        if (!plugin_op_ptr) {
            return ERROR(-1, "cannot load plugin operation " + _key + " " + _fcn);
        }
        operations_[_key] = plugin_op_ptr;
	return SUCCESS();
    }

    /// Alias for an empty rule engine context type.
    using unit = std::tuple<>;
    /// Convenience macro for a default-constructed `unit` value.
#define UNIT unit()

    /// Default rule engine context type.
    using default_re_ctx = unit;
    /// Default microservice execution context type.
    using default_ms_ctx = ruleExecInfo_t *;

    template<>
    /// Microservice manager specialization for rule execution contexts.
    class default_microservice_manager<default_ms_ctx> {
    public:
        /// Executes a microservice from a packed argument list.
        error exec_microservice_adapter(std::string _msn, default_ms_ctx _ctx, std::list<boost::any>&);

        template<typename... As>
        /// Executes a microservice with forwarded arguments.
        error exec_microservice(std::string _msn, default_ms_ctx _ctx, As&&... _ps) {
            // =-=-=-=-=-=-=-
            // debug message for creating dynPEP rules
            rodsLog(
                LOG_DEBUG,
                "microservice_manager::exec_microservice_no_audit [%s]",
                _msn.c_str() );

            // =-=-=-=-=-=-=-
            // add the output parameter

            auto l = pack(std::forward<As>(_ps)...);
            error err = exec_microservice_adapter(_msn, _ctx, l);

            return err;

        }

    };

    template<typename... As>
    /// Dispatches a callback into the stored rule execution context manager.
    error callback::operator()(const std::string& _rn,  As&& ... _ps) {
        if(callback_hdlr_.type() == typeid(rule_engine_context_manager<default_re_ctx, default_ms_ctx, AUDIT_RULE> *)) {
            auto cb = boost::any_cast<rule_engine_context_manager<default_re_ctx, default_ms_ctx, AUDIT_RULE> *>(callback_hdlr_);
            return cb->exec_rule(_rn, std::forward<As> (_ps)...);
        } else if (callback_hdlr_.type() == typeid(rule_engine_context_manager<default_re_ctx, default_ms_ctx, DONT_AUDIT_RULE> *)) {
            auto cb = boost::any_cast<rule_engine_context_manager<default_re_ctx, default_ms_ctx, DONT_AUDIT_RULE> *>(callback_hdlr_);
            return cb->exec_rule(_rn, std::forward<As> (_ps)...);
        } else {
            rodsLog(
                LOG_ERROR,
                "rule engine callback: unsupported effect handler type. encountered:\n%s\nexpected:\n%s\n%s\n",
                callback_hdlr_.type().name(),
                typeid(rule_engine_context_manager<default_re_ctx, default_ms_ctx, AUDIT_RULE> *).name(),
                typeid(rule_engine_context_manager<default_re_ctx, default_ms_ctx, DONT_AUDIT_RULE> *).name()
            );
            return ERROR(RE_TYPE_ERROR, "unsupported effect handler type");
        }
    }

    /// Converts a packed value into an `msParam_t`.
    error convertToMsParam(boost::any &itr, msParam_t *t);
    /// Converts an `msParam_t` into a packed value.
    error convertFromMsParam(boost::any& itr, msParam_t *t);

    template <typename T>
    /// Extracts a typed value from `boost::any`, or calls a fallback.
    T any_extract(boost::any _p, std::function<T()> f) {
        if(_p.type() == typeid(T*)) {
            return *boost::any_cast<T*>(_p);

        } else if(_p.type() == typeid(T)){
            return boost::any_cast<T>(_p);

        } else {
            // rodsLog(LOG_ERROR, (std::string("cannot convert type ") + _p.type().name() + " to " + typeid(T).name()).c_str());
            return f();
        }
    }

    template <typename T>
    /// Writes a value back through a `boost::any` pointer payload when possible.
    void any_update(boost::any& _p, T _p2) {
        if(_p.type() == typeid(T*)) {
            *boost::any_cast<T*>(_p) = _p2;
        }
    }

    template<typename S, typename T, typename F>
    /// Iterates over two containers in lockstep.
    void foreach2(S&& a, T&& b, F&& f) {
        typename std::remove_reference<S>::type::iterator itr = a.begin();
        typename std::remove_reference<T>::type::iterator itr2 = b.begin();
        for(;itr!=a.end() && itr2!=b.end();++itr, ++itr2) {
          f(*itr, *itr2);
        }
    }

    template< typename InputC, typename OutputC, typename UnaryOperation >
    /// Applies a unary transform into an existing output container.
    void transform(const InputC &ic, OutputC &oc, UnaryOperation func) {
        std::transform(ic.begin(), ic.end(), oc.begin(), func);
    }

    template< typename InputC, typename OutputC, typename UnaryOperation >
    /// Applies a unary transform and returns a new output container.
    OutputC transform(const InputC &ic,  UnaryOperation func) {
        OutputC oc;
        std::transform(ic.begin(), ic.end(), oc.begin(), func);
        return oc;
    }

    /// Initializes the global list of configured rule engine packs.
    std::vector<re_pack_inp<default_re_ctx> > init_global_re_packs();
    /// Holds process-wide rule engine globals.
    struct global_re_plugin_mgr {
        microservice_manager<default_ms_ctx> global_ms_mgr; ///< Global microservice manager.
        std::vector<re_pack_inp<default_re_ctx> > global_re_packs = init_global_re_packs(); ///< Global rule engine pack list.
        rule_engine_plugin_manager<default_re_ctx> global_re_plugin_mgr = rule_engine_plugin_manager<default_re_ctx>(KW_CFG_PLUGIN_TYPE_RULE_ENGINE); ///< Global plugin manager.
        rule_engine_manager<default_re_ctx, default_ms_ctx> global_re_mgr = rule_engine_manager<default_re_ctx, default_ms_ctx>(global_re_plugin_mgr, global_re_packs, global_ms_mgr); ///< Global rule engine manager.
    };

    /// Process-wide rule engine globals.
    extern std::unique_ptr<struct irods::global_re_plugin_mgr> re_plugin_globals;
}
/// Defines the standard exported rule engine plugin factory functions.
#define DEFINE_FACTORY \
    irods::pluggable_rule_engine<irods::default_re_ctx>* plugin_factory(const std::string& _inst_name, const std::string& _context) { \
        return new irods::pluggable_rule_engine<irods::default_re_ctx>( _inst_name , _context); \
    } \
    double get_plugin_interface_version() { \
        static const double PLUGIN_INTERFACE_VERSION = 1.0; \
        return PLUGIN_INTERFACE_VERSION; \
    }

#endif
