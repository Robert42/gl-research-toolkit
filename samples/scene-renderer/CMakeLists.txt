cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(Scene-Renderer)

add_executable(scene-renderer main.cpp)
target_link_libraries(scene-renderer glrt)
