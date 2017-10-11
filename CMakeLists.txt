cmake_minimum_required(VERSION 3.8)
project(oving3)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "-O3")

find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()


set(SOURCE_FILES rpcomp.c rpcomp.h)
add_executable(oving3 ${SOURCE_FILES})