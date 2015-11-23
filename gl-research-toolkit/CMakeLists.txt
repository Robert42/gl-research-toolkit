cmake_minimum_required(VERSION 2.8.11 FATAL_ERROR)

project(OpenGL-Research-Toolkit)

find_package(SFML 2.3 COMPONENTS graphics window system REQUIRED)
find_package(Qt5Core REQUIRED)


set(GLRT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/..)
set(GLM_INCLUDE_DIR ${GLRT_DIR}/external/glm)


add_library(glrt
  include/glrt/application.h
  include/glrt/dependencies.h
  include/glrt/gui/toolbar.h
  include/glrt/gui/properties.h
  include/glrt/profiler.h
  include/glrt/system.h
  src/glrt/application.cpp
  src/glrt/dependencies.cpp
  src/glrt/gui/toolbar.cpp
  src/glrt/gui/properties.cpp
  src/glrt/profiler.cpp
  src/glrt/system.cpp
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
  -std=c++11
  -Wall
  -Wextra
)

target_link_libraries(glrt
  ${SFML_LIBRARIES}
  sfgui
  glhelper
  Qt5::Core
)
