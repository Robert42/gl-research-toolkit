function(find_package_overide_root ROOT)
  if(NOT ${ROOT} AND NOT $ENV{${ROOT}} STREQUAL "")
    set(${ROOT} $ENV{${ROOT}})
  endif()

  if(${ROOT})
    set(CMAKE_PREFIX_PATH ${${ROOT}})
  endif()

  find_package(${ARGN})
endfunction()