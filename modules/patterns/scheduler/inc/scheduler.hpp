#pragma once

#include <functional>
#include <set>
#include <string>
#include <vector>

class Scheduler {
   public:
    struct Task {
        std::string name;
        std::function<void(void)> body;
    };

    struct Priority {
        Priority();
        Priority(const Priority& after);
        using TaskList = std::vector<Task>;
        TaskList before;
        TaskList at;
        TaskList after;
    };

    static void insertBefore(Task&&);
    static void insertAt(Task&&);
    static void insertAfter(Task&&);
    static void execute();

   private:
    static Scheduler& instance();

    Scheduler() = default;
    ~Scheduler() = default;
    Scheduler(const Scheduler&) = delete;
    Scheduler& operator=(const Scheduler&) = delete;

    std::vector<Priority*> m_priorities;

    friend struct Priority;
};