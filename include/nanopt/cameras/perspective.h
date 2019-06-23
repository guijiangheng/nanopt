#pragma once

#include <nanopt/core/camera.h>

namespace nanopt {

class PerspectiveCamera : public ProjectiveCamera {
public:
  PerspectiveCamera(
    const Matrix4& frame, Film& film,
    const Bounds2f screenWindow,
    float fov) noexcept
      : ProjectiveCamera(
        frame, film,
        Matrix4::perspective(fov, 0.01f, 1000.0f),
        screenWindow)
  { }

  Ray generateRay(const CameraSample& sample) const override {
    Vector3f pFilm(sample.pFilm);
    auto pCamera = rasterToCamera.applyP(pFilm);
    Ray ray(Vector3f(0), normalize(pCamera));
    return frame(ray);
  }
};

inline Bounds2f defaultScreenBounds(float aspectRatio) {
  return Bounds2f(
    Vector2f(-aspectRatio, -1.0f),
    Vector2f( aspectRatio,  1.0f)
  );
}

}
