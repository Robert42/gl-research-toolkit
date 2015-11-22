cmake_minimum_required(VERSION 2.8.11 FATAL_ERROR)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} PARENT_SCOPE)


######## SFML #################################################################
set(SFML_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/sfml)
set(SFML_ROOT ${SFML_ROOT} PARENT_SCOPE)

add_subdirectory(sfml)

######## SFGUI ################################################################
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-conversion -Wno-deprecated-declarations")
add_subdirectory(sfgui)

target_include_directories(
  sfgui
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/sfgui/include
)
