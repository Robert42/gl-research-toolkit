include(files.cmake)

find_package(Qt5Widgets 5.5 REQUIRED)

add_library(glrt
  ${SOURCE_FILES}
)

target_include_directories(
  glrt
  PUBLIC
  include
  ${GLRT_SHADER_DIR}/common-with-cpp
)

target_link_libraries(glrt
  PUBLIC
  # glhelper # TODO use again
  glrt-scene
  nvcommandlist
  Qt5::Widgets
)

target_compile_definitions(
  glrt
  PRIVATE
  GLRT_SHADER_COMPILER_PATH=\"${GLRT_SHADER_COMPILER_PATH}\"
)
