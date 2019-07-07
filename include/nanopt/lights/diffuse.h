#pragma once

#include <nanopt/core/light.h>
#include <nanopt/core/triangle.h>
#include <nanopt/core/visibilitytester.h>

namespace nanopt {

class DiffuseAreaLight : public Light {
public:
  DiffuseAreaLight(Triangle* shape, const Spectrum& intensity, bool twoSided = false) noexcept
    : shape(shape)
    , intensity(intensity)
    , twoSided(twoSided)
  { }

  Spectrum le(const Interaction& pLight, const Vector3f& wo) const {
    if (twoSided || dot(pLight.n, wo) > 0)
      return intensity;
    return Spectrum(0);
  }

  float pdf(const Interaction& ref, const Vector3f& w) const override {
    return shape->pdf(ref, w);
  }

  Spectrum sample(
    const Interaction& ref,
    const Vector2f& sample,
    Vector3f& wi, float& pdf, VisibilityTester& tester) const override {

    auto pLight = shape->sample(ref, sample, pdf);
    wi = normalize(pLight.p - ref.p);
    tester = VisibilityTester(ref, pLight.p);
    return le(pLight, -wi);
  }

public:
  Triangle* shape;
  Spectrum intensity;
  bool twoSided;
};

}
