cmake_minimum_required(VERSION 4.0)

project(Test LANGUAGES CXX)

add_executable(demo demo.cpp)
target_link_libraries(demo yuri_lib)

include(../cmake/compiler_options.cmake)
