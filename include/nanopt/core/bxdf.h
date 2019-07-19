#pragma once

#include <nanopt/math/math.h>
#include <nanopt/core/frame.h>
#include <nanopt/core/spectrum.h>
#include <nanopt/core/sampling.h>

namespace nanopt {

class BxDF {
public:
  virtual ~BxDF() = default;

  virtual bool isDelta() const {
    return false;
  }

  virtual Spectrum f(const Vector3f& wo, const Vector3f& wi) const = 0;

  virtual float pdf(const Vector3f&wo, const Vector3f& wi) const {
    return sameHemisphere(wo, wi) ? absCosTheta(wi) * InvPi : 0;
  }

  virtual Spectrum sample(
    const Vector2f& sample,
    const Vector3f& wo,
    Vector3f& wi,
    float& pdf,
    float& etaScale) const {

    etaScale = 1.0f;
    wi = consineSampleHemisphere(sample);
    if (wo.z < 0) wi.z = -wi.z;
    pdf = absCosTheta(wi) * InvPi;

    return f(wo, wi);
  }
};

}
