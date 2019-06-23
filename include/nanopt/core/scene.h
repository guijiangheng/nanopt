#pragma once

#include <nanopt/core/primitive.h>

namespace nanopt {

class Scene {
public:
  Scene(const Primitive& accel) noexcept : accel(accel)
  { }

  bool intersect(const Ray& ray, Interaction& isect) const {
    return accel.intersect(ray, isect);
  }

  bool intersect(const Ray& ray) const {
    return accel.intersect(ray);
  }

private:
  const Primitive& accel;
};

}
