cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(Single-Mesh)

add_executable(single-mesh main.cpp)
target_link_libraries(single-mesh glrt)
target_compile_definitions(single-mesh PRIVATE SHADER_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}/shader\")
