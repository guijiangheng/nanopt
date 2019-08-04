#pragma once

#include <nanopt/core/light.h>

namespace nanopt {

class InfiniteAreaLight : public Light {
public:
  bool isDelta() const override {
    return false;
  }

  bool isInfinite() const override {
    return true;
  }

  Spectrum le(const Ray& ray) const {
    return Spectrum(0);
  }

  Spectrum sample(
    const Interaction& ref,
    Vector2f& u,
    Vector3f& wi, float& pdf, VisibilityTester& tester) const override {

    return Spectrum(0);
  }
};

}
