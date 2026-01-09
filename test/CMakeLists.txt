cmake_minimum_required(VERSION 4.0)

project(Test LANGUAGES CXX)

add_executable(vulkan_hello vulkan_hello.cpp)
target_link_libraries(vulkan_hello yuri_lib)

add_test(NAME vulkan_hello COMMAND vulkan_hello)

include(../cmake/compiler_options.cmake)