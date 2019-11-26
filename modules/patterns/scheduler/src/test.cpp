#include <iostream>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "scheduler.hpp"

using namespace std;
using namespace testing;

struct Main {
    MOCK_METHOD0(update, void(void));
    MOCK_METHOD0(render, void(void));
};

TEST(scheduler, basic) {
    Main m;
    Scheduler s;

    s.add([&m] { m.update(); });
    s.add([&m] { m.render(); });

    EXPECT_CALL(m, update()).Times(1);
    EXPECT_CALL(m, render()).Times(1);

    s.execute();
}

struct MainExtension {
    MOCK_METHOD0(preUpdate, void(void));
    MOCK_METHOD0(postUpdate, void(void));
};

TEST(scheduler, extension) {
    Main m;
    Scheduler s;

    s.add([&m] { m.update(); });
    s.add([&m] { m.render(); });

    EXPECT_CALL(m, update()).Times(1);
    EXPECT_CALL(m, render()).Times(1);

    s.execute();
}