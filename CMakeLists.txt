cmake_minimum_required(VERSION 3.12)

project(cpp_experiments)

set(SOLUTION_DIR ${CMAKE_CURRENT_LIST_DIR})
set(DEPENDENCIES_DIR ${SOLUTION_DIR}/dependencies)
set(MODULES_DIR ${SOLUTION_DIR}/modules)
set(PATTERNS_DIR ${MODULES_DIR}/patterns)

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.14.0
  GIT_SHALLOW TRUE
  GIT_PROGRESS TRUE
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

include(GoogleTest)
enable_testing()

add_subdirectory(modules/_template_)
add_subdirectory(modules/patterns/pimpl)
add_subdirectory(modules/patterns/range)
add_subdirectory(modules/patterns/tree)
add_subdirectory(modules/patterns/scheduler)
# add_subdirectory(modules/boost/spirit)
# add_subdirectory(modules/sol)
add_subdirectory(modules/practice/per_day)
add_subdirectory(modules/playground)