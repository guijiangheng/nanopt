#pragma once

#include <nanopt/core/bxdf.h>

namespace nanopt {

class Diffuse : public BxDF {
public:
  Diffuse(const Spectrum& kd) noexcept : kd(kd)
  { }

  Spectrum f(const Vector3f& wo, const Vector3f& wi) const override {
    if (!sameHemisphere(wo, wi))
      return Spectrum(0);
    return Spectrum(kd) * InvPi;
  }

public:
  Spectrum kd;
};

}
