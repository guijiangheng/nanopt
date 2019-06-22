#pragma once

#include <memory>
#include <nanopt/core/primitive.h>

namespace nanopt {

enum class ShadingMode {
  Flat,
  Smooth
};

struct Mesh {
  Mesh(
    ShadingMode shadingMode,
    int nVertices, int nTriangles,
    int* indices, Vector3f* p, Vector3f* n, Vector2f* uv) noexcept :
      shadingMode(shadingMode),
      nVertices(nVertices), nTriangles(nTriangles),
      indices(indices), p(p), n(n), uv(uv)
  { }

  ShadingMode shadingMode;
  const int nVertices;
  const int nTriangles;
  std::unique_ptr<int[]> indices;
  std::unique_ptr<Vector3f[]> p;
  std::unique_ptr<Vector3f[]> n;
  std::unique_ptr<Vector2f[]> uv;
};

class Triangle : public  Primitive {
public:
  Triangle(const Mesh& mesh, int triangleIndex)
    : mesh(mesh)
    , indices(mesh.indices.get() + triangleIndex * 3)
  { }

  Vector2f getUv(const Vector2f& st) const {
    if (!mesh.uv) return st;
    auto& a = mesh.uv[indices[0]];
    auto& b = mesh.uv[indices[1]];
    auto& c = mesh.uv[indices[2]];
    return a * (1 - st.x - st.y) + b * st.x + c * st.y;
  }

  Vector3f getNormal(const Vector2f& st) const {
    auto& a = mesh.p[indices[0]];
    auto& b = mesh.p[indices[1]];
    auto& c = mesh.p[indices[2]];
    if (!mesh.uv || mesh.shadingMode == ShadingMode::Flat) {
      auto ab = b - a;
      auto ac = c - a;
      return normalize(cross(ab, ac));
    }
    return normalize(a * (1 - st.x - st.y) + b * st.x + c * st.y);
  }

  Bounds3f getBounds() const override {
    auto& a = mesh.p[indices[0]];
    auto& b = mesh.p[indices[1]];
    auto& c = mesh.p[indices[2]];
    return merge(Bounds3(a, b), c);
  }

  bool intersect(const Ray& ray) const override;

  bool intersect(const Ray& ray, Interaction& isect) const override;

private:
  const Mesh& mesh;
  const int* indices;
};

}
