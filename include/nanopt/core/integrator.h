#pragma once

#include <nanopt/core/spectrum.h>
#include <nanopt/core/scene.h>
#include <nanopt/core/sampler.h>

namespace nanopt {

class Integrator {
public:
  Integrator(const Camera& camera, Sampler& sampler) noexcept
    : camera(camera), sampler(sampler)
  { }

  virtual ~Integrator() = default;

  virtual Spectrum li(const Ray& ray, const Scene& scene) const = 0;

  void render(const Scene& scene) {
    auto offset = 0;
    for (auto p : camera.film.pixelBounds) {
      Spectrum l(0);
      sampler.startPixel();
      do {
        auto cameraSample = sampler.getCameraSample(p);
        auto ray = camera.generateRay(cameraSample);
        l += li(ray, scene);
      } while (sampler.startNextSample());
      camera.film.pixels[offset++] = l / sampler.samplesPerPixel;
    }
  }

private:
  const Camera& camera;
  Sampler& sampler;
};

}
