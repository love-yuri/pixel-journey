cmake_minimum_required(VERSION 4.0)

project(Test LANGUAGES CXX)

add_executable(reflection_test reflection.cpp)
target_link_libraries(reflection_test yuri_lib)

add_test(NAME reflection_test COMMAND reflection_test)

include(../cmake/compiler_options.cmake)