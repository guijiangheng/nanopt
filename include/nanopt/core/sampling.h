#pragma once

#include <nanopt/math/math.h>
#include <nanopt/math/vector3.h>

namespace nanopt {

inline Vector2f uniformSampleTriangle(const Vector2f& u) {
  auto x = std::sqrt(u[0]);
  return Vector2f(1 - x, u[1] * x);
}

inline Vector2f uniformSampleDisk(const Vector2f& u) {
  auto uSquare = u * 2.0f - Vector2f(1);
  if (uSquare.x == 0 && uSquare.y == 0) return Vector2f(0);
  float r, theta;
  if (std::abs(uSquare.x) > std::abs(uSquare.y)) {
    r = uSquare.x;
    theta = PiOver4 * (uSquare.y / uSquare.x);
  } else {
    r = uSquare.y;
    theta = PiOver2 - PiOver4 * (uSquare.x / uSquare.y);
  }
  return Vector2f(std::cos(theta), std::sin(theta)) * r;
}

inline Vector3f consineSampleHemisphere(const Vector2f& u) {
  auto p = uniformSampleDisk(u);
  return Vector3f(p.x, p.y, std::sqrt(std::max(0.0f, 1 - p.x * p.x)));
}

}
