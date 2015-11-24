cmake_minimum_required(VERSION 2.8.11 FATAL_ERROR)


include(cmake/package-helper.cmake)


find_package_overide_root(GLEW_ROOT GLEW REQUIRED)


add_subdirectory(external)
add_subdirectory(gl-research-toolkit)
add_subdirectory(samples)
