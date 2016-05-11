cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(OpenGL-Research-Toolkit)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  execute_process(
    COMMAND ./update-cmake.sh
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  )
endif()

include(cmake/adapt-prefix-path.cmake)

adapt_cmake_prefix_path(GLEW_DIR)
find_package(GLEW 1.13 REQUIRED)
reset_cmake_prefix_path()
adapt_cmake_prefix_path(QT5_DIR)
find_package(Qt5Core 5.5 REQUIRED)
reset_cmake_prefix_path()
find_package(OpenMP REQUIRED)

set(GLRT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(GLRT_ASSET_DIR ${GLRT_DIR}/external/assets)
set(GLRT_EXTERNAL_SHADER_DIR ${GLRT_DIR}/external/shader)
set(GLRT_SHADER_DIR ${GLRT_DIR}/shader)
set(GLRT_TEMP_SHADER_DIR ${CMAKE_CURRENT_BINARY_DIR}/temp-shader)
set(GLRT_PROFILER_DEFAULT_PORT 4315)
set(GLRT_SHADER_COMPILER_PORT 4316)
set(GLRT_SHADER_COMPILER_PATH ${CMAKE_CURRENT_BINARY_DIR}/tools/shader-compiler/shader-compiler)

add_definitions("-DGLRT_PROFILER_DEFAULT_PORT=${GLRT_PROFILER_DEFAULT_PORT}")
add_definitions("-DGLRT_SHADER_COMPILER_PORT=${GLRT_SHADER_COMPILER_PORT}")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")

add_subdirectory(external)
add_subdirectory(shader)
add_subdirectory(gl-research-toolkit)
add_subdirectory(samples)
add_subdirectory(tests)
add_subdirectory(tools)
