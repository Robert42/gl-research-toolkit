cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(OpenGL-Research-Toolkit)


if(NOT GLRT_ASSET_DIR)
message(FATAL_ERROR "GLRT_ASSET_DIR must be set")
endif()
if(NOT GLRT_SHADER_DIR)
message(FATAL_ERROR "GLRT_SHADER_DIR must be set")
endif()


option(GLRT_PROFILER "If Disabled, there the profiler is reducuced to elminate the overhead of the profiler." ON)


find_package(SDL2 REQUIRED)
find_package(Qt5Core REQUIRED)
find_package(ASSIMP REQUIRED)


set(CMAKE_AUTOMOC ON)


add_library(glrt
  include/glrt/application.h
  include/glrt/debug-camera.h
  include/glrt/dependencies.h
  include/glrt/gui/anttweakbar.h
  include/glrt/gui/toolbar.h
  include/glrt/profiler.h
  include/glrt/scene/scene.h
  include/glrt/scene/static-mesh.h
  include/glrt/system.h
  src/glrt/application.cpp
  src/glrt/debug-camera.cpp
  src/glrt/dependencies.cpp
  src/glrt/gui/anttweakbar.cpp
  src/glrt/gui/toolbar.cpp
  src/glrt/profiler.cpp
  src/glrt/scene/scene.cpp
  src/glrt/scene/static-mesh.cpp
  src/glrt/system.cpp
)

target_include_directories(
  glrt
  PUBLIC
  include
)

if(GLRT_PROFILER)
target_compile_definitions(
  glrt
  PUBLIC
  GLRT_PROFILER
)
endif()

target_compile_definitions(
  glrt
  PUBLIC
  GLRT_ASSET_DIR=\"${GLRT_ASSET_DIR}\"
  GLRT_SHADER_DIR=\"${GLRT_SHADER_DIR}\"
  GLM_FORCE_RADIANS
  GLM_SWIZZLE
)

target_include_directories(
  glrt
  SYSTEM
  PUBLIC
  ${GLM_INCLUDE_DIR}
  ${ASSIMP_INCLUDE_DIR}
)

target_compile_options(glrt
  PUBLIC
  -std=c++11
  -Wall
  -Wextra
)

target_link_libraries(glrt
  PUBLIC
  ${SDL2_LIBRARIES}
  ${ASSIMP_LIBRARY}
  glhelper
  Qt5::Core
  AntTweakBar
)
