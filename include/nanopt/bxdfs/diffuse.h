#pragma once

#include <nanopt/math/math.h>
#include <nanopt/core/bxdf.h>
#include <nanopt/core/sampling.h>

namespace nanopt {

class Diffuse : public BxDF {
public:
  Diffuse(const Spectrum& kd) noexcept : kd(kd)
  { }

  bool isDelta() const override {
    return false;
  }

  float pdf(const Vector3f& wo, const Vector3f& wi) const override {
    if (Frame::cosTheta(wo) * Frame::cosTheta(wi) <= 0)
      return 0;
    return Frame::absCosTheta(wi) * InvPi;
  }

  Spectrum f(const Vector3f& wo, const Vector3f& wi) const override {
    if (Frame::cosTheta(wo) * Frame::cosTheta(wi) <= 0)
      return Spectrum(0);
    return Spectrum(kd * InvPi);
  }

  Spectrum sample(const Vector2f& sample, const Vector3f& wo, Vector3f& wi, float& etaScale) const override {
    etaScale = 1.0f;
    wi = consineSampleHemisphere(sample);
    if (Frame::cosTheta(wo) < 0)
      wi.z = -wi.z;
    return kd;
  }

public:
  Spectrum kd;
};

}
