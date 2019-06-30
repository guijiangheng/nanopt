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

if (CMAKE_COMPILER_IS_GNUCXX)
  add_compile_options(-Wall -Wextra -pedantic -Wno-unused-parameter -Werror)
endif()

set(
  NANOPT_HEADERS
  include/nanopt/nanopt.h
  include/nanopt/accelerators/bvh.h
  include/nanopt/core/interaction.h
  include/nanopt/core/primitive.h
  include/nanopt/core/camera.h
  include/nanopt/core/film.h
  include/nanopt/core/integrator.h
  include/nanopt/core/ray.h
  include/nanopt/core/sampler.h
  include/nanopt/core/parallel.h
  include/nanopt/core/scene.h
  include/nanopt/core/spectrum.h
  include/nanopt/core/triangle.h
  include/nanopt/cameras/perspective.h
  include/nanopt/integrators/normal.h
  include/nanopt/math/math.h
  include/nanopt/math/bounds2.h
  include/nanopt/math/bounds3.h
  include/nanopt/math/vector2.h
  include/nanopt/math/vector3.h
  include/nanopt/samplers/random.h
  include/nanopt/utils/objloader.h
)

set(
  NANOPT_SRCS
  src/accelerators/bvh.cpp
  src/core/integrator.cpp
  src/core/triangle.cpp
  src/core/parallel.cpp
  src/math/matrix4.cpp
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
target_link_libraries(imageio PRIVATE ${OPENEXR_LIBS} lodepng)

if(CMAKE_COMPILER_IS_GNUCXX)
  target_link_libraries(imageio PRIVATE stdc++fs)
  target_compile_options(imageio PRIVATE -Wno-deprecated)
endif()

add_library(
  nanopt STATIC
  ${NANOPT_HEADERS}
  ${NANOPT_SRCS}
)

target_include_directories(
  nanopt PUBLIC
  "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_compile_features(nanopt PRIVATE cxx_std_17)
target_link_libraries(nanopt PRIVATE imageio)

add_executable(ajax src/main/ajax.cpp)
add_executable(bunny src/main/bunny.cpp)
add_executable(imageio-test src/tests/imageio-test.cpp)
add_executable(fireplace-room src/main/fireplace-room.cpp)

set(
  NANOPT_EXES
  ajax
  bunny
  imageio-test
  fireplace-room
)

foreach(target ${NANOPT_EXES})
  target_link_libraries(${target} PRIVATE nanopt)
  target_compile_features(${target} PRIVATE cxx_std_17)
endforeach()
