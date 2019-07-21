#pragma once

#include <nanopt/core/material.h>
#include <nanopt/core/fresnel.h>
#include <nanopt/bxdfs/diffuse.h>
#include <nanopt/bxdfs/microfacetreflection.h>
#include <nanopt/microfacets/beckmann.h>

namespace nanopt {

class PlasticMaterial : public Material {
public:
  PlasticMaterial(
    const Spectrum& kd,
    const Spectrum& ks,
    float roughness,
    bool remapRoughnes
  ) noexcept : kd(kd), ks(ks), roughness(roughness), remapRoughness(remapRoughnes)
  { }

  void computeScatteringFunctions(Interaction& isect) const override {
    if (kd.isBlack() && ks.isBlack()) return;
    isect.bsdf = new BSDF(isect);
    if (!kd.isBlack())
      isect.bsdf->add(new Diffuse(kd));
    if (!ks.isBlack()) {
      auto fresnel = new FresnelDielectric(1 / 1.5f);
      auto rough = remapRoughness ? BeckmannDistribution::roughnessToAlpha(roughness) : roughness;
      auto distribution = new BeckmannDistribution(rough, rough);
      isect.bsdf->add(new MicrofacetReflection(ks, fresnel, distribution));
    }
  }

private:
  Spectrum kd, ks;
  float roughness;
  bool remapRoughness;
};

}
