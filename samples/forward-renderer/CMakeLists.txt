cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(Forward-Renderer)

add_executable(forward-renderer main.cpp)
target_link_libraries(forward-renderer glrt)
