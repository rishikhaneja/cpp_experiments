#pragma once

// ---------------------------------------------------------------------

#include <cassert>
#include <optional>
#include <vector>
#include "range.h"

// ---------------------------------------------------------------------

namespace detail {

template <typename T>
struct Node {
    T m_value;
    Node* m_parent;
    std::vector<Node> m_children;
    size_t m_index;
    Node(const T& value, Node* parent)
        : m_value(value),
          m_parent(parent),
          m_index(parent ? parent->m_children.size() : 0) {}
    ~Node() = default;
    Node(const Node& other) = default;
    Node(Node&& other) = default;
    Node& operator=(const Node& other) = default;
    Node& operator=(Node&& other) = default;
    T& value() { return m_value; }
    auto parent() const { return m_parent; }
    auto children() { return make_range(m_children); }
    Node* next() {
        if (Node* firstchild = first_child()) {
            return firstchild;
        } else if (Node* nextsibling = next_sibling()) {
            return nextsibling;
        } else {
            Node* current = this;
            while (current = current->parent()) {
                if (Node* nextsibling = current->next_sibling()) {
                    return nextsibling;
                }
            }
            return nullptr;
        }
    }
    Node* first_child() {
        return m_children.empty() ? nullptr : &m_children.front();
    }
    bool is_last_child() const {
        return m_parent == nullptr ||
               m_index + 1 == m_parent->m_children.size();
    }
    Node* next_sibling() const {
        return is_last_child() ? nullptr : &(m_parent->m_children[m_index + 1]);
    }
    Node* insert(const T& value) {
        m_children.push_back(Node(value, this));
        return &m_children.back();
    }
};

template <typename T>
struct Tree {
    struct iterator {
        Node<T>* m_node = nullptr;
        iterator& operator++() {
            assert(m_node && "operator++ called on end");
            m_node = m_node->next();
            return *this;
        }
        T& operator*() {
            assert(m_node && "operator* called on end");
            return m_node->value();
        }
        bool operator==(const iterator& other) const {
            return this->m_node == other.m_node;
        }
        bool operator!=(const iterator& other) { return !(*this == other); }
    };
    std::optional<Node<T>> m_root;
    auto begin() { return m_root ? iterator{&*m_root} : end(); }
    const auto begin() const { return m_root ? iterator{&*m_root} : end(); }
    auto end() { return iterator{}; }
    const auto end() const { return iterator{}; }
    iterator insert(const iterator& where, const T& value) {
        if (where == end()) {
            assert(!m_root && "re-inserting root");
            m_root = Node<T>(value, nullptr);
            return iterator{&*m_root};
        } else {
            return iterator{where.m_node->insert(value)};
        }
    }
    auto children(const iterator& where) {
        assert(where.m_node && "invalid iterator");
        return where.m_node->children();
    }
};

}  // namespace detail

template <typename T>
auto make_tree() {
    return detail::Tree<T>{};
}

// ---------------------------------------------------------------------

// ---------------------------------------------------------------------