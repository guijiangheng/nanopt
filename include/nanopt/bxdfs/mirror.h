#pragma once

#include <nanopt/core/bxdf.h>

namespace nanopt {

class Mirror : public BxDF {
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

  Spectrum sample(
    const Vector2f& sample,
    const Vector3f& wo,
    Vector3f& wi,
    float& pdf,
    float& etaScale) const override {

    pdf = 1.0f;
    etaScale = 1.0f;
    wi = Vector3f(-wo.x, -wo.y, wo.z);

    return kr / absCosTheta(wi);
  }

public:
  Spectrum kr;
};

}
