cmake_minimum_required(VERSION 3.24.2 FATAL_ERROR)
project(main LANGUAGES CXX)
set(CMAKE_C_FLAGS "-pthread")
add_executable(main ./src/main.cpp)

