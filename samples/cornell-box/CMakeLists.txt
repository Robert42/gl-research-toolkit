cmake_minimum_required(VERSION 2.8.11 FATAL_ERROR)

project(Cornell-Box)

add_executable(cornell-box main.cpp)
target_link_libraries(cornell-box glrt)
