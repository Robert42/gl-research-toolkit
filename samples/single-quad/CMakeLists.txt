cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(Single-Quad)

add_executable(single-quad main.cpp)
target_link_libraries(single-quad glrt)
