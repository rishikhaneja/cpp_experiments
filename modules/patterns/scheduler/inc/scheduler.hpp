#pragma once

#include <algorithm>
#include <cassert>
#include <functional>
#include <iostream>
#include <iterator>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

using namespace std;

template <class... Ts>
struct overloaded : Ts... {
    using Ts::operator()...;
};
template <class... Ts>
overloaded(Ts...)->overloaded<Ts...>;

template <typename T>
struct recursive_wrapper {
    recursive_wrapper(T t_) : t(new T(std::move(t_))) {}
    operator const T&() const { return *t; }
    operator T&() { return *t; }
    unique_ptr<T> t;
};

class Scheduler {
   public:
    using Body = function<void(void)>;
    struct LeafTask {
        string name;
        Body body;
    };
    struct CompositeTask;
    using CompositeTaskWrapper = recursive_wrapper<CompositeTask>;
    using Task = variant<LeafTask, CompositeTaskWrapper>;

    static string getName(const Task& t) {
        return visit(
            overloaded{[](const CompositeTask& c) { return c.getName(); },
                       [](const LeafTask& l) { return l.name; }},
            t);
    }

    class CompositeTask {
       public:
        string getName() const { return m_name; }

        void pushBack(Task task) {
            auto name = Scheduler::getName(task);
            assert(m_registry.find(name) == m_registry.end());
            m_tasks.push_back(move(task));
            m_registry[move(name)] = m_tasks.size() - 1;
        }

        void insertBefore(Task task, string_view otherTask) {
            auto name = Scheduler::getName(task);
            auto otherIndex = findTaskIndex(otherTask);
            assert(otherIndex);
            m_tasks.insert(m_tasks.begin() + *otherIndex, move(task));
            updateIndicesAfter(*otherIndex);
        }

        void insertAfter(Task task, string_view otherTask) {
            auto name = Scheduler::getName(task);
            auto otherIndex = findTaskIndex(otherTask);
            assert(otherIndex);
            m_tasks.insert(m_tasks.begin() + (*otherIndex + 1), move(task));
            updateIndicesAfter(*otherIndex + 1);
        }

        Task* findTask(string_view name) {
            auto index = findTaskIndex(name);
            if (index) {
                return &(m_tasks[*index]);
            } else {
                return nullptr;
            }
        }

        CompositeTask* findCompositeTask(string_view name) {
            auto task = findTask(name);
            if (task) {
                auto compositeTaskWrapper = get_if<CompositeTaskWrapper>(task);
                if (compositeTaskWrapper) {
                    return &static_cast<CompositeTask&>(*compositeTaskWrapper);
                } else {
                    return nullptr;
                }
            } else {
                return nullptr;
            }
        }

        template <typename E>
        void execute(E&& executor = DefaultExecutor{}) {
            for (auto& t : m_tasks) {
                visit(forward<E>(executor), t);
            }
        }

        CompositeTask(string name) : m_name(name) {}
        ~CompositeTask() = default;
        CompositeTask(const CompositeTask&) = delete;
        CompositeTask& operator=(const CompositeTask&) = delete;
        CompositeTask(CompositeTask&&) = default;
        CompositeTask& operator=(CompositeTask&&) = default;

       private:
        void updateIndicesAfter(size_t index) {
            for (; index != m_tasks.size(); ++index) {
                auto& t = m_tasks[index];
                m_registry[Scheduler::getName(t)] = index;
            }
        }

        optional<size_t> findTaskIndex(string_view name) {
            auto it = m_registry.find(string(name));
            if (it != m_registry.end()) {
                return it->second;
            } else {
                return {};
            }
        }

        string m_name;
        vector<Task> m_tasks;
        unordered_map<string, size_t> m_registry;
    };

    void pushBack(Task task) { m_mainTask.pushBack(move(task)); }

    void insertBefore(Task task, string_view otherTask) {
        m_mainTask.insertBefore(move(task), otherTask);
    }

    void insertAfter(Task task, string_view otherTask) {
        m_mainTask.insertAfter(move(task), otherTask);
    }

    Task* findTask(string_view name) { return m_mainTask.findTask(name); }

    CompositeTask* findCompositeTask(string_view name) {
        return m_mainTask.findCompositeTask(name);
    }

    struct DefaultExecutor {
        void operator()(LeafTask& l) { l.body(); }
        void operator()(CompositeTask& c) { c.execute(*this); }
    };

    template <typename E = DefaultExecutor>
    void execute(E&& executor = DefaultExecutor{}) {
        m_mainTask.execute(forward<E>(executor));
    }

    Scheduler() = default;
    ~Scheduler() = default;
    Scheduler(const Scheduler&) = delete;
    Scheduler& operator=(const Scheduler&) = delete;
    Scheduler(Scheduler&&) = default;
    Scheduler& operator=(Scheduler&&) = default;

   private:
    CompositeTask m_mainTask{"main"};
};