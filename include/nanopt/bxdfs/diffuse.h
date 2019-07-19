#pragma once

#include <nanopt/math/math.h>
#include <nanopt/core/bxdf.h>
#include <nanopt/core/sampling.h>

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
