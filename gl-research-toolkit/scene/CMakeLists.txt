include(files.cmake)

find_package(Qt5Core 5.5 REQUIRED)

add_library(glrt-scene
  ${SOURCE_FILES}
)

target_link_libraries(glrt-scene
  PUBLIC
  glrt-core
  glhelper
  soil
)

target_include_directories(
  glrt-scene
  PUBLIC
  include
  Qt5::Core
)
