#pragma once

#include <nanopt/core/ray.h>

namespace nanopt {

class Interaction {
public:
  Interaction() = default;

  Vector3f offsetRayOrigin(const Vector3f& w) const {
    return p + faceForward(n, w) * RayOriginOffsetEpsilon;
  }

  Ray spawnRay(const Vector3f& w) const {
    return Ray(offsetRayOrigin(w), w);
  }

  Ray spawnRayTo(const Vector3f& target) const {
    auto w = target - p;
    auto o = offsetRayOrigin(w);
    return Ray(o, w, 1.0f - ShadowEpsilon);
  }

public:
  Vector3f p;
  Vector3f n;
  Vector2f uv;
  Vector3f wo;
  static constexpr auto ShadowEpsilon = 0.0001f;
  static constexpr auto RayOriginOffsetEpsilon = 0.00001f;
};

}
