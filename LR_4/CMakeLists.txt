cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(main LANGUAGES CXX)
set(CMAKE_CXX_FLAGS "-lrt -pthread")
#set(CMAKE_EXE_LINKER_FLAGS "-lrt -pthread")
add_executable(child ./src/child.cpp)
add_executable(main ./src/main.cpp)

target_link_libraries(main PRIVATE librt.so)
target_link_libraries(child PRIVATE librt.so)


