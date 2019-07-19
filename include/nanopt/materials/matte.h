#pragma once

#include <nanopt/core/material.h>
#include <nanopt/bxdfs/diffuse.h>

namespace nanopt {

class MatteMaterial : public Material {
public:
  explicit MatteMaterial(const Spectrum& kd) noexcept : kd(kd)
  { }

  void computeScatteringFunctions(Interaction& isect) const override {
    if (kd.isBlack()) return;
    isect.bsdf = new BSDF(isect);
    isect.bsdf->add(new Diffuse(kd));
  }

public:
  Spectrum kd;
};

}
