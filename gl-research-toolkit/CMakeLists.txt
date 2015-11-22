cmake_minimum_required(VERSION 2.8.11 FATAL_ERROR)

project(OpenGL-Research-Toolkit)

find_package(SFML 2.1 COMPONENTS graphics window system REQUIRED)

include_directories(
  ${SFML_INCLUDE_DIR}
  include
)

add_library(glrt 
  include/glrt/window.h
  src/window.cpp
)

target_link_libraries(glrt
  ${SFML_LIBRARIES}
)
