#pragma once

#include <functional>
#include <memory>

class Scheduler {
   public:
    using Task = std::function<void(void)>;

    void add(string_view name, Task task);
    void addBefore(string_view name, Task task, string_view otherTask);
    void addAfter(string_view name, Task task, string_view otherTask);
    void execute();

    Scheduler();
    ~Scheduler();

   private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;
};