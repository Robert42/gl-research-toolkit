option(GLRT_TOOLS "Enable to compile tools." ON)

if(GLRT_TOOLS)
if(GLRT_PROFILER)
add_subdirectory(profiler)
endif()
add_subdirectory(shader-compiler)
endif()
