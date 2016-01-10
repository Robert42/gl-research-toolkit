include(files.cmake)

find_package(Qt5Widgets REQUIRED)

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
  Qt5::Widgets
)
