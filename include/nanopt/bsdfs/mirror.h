#pragma once

#include <nanopt/core/bsdf.h>
#include <nanopt/core/sampling.h>

namespace nanopt {

class Mirror : public BSDF {
public:
  Mirror(const Spectrum& kr) noexcept : kr(kr)
  { }

  bool isDelta() const override {
    return true;
  }

  float pdf(const Vector3f& wo, const Vector3f& wi) const override {
    return 0;
  }

  Spectrum f(const Vector3f& wo, const Vector3f& wi) const override {
    return Spectrum(0);
  }

  Spectrum sample(const Vector2f& sample, const Vector3f& wo, Vector3f& wi) const override {
    wi = Vector3f(-wo.x, -wo.y, wo.z);
    return kr;
  }

public:
  Spectrum kr;
};

}
