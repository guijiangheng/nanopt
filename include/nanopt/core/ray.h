#pragma once

#include <nanopt/math/math.h>
#include <nanopt/math/vector3.h>

namespace nanopt {

class Ray {
public:
  Ray(
    const Vector3f& o,
    const Vector3f& d,
    float tMax = Infinity) noexcept : o(o), d(d), tMax(tMax)
  { }

  Vector3f operator()(float t) const {
    return o + d * t;
  }

public:
  Vector3f o;
  Vector3f d;
  mutable float tMax;
};

}
