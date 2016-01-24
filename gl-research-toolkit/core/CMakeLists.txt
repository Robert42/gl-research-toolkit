include(files.cmake)

if(NOT GLRT_ASSET_DIR)
message(FATAL_ERROR "GLRT_ASSET_DIR must be set")
endif()
if(NOT GLRT_SHADER_DIR)
message(FATAL_ERROR "GLRT_SHADER_DIR must be set")
endif()
if(NOT GLRT_EXTERNAL_SHADER_DIR)
message(FATAL_ERROR "GLRT_EXTERNAL_SHADER_DIR must be set")
endif()

find_package(SDL2 REQUIRED)
find_package(Qt5Core REQUIRED)

add_library(glrt-core
  ${SOURCE_FILES}
)

if(GLRT_PROFILER)
target_compile_definitions(
  glrt-core
  PUBLIC
  GLRT_PROFILER
)
endif()

target_include_directories(
  glrt-core
  PUBLIC
  include
  ${GLM_INCLUDE_DIR}
)

target_compile_definitions(
  glrt-core
  PUBLIC
  GLRT_ASSET_DIR=\"${GLRT_ASSET_DIR}\"
  GLRT_SHADER_DIR=\"${GLRT_SHADER_DIR}\"
  GLRT_EXTERNAL_SHADER_DIR=\"${GLRT_EXTERNAL_SHADER_DIR}\"
  GLRT_TEMP_SHADER_DIR=\"${GLRT_TEMP_SHADER_DIR}\"
  GLM_FORCE_RADIANS
  GLM_SWIZZLE
  GLM_STATIC_CONST_MEMBERS
)

target_compile_options(glrt-core
  PUBLIC
  -std=c++11
  -Wall
  -Wextra
)

target_link_libraries(glrt-core
  PUBLIC
  ${SDL2_LIBRARIES}
  assimp
  Qt5::Core
  AntTweakBar
  angelscript-interaction
)
