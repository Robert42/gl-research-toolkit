cmake_minimum_required(VERSION 3.1 FATAL_ERROR)


include(cmake/package-helper.cmake)


find_package_overide_root(GLEW_ROOT GLEW REQUIRED)

set(GLRT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(GLRT_ASSET_DIR ${GLRT_DIR}/external/assets)
set(GLRT_SHADER_DIR ${GLRT_DIR}/shader)

set(GLM_INCLUDE_DIR ${GLRT_DIR}/external/glm)

add_subdirectory(external)
add_subdirectory(gl-research-toolkit)
add_subdirectory(samples)
add_subdirectory(tests)
add_subdirectory(tools)
