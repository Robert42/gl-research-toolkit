include(files.cmake)

find_package(Qt5Widgets 5.5 REQUIRED)

add_library(glrt-scene
  ${SOURCE_FILES}
)

target_link_libraries(glrt-scene
  PUBLIC
  glrt-core
  glhelper
  Qt5::Widgets
  openvdb
  open_exr
)

target_include_directories(
  glrt-scene
  PUBLIC
  include
)
