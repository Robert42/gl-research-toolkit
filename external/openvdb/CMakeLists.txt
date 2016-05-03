add_library(openvdb INTERFACE)
target_include_directories(openvdb INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(openvdb INTERFACE glm)
