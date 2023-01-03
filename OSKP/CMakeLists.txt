cmake_minimum_required(VERSION 3.8 FATAL_ERROR)
project(lab6-8 LANGUAGES CXX)

set(CMAKE_CXX_FLAGS "-lzmq")

add_executable(main ./src/main.cpp ./src/allocator_list.cpp ./src/allocator_mkk.cpp)

