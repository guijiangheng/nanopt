#pragma once

#include <nanopt/math/vector2.h>
#include <nanopt/math/matrix4.h>
#include <nanopt/core/film.h>

namespace nanopt {

struct CameraSample {
  Vector2f pFilm;
  Vector2f pLens;
};

class Camera {
public:
  Camera(const Matrix4& frame, Film& film) noexcept
    : frame(frame), film(film)
  { }

  virtual ~Camera() = default;

  virtual Ray generateRay(const CameraSample& sample) const = 0;

public:
  Matrix4 frame;
  Film& film;
};

}
