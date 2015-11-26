option(GLRT_TOOLS "Enable to compile tools." ON)

if(GLRT_TOOLS)
add_subdirectory(profiler)
endif()