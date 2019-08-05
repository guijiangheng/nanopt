#pragma once

#include <nanopt/core/integrator.h>

namespace nanopt {

class PathIntegrator : public Integrator {
public:
  PathIntegrator(const Camera& camera, Sampler& sampler, int maxDepth = 5)
    : Integrator(camera, sampler)
    , maxDepth(maxDepth)
  { }

  static float powerHeuristic(float a, float b) {
    return (a * a) / (a * a + b * b);
  }

  Spectrum li(const Ray& ray, const Scene& scene) const override;

  Spectrum estimateDirect(
    const Interaction& isect,
    const Scene& scene) const;

public:
  int maxDepth;
};

}
