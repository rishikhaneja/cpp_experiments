#pragma once

#include <cstddef>
#include <iterator>
#include <type_traits>

template <typename T>
struct range {
    T m_begin;
    T m_end;
    T& begin() { return m_begin; }
    const T& begin() const { return m_begin; }
    T& end() { return m_end; }
    const T& end() const { return m_end; }
};

auto make_range = [](auto a, auto b) {
    static_assert(std::is_same<decltype(a), decltype(b)>::value);
    return range<decltype(a)>{a, b};
};

template <typename T>
struct indexed_iterator {
    using difference_type = size_t;
    using value_type = T;
    using pointer = T*;
    using reference = T&;
    using iterator_category = std::forward_iterator_tag;
    indexed_iterator(const T& it, size_t index, size_t distance)
        : m_it(it), m_index(index), m_last(distance - 1) {}
    indexed_iterator(const indexed_iterator& other)
        : m_it(other.m_it), m_index(other.m_index), m_last(other.m_last) {}
    T m_it;
    size_t m_index;
    size_t m_last;
    size_t index() const { return m_index; }
    typename T::value_type value() const { return *m_it; }
    bool is_first() const { return m_index == 0; }
    bool is_last() const { return m_index == m_last; }
    indexed_iterator& operator++(int) {
        ++m_index;
        ++m_it;
        return *this;
    }
    void operator++() {
        ++m_index;
        ++m_it;
    }
    indexed_iterator& operator*() { return *this; }
    const indexed_iterator& operator*() const { return *this; }
    bool operator!=(const indexed_iterator& other) {
        return m_it != other.m_it;
    }
};

auto make_indexed_iterator = [](auto it, size_t index, size_t distance) {
    return indexed_iterator<decltype(it)>{it, index, distance};
};

auto make_indexed_range = [](const auto&... args) {
    auto make_indexed_range_1 = [](const auto& a) {
        auto d = distance(begin(a), end(a));
        return make_range(make_indexed_iterator(begin(a), 0, d),
                          make_indexed_iterator(end(a), d, d));
    };
    auto make_indexed_range_2 = [](auto a, auto b) {
        static_assert(std::is_same<decltype(a), decltype(b)>::value);
        auto d = distance(a, b);
        return make_range(make_indexed_iterator(a, 0, d),
                          make_indexed_iterator(b, d, d));
    };
    static_assert(sizeof...(args) == 1 || sizeof...(args) == 2);
    if constexpr (sizeof...(args) == 1) {
        return make_indexed_range_1(args...);
    } else if constexpr (sizeof...(args) == 2) {
        return make_indexed_range_2(args...);
    }
};