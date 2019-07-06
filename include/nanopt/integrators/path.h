#pragma once

#include <nanopt/core/integrator.h>


namespace nanopt {

class PathIntegrator : public Integrator {
public:
  PathIntegrator(const Camera& camera, Sampler& sampler)
    : Integrator(camera, sampler)
  { }

  Spectrum li(const Ray& ray, const Scene& scene) const override {
    Interaction isect;
    if (scene.intersect(ray, isect))
      return Spectrum(abs(isect.n));
    return Spectrum(0);
  }
};

}
