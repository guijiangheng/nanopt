#pragma once

#include <nanopt/core/integrator.h>

namespace nanopt {

class PathIntegrator : public Integrator {
public:
  PathIntegrator(const Camera& camera, Sampler& sampler, int maxDepth)
    : Integrator(camera, sampler)
    , maxDepth(maxDepth)
  { }

  static float powerHeuristic(float a, float b) {
    return (a * a) / (a * a + b * b);
  }

  Spectrum li(const Ray& ray, const Scene& scene) const override;

  Spectrum estimateDirect(
    const Interaction& isect,
    const Light& light,
    const Scene& scene) const;

  Spectrum sampleOneLight(const Interaction& isect, const Scene& scene) const {
    auto nLights = scene.lights.size();
    if (!nLights) return Spectrum(0);
    auto light = scene.lights[(std::size_t)(sampler.get1D() * nLights)];
    return estimateDirect(isect, *light, scene) * nLights;
  }

public:
  int maxDepth;
};

}
