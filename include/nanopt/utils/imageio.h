#pragma once

#include <string>
#include <memory>
#include <nanopt/core/spectrum.h>

namespace nanopt {

std::unique_ptr<Spectrum[]>
readImage(const std::string& filename, int& width, int& height);

void writeImage(
  const std::string& filename,
  int width, int height, Spectrum* data
);

}
