#pragma once

#include <nanopt/core/integrator.h>

namespace nanopt {

class NormalIntegrator : public Integrator {
public:
  NormalIntegrator(const Camera& camera, Sampler& sampler)
    : Integrator(camera, sampler)
  { }

  Spectrum li(const Ray& ray, const Scene& scene) const override {
    Interaction isect;
    if (scene.intersect(ray, isect))
      return Spectrum(abs(isect.ns));
    return Spectrum(0);
  }
};

}
