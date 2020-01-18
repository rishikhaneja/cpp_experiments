#include <iostream>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#define SOL_ALL_SAFETIES_ON 1
#include "sol.hpp"

using namespace std;
using namespace testing;

TEST(helloworld, basic) {
    cout << "=== opening a state ===" << endl;

    sol::state lua;
    lua.open_libraries(sol::lib::base, sol::lib::package);
    lua.script("print('bark bark bark!')");

    cout << endl;
}
