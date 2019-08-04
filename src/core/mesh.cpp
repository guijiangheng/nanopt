#include <cstring>
#include <nanopt/core/mesh.h>

namespace nanopt {

Mesh::Mesh(
  ShadingMode shadingMode,
  int nVertices,
  int nTriangles,
  int* indices,
  Vector3f* p,
  Vector3f* n,
  Vector2f* uv) noexcept
    : shadingMode(shadingMode)
    , nVertices(nVertices)
    , nTriangles(nTriangles)
    , indices(indices)
    , p(p), n(n), uv(uv)
    , material(nullptr), light(nullptr)
    , discretePdf(nTriangles) {

  for (auto i = 0; i < nTriangles; ++i)
    discretePdf.append(getArea(i));
  discretePdf.normalize();
}

Mesh::Mesh(const Matrix4& frame, const Mesh& mesh)
  : shadingMode(mesh.shadingMode)
  , nVertices(mesh.nVertices)
  , nTriangles(mesh.nTriangles)
  , material(mesh.material)
  , light(mesh.light)
  , discretePdf(mesh.nTriangles) {

  indices.reset(new int[nTriangles * 3]);
  memcpy(indices.get(), mesh.indices.get(), sizeof(int) * nTriangles * 3);

  p.reset(new Vector3f[nVertices]);
  for (auto i = 0; i < nVertices; ++i) {
    p[i] = frame.applyP(mesh.p[i]);
  }

  if (mesh.n) {
    n.reset(new Vector3f[nVertices]);
    for (auto i = 0; i < nVertices; ++i)
      n[i] = normalize(frame.applyN(mesh.n[i]));
  }

  if (mesh.uv) {
    uv.reset(new Vector2f[nVertices]);
    memcpy(uv.get(), mesh.uv.get(), sizeof(Vector2f) * nVertices);
  }

  for (auto i = 0; i < nTriangles; ++i)
    discretePdf.append(getArea(i));
  discretePdf.normalize();
}

Interaction Mesh::sample(int index, const Vector2f& u, float& pdf) const {
  auto uv = uniformSampleTriangle(u);
  auto& a = p[indices[3 * index]];
  auto& b = p[indices[3 * index + 1]];
  auto& c = p[indices[3 * index + 2]];
  Interaction isect;
  isect.p = barycentric(a, b, c, uv);
  isect.n = cross(c - a, b - a);
  pdf = 2 / isect.n.length();
  isect.n.normalize();
  return isect;
}

Interaction Mesh::sample(const Interaction& ref, Vector2f& u, float& pdf) const {
  auto triIndex = discretePdf.sample(u[0], pdf);
  float triPdf;
  auto pLight = sample(triIndex, u, triPdf);
  pdf *= triPdf;
  auto w = pLight.p - ref.p;
  auto lengthSquared = w.lengthSquared();
  pdf *= lengthSquared * std::sqrt(lengthSquared) / absdot(pLight.n, w);
  return pLight;
}

bool Mesh::intersect(int index, const Ray& ray) const {
  auto& a = p[indices[3 * index]];
  auto& b = p[indices[3 * index + 1]];
  auto& c = p[indices[3 * index + 2]];

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

  return true;
}

// ref https://cadxfem.org/inf/Fast%20MinimumStorage%20RayTriangle%20Intersection.pdf
bool Mesh::intersect(int index, const Ray& ray, Interaction& isect) const {
  auto& a = p[indices[3 * index]];
  auto& b = p[indices[3 * index + 1]];
  auto& c = p[indices[3 * index + 2]];

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

  ray.tMax = dist;
  isect.uv = Vector2f(u, v);

  return true;
}

void Mesh::computeIntersection(int index, Interaction& isect) const {
  auto ia = indices[3 * index];
  auto ib = indices[3 * index + 1];
  auto ic = indices[3 * index + 2];

  auto& a = p[ia];
  auto& b = p[ib];
  auto& c = p[ic];
  isect.p = barycentric(a, b, c, isect.uv);
  isect.n = normalize(cross(c - a, b - a));

  if (!n || shadingMode == ShadingMode::Flat) {
    isect.ns = isect.n;
  } else {
    isect.ns = normalize(barycentric(n[ia], n[ib], n[ic], isect.uv));
  }

  if (uv) {
    isect.uv = barycentric(uv[ia], uv[ib], uv[ic], isect.uv);
  }
}

}
