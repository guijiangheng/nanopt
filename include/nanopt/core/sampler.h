#pragma once

#include <cstdint>
#include <memory>
#include <nanopt/core/camera.h>

namespace nanopt {

class Sampler {
public:
  explicit Sampler(std::int64_t samplesPerPixel) noexcept
    : samplesPerPixel(samplesPerPixel)
  { }

  virtual ~Sampler() = default;

  virtual float get1D() = 0;

  virtual Vector2f get2D() = 0;

  virtual std::unique_ptr<Sampler> clone(int seed) const = 0;

  void startPixel() {
    currentPixelSampleIndex = 0;
  }

  bool startNextSample() {
    ++currentPixelSampleIndex;
    return currentPixelSampleIndex < samplesPerPixel;
  }

  CameraSample getCameraSample(const Vector2i& pRaster) {
    return { (Vector2f)pRaster + get2D() };
  }

public:
  std::int64_t samplesPerPixel;

protected:
  std::int16_t currentPixelSampleIndex;
};

}
