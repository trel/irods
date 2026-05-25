#ifndef IRODS_GENQUERY2_ABSTRACT_SYNTAX_TREE_DATA_TYPES_HPP
#define IRODS_GENQUERY2_ABSTRACT_SYNTAX_TREE_DATA_TYPES_HPP

#include <boost/variant.hpp>

#include <string>
#include <utility>
#include <variant>
#include <vector>

namespace irods::experimental::genquery2
{
    struct column
    {
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

        std::string name;
        std::string type_name;
    }; // struct column

    struct function
    {
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

        std::string name;
        std::vector<std::variant<std::string, column, function>> arguments;
        bool distinct;
    }; // struct function

    struct condition_like
    {
        condition_like() = default;

        /// Constructs a LIKE condition expression.
        ///
        /// \param[in] string_literal The comparison pattern.
        explicit condition_like(std::string string_literal)
            : string_literal{std::move(string_literal)}
        {
        }

        std::string string_literal;
    }; // struct condition_like

    struct condition_in
    {
        condition_in() = default;

        /// Constructs an IN condition expression.
        ///
        /// \param[in] list_of_string_literals The set of candidate string literals.
        explicit condition_in(std::vector<std::string> list_of_string_literals)
            : list_of_string_literals{std::move(list_of_string_literals)}
        {
        }

        std::vector<std::string> list_of_string_literals;
    }; // struct condition_in

    struct condition_between
    {
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

        std::string low;
        std::string high;
    }; // struct condition_between

    struct condition_equal
    {
        condition_equal() = default;

        /// Constructs an equality condition expression.
        ///
        /// \param[in] string_literal The comparison value.
        explicit condition_equal(std::string string_literal)
            : string_literal{std::move(string_literal)}
        {
        }

        std::string string_literal;
    }; // struct condition_equal

    struct condition_not_equal
    {
        condition_not_equal() = default;

        /// Constructs an inequality condition expression.
        ///
        /// \param[in] string_literal The comparison value.
        explicit condition_not_equal(std::string string_literal)
            : string_literal{std::move(string_literal)}
        {
        }

        std::string string_literal;
    }; // struct condition_not_equal

    struct condition_less_than
    {
        condition_less_than() = default;

        /// Constructs a less-than condition expression.
        ///
        /// \param[in] string_literal The comparison value.
        explicit condition_less_than(std::string string_literal)
            : string_literal{std::move(string_literal)}
        {
        }

        std::string string_literal;
    }; // struct condition_less_than

    struct condition_less_than_or_equal_to
    {
        condition_less_than_or_equal_to() = default;

        /// Constructs a less-than-or-equal condition expression.
        ///
        /// \param[in] string_literal The comparison value.
        explicit condition_less_than_or_equal_to(std::string string_literal)
            : string_literal{std::move(string_literal)}
        {
        }

        std::string string_literal;
    }; // struct condition_less_than_or_equal_to

    struct condition_greater_than
    {
        condition_greater_than() = default;

        /// Constructs a greater-than condition expression.
        ///
        /// \param[in] string_literal The comparison value.
        explicit condition_greater_than(std::string string_literal)
            : string_literal{std::move(string_literal)}
        {
        }

        std::string string_literal;
    }; // struct condition_greater_than

    struct condition_greater_than_or_equal_to
    {
        condition_greater_than_or_equal_to() = default;

        /// Constructs a greater-than-or-equal condition expression.
        ///
        /// \param[in] string_literal The comparison value.
        explicit condition_greater_than_or_equal_to(std::string string_literal)
            : string_literal{std::move(string_literal)}
        {
        }

        std::string string_literal;
    }; // struct condition_greater_than_or_equal_to

    struct condition_is_null
    {
    }; // struct condition_is_null

    struct condition_is_not_null
    {
    }; // struct condition_is_not_null

    struct condition_operator_not;

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

    struct condition_operator_not
    {
        condition_operator_not() = default;

        /// Constructs a negated condition expression.
        ///
        /// \param[in] expression The condition expression to negate.
        condition_operator_not(condition_expression expression)
            : expression{std::move(expression)}
        {
        }

        condition_expression expression;
    }; // struct condition_operator_not

    struct condition
    {
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

        std::variant<column, function> lhs;
        condition_expression expression;
    }; // struct condition

    struct logical_and;
    struct logical_or;
    struct logical_not;
    struct logical_grouping;

    // clang-format off
    using projection        = boost::variant<function, column>;
    using projections       = std::vector<projection>;
    using condition_wrapper = boost::variant<logical_and, logical_or, logical_not, logical_grouping, condition>;
    using conditions        = std::vector<condition_wrapper>;
    // clang-format on

    struct logical_and
    {
        conditions condition;
    }; // struct logical_and

    struct logical_or
    {
        conditions condition;
    }; // struct logical_or

    struct logical_not
    {
        conditions condition;
    }; // struct logical_not

    struct logical_grouping
    {
        conditions conditions;
    }; // struct logical_grouping

    struct sort_expression
    {
        std::variant<column, function> expr;
        bool ascending_order = true;
    }; // struct sort_expression

    struct group_by
    {
        std::vector<std::variant<function, column>> expressions;
    }; // struct group_by

    struct order_by
    {
        std::vector<sort_expression> sort_expressions;
    }; // struct order_by

    struct range
    {
        std::string offset;
        std::string number_of_rows;
    }; // struct range

    struct select
    {
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

        projections projections;
        conditions conditions;
        group_by group_by;
        order_by order_by;
        range range;
        bool distinct = false;
    }; // struct select
} // namespace irods::experimental::genquery2

#endif // IRODS_GENQUERY2_ABSTRACT_SYNTAX_TREE_DATA_TYPES_HPP
