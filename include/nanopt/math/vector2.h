#pragma once

#include <cmath>

namespace nanopt {

template <typename T>
class Vector2 {
public:
  Vector2() = default;

  explicit Vector2(T x) noexcept : x(x), y(x)
  { }

  Vector2(T x, T y) noexcept : x(x), y(y)
  { }

  template <typename U>
  explicit Vector2(const Vector2<U>& v) noexcept : x((T)v.x), y((T)v.y)
  { }

  T& operator[](int index) {
    return (&x)[index];
  }

  T operator[](int index) const {
    return (&x)[index];
  }

  Vector2 operator+(const Vector2& v) const {
    return { x + v.x, y + v.y };
  }

  Vector2& operator+=(const Vector2& v) {
    x += v.x;
    y += v.y;
    return *this;
  }

  Vector2 operator-(const Vector2& v) const {
    return { x - v.x, y - v.y };
  }

  Vector2& operator-=(const Vector2& v) {
    x -= v.x;
    y -= v.y;
    return *this;
  }

  Vector2 operator*(T k) const {
    return { x * k, y * k };
  }

  Vector2& operator*=(T k) {
    x *= k;
    y *= k;
    return *this;
  }

  Vector2 operator/(T k) const {
    k = 1 / k;
    return { x * k, y * k };
  }

  Vector2 operator/=(T k) {
    k = 1 / k;
    x *= k;
    y *= k;
    return *this;
  }

  Vector2 operator-() const {
    return { -x, -y };
  }

  T lengthSquared() const {
    return x * x + y * y;
  }

  T length() const {
    return std::sqrt(lengthSquared());
  }

  Vector2& normalize() {
    *this /= length();
    return *this;
  }

  bool operator==(const Vector2& v) const {
    return x == v.x && y == v.y;
  }

  bool operator!=(const Vector2& v) const {
    return x != v.x || y != v.y;
  }

public:
  T x, y;
};

using Vector2i = Vector2<int>;
using Vector2f = Vector2<float>;

template <typename T>
T dot(const Vector2<T>& a, const Vector2<T>& b) {
  return a.x * b.x + a.y * b.y;
}

template <typename T>
T absdot(const Vector2<T>& a, const Vector2<T>& b) {
  return std::abs(dot(a, b));
}

template <typename T>
Vector2<T> abs(const Vector2<T>& v) {
  return { std::abs(v.x), std::abs(v.y) };
}

template <typename T>
Vector2<T> min(const Vector2<T>& a, const Vector2<T>& b) {
  return {
    std::min(a.x, b.x),
    std::min(a.y, b.y)
  };
}

template <typename T>
Vector2<T> max(const Vector2<T>& a, const Vector2<T>& b) {
  return Vector2<T>(
    std::max(a.x, b.x),
    std::max(a.y, b.y)
  );
}

inline Vector2f lerp(const Vector2f& a, const Vector2f& b, float t) {
  return a + (b - a) * t;
}


inline Vector2f barycentric(
  const Vector2f& a,
  const Vector2f& b,
  const Vector2f& c,
  const Vector2f& uv) {

  return a * (1 - uv.x - uv.y) + b * uv.x + c * uv.y;
}

inline Vector2f normalize(const Vector2f& v) {
  return v / v.length();
}

inline float distance(const Vector2f& a, const Vector2f& b) {
  return (a - b).length();
}

}
