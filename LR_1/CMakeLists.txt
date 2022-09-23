cmake_minimum_required(VERSION 3.24.2 FATAL_ERROR)

project(main LANGUAGES CXX)
add_executable(child ./child.cpp)
add_executable(main ./main.cpp)
