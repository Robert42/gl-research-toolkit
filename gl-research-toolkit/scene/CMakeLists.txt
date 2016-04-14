include(files.cmake)

find_package(Qt5Widgets 5.5 REQUIRED)

add_library(glrt-scene
  ${SOURCE_FILES}
)

target_link_libraries(glrt-scene
  PUBLIC
  glrt-core
  glhelper
  soil
  Qt5::Widgets
)

target_include_directories(
  glrt-scene
  PUBLIC
  include
  ${GLRT_EXTERNAL_SHADER_DIR}
)
