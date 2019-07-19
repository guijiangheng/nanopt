#pragma once

#include <nanopt/core/material.h>
#include <nanopt/bxdfs/fresnelspecular.h>

namespace nanopt {

class GlassMaterial : public Material {
public:
  GlassMaterial(const Spectrum& kr, const Spectrum& kt, float eta) noexcept
    : kr(kr), kt(kt), eta(eta)
  { }

  void computeScatteringFunctions(Interaction& isect) const override {
    if (!kr.isBlack() && !kt.isBlack())
      isect.bsdf = new FresnelSpecular(kr, kt, eta);
  }

public:
  Spectrum kr, kt;
  float eta;
};

}
