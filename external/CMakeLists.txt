cmake_minimum_required(VERSION 2.8.11 FATAL_ERROR)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} PARENT_SCOPE)


######## glhelpers #################################################################
add_subdirectory(glhelper)
