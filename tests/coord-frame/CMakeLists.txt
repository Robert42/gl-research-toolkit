cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(Test-Coord-Frame)

add_executable(test-coord-frame main.cpp)
target_link_libraries(test-coord-frame glrt testing-framework)
