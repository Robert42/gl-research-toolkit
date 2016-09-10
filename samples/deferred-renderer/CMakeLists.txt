cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(Deferred-Renderer)

add_executable(deferred-renderer main.cpp)
target_link_libraries(deferred-renderer glrt)
