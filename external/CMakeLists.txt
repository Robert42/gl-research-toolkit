cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} PARENT_SCOPE)


find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)
find_package(Qt5Core REQUIRED)


######## glhelpers #############################################################
set(GLHELPER_CONFIG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/glhelperconfig/)
set(GLHELPER_ADDITIONAL_LIBRARIES Qt5::Core)
add_subdirectory(glhelper)
target_include_directories(glhelper PUBLIC ${GLM_INCLUDE_DIR})
target_sources(glhelper PUBLIC ${GLHELPER_CONFIG_DIR}/glhelperconfig.hpp)

######## Assimp #############################################################
function(add_assimp)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -w")
add_subdirectory(assimp EXCLUDE_FROM_ALL)
endfunction()
add_assimp()

######## AntTweakBar ###########################################################
set(ATB_OPENGL_HEADER "GL/glew.h")
add_subdirectory(AntTweakBar EXCLUDE_FROM_ALL)
target_link_libraries(AntTweakBar INTERFACE ${GLEW_LIBRARY} ${OPENGL_gl_LIBRARY})

######## Shader ################################################################
add_subdirectory(shader EXCLUDE_FROM_ALL)

######## Angelscript ###########################################################
add_subdirectory(angelscript-integration)
