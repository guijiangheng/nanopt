#pragma once

#include <nanopt/math/vector3.h>

namespace nanopt {

class Frame {
public:
  explicit Frame(const Vector3f& n) noexcept : n(n) {
    coordinateSystem(n, s, t);
  }

  Vector3f toLocal(const Vector3f& p) const {
    return Vector3f(dot(p, s), dot(p, t), dot(p, n));
  }

  Vector3f toWorld(const Vector3f& p) const {
    return s * p.x + t * p.y + n * p.z;
  }

public:
  Vector3f n, s, t;
};

inline float cosTheta(const Vector3f& w) {
  return w.z;
}

inline float absCosTheta(const Vector3f& w) {
  return std::abs(w.z);
}

inline bool sameHemisphere(const Vector3f& wo, const Vector3f& wi) {
  return wo.z * wi.z > 0;
}

}
