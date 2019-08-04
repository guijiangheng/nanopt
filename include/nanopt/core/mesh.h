#pragma once

#include <memory>
#include <nanopt/math/matrix4.h>

namespace nanopt {

enum class ShadingMode {
  Flat,
  Smooth
};

class Mesh {
public:
  Mesh(
    ShadingMode shadingMode,
    int nVertices,
    int nTriangles,
    int* indices,
    Vector3f* p,
    Vector3f* n,
    Vector2f* uv) noexcept;

  Mesh(const Matrix4& frame, const Mesh& mesh);

public:
  ShadingMode shadingMode;
  const int nVertices;
  const int nTriangles;
  std::unique_ptr<int[]> indices;
  std::unique_ptr<Vector3f[]> p;
  std::unique_ptr<Vector3f[]> n;
  std::unique_ptr<Vector2f[]> uv;
};

}
