#include "irods/rodsErrorTable.h"
#include <boost/format.hpp>
#include <iostream>

// The matcher class
template <typename T = IRODS_ERROR_ENUM>
/// \brief Catch2 matcher for comparing iRODS error values.
class error_enum : public Catch::Matchers::MatcherBase<T>
{
    T rhs_;
public:
    /// Constructs a matcher for the provided error value.
    explicit error_enum(const T _rhs) : rhs_{_rhs} {}

    // Performs the test for this matcher
    /// Returns true if the tested value matches the expected error value.
    bool match(T const& _lhs) const override {
        return _lhs == rhs_;
    }

    // Produces a string describing what this matcher does. It should
    // include any provided data (the begin/ end in this case) and
    // be written as if it were stating a fact (in the output it will be
    // preceded by the value under test).
    /// Describes the matcher expectation for test output.
    virtual std::string describe() const override {
        return (boost::format("is equal to %lld") % rhs_).str();
    }
};

// The builder function
/// Builds an iRODS error matcher for the provided value.
inline error_enum<long long> equals_irods_error(const long long _rhs) {
    return error_enum(_rhs);
}
