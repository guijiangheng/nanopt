#pragma once

#include <nanopt/core/bsdf.h>
#include <nanopt/core/interaction.h>

namespace nanopt {

class Material {
public:
  virtual ~Material() = default;
  virtual void computeScatteringFunctions(Interaction& isect) const = 0;
};

}
