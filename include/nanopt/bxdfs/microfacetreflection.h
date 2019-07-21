#pragma once

#include <nanopt/core/bxdf.h>
#include <nanopt/core/fresnel.h>
#include <nanopt/core/microfacet.h>

namespace nanopt {

class MicrofacetReflection : public BxDF {
public:
  MicrofacetReflection(
    const Spectrum& ks,
    const Fresnel* fresnel,
    const MicrofacetDistribution* distribution
  ) noexcept : ks(ks), fresnel(fresnel), distribution(distribution)
  { }

  ~MicrofacetReflection() {
    delete fresnel;
    delete distribution;
  }

  float pdf(const Vector3f& wo, const Vector3f& wi) const override {
    if (!sameHemisphere(wo, wi)) return 0;
    auto wh = normalize(wo + wi);
    return distribution->pdf(wh) / (4 * dot(wo, wh));
  }

  Spectrum f(const Vector3f& wo, const Vector3f& wi) const override {
    if (!sameHemisphere(wo, wi)) return Spectrum(0);
    auto wh = normalize(wo + wi);
    auto cosThetaO = cosTheta(wo);
    auto cosThetaI = cosTheta(wi);
    auto f = fresnel->evaluate(dot(wi, wh));
    return ks * distribution->d(wh) * distribution->g(wo, wi) * f / (4 * cosThetaI * cosThetaO);
  }

  Spectrum sample(
    const Vector2f& sample,
    const Vector3f& wo,
    Vector3f& wi,
    float& pdf,
    float& etaScale) const override {

    auto wh = distribution->sample(sample);
    wi = reflect(wo, wh);
    if (!sameHemisphere(wo, wi)) return Spectrum(0);
    pdf = distribution->pdf(wh) / (4 * dot(wo, wh));
    return f(wo, wi);
  }

private:
  Spectrum ks;
  const Fresnel* fresnel;
  const MicrofacetDistribution* distribution;
};

}
