add_library(lodepng STATIC
  "${CMAKE_CURRENT_SOURCE_DIR}/lodepng/lodepng.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/lodepng/lodepng.cpp"
)

target_compile_features(lodepng PRIVATE cxx_std_17)
set(LODEPNG_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)

add_subdirectory(filesystem)
set(GHC_FILESYSTEM_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/filesystem/include" PARENT_SCOPE)

set(TINYEXR_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/tinyexr" PARENT_SCOPE)
