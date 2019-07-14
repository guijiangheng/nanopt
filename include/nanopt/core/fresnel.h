#pragma once

#include <nanopt/core/spectrum.h>

namespace nanopt {

float frDielectric(float cosThetaI, float eta);

Spectrum frConductor(
  float cosThetaI,
  const Spectrum& etaI,
  const Spectrum& etaT,
  const Spectrum& k);


class Fresnel {
public:
  virtual ~Fresnel() = default;
  virtual Spectrum evaluate(float cosThetaI) const = 0;
};

class FresnelDielectric : public Fresnel {
public:
  FresnelDielectric(float eta) noexcept : eta(eta)
  { }

  Spectrum evaluate(float cosThetaI) const override {
    return Spectrum(frDielectric(cosThetaI, eta));
  }

private:
  float eta;
};

class FresnelConductor : public Fresnel {
public:
  FresnelConductor(const Spectrum& etaI, const Spectrum& etaT, const Spectrum& k)
    : etaI(etaI), etaT(etaT), k(k)
  { }

  Spectrum evaluate(float cosThetaI) const override {
    return frConductor(cosThetaI, etaI, etaT, k);
  }

private:
  Spectrum etaI, etaT, k;
};

}
