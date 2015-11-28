cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(Test-Shader-Includes)

add_executable(test-shader-includes main.cpp)
target_link_libraries(test-shader-includes glrt)
target_compile_definitions(test-shader-includes PRIVATE SHADER_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}/shader\")
