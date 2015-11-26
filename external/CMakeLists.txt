cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} PARENT_SCOPE)


find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)


######## glhelpers #################################################################
add_subdirectory(glhelper)


######## AntTweakBar ###########################################################
set(ATB_OPENGL_HEADER "GL/glew.h")
add_subdirectory(AntTweakBar EXCLUDE_FROM_ALL)
target_link_libraries(AntTweakBar INTERFACE ${GLEW_LIBRARY} ${OPENGL_gl_LIBRARY})
