cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(Benchmark)

add_executable(benchmark main.cpp)
target_link_libraries(benchmark glrt)
