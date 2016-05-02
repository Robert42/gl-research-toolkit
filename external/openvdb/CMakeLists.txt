add_library(openvdb SHARED
  include/openvdb/triangle-distance.h
  src/openvdb/triangle-distance.cpp
)
target_include_directories(openvdb PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(openvdb glm)
