#include <nanopt/core/triangle.h>
#include <nanopt/core/interaction.h>

namespace nanopt {

Interaction::~Interaction() noexcept {
  if (bsdf) delete bsdf;
}

void Interaction::computeScatteringFunctions() {
  triangle->material->computeScatteringFunctions(*this);
}

}
