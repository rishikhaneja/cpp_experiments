#include <iostream>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
//#include "scheduler.hpp"

#include <functional>
#include <set>
#include <string>
#include <vector>

using namespace std;
using namespace testing;

struct Scheduler {
    struct Task {
        enum class Mode { OneShot, Recurring };
        using Body = function<void(void)>;

        string name;
        Body body;
        Mode mode = Mode::Recurring;
    };

    using TaskExecutor = function<void(const Task&)>;
    using TaskList = vector<Task>;

    using Stage = TaskList;

    void executeAll() {
        executeAll([this](const auto& task) { task.body(); });
    }
    void executeAll(TaskExecutor executor) {
        for (auto& stage : stages) {
            stage->erase(remove_if(begin(*stage),
                                   end(*stage),
                                   [&executor](auto& task) {
                                       executor(task);
                                       return task.mode == Task::Mode::OneShot;
                                   }),
                         end(*stage));
            if (!proceed) {
                return;
            }
        }
    }
    vector<Stage*> stages;
    bool proceed = true;
};

class Application {
   public:
    static Scheduler::Stage UpdateStage;
    static Scheduler::Stage LayoutStage;
    static Scheduler::Stage RenderStage;
    void init() {
        // default stages
        m_scheduler.stages = {&UpdateStage, &LayoutStage, &RenderStage};

        // default tasks
        UpdateStage.push_back(
            {"update", [this] { update(); }, Scheduler::Task::Mode::Recurring});
        LayoutStage.push_back(
            {"layout", [this] { layout(); }, Scheduler::Task::Mode::Recurring});
        RenderStage.push_back(
            {"render", [this] { render(); }, Scheduler::Task::Mode::Recurring});

        onInit();
    }
    void main() {
        while (!m_quit) {
            m_scheduler.executeAll();
        }
    }
    void update() {
        // do something
        if (m_dirty) {
            m_scheduler.proceed = true;
        }
    }
    void layout() {}
    void render() {}
    void quit() {}

   protected:
    Scheduler m_scheduler;
    bool m_quit = false;

   private:
    virtual void onInit(){};

    bool m_dirty = false;
};

Scheduler::Stage Application::UpdateStage;
Scheduler::Stage Application::LayoutStage;
Scheduler::Stage Application::RenderStage;

class MyApplication : public Application {
    virtual void onInit() override {
        RenderStage.insert(end(RenderStage),
                           {"postRender",
                            [this] { postRender(); },
                            Scheduler::Task::Mode::Recurring});
    }
    void preUpdate() {}
    void postRender() { quit(); }
};

TEST(scheduler, basic) {
    MyApplication app;
    app.init();
    app.main();
}