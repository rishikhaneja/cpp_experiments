#include <iostream>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "scheduler.hpp"

using namespace std;
using namespace testing;

struct Main {
    MOCK_METHOD0(update, void(void));
    MOCK_METHOD0(render1, void(void));
    MOCK_METHOD0(render2, void(void));
};

struct MainExtension {
    MOCK_METHOD0(preUpdate, void(void));
    MOCK_METHOD0(postUpdate, void(void));
    MOCK_METHOD0(preRender2, void(void));
    MOCK_METHOD0(postRender2, void(void));
};

using Composite = Scheduler::CompositeTask;
using Leaf = Scheduler::LeafTask;

struct CallOrderPrinter {
    string indent;
    void operator()(Leaf& l) { cout << indent << l.name << "\n"; }
    void operator()(Composite& c) {
        cout << indent << c.getName() << "\n";
        c.execute(CallOrderPrinter{indent + indent});
    }
};

TEST(scheduler, basic) {
    Main m;
    Scheduler s;

    s.pushBack(Leaf{"update", [&m] { m.update(); }});

    {
        Composite render{"render"};
        render.pushBack(Leaf{"render1", [&m] { m.render1(); }});
        render.pushBack(Leaf{"render2", [&m] { m.render2(); }});
        s.pushBack(move(render));
    }

    MainExtension me;

    s.insertBefore(Leaf{"preUpdate", [&me] { me.preUpdate(); }}, "update");
    s.insertAfter(Leaf{"postUpdate", [&me] { me.postUpdate(); }}, "update");

    {
        auto render = s.findCompositeTask("render");
        render->insertBefore(Leaf{"preRender2", [&me] { me.preRender2(); }},
                             "render2");
        render->insertAfter(Leaf{"postRender2", [&me] { me.postRender2(); }},
                            "render2");
    }

    {
        InSequence seq;

        EXPECT_CALL(me, preUpdate()).Times(1);
        EXPECT_CALL(m, update()).Times(1);
        EXPECT_CALL(me, postUpdate()).Times(1);
        EXPECT_CALL(m, render1()).Times(1);
        EXPECT_CALL(me, preRender2()).Times(1);
        EXPECT_CALL(m, render2()).Times(1);
        EXPECT_CALL(me, postRender2()).Times(1);
    }

    s.execute();

    cout << "Call Order:\n";
    s.execute(CallOrderPrinter{"  "});
}
