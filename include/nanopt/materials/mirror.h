#pragma once

#include <nanopt/core/spectrum.h>
#include <nanopt/core/material.h>
#include <nanopt/bsdfs/mirror.h>

namespace nanopt {

class MirrorMaterial : public Material {
public:
  explicit MirrorMaterial(const Spectrum& kr) noexcept : kr(kr)
  { }

  void computeScatteringFunctions(Interaction& isect) const override {
    isect.bsdf = new Mirror(kr);
  }

public:
  Spectrum kr;
};

}
