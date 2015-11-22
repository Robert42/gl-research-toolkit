cmake_minimum_required(VERSION 2.8.11 FATAL_ERROR)

project(Cornell-Box)

find_package(SFML 2.1 COMPONENTS graphics window system REQUIRED)

include_directories(SFML_INCLUDE_DIR)

add_executable(cornell-box main.cpp)
target_link_libraries(cornell-box ${SFML_LIBRARIES})
