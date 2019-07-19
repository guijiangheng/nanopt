#pragma once

#include <nanopt/core/bxdf.h>
#include <nanopt/core/fresnel.h>

namespace nanopt {

class FresnelSpecular : public BxDF {
public:
  FresnelSpecular(const Spectrum& kr, const Spectrum& kt, float eta) noexcept
    : kr(kr), kt(kt), eta(eta)
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

  Spectrum sample(
    const Vector2f& sample,
    const Vector3f& wo,
    Vector3f& wi,
    float& pdf,
    float& etaScale) const override {

    auto f = frDielectric(cosTheta(wo), eta);

    if (sample.x < f) {
      pdf = f;
      etaScale = 1.0f;
      wi = Vector3f(-wo.x, -wo.y, wo.z);
      return kr * f / absCosTheta(wi);
    } else {
      auto entering = cosTheta(wo) > 0;
      auto eta = entering ? this->eta : 1 / this->eta;
      auto n = entering ? Vector3f(0, 0, 1) : Vector3f(0, 0, -1);
      pdf = 1 - f;
      etaScale = eta * eta;
      wi = refract(wo, n, eta);
      return kt * (1 - f) / etaScale / absCosTheta(wi);
    }
  }

public:
  const Spectrum kr, kt;
  float eta;
};

}
