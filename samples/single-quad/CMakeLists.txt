cmake_minimum_required(VERSION 2.8.11 FATAL_ERROR)

project(Single-Quad)

add_executable(single-quad main.cpp)
target_link_libraries(single-quad glrt)
