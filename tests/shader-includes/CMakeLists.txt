cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(Test-Shader-Includes)

add_executable(test-shader-includes main.cpp)
target_link_libraries(test-shader-includes glrt)
