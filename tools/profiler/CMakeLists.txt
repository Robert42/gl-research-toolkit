cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(Profiler)

find_package(Qt5Widgets 5.5 REQUIRED)
find_package(Qt5Network 5.5 REQUIRED)

set(CMAKE_AUTOMOC ON)

qt5_wrap_ui(UI_HEADERS dialog.ui connection-widget.ui)

list(GET UI_HEADERS 0 FIRST_UI_HEADER)
get_filename_component(UI_HEADER_DIR ${FIRST_UI_HEADER} DIRECTORY)

add_executable(profiler
  main.cpp
  dialog.cpp
  dialog.h
  connection-widget.cpp
  connection-widget.h
  ${UI_HEADERS}
)

target_include_directories(profiler PRIVATE ${UI_HEADER_DIR})
target_link_libraries(profiler Qt5::Widgets Qt5::Network glrt)
