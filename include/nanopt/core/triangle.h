#pragma once

#include <memory>
#include <nanopt/math/bounds3.h>
#include <nanopt/core/interaction.h>

namespace nanopt {

enum class ShadingMode {
  Flat,
  Smooth
};

struct Mesh {
  Mesh(
    ShadingMode shadingMode,
    int nVertices, int nTriangles,
    int* indices, Vector3f* p, Vector3f* n, Vector2f* uv
  ) noexcept :
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

class Triangle {
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

  Bounds3f getBounds() const {
    auto& a = mesh.p[indices[0]];
    auto& b = mesh.p[indices[1]];
    auto& c = mesh.p[indices[2]];
    return merge(Bounds3(a, b), c);
  }

  // ref https://cadxfem.org/inf/Fast%20MinimumStorage%20RayTriangle%20Intersection.pdf
  bool intersect(const Ray& ray, float& tHit, Interaction& isect) const {
    auto& b = mesh.p[indices[1]];
    auto& a = mesh.p[indices[0]];
    auto& c = mesh.p[indices[2]];

    auto e1 = b - a;
    auto e2 = c - a;
    auto p = cross(ray.d, e2);
    auto det = dot(p, e1);
    if (std::abs(det) < 0.000001f) return false;

    auto t = ray.o - a;
    auto detInv = 1 / det;
    auto u = dot(p, t) * detInv;
    if (u < 0 || u > 1) return false;

    auto q = cross(t, e1);
    auto v = dot(q, ray.d) * detInv;
    if (v < 0 || u + v > 1) return false;

    auto dist = dot(q, e2) * detInv;
    if (dist <= 0 || dist > ray.tMax) return false;

    auto st = Vector2f(u, v);
    tHit = dist;
    isect.p = a + e1 * u + e2 * v;
    isect.wo = -ray.d;
    isect.n = getNormal(st);
    isect.uv = getUv(st);
  }

private:
  const Mesh& mesh;
  const int* indices;
};

}
