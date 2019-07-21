#pragma once

#include <nanopt/core/microfacet.h>

namespace nanopt {

class BeckmannDistribution : public MicrofacetDistribution {
public:
  BeckmannDistribution(float alphax, float alphay) noexcept
    : alphax(alphax), alphay(alphay)
  { }

  float lambda(const Vector3f& w) const override;

  float d(const Vector3f& wh) const override;

  Vector3f sample(const Vector2f& u) const override;

  static float roughnessToAlpha(float roughness) {
    roughness = std::max(roughness, 0.001f);
    auto x = std::log(roughness);
    return 1.62142f + 0.819955f * x + 0.1734f * x * x +
           0.0171201f * x * x * x + 0.000640711f * x * x * x * x;
  }

private:
  float alphax, alphay;
};

}
