cmake_minimum_required(VERSION 2.8.11 FATAL_ERROR)

project(OpenGL-Research-Toolkit)

find_package(SFML 2.1 COMPONENTS graphics window system REQUIRED)

add_library(glrt 
  include/glrt/application.h
  src/application.cpp
)

target_include_directories(
  glrt
  PUBLIC
  ${SFML_INCLUDE_DIR}
  include
)

target_compile_options(glrt
  PUBLIC
  "-std=c++11"
)

target_link_libraries(glrt
  ${SFML_LIBRARIES}
)
