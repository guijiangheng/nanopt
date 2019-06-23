#pragma once

#include <memory>
#include <nanopt/math/bounds2.h>
#include <nanopt/core/spectrum.h>
#include <nanopt/utils/imageio.h>

namespace nanopt {

class Film {
public:
  Film(
    const Vector2i& resolution,
    const Bounds2f& cropWindow = Bounds2f(Vector2f(0.0f), Vector2f(1.0f)))
      : resolution(resolution)
      , pixelBounds(
        Vector2i((int)std::ceil(resolution.x * cropWindow.pMin.x), (int)std::ceil(resolution.x * cropWindow.pMin.y)),
        Vector2i((int)std::ceil(resolution.x * cropWindow.pMax.x), (int)std::ceil(resolution.y * cropWindow.pMax.y)))
      , pixels(new Spectrum[pixelBounds.area()])
  { }

  void writeImage(const std::string& filename) {
    auto diag = pixelBounds.diag();
    nanopt::writeImage(filename, diag.x, diag.y, pixels.get());
  }

public:
  Vector2i resolution;
  Bounds2i pixelBounds;
  std::unique_ptr<Spectrum[]> pixels;
};

}
