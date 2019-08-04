#pragma once

#include <nanopt/core/light.h>
#include <nanopt/core/interaction.h>
#include <nanopt/core/visibilitytester.h>

namespace nanopt {

class PointLight : public Light {
public:
  PointLight(const Vector3f& pLight, const Spectrum& intensity) noexcept
    : pLight(pLight), intensity(intensity)
  { }

  Spectrum sample(
    const Interaction& ref,
    Vector2f& u,
    Vector3f& wi, float& pdf, VisibilityTester& tester) const override {

    pdf = 1;
    auto d = pLight - ref.p;
    wi = normalize(d);
    tester = VisibilityTester(ref, pLight);
    return intensity / d.lengthSquared();
  }

public:
  Vector3f pLight;
  Spectrum intensity;
};

}
