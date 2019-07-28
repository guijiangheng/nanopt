#pragma once

#include <cmath>
#include <algorithm>
#include <nanopt/math/vector2.h>

namespace nanopt {

template <typename T>
class Vector3 {
public:
  Vector3() = default;

  explicit Vector3(T x) noexcept : x(x), y(x), z(x)
  { }

  explicit Vector3(const Vector2<T>& v) noexcept : x(v.x), y(v.y), z(0)
  { }

  Vector3(T x, T y, T z) noexcept : x(x), y(y), z(z)
  { }

  T& operator[](int index) {
    return (&x)[index];
  }

  T operator[](int index) const {
    return (&x)[index];
  }

  Vector3 operator+(const Vector3& v) const {
    return { x + v.x, y + v.y, z + v.z };
  }

  Vector3& operator+=(const Vector3& v) {
    x += v.x;
    y += v.y;
    z += v.z;
    return *this;
  }

  Vector3 operator-(const Vector3& v) const {
    return { x - v.x, y - v.y, z - v.z };
  }

  Vector3& operator-=(const Vector3& v) {
    x -= v.x;
    y -= v.y;
    z -= v.z;
    return *this;
  }

  Vector3 operator*(T k) const {
    return { x * k, y * k, z * k };
  }

  Vector3& operator*=(T k) {
    x *= k;
    y *= k;
    z *= k;
    return *this;
  }

  Vector3 operator*(const Vector3& v) const {
    return { x * v.x, y * v.y, z * v.z };
  }

  Vector3& operator*=(const Vector3& v) {
    x *= v.x;
    y *= v.y;
    z *= v.z;
    return *this;
  }

  Vector3 operator/(T k) const {
    k = 1 / k;
    return { x * k, y * k, z * k };
  }

  Vector3 operator/=(T k) {
    k = 1 / k;
    x *= k;
    y *= k;
    z *= k;
    return *this;
  }

  Vector3 operator/(const Vector3& v) const {
    return { x / v.x, y / v.y, z / v.z };
  }

  Vector3& operator/=(const Vector3& v) {
    x /= v.x;
    y /= v.y;
    z /= v.z;
    return *this;
  }

  Vector3 operator-() const {
    return { -x, -y, -z };
  }

  T lengthSquared() const {
    return x * x + y * y + z * z;
  }

  T length() const {
    return std::sqrt(lengthSquared());
  }

  T maxComponent() const {
    return x > y ? (x > z ? x : z) : (y > z ? y : z);
  }

  Vector3& normalize() {
    *this /= length();
    return *this;
  }

  bool operator==(const Vector3& v) const {
    return x == v.x && y == v.y && z == v.z;
  }

  bool operator!=(const Vector3& v) const {
    return x != v.x || y != v.y || z != v.z;
  }

public:
  T x, y, z;
};

using Vector3i = Vector3<int>;
using Vector3f = Vector3<float>;

template <typename T>
T dot(const Vector3<T>& a, const Vector3<T>& b) {
  return a.x * b.x + a.y * b.y + a.z * b.z;
}

template <typename T>
T absdot(const Vector3<T>& a, const Vector3<T>& b) {
  return std::abs(dot(a, b));
}

template <typename T>
Vector3<T> cross(const Vector3<T>& a, const Vector3<T>& b) {
  return {
    a.y * b.z - a.z * b.y,
    a.z * b.x - a.x * b.z,
    a.x * b.y - a.y * b.x
  };
}

template <typename T>
Vector3<T> abs(const Vector3<T>& v) {
  return { std::abs(v.x), std::abs(v.y), std::abs(v.z) };
}

template <typename T>
Vector3<T> min(const Vector3<T>& a, const Vector3<T>& b) {
  return {
    std::min(a.x, b.x),
    std::min(a.y, b.y),
    std::min(a.z, b.z)
  };
}

template <typename T>
Vector3<T> max(const Vector3<T>& a, const Vector3<T>& b) {
  return Vector3<T>(
    std::max(a.x, b.x),
    std::max(a.y, b.y),
    std::max(a.z, b.z)
  );
}

inline Vector3f lerp(const Vector3f& a, const Vector3f& b, float t) {
  return a + (b - a) * t;
}

inline Vector3f barycentric(
  const Vector3f& a,
  const Vector3f& b,
  const Vector3f& c,
  const Vector2f& uv) {

  return a * (1 - uv.x - uv.y) + b * uv.x + c * uv.y;
}

inline Vector3f faceForward(const Vector3f& n, const Vector3f& v) {
  return dot(n, v) > 0 ? n : -n;
}

inline Vector3f reflect(const Vector3f& w, const Vector3f& n) {
  return -w + n * dot(w, n) * 2;
}

inline Vector3f refract(const Vector3f& wi, const Vector3f& n, float eta) {
  eta = 1 / eta;
  auto cosThetaI = wi.z * n.z;
  auto sin2ThetaI = std::max(0.0f, 1 - cosThetaI * cosThetaI);
  auto sin2ThetaT = sin2ThetaI * eta * eta;
  auto cosThetaT = std::sqrt(std::max(0.0f, 1 - sin2ThetaT));
  return Vector3f(-wi.x * eta, -wi.y * eta, -wi.z * eta + (eta * cosThetaI - cosThetaT) * n.z);
}

inline Vector3f normalize(const Vector3f& v) {
  return v / v.length();
}

inline float distance(const Vector3f& a, const Vector3f& b) {
  return (a - b).length();
}

inline void coordinateSystem(const Vector3f& a, Vector3f& b, Vector3f& c) {
  b = std::abs(a.x) > std::abs(a.y) ?
    Vector3f(-a.z, 0, a.x) / std::sqrt(a.x * a.x + a.z * a.z) :
    Vector3f(0, -a.z, a.y) / std::sqrt(a.y * a.y + a.z * a.z);
  c = cross(a, b);
}

}
