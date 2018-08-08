#include "interface.hpp"

#include <iostream>

struct interface::impl {
  void function() { std::cout << "pimpl" << std::endl; }
};

interface::interface() : m_impl(new impl) {}

interface::~interface() = default;

void interface::function() { m_impl->function(); }
