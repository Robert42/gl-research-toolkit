add_library(nvcommandlist SHARED
  include/nvcommandlist.h
  src/nvcommandlist.cpp
)
target_include_directories(nvcommandlist PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(nvcommandlist glew)
