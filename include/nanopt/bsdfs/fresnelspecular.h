#pragma once

#include <nanopt/core/frame.h>
#include <nanopt/core/bsdf.h>
#include <nanopt/core/fresnel.h>

namespace nanopt {

class FresnelSpecular : public BSDF {
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

  Spectrum sample(const Vector2f& sample, const Vector3f& wo, Vector3f& wi, float& etaScale) const override {
    auto k = frDielectric(Frame::cosTheta(wo), eta);
    if (sample.x < k) {
      etaScale = 1.0f;
      wi = Vector3f(-wo.x, -wo.y, wo.z);
      return kr;
    } else {
      auto eta = this->eta;
      auto n = Vector3f(0, 0, 1);
      if (Frame::cosTheta(wo) < 0) {
        n.z = -1;
        eta = 1 / eta;
      }
      wi = refract(wo, n, eta);
      etaScale = eta * eta;
      return kt / etaScale;
    }
  }

public:
  const Spectrum kr, kt;
  float eta;
};

}
