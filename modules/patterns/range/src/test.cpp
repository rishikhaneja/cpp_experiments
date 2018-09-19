#include "range.hpp"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

using namespace std;
using namespace testing;

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

TEST(range, basic) {
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