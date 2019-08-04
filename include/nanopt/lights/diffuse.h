#pragma once

#include <nanopt/core/light.h>
#include <nanopt/core/mesh.h>

namespace nanopt {

class DiffuseAreaLight : public Light {
public:
  DiffuseAreaLight(Mesh* mesh, const Spectrum& intensity, bool twoSided = false) noexcept
    : mesh(mesh)
    , intensity(intensity)
    , twoSided(twoSided) {

    mesh->light = this;
  }

  bool isDelta() const override {
    return false;
  }

  Spectrum le(const Interaction& pLight, const Vector3f& wo) const {
    if (twoSided || dot(pLight.n, wo) > 0)
      return intensity;
    return Spectrum(0);
  }

  Spectrum sample(
    const Interaction& ref,
    Vector2f& u,
    Vector3f& wi, float& pdf, VisibilityTester& tester) const override {

    auto pLight = mesh->sample(ref, u, pdf);
    wi = normalize(pLight.p - ref.p);
    tester = VisibilityTester(ref, pLight.p);
    return le(pLight, -wi);
  }

public:
  Mesh* mesh;
  Spectrum intensity;
  bool twoSided;
};

}
