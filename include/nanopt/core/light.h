#pragma once

#include <nanopt/core/ray.h>
#include <nanopt/core/spectrum.h>
#include <nanopt/core/interaction.h>
#include <nanopt/core/visibilitytester.h>

namespace nanopt {

class Light {
public:
  virtual ~Light() = default;

  virtual bool isDelta() const = 0;

  virtual float pdf(const Interaction& ref, const Vector3f& w) const = 0;

  virtual Spectrum sample(
    const Interaction& ref,
    const Vector2f& sample,
    Vector3f& wi, float& pdf, VisibilityTester& tester) const = 0;
};

}
