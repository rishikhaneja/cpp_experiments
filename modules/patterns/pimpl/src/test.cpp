#include "interface.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

using namespace std;
using namespace testing;

TEST(pimpl, basic) {
    interface i;
    i.function();
}