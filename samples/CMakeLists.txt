option(GLRT_SAMPLES "Enable to compile tools." ON)

if(GLRT_SAMPLES)
add_subdirectory(cornell-box)
add_subdirectory(single-quad)
add_subdirectory(single-mesh)
add_subdirectory(template_minimum)
endif()
