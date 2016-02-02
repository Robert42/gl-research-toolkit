cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

include(files.cmake)

set(CMAKE_AUTOMOC ON)

project(Tests)

add_executable(tests
  ${SOURCE_FILES}
)
target_link_libraries(tests glrt)
target_compile_definitions(tests PRIVATE SHADER_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}/shader-includes/shader\")
target_include_directories(tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/testing-framework)
