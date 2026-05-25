#ifndef IRODS_RE_RULEEXISTSHELPER_HPP
#define IRODS_RE_RULEEXISTSHELPER_HPP

#include "irods/irods_error.hpp"

#include <vector>
#include <string>

#include <boost/regex.hpp>

/// Provides cached regex checks for rule and PEP existence tests.
class RuleExistsHelper {
public:
    /// Returns the process-wide singleton instance.
    static RuleExistsHelper* Instance();
    /// Registers a rule name pattern for later existence checks.
    void registerRuleRegex( const std::string& _regex );
    /// Returns whether the named operation matches a registered rule.
    bool checkOperation( const std::string& _op_name );
    /// Returns whether a pre-PEP exists for the namespace and operation.
    bool checkPrePep( const std::string& _ns, const std::string& _op_name );
    /// Returns whether a post-PEP exists for the namespace and operation.
    bool checkPostPep( const std::string& _ns, const std::string& _op_name );
    /// Returns whether a dynamic PEP exists for the namespace and operation.
    bool checkDynPeps( const std::string& _ns, const std::string& _op_name );
protected:
private:
    /// Constructs the singleton helper.
    RuleExistsHelper(){};
    /// Singleton instance storage.
    static RuleExistsHelper* _instance;
    /// Compiled regular expressions used for rule existence checks.
    std::vector<boost::regex> ruleRegexes;
};

#endif // IRODS_RE_RULEEXISTSHELPER_HPP
