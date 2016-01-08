option(GLRT_TESTS "Enable to compile tests." ON)

if(GLRT_TESTS)
add_subdirectory(testing-framework)
add_subdirectory(coord-frame)
add_subdirectory(shader-includes)
add_subdirectory(shader-library)
endif()
