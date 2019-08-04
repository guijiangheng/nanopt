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
    , p(p), n(n), uv(uv) {

}

Mesh::Mesh(const Matrix4& frame, const Mesh& mesh)
  : shadingMode(mesh.shadingMode)
  , nVertices(mesh.nVertices)
  , nTriangles(mesh.nTriangles) {

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
}

}
