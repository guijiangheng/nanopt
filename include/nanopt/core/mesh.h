#pragma once

#include <memory>
#include <cstring>
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
    int nVertices, int nTriangles,
    int* indices, Vector3f* p, Vector3f* n, Vector2f* uv) noexcept
      : shadingMode(shadingMode)
      , nVertices(nVertices)
      , nTriangles(nTriangles)
      , indices(indices)
      , p(p), n(n), uv(uv)
  { }

  Mesh(const Matrix4& frame, const Mesh& m)
    : shadingMode(m.shadingMode)
    , nVertices(m.nVertices)
    , nTriangles(m.nTriangles) {

    indices.reset(new int[nTriangles * 3]);
    memcpy(indices.get(), m.indices.get(), sizeof(int) * nTriangles * 3);

    p.reset(new Vector3f[nVertices]);
    for (auto i = 0; i < nVertices; ++i) {
      p[i] = frame.applyP(m.p[i]);
    }

    if (m.uv) {
      uv.reset(new Vector2f[nVertices]);
      memcpy(uv.get(), m.uv.get(), sizeof(Vector2f) * nVertices);
    }
  }

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
