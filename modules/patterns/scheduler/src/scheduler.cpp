#include "scheduler.hpp"

#include <algorithm>
#include <cassert>

using namespace std;

Scheduler& Scheduler::instance() {
    static Scheduler instance;
    return instance;
}

Scheduler::Priority::Priority() {
    Scheduler::instance().m_priorities.push_back(this);
}

Scheduler::Priority::Priority(const Priority& after) {
    auto& priorities = Scheduler::instance().m_priorities;
    auto it = find(cbegin(priorities), cend(priorities), &after);
    assert(it != priorities.cend());
    priorities.insert(it + 1, this);
}

void Scheduler::insertBefore(Task&& t) {}

void Scheduler::insertAt(Task&& t) {}

void Scheduler::insertAfter(Task&& t) {}

void Scheduler::execute() {
    auto& priorities = Scheduler::instance().m_priorities;
    for (auto& p : priorities) {
        for (auto& t : p->before) {
            t.body();
        }
        for (auto& t : p->at) {
            t.body();
        }
        for (auto& t : p->after) {
            t.body();
        }
    }
}