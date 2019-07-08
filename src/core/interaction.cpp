#include <nanopt/core/triangle.h>
#include <nanopt/lights/diffuse.h>
#include <nanopt/core/interaction.h>

namespace nanopt {

Interaction::~Interaction() noexcept {
  if (bsdf) delete bsdf;
}

Spectrum Interaction::le(const Vector3f& w) const {
  if (triangle->light)
    return triangle->light->le(*this, w);
  return Spectrum(0);
}

void Interaction::computeScatteringFunctions() {
  if (triangle->material) {
    triangle->material->computeScatteringFunctions(*this);
  }
}

}
