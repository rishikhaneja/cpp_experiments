#include <algorithm>
#include <cassert>
#include <iostream>
#include <map>
#include <memory>
#include <numeric>
#include <optional>
#include <stack>
#include <string_view>
#include <type_traits>
#include <vector>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

using namespace std;
using namespace testing;

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
    static_assert(is_same<decltype(a), decltype(b)>::value);
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

auto make_indexed_range_1 = [](const auto& a) {
    auto d = distance(begin(a), end(a));
    return make_range(make_indexed_iterator(begin(a), 0, d),
                      make_indexed_iterator(end(a), d, d));
};

auto make_indexed_range_2 = [](auto a, auto b) {
    static_assert(is_same<decltype(a), decltype(b)>::value);
    auto d = distance(a, b);
    return make_range(make_indexed_iterator(a, 0, d),
                      make_indexed_iterator(b, d, d));
};

auto make_indexed_range = [](const auto&... args) {
    static_assert(sizeof...(args) == 1 || sizeof...(args) == 2);
    if constexpr (sizeof...(args) == 1) {
        return make_indexed_range_1(args...);
    } else if constexpr (sizeof...(args) == 2) {
        return make_indexed_range_2(args...);
    }
};

auto print_vector = [](auto a, auto b) {
    for (const auto& x : make_indexed_range(a, b)) {
        if (x.is_first()) {
            cout << "(";
        } else {
            cout << ", ";
        }
        cout << x.value();
        if (x.is_last()) {
            cout << ")\n";
        }
    }
};

auto print_map = [](auto a, auto b) {
    for (const auto& x : make_indexed_range(a, b)) {
        if (x.is_first()) {
            cout << "[";
        } else {
            cout << ", ";
        }
        cout << "(" << x.value().first << ":" << x.value().second << ")";
        if (x.is_last()) {
            cout << "]\n";
        }
    }
};

TEST(per_day, 0) {
    auto v = []() {
        vector<int> v(10);
        generate(begin(v), end(v), []() { return rand() % 3 + 1; });
        return v;
    }();
    print_vector(begin(v), end(v));
    print_vector(rbegin(v), rend(v));
    sort(begin(v), end(v));
    print_vector(begin(v), end(v));
    auto m = [&v]() {
        map<int, int> m;
        auto v_range = make_indexed_range(v);
        transform(
            begin(v_range),
            end(v_range),
            inserter(m, m.end()),
            [](const auto& i) { return make_pair(i.index(), i.value()); });
        return m;
    }();
    print_map(begin(m), end(m));
    {
        auto it = remove(begin(v), end(v), 1);
        print_vector(begin(v), end(v));
        v.erase(it, v.end());
        print_vector(begin(v), end(v));
    }
}
// Given a list of numbers and a number k, return whether any two numbers from
// the list add up to k. For example, given [10, 15, 3, 7] and k of 17, return
// true since 10 + 7 is 17.
TEST(per_day, 1) {
    auto solve = [](auto first, auto last, auto sum) {
        static_assert(std::is_same_v<decltype(first), decltype(last)>);
        using value_type =
            typename std::iterator_traits<decltype(first)>::value_type;
        std::vector<std::pair<value_type, value_type>> result;
        std::vector<value_type> arr(first, last);
        std::sort(begin(arr), end(arr));
        auto middle = [&arr] {
            auto it = begin(arr);
            std::advance(it, (std::distance(begin(arr), end(arr)) + 1) / 2);
            return it;
        }();
        std::for_each(begin(arr), middle, [&](const int& x) {
            auto y = std::lower_bound(begin(arr), end(arr), sum - x);
            if (y != arr.end() && *y == sum - x && &*y != &x) {
                result.push_back(make_pair(x, *y));
            }
        });
        return result;
    };
    auto test = [&](auto first, auto last) {
        auto x = solve(first, last, 6);
        for_each(std::begin(x), std::end(x), [](const auto& i) {
            std::cout << "(" << i.first << "," << i.second << ")" << std::endl;
        });
        std::cout << std::endl;
    };
    int arr[] = {1, 2, 3, 4, 5, 6};
    test(begin(arr), end(arr));
    int arr2[] = {1, 2, 3, 4, 5};
    test(begin(arr2), end(arr2));
}

// Given an array of integers, return a new array such that each element at
// index i of the new array is the product of all the numbers in the original
// array except the one at i.
// For example, if our input was [1, 2, 3, 4, 5], the expected output would be
// [120, 60, 40, 30, 24]. If our input was [3, 2, 1], the expected output would
// be [2, 3, 6].
// Follow-up: what if you can't use division?
TEST(per_day, 2) {
    auto solve = [](const auto& v) {
        vector<int> r;
        r.reserve(v.size());
        std::transform(v.begin(),
                       v.end(),
                       std::back_inserter(r),
                       [first = v.begin(), last = v.end()](const auto& i) {
                           return std::accumulate(
                               first, last, 1, [&i](int a, const int& j) {
                                   return a * (&j != &i ? j : 1);
                               });
                       });
        return r;
    };
    auto r = solve(vector{1, 2, 3, 4, 5});
    print_vector(r.begin(), r.end());
}

template <typename T>
struct node {
    using node_ptr = std::unique_ptr<node<T>>;
    node() = default;
    ~node() = default;
    node(const T& a, node_ptr b, node_ptr c)
        : value(a), left(std::move(b)), right(std::move(c)) {}
    T value;
    node_ptr left;
    node_ptr right;
};
auto serialize = [](const auto& n) -> std::string {
    return std::to_string(n.value) + "(" + (n.left ? serialize(*n.left) : "") +
           "," + (n.right ? serialize(*n.right) : "") + ")";
};

// Given the root to a binary tree, implement serialize(root), which serializes
// the tree into a string, and deserialize(s), which deserializes the string
// back into the tree. node = Node('root', Node('left', Node('left.left')),
// Node('right')) assert deserialize(serialize(node)).left.left.val ==
// 'left.left'
TEST(per_day, 3) {
    auto deserialize = [](auto& n, const std::string_view str) -> size_t {
        using node_t = std::remove_reference_t<decltype(n)>;
        bool value = true, left = true;
        stack<node_t*> stk;
        stk.push(&n);
        auto it = 0, val_start = 0;
        auto extract_value = [&]() {
            if (value) {
                stk.top()->value = stoi(string(str.substr(val_start, it)));
                value = false;
            }
        };
        while (it < str.size()) {
            if (str.at(it) == '(') {
                extract_value();
                val_start = it + 1;
                left = true;
            } else if (str.at(it) == ',') {
                extract_value();
                left = false;
            } else if (str.at(it) == ')') {
                extract_value();
                stk.pop();
            } else {
                if (!value) {
                    value = true;
                    val_start = it;
                    auto n = std::make_unique<node_t>();
                    auto x = n.get();
                    if (left) {
                        stk.top()->left = std::move(n);
                    } else {
                        stk.top()->right = std::move(n);
                    }
                    stk.push(x);
                }
            }
            ++it;
        }
    };
    using node_int = node<int>;
    node_int n{
        1,
        std::make_unique<node_int>(
            2, std::make_unique<node_int>(3, nullptr, nullptr), nullptr),
        std::make_unique<node_int>(
            4, nullptr, std::make_unique<node_int>(5, nullptr, nullptr))};
    auto s = serialize(n);
    std::cout << s << std::endl;
    node_int m;
    deserialize(m, s);
    std::cout << serialize(m) << std::endl;
}

// iven an array of integers, find the first missing positive integer in linear
// time and constant space. In other words, find the lowest positive integer
// that does not exist in the array. The array can contain duplicates and
// negative numbers as well.
// For example, the input [3, 4, -1, 1] should give 2. The input [1, 2, 0]
// should give 3.
// You can modify the input array in-place.
TEST(per_day, 4) {
    auto solve = [](vector<int>&& v) {
        std::sort(v.begin(), v.end());
        auto it = std::adjacent_find(
            v.begin(), v.end(), [](const auto& i, const auto& j) {
                return i > 0 and j > 0 and j != i + 1;
            });
        if (it == v.end()) {
            return *(v.end() - 1) + 1;
        } else {
            return *it + 1;
        }
    };
    assert(2 == solve({3, 4, -1, 1}));
    assert(3 == solve({1, 2, 0}));
    cout << "success" << endl;
}