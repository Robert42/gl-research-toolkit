cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(Test-Arrays)

add_executable(test-arrays
  main.cpp
  trait-handling.cpp
  fragmented-array.test.cpp
  statespy.cpp
  statespy.h
)
target_link_libraries(test-arrays glrt testing-framework)
