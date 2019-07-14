#pragma once

#include <vector>
#include <nanopt/core/mesh.h>
#include <nanopt/core/sampling.h>
#include <nanopt/core/primitive.h>
#include <nanopt/core/material.h>

namespace nanopt {

class DiffuseAreaLight;

class Triangle : public  Primitive {
public:
  Triangle(
    const Mesh& mesh, int triangleIndex, Material* material = nullptr)
      : mesh(mesh)
      , indices(mesh.indices.get() + triangleIndex * 3)
      , material(material)
      , light(nullptr)
  { }

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
    auto& a = mesh.p[indices[0]];
    auto& b = mesh.p[indices[1]];
    auto& c = mesh.p[indices[2]];
    auto p = barycentric(a, b, c, isect.uv);
    auto n = normalize(cross(c - a, b - a));
    return (p - ref.p).lengthSquared() / (absdot(n, w) * area());
  }

  Interaction sample(const Vector2f& u, float& pdf) const {
    auto uv = uniformSampleTriangle(u);
    auto& a = mesh.p[indices[0]];
    auto& b = mesh.p[indices[1]];
    auto& c = mesh.p[indices[2]];
    Interaction isect;
    isect.p = barycentric(a, b, c, uv);
    isect.n = normalize(cross(c - a, b - a));
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

  void computeIntersection(Interaction& isect) const;

  bool intersect(const Ray& ray) const override;

  bool intersect(const Ray& ray, Interaction& isect) const override;

public:
  const Mesh& mesh;
  const int* indices;
  Material* material;
  DiffuseAreaLight* light;
};

inline std::vector<Triangle*> createTriangleMesh(const Mesh& mesh, Material* material = nullptr) {
  std::vector<Triangle*> triangles;
  triangles.reserve(mesh.nTriangles);
  for (auto i = 0; i < mesh.nTriangles; ++i)
    triangles.push_back(new Triangle(mesh, i, material));
  return triangles;
}

}
