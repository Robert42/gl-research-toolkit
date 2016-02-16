cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

include(files.cmake)

set(TEST_VECTOR_HEADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../external/snippets/include)
set(TEST_VECTOR_HEADER ${TEST_VECTOR_HEADER_DIR}/test-vectors.h)

set(CMAKE_AUTOMOC ON)

project(Tests)

add_executable(tests
  ${SOURCE_FILES}
  ${TEST_VECTOR_HEADER}
)
target_link_libraries(tests glrt gtest_main)
target_compile_definitions(tests PRIVATE SHADER_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}/shader-includes/shader\")
target_include_directories(tests PRIVATE ${TEST_VECTOR_HEADER_DIR})