cmake_minimum_required(VERSION 3.0)
project(nanopt LANGUAGES CXX)

if (NOT IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/ext/lodepng")
  message(FATAL_ERROR "The lodepng submodule directory is missing! "
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

  include/nanopt/bxdfs/diffuse.h
  include/nanopt/bxdfs/mirror.h
  include/nanopt/bxdfs/specular.h
  include/nanopt/bxdfs/fresnelspecular.h
  include/nanopt/bxdfs/microfacetreflection.h

  include/nanopt/core/bxdf.h
  include/nanopt/core/bsdf.h
  include/nanopt/core/camera.h
  include/nanopt/core/distribution1d.h
  include/nanopt/core/distribution2d.h
  include/nanopt/core/film.h
  include/nanopt/core/frame.h
  include/nanopt/core/fresnel.h
  include/nanopt/core/integrator.h
  include/nanopt/core/interaction.h
  include/nanopt/core/mesh.h
  include/nanopt/core/material.h
  include/nanopt/core/microfacet.h
  include/nanopt/core/ray.h
  include/nanopt/core/sampler.h
  include/nanopt/core/parallel.h
  include/nanopt/core/primitive.h
  include/nanopt/core/scene.h
  include/nanopt/core/spectrum.h
  include/nanopt/core/triangle.h

  include/nanopt/cameras/perspective.h

  include/nanopt/integrators/ao.h
  include/nanopt/integrators/normal.h
  include/nanopt/integrators/path.h

  include/nanopt/lights/point.h
  include/nanopt/lights/diffuse.h
  include/nanopt/lights/infinite.h

  include/nanopt/microfacets/beckmann.h

  include/nanopt/materials/matte.h
  include/nanopt/materials/mirror.h
  include/nanopt/materials/glass.h
  include/nanopt/materials/plastic.h

  include/nanopt/math/math.h
  include/nanopt/math/bounds2.h
  include/nanopt/math/bounds3.h
  include/nanopt/math/vector2.h
  include/nanopt/math/vector3.h

  include/nanopt/samplers/random.h

  include/nanopt/utils/objloader.h
  include/nanopt/utils/plyloader.h
)

set(
  NANOPT_SRCS
  src/accelerators/bvh.cpp
  src/core/distribution1d.cpp
  src/core/fresnel.cpp
  src/core/integrator.cpp
  src/core/interaction.cpp
  src/core/triangle.cpp
  src/core/parallel.cpp
  src/core/visibilitytester.cpp
  src/integrators/path.cpp
  src/microfacets/beckmann.cpp
  src/math/matrix4.cpp
  src/utils/objloader.cpp
  src/utils/plyloader.cpp
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
  ${TINYEXR_INCLUDE_DIR}
)

target_compile_features(imageio PRIVATE cxx_std_17)
target_link_libraries(imageio PRIVATE lodepng)

add_library(
  nanopt STATIC
  ${NANOPT_HEADERS}
  ${NANOPT_SRCS}
)

target_include_directories(
  nanopt PUBLIC
  "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

find_package(Threads)
target_compile_features(nanopt PRIVATE cxx_std_17)
target_link_libraries(nanopt PRIVATE imageio ${CMAKE_THREAD_LIBS_INIT})

add_executable(ao src/main/ao.cpp)
add_executable(ajax src/main/ajax.cpp)
add_executable(cbox src/main/cbox.cpp)
add_executable(point src/main/point.cpp)
add_executable(bunny src/main/bunny.cpp)
add_executable(dragon src/main/dragon.cpp)
add_executable(imageio-test src/tests/imageio-test.cpp)
add_executable(fireplace-room src/main/fireplace-room.cpp)
add_executable(plastic src/main/plastic.cpp)
add_executable(table src/main/table.cpp)
add_executable(mis src/main/mis.cpp)

set(
  NANOPT_EXES
  ao
  ajax
  cbox
  bunny
  point
  dragon
  imageio-test
  fireplace-room
  plastic
  table
  mis
)

foreach(target ${NANOPT_EXES})
  target_link_libraries(${target} PRIVATE nanopt)
  target_compile_features(${target} PRIVATE cxx_std_17)
endforeach()
