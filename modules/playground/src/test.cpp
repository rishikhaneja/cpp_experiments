#include <iostream>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

using namespace std;
using namespace testing;

TEST(playground, helloworld) { std::cout << "Hello World!" << std::endl; }