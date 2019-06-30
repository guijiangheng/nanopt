#pragma once

#include <nanopt/math/vector3.h>

namespace nanopt {

class Spectrum {
public:
  Spectrum() = default;

  explicit Spectrum(float v) noexcept : e { v, v, v }
  { }

  Spectrum(float x, float y, float z) noexcept : e { x, y, z }
  { }

  explicit Spectrum(const Vector3f& v) noexcept : e { v.x, v.y, v.z }
  { }

  Spectrum operator+(const Spectrum& rhs) const {
    return { e[0] + rhs[0], e[1] + rhs[1], e[2] + rhs[2] };
  }

  Spectrum& operator+=(const Spectrum& rhs) {
    e[0] += rhs[0];
    e[1] += rhs[1];
    e[2] += rhs[2];
    return *this;
  }

  Spectrum operator-(const Spectrum& rhs) const {
    return { e[0] - rhs[0], e[1] - rhs[1], e[2] - rhs[2] };
  }

  Spectrum& operator-=(const Spectrum& rhs) {
    e[0] -= rhs[0];
    e[1] -= rhs[1];
    e[2] -= rhs[2];
    return *this;
  }

  Spectrum operator*(float k) const {
    return { e[0] * k, e[1] * k, e[2] * k };
  }

  Spectrum& operator*=(float k) {
    e[0] *= k;
    e[1] *= k;
    e[2] *= k;
    return *this;
  }

  Spectrum operator*(const Spectrum& rhs) const {
    return { e[0] * rhs[0], e[1] * rhs[1], e[2] * rhs[2] };
  }

  Spectrum& operator*=(const Spectrum& rhs) {
    e[0] *= rhs[0];
    e[1] *= rhs[1];
    e[2] *= rhs[2];
    return *this;
  }

  Spectrum operator/(float k) const {
    k = 1 / k;
    return { e[0] * k, e[1] * k, e[2] * k };
  }

  Spectrum& operator/=(float k) {
    k = 1 / k;
    e[0] *= k;
    e[1] *= k;
    e[2] *= k;
    return *this;
  }

  float operator[](int index) const {
    return e[index];
  }

  float& operator[](int index) {
    return e[index];
  }

  Spectrum toRGB() const {
    return {
       3.240479f * e[0] - 1.537150f * e[1] - 0.498535f * e[2],
      -0.969256f * e[0] + 1.875991f * e[1] + 0.041556f * e[2],
       0.055648f * e[0] - 0.204043f * e[1] + 1.057311f * e[2]
    };
  }

  Spectrum toXYZ() const {
    return {
      0.412453f * e[0] + 0.357580f * e[1] + 0.180423f * e[2],
      0.212671f * e[0] + 0.715160f * e[1] + 0.072169f * e[2],
      0.019334f * e[0] + 0.119193f * e[1] + 0.950227f * e[2]
    };
  }

  float y() const {
    return e[0] * 0.212671f + e[1] * 0.715160f + e[2] * 0.072169f;
  }

  float maxComponent() const {
    if (e[0] > e[1]) {
      return e[0] > e[2] ? e[0] : e[2];
    } else {
      return e[1] > e[2] ? e[1] : e[2];
    }
  }

  bool isBlack() const {
    return e[0] == 0 && e[1] == 0 && e[2] == 0;
  }

  bool operator==(const Spectrum& rhs) const {
    return e[0] == rhs.e[0] && e[1] == rhs.e[1] && e[2] == rhs.e[2];
  }

  bool operator!=(const Spectrum& rhs) const {
    return e[0] != rhs.e[0] || e[1] != rhs.e[1] || e[2] != rhs.e[2];
  }

public:
  float e[3];
};

inline Spectrum lerp(const Spectrum& a, const Spectrum& b, float t) {
  return a + (b - a) * t;
}

inline float gammaCorrect(float v) {
  if (v <= 0.0031308f) return 12.92f * v;
  return 1.055f * std::pow(v, (1.f / 2.4f)) - 0.055f;
}

inline Spectrum gammaCorrect(const Spectrum& s) {
  return {
    gammaCorrect(s[0]),
    gammaCorrect(s[1]),
    gammaCorrect(s[2])
  };
}

inline float inverseGammaCorrect(float v) {
  if (v <= 0.04045f) return v / 12.92f;
  return std::pow((v + 0.055f) / 1.055f, 2.4f);
}

inline Spectrum inverseGammaCorrect(const Spectrum& s) {
  return {
    inverseGammaCorrect(s[0]),
    inverseGammaCorrect(s[1]),
    inverseGammaCorrect(s[2])
  };
}

}
