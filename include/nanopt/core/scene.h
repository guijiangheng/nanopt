#pragma once

#include <vector>
#include <nanopt/core/light.h>
#include <nanopt/core/accel.h>
#include <nanopt/lights/infinite.h>

namespace nanopt {

class Scene {
public:
  Scene(
    const Accelerator& accel,
    std::vector<Light*>&& lights = {}) noexcept
      : accel(accel)
      , lights(std::move(lights))
      , infiniteLight(nullptr)
  { }

  ~Scene() noexcept {
    for (auto light : lights)
      delete light;
    if (infiniteLight) delete infiniteLight;
  }

  bool intersect(const Ray& ray, Interaction& isect) const {
    return accel.intersect(ray, isect);
  }

  bool intersect(const Ray& ray) const {
    return accel.intersect(ray);
  }

public:
  const Accelerator& accel;
  std::vector<Light*> lights;
  InfiniteAreaLight* infiniteLight;
};

}
