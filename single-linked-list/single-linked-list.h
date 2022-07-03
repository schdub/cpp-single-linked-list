#pragma once

#include <cassert>
#include <cstddef>
#include <string>
#include <utility>
#include <algorithm>
#include <stack>

template <typename Type>
class SingleLinkedList {
    // Узел списка
    struct Node {
        Node() = default;
        Node(const Type& val, Node* next)
            : value(val)
            , next_node(next) {
        }
        Type value;
        Node* next_node = nullptr;
    };

    template <typename ValueType>
    class BasicIterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = Type;
        using difference_type = std::ptrdiff_t;
        using pointer = ValueType*;
        using reference = ValueType&;

        BasicIterator() = default;

        BasicIterator(const BasicIterator<Type>& other) noexcept {
            if (*this != other) {
                node_ = other.node_;
            }
        }
        BasicIterator& operator=(const BasicIterator& rhs) = default;

        [[nodiscard]]
        bool operator==(const BasicIterator<const Type>& rhs) const noexcept {
            return (this->node_ == rhs.node_);
        }
        [[nodiscard]]
        bool operator!=(const BasicIterator<const Type>& rhs) const noexcept {
            return !(*this == rhs);
        }
        [[nodiscard]]
        bool operator==(const BasicIterator<Type>& rhs) const noexcept {
            return (this->node_ == rhs.node_);
        }
        [[nodiscard]]
        bool operator!=(const BasicIterator<Type>& rhs) const noexcept {
            return !(*this == rhs);
        }

        BasicIterator& operator++() noexcept {
            assert(node_ != nullptr);
            node_ = node_->next_node;
            return *this;
        }
        BasicIterator operator++(int) noexcept {
            assert(node_ != nullptr);
            BasicIterator current(node_);
            node_ = node_->next_node;
            return current;
        }

        [[nodiscard]]
        reference operator*() const noexcept {
            assert(node_ != nullptr);
            return node_->value;
        }
        [[nodiscard]]
        pointer operator->() const noexcept {
            assert(node_ != nullptr);
            return &(node_->value);
        }

    private:
        friend class SingleLinkedList;
        explicit BasicIterator(Node* node)
            : node_(node)
        {}
        Node* node_ = nullptr;
    };

public:
    using Iterator = BasicIterator<Type>;
    using ConstIterator = BasicIterator<const Type>;

    SingleLinkedList(std::initializer_list<Type> values) {
        copy_reversed(values);
    }

    SingleLinkedList(const SingleLinkedList& other) {
        // Сначала надо удостовериться, что текущий список пуст
        assert(size_ == 0 && head_.next_node == nullptr);
        SingleLinkedList tmp;
        tmp.copy_reversed(other);
        swap(tmp);
    }

    SingleLinkedList& operator=(const SingleLinkedList& rhs) {
        if (this != &rhs) {
            SingleLinkedList tmp(rhs);
            swap(tmp);
        }
        return *this;
    }

    void swap(SingleLinkedList& other) noexcept {
        std::swap(this->head_.next_node, other.head_.next_node);
        std::swap(this->size_, other.size_);
    }

    [[nodiscard]]
    Iterator before_begin() noexcept {
        return Iterator{&head_};
    }
    [[nodiscard]]
    Iterator begin() noexcept {
        return Iterator{head_.next_node};
    }
    [[nodiscard]]
    Iterator end() noexcept {
        return Iterator{nullptr};
    }
    // Константные версии begin/end для обхода списка без возможности модификации его элементов
    [[nodiscard]]
    ConstIterator before_begin() const noexcept {
        return ConstIterator{&head_};
    }
    [[nodiscard]]
    ConstIterator begin() const noexcept {
        return ConstIterator{head_.next_node};
    }
    [[nodiscard]]
    ConstIterator end() const noexcept {
        return ConstIterator{nullptr};
    }
    // Методы для удобного получения константных итераторов у неконстантного контейнера
    [[nodiscard]]
    ConstIterator cbefore_begin() const noexcept {
        return ConstIterator{const_cast<Node*>(&head_)};
    }
    [[nodiscard]]
    ConstIterator cbegin() const noexcept {
        return ConstIterator{head_.next_node};
    }
    [[nodiscard]]
    ConstIterator cend() const noexcept {
        return ConstIterator{nullptr};
    }

    /*
     * Вставляет элемент value после элемента, на который указывает pos.
     * Возвращает итератор на вставленный элемент
     * Если при создании элемента будет выброшено исключение, список останется в прежнем состоянии
     */
    Iterator InsertAfter(ConstIterator pos, const Type& value) {
        Node * new_node = nullptr;
        try {
            new_node = new Node(value, pos.node_->next_node);
            pos.node_->next_node = new_node;
            size_ += 1;
        } catch (...) {
            if (new_node != nullptr) {
                delete new_node;
            }
            throw;
        }
        return Iterator{pos.node_->next_node};
    }

    void PopFront() noexcept {
        if (IsEmpty() == false) {
            assert(size_ > 0);
            assert(head_.next_node != nullptr);
            Node * ptrNext = head_.next_node->next_node;
            delete head_.next_node;
            head_.next_node = ptrNext;
            size_ -= 1;
        }
    }

    /*
     * Удаляет элемент, следующий за pos.
     * Возвращает итератор на элемент, следующий за удалённым
     */
    Iterator EraseAfter(ConstIterator pos) noexcept {
        if (pos.node_->next_node != nullptr) {
            Node * node_2del      = pos.node_->next_node;
            Node * node_remaining = node_2del->next_node;
            pos.node_->next_node  = node_remaining;
            delete node_2del;
            size_ -= 1;
        }
        return Iterator{pos.node_->next_node};
    }

    // Возвращает количество элементов в списке за время O(1)
    [[nodiscard]] size_t GetSize() const noexcept {
        return size_;
    }

    // Сообщает, пустой ли список за время O(1)
    [[nodiscard]] bool IsEmpty() const noexcept {
        return (GetSize() == 0);
    }

    void PushFront(const Type & value) {
        head_.next_node = new Node(value, head_.next_node);
        ++size_;
    }

    void Clear() noexcept {
        for (Node *p = head_.next_node, *t = nullptr ; p != nullptr; ) {
            t = p->next_node;
            delete p;
            p = t;
        }
        head_.next_node = nullptr;
        size_ = 0;
    }

    SingleLinkedList() = default;
    ~SingleLinkedList() {
        Clear();
    }

private:

    template <typename Container>
    void copy_reversed(const Container& container) {
        std::stack<Type> rev_stack;
        for (const auto t : container) {
            rev_stack.push(t);
        }
        while (rev_stack.empty() == false) {
            PushFront(rev_stack.top());
            rev_stack.pop();
        }
    }

    Node head_ = Node();
    size_t size_ = 0;
};

template <typename Type>
void swap(SingleLinkedList<Type>& lhs, SingleLinkedList<Type>& rhs) noexcept {
    lhs.swap(rhs);
}

template <typename Type>
bool operator==(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
bool operator!=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return !(lhs == rhs);
}

template <typename Type>
bool operator<(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
bool operator<=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return !(rhs < lhs);
}

template <typename Type>
bool operator>(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return (rhs < lhs);
}

template <typename Type>
bool operator>=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return !(rhs > lhs);
}
