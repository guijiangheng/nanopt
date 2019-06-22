cmake_minimum_required(VERSION 3.0)
project(nanopt LANGUAGES CXX)

if (NOT IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/ext/openexr")
  message(FATAL_ERROR "The OpenEXR submodule directory is missing! "
    "You probably did not clone the project with --recursive. It is possible to recover by running "
    "\"git submodule update --init --recursive\"")
endif()

if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'Release' as none was specified.")
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

add_subdirectory(ext)
add_compile_options(-Wall -Wextra -pedantic -Wno-unused-parameter -Werror)

set(
  NANOPT_HEADERS

  include/nanopt/core/interaction.h
  include/nanopt/core/ray.h
  include/nanopt/core/spectrum.h
  include/nanopt/core/triangle.h

  include/nanopt/math/bounds3.h
  include/nanopt/math/vector2.h
  include/nanopt/math/vector3.h

  include/nanopt/utils/objloader.h
)

set(
  NANOPT_SRCS
  src/utils/objloader.cpp
)

add_library(
  imageio STATIC
  include/nanopt/utils/imageio.h
  src/utils/imageio.cpp
)

target_include_directories(
  imageio PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/include"
  ${LODEPNG_INCLUDE_DIR}
  ${OPENEXR_INCLUDE_DIRS}
)

target_compile_features(imageio PRIVATE cxx_std_14)
target_compile_options(imageio PRIVATE -Wno-deprecated)
target_link_libraries(imageio PRIVATE ${OPENEXR_LIBS} lodepng stdc++fs)

add_library(
  nanopt STATIC
  ${NANOPT_HEADERS}
  ${NANOPT_SRCS}
)

target_include_directories(
  nanopt PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_compile_features(nanopt PRIVATE cxx_std_17)
target_link_libraries(nanopt PRIVATE imageio)

add_executable(imageio-test src/tests/imageio-test.cpp)
target_compile_features(imageio-test PRIVATE cxx_std_17)
target_link_libraries(imageio-test PRIVATE nanopt)
target_include_directories(
  imageio-test PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/include"
)
