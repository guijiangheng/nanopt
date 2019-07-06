#pragma once

#include <nanopt/core/spectrum.h>

namespace nanopt {

class BSDF {
public:
  virtual bool isDelta() const = 0;

  virtual float pdf(const Vector3f&wo, const Vector3f& wi) const = 0;

  virtual Spectrum eval(const Vector3f& wo, const Vector3f& wi) const = 0;

  virtual Spectrum sample(
    const Vector3f& wo,
    Vector3f& wi,
    const Vector2f& sample) const = 0;
};

}
