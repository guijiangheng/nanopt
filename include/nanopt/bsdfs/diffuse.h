#pragma once

#include <nanopt/math/math.h>
#include <nanopt/core/bsdf.h>
#include <nanopt/core/frame.h>
#include <nanopt/core/sampling.h>

namespace nanopt {

class Diffuse : public BSDF {
public:
  Diffuse(const Spectrum& albedo) : albedo(albedo)
  { }

  bool isDelta() const override {
    return true;
  }

  float pdf(const Vector3f& wo, const Vector3f& wi) const override {
    if (Frame::cosTheta(wi) < 0) return 0;
    return Frame::cosTheta(wi) * InvPi;
  }

  Spectrum eval(const Vector3f& wo, const Vector3f& wi) const override {
    if (Frame::cosTheta(wi) < 0) return Spectrum(0);
    return Spectrum(albedo * InvPi);
  }

  Spectrum sample(const Vector3f& wo, Vector3f& wi, const Vector2f& sample ) const override {
    wi =consineSampleHemisphere(sample);
    return albedo;
  }

public:
  Spectrum albedo;
};

}
