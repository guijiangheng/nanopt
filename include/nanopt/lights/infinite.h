#pragma once

#include <nanopt/core/light.h>

namespace nanopt {

class InfiniteAreaLight : public Light {
public:
  bool isDelta() const override {
    return false;
  }

  Spectrum le(const Ray& ray) const {
    return Spectrum(0);
  }

  float pdf(const Interaction& ref, const Vector3f& w) const override {
    return 1;
  }

  Spectrum sample(
    const Interaction& ref,
    const Vector2f& sample,
    Vector3f& wi, float& pdf, VisibilityTester& tester) const override {

    return Spectrum(0);
  }
};

}
