cmake_minimum_required(VERSION 3.8 FATAL_ERROR)
project(lab6-8 LANGUAGES CXX)

set(CMAKE_CXX_FLAGS "-lzmq")

add_executable(client ./src/client.cpp ./src/tree.cpp)
add_executable(server ./src/main.cpp ./src/tree.cpp)

target_link_libraries(client zmq)
target_link_libraries(server zmq)
