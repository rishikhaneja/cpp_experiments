cmake_minimum_required(VERSION 3.12)

get_filename_component(EXECUTABLE_NAME ${CMAKE_CURRENT_LIST_DIR} NAME)

project(${EXECUTABLE_NAME})

include_directories(inc)

find_package(Lua REQUIRED)
include_directories(${LUA_INCLUDE_DIR})
include_directories(${DEPENDENCIES_DIR}/sol)

file(GLOB SOURCES "src/*.cpp")

add_executable(${EXECUTABLE_NAME} ${SOURCES})

set_target_properties(${EXECUTABLE_NAME} PROPERTIES
    CXX_STANDARD 17
)

target_link_libraries(${EXECUTABLE_NAME} GTest::gtest_main GTest::gmock_main ${LUA_LIBRARIES})

gtest_discover_tests(${EXECUTABLE_NAME})