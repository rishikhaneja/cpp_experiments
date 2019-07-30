#include "tree.h"

#include "range.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

using namespace std;
using namespace testing;

struct Thing {
    int m_i;
    Thing(int i) : m_i(i) { cout << "Constructed: " << m_i << endl; }
    ~Thing() { cout << "Destroyed: " << m_i << endl; }
    Thing(const Thing& t) : m_i(t.m_i) { cout << "Copied: " << m_i << endl; }
    Thing(Thing&& t) : m_i(t.m_i) { cout << "Moved: " << m_i << endl; }
    Thing& operator=(Thing&& t) {
        m_i = t.m_i;
        cout << "Move assigned: " << m_i << endl;
        return *this;
    }
    operator int() const { return m_i; }
};

TEST(test, basic) {
    auto t = make_tree<Thing>();
    string observed;
    for (const auto& i : t) {
        observed += (int)i;
    }
    EXPECT_EQ("", observed);

    auto node_1 = t.insert(t.end(), 1);
    auto node_2 = t.insert(node_1, 2);

    cout << "children of 1:\n";
    for (const auto& i : t.children(node_1)) {
        cout << i.m_value;
    }
    cout << endl;

    t.insert(node_1, 5);
    t.insert(node_2, 3);
    // t.insert(node_2, 4);
    observed = "";
    for (const auto& i : t) {
        observed += (int)i;
        cout << "iter" << observed << "\n";
    }
    EXPECT_EQ("12345", observed);
}