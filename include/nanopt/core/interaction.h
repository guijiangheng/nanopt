#pragma once

#include <nanopt/core/ray.h>
#include <nanopt/core/spectrum.h>

namespace nanopt {

class BSDF;
class Triangle;

class Interaction {
public:
  Interaction() noexcept : bsdf(nullptr)
  { }

  ~Interaction() noexcept;

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

  Spectrum le(const Vector3f& w) const;

  void computeScatteringFunctions();

public:
  Vector3f p;
  Vector3f n;
  Vector2f uv;
  Vector3f wo;
  BSDF* bsdf;
  Triangle* triangle;
  static constexpr auto ShadowEpsilon = 0.0001f;
  static constexpr auto RayOriginOffsetEpsilon = 0.00001f;
};

}
