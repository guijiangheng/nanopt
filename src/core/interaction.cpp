#include <nanopt/core/mesh.h>
#include <nanopt/lights/diffuse.h>
#include <nanopt/core/interaction.h>

namespace nanopt {

Interaction::~Interaction() noexcept {
  if (bsdf) delete bsdf;
}

Spectrum Interaction::le(const Vector3f& w) const {
  if (mesh->light)
    return mesh->light->le(*this, w);
  return Spectrum(0);
}

void Interaction::computeScatteringFunctions() {
  if (mesh->material) {
    mesh->material->computeScatteringFunctions(*this);
  }
}

}
