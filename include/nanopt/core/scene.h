#pragma once

#include <vector>
#include <nanopt/core/light.h>
#include <nanopt/core/primitive.h>

namespace nanopt {

class Scene {
public:
  Scene(
    const Primitive& accel,
    std::vector<Light*>&& lights = {}) noexcept
      : accel(accel)
      , lights(std::move(lights))
  { }

  ~Scene() {
    for (auto light : lights)
      delete light;
  }

  bool intersect(const Ray& ray, Interaction& isect) const {
    return accel.intersect(ray, isect);
  }

  bool intersect(const Ray& ray) const {
    return accel.intersect(ray);
  }

private:
  const Primitive& accel;
  std::vector<Light*> lights;
};

}
