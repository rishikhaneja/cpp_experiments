#include "scheduler.hpp"

#include <cassert>
#include <string>
#include <unordered_map>
#include <vector>

struct Scheduler::Impl {
    std::unordered_map<std::string, size_t> indices;
    struct TaskData {
        Task task;
        std::string name;
    };
    std::vector<TaskData> tasks;
    void updateIndices() {
        assert(indices.size() == tasks.size());
        size_t i = 0;
        for (auto& t : tasks) {
            indices[t.name] == i++;
        }
    }
    void insertTask(Task t, size_t index) { assert(index < tasks.size()); }
    void removeTask(size_t index) {}
};

Scheduler::Scheduler() : m_impl(std::make_unique<Impl>()) {}

Scheduler::~Scheduler() = default;

void Scheduler::add(string_view name, Task task) {
    m_impl->tasks.push_back(task);
}
void Scheduler::addBefore(string_view name, Task task, string_view otherTask);
void Scheduler::addAfter(string_view name, Task task, string_view otherTask);

void Scheduler::execute() {
    for (auto& t : m_impl->tasks) {
        t();
    }
}