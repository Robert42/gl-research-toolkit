cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} PARENT_SCOPE)

set(GLM_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/glm)

find_package(GLEW 1.13 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(Qt5Core 5.5 REQUIRED)
find_package(ASSIMP 3.2 REQUIRED)
find_package(SDL2 REQUIRED)


######## googletest ############################################################
add_subdirectory(googletest EXCLUDE_FROM_ALL)

######## glew ##################################################################
add_library(glew INTERFACE)
target_include_directories(glew INTERFACE ${GLEW_INCLUDE_DIRS})
target_link_libraries(glew INTERFACE ${GLEW_LIBRARY} ${OPENGL_gl_LIBRARY})


######## glm ###################################################################
add_library(glm INTERFACE)
target_include_directories(glm INTERFACE ${GLM_INCLUDE_DIR})

######## glhelpers #############################################################
set(GLHELPER_CONFIG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/glhelperconfig/)
set(GLHELPER_ADDITIONAL_LIBRARIES Qt5::Core)
add_subdirectory(glhelper)
target_include_directories(glhelper PUBLIC ${GLM_INCLUDE_DIR})
target_sources(glhelper PUBLIC ${GLHELPER_CONFIG_DIR}/glhelperconfig.hpp)

######## AntTweakBar ###########################################################
set(ATB_OPENGL_HEADER "GL/glew.h")
add_subdirectory(AntTweakBar EXCLUDE_FROM_ALL)
target_link_libraries(AntTweakBar INTERFACE ${GLEW_LIBRARY} ${OPENGL_gl_LIBRARY})

######## Shader ################################################################
add_subdirectory(shader EXCLUDE_FROM_ALL)

######## Angelscript ###########################################################
add_subdirectory(angelscript-integration)

######## Nvidia ################################################################
add_subdirectory(nvcommandlist)
