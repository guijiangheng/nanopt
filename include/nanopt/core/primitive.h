#pragma once

#include <nanopt/math/bounds3.h>
#include <nanopt/core/ray.h>
#include <nanopt/core/interaction.h>

namespace nanopt {

class Primitive {
public:
  virtual ~Primitive() = default;
  virtual Bounds3f getBounds() const = 0;
  virtual bool intersect(const Ray& ray) const = 0;
  virtual bool intersect(const Ray& ray, Interaction& isect) const = 0;
};

}
