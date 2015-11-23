cmake_minimum_required(VERSION 2.8.11 FATAL_ERROR)

if(NOT GLEW_ROOT AND NOT $ENV{GLEW_ROOT} STREQUAL "")
  set(GLEW_ROOT $ENV{GLEW_ROOT})
endif()

add_subdirectory(external)
add_subdirectory(gl-research-toolkit)
add_subdirectory(samples)
