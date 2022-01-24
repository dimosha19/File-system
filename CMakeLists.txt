cmake_minimum_required(VERSION 3.13)  # CMake version check
project(tests)               # Create project "simple_example"
set(CMAKE_CXX_STANDARD 20)            # Enable c++14 standard

add_executable(tests linked_heap.cpp)