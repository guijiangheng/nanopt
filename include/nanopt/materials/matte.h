#pragma once

#include <nanopt/core/spectrum.h>
#include <nanopt/core/material.h>
#include <nanopt/bsdfs/diffuse.h>

namespace nanopt {

class MatteMaterial : public Material {
public:
  explicit MatteMaterial(const Spectrum& kd) noexcept : kd(kd)
  { }

  void computeScatteringFunctions(Interaction& isect) const override {
    if (!kd.isBlack()) {
      isect.bsdf = new Diffuse(kd);
    }
  }

public:
  Spectrum kd;
};

}
