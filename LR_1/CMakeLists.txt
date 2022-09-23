cmake_minimum_required(VERSION 3.24.2 FATAL_ERROR)

project(main LANGUAGES CXX)
add_executable(child ./src/child.cpp)
add_executable(main ./src/main.cpp)
