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
    if (scene.intersect(ray, isect)) {
      Spectrum ret(0);
      for (auto i = 0; i < samples; ++i) {
        auto p = consineSampleHemisphere(sampler.get2D());
        auto frame = Frame(isect.n);
        auto w = frame.toWorld(p);
        auto r = isect.spawnRay(w);
        r.tMax = 1.0f;
        auto ao = scene.intersect(r) ? Spectrum(0) : Spectrum(1);
        ret += ao;
      }
      return ret / samples;
    }
    return Spectrum(0);
  }

public:
  int samples;
};

}
