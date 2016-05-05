add_library(openvdb INTERFACE)
target_include_directories(openvdb INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(openvdb INTERFACE glm)

add_custom_target(
  openvdb_src
  SOURCES
  include/openvdb/triangle-distance.h
  include/openvdb/triangle-distance.inl
)
