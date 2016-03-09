cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(Simple-CommandList)

add_executable(simple-commandlist main.cpp)
target_link_libraries(simple-commandlist glrt)
