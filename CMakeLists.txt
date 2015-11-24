cmake_minimum_required(VERSION 3.1 FATAL_ERROR)


include(cmake/package-helper.cmake)


find_package_overide_root(GLEW_ROOT GLEW REQUIRED)

set(GLRT_ASSET_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/assets)

add_subdirectory(external)
add_subdirectory(gl-research-toolkit)
add_subdirectory(samples)
add_subdirectory(tools)
