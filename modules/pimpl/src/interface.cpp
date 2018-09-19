#include "interface.hpp"

#include <iostream>
#include <memory>

struct interface::impl {
  void function() { std::cout << "pimpl" << std::endl; }
};

interface::interface() : m_impl(std::make_unique<impl>()) {}

interface::~interface() = default;

void interface::function() { m_impl->function(); }
