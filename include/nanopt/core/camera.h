#pragma once

#include <nanopt/math/matrix4.h>
#include <nanopt/math/bounds2.h>
#include <nanopt/core/film.h>

namespace nanopt {

struct CameraSample {
  Vector2f pFilm;
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

class ProjectiveCamera : public Camera {
public:
  ProjectiveCamera(
      const Matrix4& frame,
      Film& film,
      const Matrix4& cameraToScreen,
      const Bounds2f& screenWindow) noexcept : Camera(frame, film) {
    auto screenToRaster =
      Matrix4::scale(film.resolution.x, film.resolution.y, 1) *
      Matrix4::scale(
        1 / (screenWindow.pMax.x - screenWindow.pMin.x),
        1 / (screenWindow.pMin.y - screenWindow.pMax.y), 1) *
      Matrix4::translate(-screenWindow.pMin.x, -screenWindow.pMax.y, 0);
    auto cameraToRaster = screenToRaster * cameraToScreen;
    rasterToCamera = inverse(cameraToRaster);
  }

public:
  Matrix4 rasterToCamera;
};

}
