cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

find_package(Qt5Svg 5.5 REQUIRED)

project(Benchmark)

add_executable(benchmark main.cpp)
target_link_libraries(benchmark glrt Qt5::Svg)
