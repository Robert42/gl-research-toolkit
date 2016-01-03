cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(Test-Angelscript-Ref)

add_executable(test-angelscript-ref common.h main.cpp ref-access-functions.cpp)
target_link_libraries(test-angelscript-ref glrt)
target_compile_definitions(test-angelscript-ref PRIVATE SCRIPT_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}/scripts\")
