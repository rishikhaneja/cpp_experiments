#pragma once

// ---------------------------------------------------------------------

#include <cstddef>
#include <iterator>
#include <type_traits>

// ---------------------------------------------------------------------

namespace detail {

template <typename T>
struct Range {
    // -----------------------------------------------------------------------
    T m_begin;
    T m_end;
    // -----------------------------------------------------------------------
    T& begin() { return m_begin; }
    const T& begin() const { return m_begin; }
    T& end() { return m_end; }
    const T& end() const { return m_end; }
    // -----------------------------------------------------------------------
};  // Range

}  // namespace detail

/**
 * @brief makes a Range
 * @param a pair of iterators
 */
auto make_range = [](const auto&... args) {
    // -----------------------------------------------------------------------
    static_assert(sizeof...(args) == 1 || sizeof...(args) == 2);
    // -----------------------------------------------------------------------
    auto make_range_from_range = [&](const auto& a) {
        return detail::Range<decltype(begin(a))>{begin(a), end(a)};
    };
    // -----------------------------------------------------------------------
    auto make_range_from_pairs = [&](auto a, auto b) {
        static_assert(std::is_same<decltype(a), decltype(b)>::value);
        return detail::Range<decltype(a)>{a, b};
    };
    // -----------------------------------------------------------------------
    if constexpr (sizeof...(args) == 1) {
        return make_range_from_range(args...);
    } else if constexpr (sizeof...(args) == 2) {
        return make_range_from_pairs(args...);
    }
};

// ---------------------------------------------------------------------

namespace detail {

template <typename T>
struct indexed_iterator {
    // -----------------------------------------------------------------------
    using difference_type = size_t;
    using value_type = T;
    using pointer = T*;
    using reference = T&;
    using iterator_category = std::forward_iterator_tag;
    // -----------------------------------------------------------------------
    T m_it;
    size_t m_index;
    size_t m_last;
    // -----------------------------------------------------------------------
    indexed_iterator(const T& it, size_t index, size_t distance)
        : m_it(it), m_index(index), m_last(distance - 1) {}
    indexed_iterator(const indexed_iterator& other)
        : m_it(other.m_it), m_index(other.m_index), m_last(other.m_last) {}
    // -----------------------------------------------------------------------
    size_t index() const { return m_index; }
    typename T::value_type value() const { return *m_it; }
    bool is_first() const { return m_index == 0; }
    bool is_last() const { return m_index == m_last; }
    // -----------------------------------------------------------------------
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
    // -----------------------------------------------------------------------
};  // indexed_iterator

}  // namespace detail

/**
 * @brief makes an indexed Range
 * @param a Range or a pair of iterators
 */
auto make_indexed_range = [](const auto&... args) {
    // -----------------------------------------------------------------------
    static_assert(sizeof...(args) == 1 || sizeof...(args) == 2);
    // -----------------------------------------------------------------------
    auto make_indexed_iterator = [](auto it, size_t index, size_t distance) {
        return detail::indexed_iterator<decltype(it)>{it, index, distance};
    };
    // -----------------------------------------------------------------------
    auto make_indexed_range_from_range = [&](const auto& a) {
        auto d = distance(begin(a), end(a));
        return make_range(make_indexed_iterator(begin(a), 0, d),
                          make_indexed_iterator(end(a), d, d));
    };
    // -----------------------------------------------------------------------
    auto make_indexed_range_from_pairs = [&](auto a, auto b) {
        static_assert(std::is_same<decltype(a), decltype(b)>::value);
        auto d = distance(a, b);
        return make_range(make_indexed_iterator(a, 0, d),
                          make_indexed_iterator(b, d, d));
    };
    // -----------------------------------------------------------------------
    if constexpr (sizeof...(args) == 1) {
        return make_indexed_range_from_range(args...);
    } else if constexpr (sizeof...(args) == 2) {
        return make_indexed_range_from_pairs(args...);
    }
    // -----------------------------------------------------------------------
};  // make_indexed_range

// ---------------------------------------------------------------------