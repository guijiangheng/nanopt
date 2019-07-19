#pragma once

#include <nanopt/core/material.h>
#include <nanopt/bxdfs/mirror.h>

namespace nanopt {

class MirrorMaterial : public Material {
public:
  explicit MirrorMaterial(const Spectrum& kr) noexcept : kr(kr)
  { }

  void computeScatteringFunctions(Interaction& isect) const override {
    if (kr.isBlack()) return;
    isect.bsdf = new BSDF(isect);
    isect.bsdf->add(new Mirror(kr));
  }

public:
  Spectrum kr;
};

}
