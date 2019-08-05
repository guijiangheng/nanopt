#pragma once

#include <nanopt/core/bxdf.h>
#include <nanopt/core/frame.h>
#include <nanopt/core/spectrum.h>
#include <nanopt/core/interaction.h>

namespace nanopt {

class BSDF {
public:
  BSDF(const Interaction& isect) noexcept : nBxDFs(0), shFrame(isect.ns)
  { }

  ~BSDF() noexcept {
    for (auto i = 0; i < nBxDFs; ++i)
      delete bxdfs[i];
  }

  bool isDelta() const {
    if (nBxDFs == 1 && bxdfs[0]->isDelta())
      return true;
    return false;
  }

  void add(BxDF* bxdf) {
    bxdfs[nBxDFs++] = bxdf;
  }

  Vector3f toLocal(const Vector3f& w) const {
    return shFrame.toLocal(w);
  }

  Vector3f toWorld(const Vector3f& w) const {
    return shFrame.toWorld(w);
  }

  Spectrum f(const Vector3f& woWorld, const Vector3f& wiWorld) const {
    Spectrum f(0);
    auto wo = toLocal(woWorld);
    auto wi = toLocal(wiWorld);
    for (auto i = 0; i < nBxDFs; ++i)
      f += bxdfs[i]->f(wo, wi);
    return f;
  }

  Spectrum sample(
    const Vector2f& u,
    const Vector3f& woWorld,
    Vector3f& wiWorld,
    float& pdf,
    float& etaScale) const {

    auto n = std::min((int)u[0] * nBxDFs, nBxDFs - 1);
    auto uRemapped = Vector2f(u[0] * nBxDFs - n, u[1]);
    auto wo = toLocal(woWorld);
    Vector3f wi;
    auto f = bxdfs[n]->sample(uRemapped, wo, wi, pdf, etaScale);

    for (auto i = 0; i < nBxDFs; ++i) {
      if (i == n) continue;
      f += bxdfs[i]->f(wo, wi);
      pdf += bxdfs[i]->pdf(wo, wi);
    }

    pdf /= nBxDFs;
    wiWorld = toWorld(wi);

    return f;
  }

  float pdf(const Vector3f& woWorld, const Vector3f& wiWorld) const {
    auto wo = toLocal(woWorld);
    auto wi = toLocal(wiWorld);
    float pdf = 0;
    for (auto i = 0; i < nBxDFs; ++i)
      pdf += bxdfs[i]->pdf(wo, wi);
    return pdf / nBxDFs;
  }

private:
  static constexpr int MaxBxDFs = 8;
  int nBxDFs;
  BxDF* bxdfs[MaxBxDFs];
  Frame shFrame;
};

}
