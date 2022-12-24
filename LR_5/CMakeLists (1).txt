cmake_minimum_required(VERSION 3.22.0)
project(5 LANGUAGES CXX)

set(DYNAMIC_SOURCE src/main2.cpp)
set(STATIC_SOURCE src/main1.cpp)

set(LIBRARIES1_SOURCE src/lib1.cpp)
set(LIBRARIES2_SOURCE src/lib2.cpp)
set(LIBRARIES_INCLUDE include/lib.h)

add_library(
        lib1 SHARED
        ${LIBRARIES_INCLUDE}
        ${LIBRARIES1_SOURCE}
)

add_library(
        lib2 SHARED
        ${LIBRARIES_INCLUDE}
        ${LIBRARIES2_SOURCE}
)

add_library(
        lib_st1 STATIC
        ${LIBRARIES_INCLUDE}
        ${LIBRARIES1_SOURCE}
)

add_library(
        lib_st2 STATIC
        ${LIBRARIES_INCLUDE}
        ${LIBRARIES2_SOURCE}
)

if (UNIX)
    add_definitions(-DOS=UNIX)
endif()

if (APPLE)
    add_definitions(-DOS=APPLE)
endif()

if (WIN32)
    add_definitions(-DOS=WIN32)
endif()

add_executable(main2 ${DYNAMIC_SOURCE})
add_executable(main11 ${STATIC_SOURCE})
add_executable(main12 ${STATIC_SOURCE})

target_link_libraries(main11 PRIVATE lib_st1)
target_link_libraries(main12 PRIVATE lib_st2)

target_link_libraries(main2 PRIVATE dl)


