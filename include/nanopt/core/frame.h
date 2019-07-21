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

float sinTheta(const Vector3f& w);
float cosTheta(const Vector3f& w);
float tanTheta(const Vector3f& w);
float sin2Theta(const Vector3f& w);
float cos2Theta(const Vector3f& w);
float tan2Theta(const Vector3f& w);

float sinPhi(const Vector3f& w);
float cosPhi(const Vector3f& w);
float sin2Phi(const Vector3f& w);
float cos2Phi(const Vector3f& w);

inline float sinTheta(const Vector3f& w) {
  return std::sqrt(sin2Theta(w));
}

inline float sin2Theta(const Vector3f& w) {
  return std::max(0.0f, 1 - cos2Theta(w));
}

inline float cosTheta(const Vector3f& w) {
  return w.z;
}

inline float cos2Theta(const Vector3f& w) {
  return w.z * w.z;
}

inline float absCosTheta(const Vector3f& w) {
  return std::abs(w.z);
}

inline float tanTheta(const Vector3f& w) {
  return sinTheta(w) / cosTheta(w);
}

inline float tan2Theta(const Vector3f& w) {
  return sin2Theta(w) / cos2Theta(w);
}

inline float sinPhi(const Vector3f& w) {
  auto sinTheta = nanopt::sinTheta(w);
  return sinTheta == 0 ? 0 : clamp(w.y / sinTheta, -1.0f, 1.0f);
}

inline float sin2Phi(const Vector3f& w) {
  return sinPhi(w) * sinPhi(w);
}

inline float cosPhi(const Vector3f& w) {
  auto sinTheta = nanopt::sinTheta(w);
  return sinTheta == 0 ? 0 : clamp(w.x / sinTheta, -1.0f, 1.0f);
}

inline float cos2Phi(const Vector3f& w) {
  return cosPhi(w) * cosPhi(w);
}

inline bool sameHemisphere(const Vector3f& wo, const Vector3f& wi) {
  return wo.z * wi.z > 0;
}

inline Vector3f sphericalDirection(float sinTheta, float cosTheta, float phi) {
  return Vector3f(sinTheta * std::cos(phi), sinTheta * std::sin(phi), cosTheta);
}

}
