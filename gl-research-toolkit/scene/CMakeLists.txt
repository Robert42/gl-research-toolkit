include(files.cmake)

add_library(glrt-scene
  ${SOURCE_FILES}
)

target_link_libraries(glrt-scene
  glrt-core
  glhelper #TODO move to the renderer
)

target_include_directories(
  glrt-scene
  PUBLIC
  include
)
