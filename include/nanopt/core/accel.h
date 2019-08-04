#pragma once

#include <algorithm>
#include <nanopt/core/mesh.h>

namespace nanopt {

enum class BuildMethod { SAH, HLBVH };

class Accelerator {
public:
  Accelerator() noexcept {
    primOffset.push_back(0);
  }

  virtual ~Accelerator() = default;

  void addMesh(const Mesh& mesh) {
    meshs.push_back(&mesh);
    primOffset.push_back(primOffset.back() + mesh.nTriangles);
  }

  int getPrimitiveCount() const {
    return primOffset.back();
  }

  const Mesh* findMesh(int& index) const {
    auto itr = std::lower_bound(primOffset.begin(), primOffset.end(), index) - 1;
    index -= *itr;
    return meshs[itr - primOffset.begin()];
  }

  Bounds3f getBounds(int index) const {
    auto mesh = findMesh(index);
    return mesh->getBounds(index);
  }

  virtual void build(BuildMethod method = BuildMethod::SAH) = 0;

  virtual Bounds3f getBounds() const = 0;

  virtual bool intersect(const Ray& ray) const = 0;

  virtual bool intersect(const Ray& ray, Interaction& isect) const = 0;

protected:
  std::vector<int> primOffset;
  std::vector<int> primIndices;
  std::vector<const Mesh*> meshs;
};

}
