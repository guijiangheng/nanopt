#pragma once

#include <nanopt/core/ray.h>
#include <nanopt/core/spectrum.h>
#include <nanopt/core/interaction.h>
#include <nanopt/core/visibilitytester.h>

namespace nanopt {

class Light {
public:
  virtual ~Light() = default;

  virtual bool isDelta() const {
    return true;
  }

  virtual bool isInfinite() const {
    return false;
  }

  virtual Spectrum sample(
    const Interaction& ref,
    Vector2f& sample,
    Vector3f& wi, float& pdf, VisibilityTester& tester) const = 0;
};

}
