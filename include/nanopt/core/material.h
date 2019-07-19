#pragma once

#include <nanopt/core/bxdf.h>
#include <nanopt/core/interaction.h>

namespace nanopt {

class Material {
public:
  virtual ~Material() = default;
  virtual void computeScatteringFunctions(Interaction& isect) const = 0;
};

}
