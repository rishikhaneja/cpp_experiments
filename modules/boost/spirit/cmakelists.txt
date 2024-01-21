cmake_minimum_required(VERSION 3.12)

get_filename_component(EXECUTABLE_NAME ${CMAKE_CURRENT_LIST_DIR} NAME)

project(${EXECUTABLE_NAME})

set(BOOST_INCLUDE_LIBRARIES spirit thread)
set(BOOST_ENABLE_CMAKE ON)

include(FetchContent)
FetchContent_Declare(
  Boost
  GIT_REPOSITORY https://github.com/boostorg/boost.git
  GIT_TAG boost-1.84.0
  GIT_SHALLOW TRUE
  GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(Boost)

include_directories(inc)

file(GLOB SOURCES "src/*.cpp")

add_executable(${EXECUTABLE_NAME} ${SOURCES})

set_target_properties(${EXECUTABLE_NAME} PROPERTIES
    CXX_STANDARD 17
)

target_link_libraries(${EXECUTABLE_NAME} PRIVATE Boost::spirit GTest::gtest_main GTest::gmock_main)

gtest_discover_tests(${EXECUTABLE_NAME})