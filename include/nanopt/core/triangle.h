#pragma once

#include <vector>
#include <nanopt/core/mesh.h>
#include <nanopt/core/sampling.h>
#include <nanopt/core/primitive.h>
#include <nanopt/core/material.h>

namespace nanopt {

class Triangle : public  Primitive {
public:
  Triangle(const Mesh& mesh, int triangleIndex, Material* material = nullptr)
    : mesh(mesh)
    , material(material)
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
    if (!mesh.n || mesh.shadingMode == ShadingMode::Flat) {
      auto& a = mesh.p[indices[0]];
      auto& b = mesh.p[indices[1]];
      auto& c = mesh.p[indices[2]];
      auto e1 = b - a;
      auto e2 = c - a;
      return normalize(cross(e2, e1));
    }
    auto& a = mesh.n[indices[0]];
    auto& b = mesh.n[indices[1]];
    auto& c = mesh.n[indices[2]];
    return normalize(a * (1 - st.x - st.y) + b * st.x + c * st.y);
  }

  Bounds3f getBounds() const override {
    auto& a = mesh.p[indices[0]];
    auto& b = mesh.p[indices[1]];
    auto& c = mesh.p[indices[2]];
    return merge(Bounds3(a, b), c);
  }

  float area() const {
    auto& a = mesh.p[indices[0]];
    auto& b = mesh.p[indices[1]];
    auto& c = mesh.p[indices[2]];
    return cross(b - a, b - c).length() / 2;
  }

  float pdf(const Interaction& ref, const Vector3f& w) const {
    Interaction isect;
    auto ray = ref.spawnRay(w);
    if (!intersect(ray, isect)) return 0;
    return (isect.p - ref.p).lengthSquared() / (absdot(isect.n, w) * area());
  }

  Interaction sample(const Vector2f& u, float& pdf) const {
    auto uv = uniformSampleTriangle(u);
    auto& a = mesh.p[indices[0]];
    auto& b = mesh.p[indices[1]];
    auto& c = mesh.p[indices[2]];
    Interaction isect;
    isect.p = a * uv[0] + b * uv[1] + c * (1 - uv[0] - uv[1]);
    isect.n = getNormal(uv);
    pdf = 1 / area();
    return isect;
  }

  Interaction sample(const Interaction& ref, const Vector2f& u, float& pdf) const {
    auto pLight = sample(u, pdf);
    auto d = pLight.p - ref.p;
    auto w = normalize(d);
    pdf *= d.lengthSquared() / absdot(pLight.n, w);
    return pLight;
  }

  bool intersect(const Ray& ray) const override;

  bool intersect(const Ray& ray, Interaction& isect) const override;

public:
  const Mesh& mesh;
  Material* material;
  const int* indices;
};

inline std::vector<Triangle*> createTriangleMesh(const Mesh& mesh, Material* material = nullptr) {
  std::vector<Triangle*> triangles;
  triangles.reserve(mesh.nTriangles);
  for (auto i = 0; i < mesh.nTriangles; ++i)
    triangles.push_back(new Triangle(mesh, i, material));
  return triangles;
}

}
