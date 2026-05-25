#ifndef IRODS_GENQUERY2_ABSTRACT_SYNTAX_TREE_DATA_TYPES_HPP
#define IRODS_GENQUERY2_ABSTRACT_SYNTAX_TREE_DATA_TYPES_HPP

#include <boost/variant.hpp>

#include <string>
#include <utility>
#include <variant>
#include <vector>

namespace irods::experimental::genquery2
{
    /// \brief Represents a projected or referenced column.
    struct column
    {
        /// \brief Constructs an empty column node.
        column() = default;

        /// Constructs a column reference with a column name.
        ///
        /// \param[in] name The column name.
        explicit column(std::string name)
            : name{std::move(name)}
        {
        }

        /// Constructs a column reference with a column name and type name.
        ///
        /// \param[in] name The column name.
        /// \param[in] type_name The associated type name.
        column(std::string name, std::string type_name)
            : name{std::move(name)}
            , type_name{std::move(type_name)}
        {
        }

        std::string name;      ///< The column name.
        std::string type_name; ///< The associated type name.
    }; // struct column

    /// \brief Represents a function call expression.
    struct function
    {
        /// \brief Constructs an empty function node.
        function() = default;

        /// Constructs a function call node.
        ///
        /// \param[in] name The function name.
        /// \param[in] _arguments The arguments passed to the function.
        /// \param[in] _distinct Indicates whether the function applies DISTINCT semantics.
        function(std::string name,
                 std::vector<std::variant<std::string, column, function>> _arguments,
                 bool _distinct = false)
            : name{std::move(name)}
            , arguments{std::move(_arguments)}
            , distinct{_distinct}
        {
        }

        std::string name;                                        ///< The function name.
        std::vector<std::variant<std::string, column, function>> arguments; ///< The function arguments.
        bool distinct;                                           ///< Indicates whether DISTINCT is applied.
    }; // struct function

    /// \brief Represents a LIKE comparison.
    struct condition_like
    {
        /// \brief Constructs an empty LIKE condition.
        condition_like() = default;

        /// Constructs a LIKE condition expression.
        ///
        /// \param[in] string_literal The comparison pattern.
        explicit condition_like(std::string string_literal)
            : string_literal{std::move(string_literal)}
        {
        }

        std::string string_literal; ///< The comparison pattern.
    }; // struct condition_like

    /// \brief Represents an IN comparison.
    struct condition_in
    {
        /// \brief Constructs an empty IN condition.
        condition_in() = default;

        /// Constructs an IN condition expression.
        ///
        /// \param[in] list_of_string_literals The set of candidate string literals.
        explicit condition_in(std::vector<std::string> list_of_string_literals)
            : list_of_string_literals{std::move(list_of_string_literals)}
        {
        }

        std::vector<std::string> list_of_string_literals; ///< The list of candidate values.
    }; // struct condition_in

    /// \brief Represents a BETWEEN comparison.
    struct condition_between
    {
        /// \brief Constructs an empty BETWEEN condition.
        condition_between() = default;

        /// Constructs a BETWEEN condition expression.
        ///
        /// \param[in] low The lower bound string literal.
        /// \param[in] high The upper bound string literal.
        condition_between(std::string low, std::string high)
            : low{std::move(low)}
            , high{std::move(high)}
        {
        }

        std::string low;  ///< The lower bound.
        std::string high; ///< The upper bound.
    }; // struct condition_between

    /// \brief Represents an equality comparison.
    struct condition_equal
    {
        /// \brief Constructs an empty equality condition.
        condition_equal() = default;

        /// Constructs an equality condition expression.
        ///
        /// \param[in] string_literal The comparison value.
        explicit condition_equal(std::string string_literal)
            : string_literal{std::move(string_literal)}
        {
        }

        std::string string_literal; ///< The comparison value.
    }; // struct condition_equal

    /// \brief Represents an inequality comparison.
    struct condition_not_equal
    {
        /// \brief Constructs an empty inequality condition.
        condition_not_equal() = default;

        /// Constructs an inequality condition expression.
        ///
        /// \param[in] string_literal The comparison value.
        explicit condition_not_equal(std::string string_literal)
            : string_literal{std::move(string_literal)}
        {
        }

        std::string string_literal; ///< The comparison value.
    }; // struct condition_not_equal

    /// \brief Represents a less-than comparison.
    struct condition_less_than
    {
        /// \brief Constructs an empty less-than condition.
        condition_less_than() = default;

        /// Constructs a less-than condition expression.
        ///
        /// \param[in] string_literal The comparison value.
        explicit condition_less_than(std::string string_literal)
            : string_literal{std::move(string_literal)}
        {
        }

        std::string string_literal; ///< The comparison value.
    }; // struct condition_less_than

    /// \brief Represents a less-than-or-equal comparison.
    struct condition_less_than_or_equal_to
    {
        /// \brief Constructs an empty less-than-or-equal condition.
        condition_less_than_or_equal_to() = default;

        /// Constructs a less-than-or-equal condition expression.
        ///
        /// \param[in] string_literal The comparison value.
        explicit condition_less_than_or_equal_to(std::string string_literal)
            : string_literal{std::move(string_literal)}
        {
        }

        std::string string_literal; ///< The comparison value.
    }; // struct condition_less_than_or_equal_to

    /// \brief Represents a greater-than comparison.
    struct condition_greater_than
    {
        /// \brief Constructs an empty greater-than condition.
        condition_greater_than() = default;

        /// Constructs a greater-than condition expression.
        ///
        /// \param[in] string_literal The comparison value.
        explicit condition_greater_than(std::string string_literal)
            : string_literal{std::move(string_literal)}
        {
        }

        std::string string_literal; ///< The comparison value.
    }; // struct condition_greater_than

    /// \brief Represents a greater-than-or-equal comparison.
    struct condition_greater_than_or_equal_to
    {
        /// \brief Constructs an empty greater-than-or-equal condition.
        condition_greater_than_or_equal_to() = default;

        /// Constructs a greater-than-or-equal condition expression.
        ///
        /// \param[in] string_literal The comparison value.
        explicit condition_greater_than_or_equal_to(std::string string_literal)
            : string_literal{std::move(string_literal)}
        {
        }

        std::string string_literal; ///< The comparison value.
    }; // struct condition_greater_than_or_equal_to

    /// \brief Represents an IS NULL comparison.
    struct condition_is_null
    {
    }; // struct condition_is_null

    /// \brief Represents an IS NOT NULL comparison.
    struct condition_is_not_null
    {
    }; // struct condition_is_not_null

    struct condition_operator_not;

    /// \brief Holds a concrete condition expression variant.
    using condition_expression = boost::variant<condition_like,
                                                condition_in,
                                                condition_between,
                                                condition_equal,
                                                condition_not_equal,
                                                condition_less_than,
                                                condition_less_than_or_equal_to,
                                                condition_greater_than,
                                                condition_greater_than_or_equal_to,
                                                condition_is_null,
                                                condition_is_not_null,
                                                boost::recursive_wrapper<condition_operator_not>>;

    /// \brief Represents a negated condition expression.
    struct condition_operator_not
    {
        /// \brief Constructs an empty negated condition.
        condition_operator_not() = default;

        /// Constructs a negated condition expression.
        ///
        /// \param[in] expression The condition expression to negate.
        condition_operator_not(condition_expression expression)
            : expression{std::move(expression)}
        {
        }

        condition_expression expression; ///< The condition expression to negate.
    }; // struct condition_operator_not

    /// \brief Represents a comparison against a column or function.
    struct condition
    {
        /// \brief Constructs an empty condition node.
        condition() = default;

        /// Constructs a condition against a column.
        ///
        /// \param[in] column The column on the left-hand side of the condition.
        /// \param[in] expression The comparison expression applied to the column.
        condition(column column, condition_expression expression)
            : lhs{std::move(column)}
            , expression{std::move(expression)}
        {
        }

        /// Constructs a condition against a function call.
        ///
        /// \param[in] func The function call on the left-hand side of the condition.
        /// \param[in] expression The comparison expression applied to the function result.
        condition(function func, condition_expression expression)
            : lhs{std::move(func)}
            , expression{std::move(expression)}
        {
        }

        std::variant<column, function> lhs; ///< The left-hand side expression.
        condition_expression expression;    ///< The comparison expression.
    }; // struct condition

    struct logical_and;
    struct logical_or;
    struct logical_not;
    struct logical_grouping;

    // clang-format off
    /// \brief Holds a single projection expression.
    using projection = boost::variant<function, column>;
    /// \brief Holds a list of projection expressions.
    using projections = std::vector<projection>;
    /// \brief Holds a logical or leaf condition node.
    using condition_wrapper = boost::variant<logical_and, logical_or, logical_not, logical_grouping, condition>;
    /// \brief Holds a list of condition nodes.
    using conditions = std::vector<condition_wrapper>;
    // clang-format on

    /// \brief Represents a logical AND expression.
    struct logical_and
    {
        conditions condition; ///< The nested conditions joined by AND.
    }; // struct logical_and

    /// \brief Represents a logical OR expression.
    struct logical_or
    {
        conditions condition; ///< The nested conditions joined by OR.
    }; // struct logical_or

    /// \brief Represents a logical NOT expression.
    struct logical_not
    {
        conditions condition; ///< The nested conditions negated by NOT.
    }; // struct logical_not

    /// \brief Represents a parenthesized logical expression.
    struct logical_grouping
    {
        conditions conditions; ///< The grouped logical expression.
    }; // struct logical_grouping

    /// \brief Represents one ORDER BY expression.
    struct sort_expression
    {
        std::variant<column, function> expr; ///< The expression to sort by.
        bool ascending_order = true;         ///< Indicates ascending sort order when true.
    }; // struct sort_expression

    /// \brief Represents a GROUP BY clause.
    struct group_by
    {
        std::vector<std::variant<function, column>> expressions; ///< The grouping expressions.
    }; // struct group_by

    /// \brief Represents an ORDER BY clause.
    struct order_by
    {
        std::vector<sort_expression> sort_expressions; ///< The ordering expressions.
    }; // struct order_by

    /// \brief Represents an OFFSET and row-count range.
    struct range
    {
        std::string offset;         ///< The starting row offset.
        std::string number_of_rows; ///< The requested number of rows.
    }; // struct range

    /// \brief Represents a SELECT query AST node.
    struct select
    {
        /// \brief Constructs an empty SELECT node.
        select() = default;

        /// Constructs a select AST node with projections and conditions.
        ///
        /// \param[in] projections The projection expressions requested by the query.
        /// \param[in] conditions The filter expressions applied to the query.
        select(projections projections, conditions conditions)
            : projections(std::move(projections))
            , conditions(std::move(conditions))
        {
        }

        projections projections; ///< The requested projection expressions.
        conditions conditions;   ///< The filter expressions.
        group_by group_by;       ///< The GROUP BY clause.
        order_by order_by;       ///< The ORDER BY clause.
        range range;             ///< The OFFSET/LIMIT clause.
        bool distinct = false;   ///< Indicates whether SELECT DISTINCT is requested.
    }; // struct select
} // namespace irods::experimental::genquery2

#endif // IRODS_GENQUERY2_ABSTRACT_SYNTAX_TREE_DATA_TYPES_HPP
