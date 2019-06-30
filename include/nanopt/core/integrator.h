#pragma once

#include <nanopt/core/spectrum.h>
#include <nanopt/core/scene.h>
#include <nanopt/core/sampler.h>
#include <nanopt/core/parallel.h>

namespace nanopt {

class Integrator {
public:
  Integrator(const Camera& camera, Sampler& sampler) noexcept
    : camera(camera), sampler(sampler)
  { }

  virtual ~Integrator() = default;

  virtual Spectrum li(const Ray& ray, const Scene& scene) const = 0;

  void render(const Scene& scene);

private:
  const Camera& camera;
  Sampler& sampler;
};

}
