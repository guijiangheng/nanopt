#pragma once

#include <nanopt/core/frame.h>
#include <nanopt/core/sampling.h>
#include <nanopt/core/integrator.h>


namespace nanopt {

class AmbientOcclusionIntegrator : public Integrator {
public:
  AmbientOcclusionIntegrator(const Camera& camera, Sampler& sampler, int samples)
    : Integrator(camera, sampler), samples(samples)
  { }

  Spectrum li(const Ray& ray, const Scene& scene) const override {
    Interaction isect;
    Spectrum ret(0);
    if (!scene.intersect(ray, isect))
      return ret;
    auto frame = Frame(isect.n);
    for (auto i = 0; i < samples; ++i) {
      auto p = cosineSampleHemisphere(sampler.get2D());
      auto w = frame.toWorld(p);
      auto r = isect.spawnRay(w);
      r.tMax = 1.0f;
      ret += scene.intersect(r) ? Spectrum(0) : Spectrum(1);
    }
    return ret / samples;
  }

public:
  int samples;
};

}
