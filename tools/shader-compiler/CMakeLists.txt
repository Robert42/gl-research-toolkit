cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(Shader-Compiler)

find_package(Qt5Widgets 5.5 REQUIRED)

set(CMAKE_AUTOMOC ON)

add_executable(shader-compiler
  main.cpp
  compiler.h
  compiler.cpp
  debugmessage.cpp
  debugmessage.h
  ${UI_HEADERS}
)

target_include_directories(shader-compiler PRIVATE ${UI_HEADER_DIR})
target_link_libraries(shader-compiler Qt5::Widgets glrt)
