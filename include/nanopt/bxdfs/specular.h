#pragma once

#include <nanopt/core/frame.h>
#include <nanopt/core/bxdf.h>
#include <nanopt/core/fresnel.h>

namespace nanopt {

class SpecularReflection : public BxDF {
public:
  SpecularReflection(Fresnel* fresnel, const Spectrum& kr) noexcept
    : fresnel(fresnel), kr(kr)
  { }

  ~SpecularReflection() {
    delete fresnel;
  }

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
    etaScale = 1.0f;
    wi = Vector3f(-wo.x, -wo.y, wo.z);
    return fresnel->evaluate(Frame::cosTheta(wi)) * kr;
  }

public:
  Fresnel* fresnel;
  Spectrum kr;
};


}
