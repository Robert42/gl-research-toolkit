cmake_minimum_required(VERSION 2.8.11 FATAL_ERROR)

project(OpenGL-Research-Toolkit)

find_package(SFML 2.3 COMPONENTS graphics window system REQUIRED)


set(GLRT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/..)
set(GLM_INCLUDE_DIR ${GLRT_DIR}/external/glm)


add_library(glrt 
  include/glrt/application.h
  include/glrt/debug-gui.h
  include/glrt/dependencies.h
  src/application.cpp
  src/debug-gui.cpp
  src/dependencies.cpp
)

target_include_directories(
  glrt
  PUBLIC
  include
)

target_include_directories(
  glrt
  SYSTEM
  PUBLIC
  ${SFML_INCLUDE_DIR}
  ${GLM_INCLUDE_DIR}
)

target_compile_options(glrt
  PUBLIC
  "-std=c++11"
)

target_link_libraries(glrt
  ${SFML_LIBRARIES}
  sfgui
)
