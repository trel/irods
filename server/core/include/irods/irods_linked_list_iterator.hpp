#ifndef IRODS_LINKED_LIST_ITERATOR_HPP
#define IRODS_LINKED_LIST_ITERATOR_HPP

/// \file

#include <iterator>
#include <type_traits>
#include <utility>

namespace irods
{
    /// \brief SFINAE helper requiring a public `next` pointer of type `T*`.
    /// \tparam T Node type to inspect.
    template <typename T>
    using has_next_pointer = std::enable_if_t<std::is_same<T*, decltype(std::declval<T>().next)>::value>;

    /// \brief Removes cv-qualifiers and references from a type.
    /// \tparam T Type to normalize.
    template <typename T>
    using remove_cvref_t = std::remove_cv_t<std::remove_reference_t<T>>;

    /// \brief Forward iterator over a singly linked list exposing a `next` pointer.
    /// \tparam T Node type stored by the linked list.
    template <typename T,
              typename = has_next_pointer<remove_cvref_t<T>>>
    class linked_list_iterator
    {
    public:
        using difference_type   = long; ///< Type used to express iterator distances.
        using value_type        = T; ///< Node value type.
        using pointer           = value_type*; ///< Pointer to the current node.
        using reference         = value_type&; ///< Reference to the current node.
        using iterator_category = std::forward_iterator_tag; ///< Iterator category tag.

        /// \brief Constructs an end iterator.
        linked_list_iterator() = default;

        /// \brief Constructs an iterator positioned at the provided node.
        /// \param[in] _node Pointer to the current node.
        explicit linked_list_iterator(pointer _node)
            : node_{_node}
        {
        }

        /// \brief Advances the iterator to the next node.
        /// \return Reference to the advanced iterator.
        linked_list_iterator& operator++() noexcept
        {
            if (node_) {
                node_ = node_->next;
            }

            return *this;
        }

        /// \brief Advances the iterator and returns its previous value.
        /// \return Iterator value before advancement.
        linked_list_iterator operator++(int) noexcept
        {
            auto it{*this};
            ++(*this);
            return it;
        }

        /// \brief Returns whether two iterators reference the same node.
        /// \param[in] _other Iterator to compare against.
        /// \return True if both iterators reference the same node.
        bool operator==(const linked_list_iterator& _other) const noexcept
        {
            return node_ == _other.node_;
        }

        /// \brief Returns whether two iterators reference different nodes.
        /// \param[in] _other Iterator to compare against.
        /// \return True if the iterators reference different nodes.
        bool operator!=(const linked_list_iterator& _other) const noexcept
        {
            return !(*this == _other);
        }

        /// \brief Returns a reference to the current node.
        /// \return Reference to the node referenced by the iterator.
        reference operator*() const
        {
            return *node_;
        }

        /// \brief Returns a pointer to the current node.
        /// \return Pointer to the node referenced by the iterator.
        pointer operator->() const
        {
            return node_;
        }

    private:
        pointer node_; ///< Pointer to the current node.
    }; // linked_list_iterator
} // namespace irods

#endif // IRODS_LINKED_LIST_ITERATOR_HPP
