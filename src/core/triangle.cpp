#include <nanopt/core/triangle.h>

namespace nanopt {

bool Triangle::intersect(const Ray& ray) const {
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

  return true;
}

// ref https://cadxfem.org/inf/Fast%20MinimumStorage%20RayTriangle%20Intersection.pdf
bool Triangle::intersect(const Ray& ray, Interaction& isect) const {
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
  ray.tMax = dist;
  isect.p = a + e1 * u + e2 * v;
  isect.wo = -ray.d;
  isect.n = getNormal(st);
  isect.uv = getUv(st);

  return true;
}

}
