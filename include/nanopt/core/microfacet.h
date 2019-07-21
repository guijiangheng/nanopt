#pragma once

#include <nanopt/core/frame.h>

namespace nanopt {

class MicrofacetDistribution {
public:
  virtual ~MicrofacetDistribution() = default;

  virtual float d(const Vector3f& wh) const = 0;

  virtual float lambda(const Vector3f& w) const = 0;

  virtual Vector3f sample(const Vector2f& u) const = 0;

  float pdf(const Vector3f& wh) const {
    return d(wh) * absCosTheta(wh);
  }

  float g1(const Vector3f& w) const {
    return 1 / (1 + lambda(w));
  }

  float g(const Vector3f& wo, const Vector3f& wi) const {
    return 1 / (1 + lambda(wo) + lambda(wi));
  }
};

}
