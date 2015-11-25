cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(OpenGL-Research-Toolkit)


if(NOT GLRT_ASSET_DIR)
message(FATAL_ERROR "GLRT_ASSET_DIR must be set")
endif()


option(GLRT_PROFILER "If Disabled, there the profiler is reducuced to elminate the overhead of the profiler." ON)


find_package(SDL2 REQUIRED)
find_package(Qt5Core REQUIRED)
find_package(ASSIMP REQUIRED)


set(GLRT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/..)
set(GLM_INCLUDE_DIR ${GLRT_DIR}/external/glm)


add_library(glrt
  include/glrt/application.h
  include/glrt/dependencies.h
  include/glrt/gui/toolbar.h
  include/glrt/profiler.h
  include/glrt/scene/static-mesh.h
  include/glrt/shader/shader-cache.h
  include/glrt/shader/shader-factory.h
  include/glrt/system.h
  src/glrt/application.cpp
  src/glrt/dependencies.cpp
  src/glrt/gui/toolbar.cpp
  src/glrt/profiler.cpp
  src/glrt/scene/static-mesh.cpp
  src/glrt/shader/shader-cache.cpp
  src/glrt/shader/shader-factory.cpp
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
  -DGLRT_PROFILER
)
endif()

target_compile_definitions(
  glrt
  PUBLIC
  GLRT_ASSET_DIR=\"${GLRT_ASSET_DIR}\"
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
