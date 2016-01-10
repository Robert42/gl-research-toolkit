cmake_minimum_required(VERSION 3.1 FATAL_ERROR)


include(cmake/package-helper.cmake)


find_package_overide_root(GLEW_ROOT GLEW REQUIRED)

set(GLRT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(GLRT_ASSET_DIR ${GLRT_DIR}/external/assets)
set(GLRT_EXTERNAL_SHADER_DIR ${GLRT_DIR}/external/shader)
set(GLRT_SHADER_DIR ${GLRT_DIR}/shader)
set(GLRT_TEMP_SHADER_DIR ${CMAKE_CURRENT_BINARY_DIR}/temp-shader)
set(GLRT_PROFILER_DEFAULT_PORT 4315)

add_definitions("-DGLRT_PROFILER_DEFAULT_PORT=${GLRT_PROFILER_DEFAULT_PORT}")

set(GLM_INCLUDE_DIR ${GLRT_DIR}/external/glm)

add_subdirectory(external)
add_subdirectory(shader)
add_subdirectory(gl-research-toolkit)
add_subdirectory(samples)
add_subdirectory(tests)
add_subdirectory(tools)
