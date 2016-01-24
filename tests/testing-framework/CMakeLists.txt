cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(Testing-Framework)

add_library(testing-framework testing-framework.h testing-application.h testing-application.cpp)
target_link_libraries(testing-framework glrt)
target_include_directories(testing-framework PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
