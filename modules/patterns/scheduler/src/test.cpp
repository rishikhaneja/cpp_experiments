#include <iostream>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "scheduler.hpp"

using namespace std;
using namespace testing;

using Priority = Scheduler::Priority;

namespace {
Priority update;
Priority layout;
Priority render;

struct Main {
    MOCK_METHOD0(update, void(void));
    MOCK_METHOD0(layout, void(void));
    MOCK_METHOD0(render, void(void));
};

struct MainExtension {
    MOCK_METHOD0(preUpdate, void(void));
    MOCK_METHOD0(postUpdate, void(void));
    MOCK_METHOD0(final, void(void));
};

}  // namespace

TEST(scheduler, basic) {
    Main m;
    update.at = {{"update", [&m]() { m.update(); }}};
    layout.at = {{"layout", [&m]() { m.layout(); }}};
    render.at = {{"render", [&m]() { m.render(); }}};

    MainExtension me;

    Priority final(render);

    update.before.push_back({"preUpdate", [&me]() { me.preUpdate(); }});
    update.after.push_back({"postUpdate", [&me]() { me.postUpdate(); }});
    final.at = {{"final", [&me]() { me.final(); }}};

    {
        InSequence seq;
        EXPECT_CALL(me, preUpdate());
        EXPECT_CALL(m, update());
        EXPECT_CALL(me, postUpdate());
        EXPECT_CALL(m, layout());
        EXPECT_CALL(m, render());
        EXPECT_CALL(me, final());
    }

    Scheduler::execute();
}
