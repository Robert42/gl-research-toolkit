cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(Test-Shader-Library)

add_executable(test-shader-library main.cpp)
target_link_libraries(test-shader-library glrt)
