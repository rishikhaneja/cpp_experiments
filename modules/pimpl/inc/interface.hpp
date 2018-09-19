#pragma once

#include <memory>

struct interface {
    interface();
    ~interface();

    void function();

   private:
    struct impl;
    std::unique_ptr<impl> m_impl;
};