#pragma once

#include <memory>
#include <nanopt/math/matrix4.h>
#include <nanopt/math/bounds3.h>
#include <nanopt/core/sampling.h>
#include <nanopt/core/distrib.h>
#include <nanopt/core/material.h>

namespace nanopt {

class DiffuseAreaLight;

enum class ShadingMode {
  Flat,
  Smooth
};

class Mesh {
public:
  Mesh(
    ShadingMode shadingMode,
    int nVertices,
    int nTriangles,
    int* indices,
    Vector3f* p,
    Vector3f* n,
    Vector2f* uv) noexcept;

  Mesh(const Matrix4& frame, const Mesh& mesh);

  bool isLight() const {
    return light == nullptr;
  }

  float getArea(int index) const {
    auto& a = p[indices[3 * index]];
    auto& b = p[indices[3 * index + 1]];
    auto& c = p[indices[3 * index + 2]];
    return cross(b - a, b - c).length() / 2;
  }

  Bounds3f getBounds(int index) const {
    auto& a = p[indices[3 * index]];
    auto& b = p[indices[3 * index + 1]];
    auto& c = p[indices[3 * index + 2]];
    return merge(Bounds3(a, b), c);
  }

  Interaction sample(int index, const Vector2f& u, float& pdf) const;

  Interaction sample(const Interaction& ref, Vector2f& u, float& pdf) const;

  void computeIntersection(int index, Interaction& isect) const;

  bool intersect(int index, const Ray& ray) const;

  bool intersect(int index, const Ray& ray, Interaction& isect) const;

public:
  ShadingMode shadingMode;
  const int nVertices;
  const int nTriangles;
  std::unique_ptr<int[]> indices;
  std::unique_ptr<Vector3f[]> p;
  std::unique_ptr<Vector3f[]> n;
  std::unique_ptr<Vector2f[]> uv;
  Material* material;
  DiffuseAreaLight* light;
  Distribution1D discretePdf;
};

}
