#pragma once

#include <vector>
#include <nanopt/accelerators/bvh.h>
#include <nanopt/core/light.h>
#include <nanopt/lights/infinite.h>

namespace nanopt {

class Scene {
public:
  Scene() noexcept : accel(new BVHAccel()), infiniteLight(nullptr)
  { }

  ~Scene() noexcept {
    for (auto light : lights)
      delete light;
    delete accel;
  }

  void addLight(Light* light) {
    lights.push_back(light);
    if (light->isInfinite())
      infiniteLight = (InfiniteAreaLight*)light;
  }

  void addMesh(const Mesh& mesh) {
    accel->addMesh(mesh);
    if (mesh.isLight()) lights.push_back((Light*)mesh.light);
  }

  void activate(BuildMethod method = BuildMethod::SAH) {
    accel->build(method);
  }

  bool intersect(const Ray& ray, Interaction& isect) const {
    return accel->intersect(ray, isect);
  }

  bool intersect(const Ray& ray) const {
    return accel->intersect(ray);
  }

public:
  Accelerator* accel;
  InfiniteAreaLight* infiniteLight;
  std::vector<Light*> lights;
};

}
