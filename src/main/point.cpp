#include <nanopt/nanopt.h>

using namespace nanopt;

int main() {
  auto material = std::make_unique<MatteMaterial>(Spectrum(1));
  auto mesh = loadMeshOBJ("../scenes/ajax.obj");
  auto triangles = createTriangleMesh(mesh, material.get());
  BVHAccel accel(std::move(triangles));
  Film film(Vector2i(900, 900));

  std::vector<Light*> lights;
  lights.push_back(new PointLight(Vector3f(-20, 40, -20), Spectrum(2992)));
  Scene scene(accel, std::move(lights));

  PerspectiveCamera camera(
    Matrix4::lookAt(
      Vector3f(-65.6055, 47.5762, -24.3583),
      Vector3f(-64.8161, 47.2211, -23.8576),
      Vector3f(0.299858, 0.934836, 0.190177)
    ),
    film,
    Bounds2f(Vector2f(-1, -1), Vector2f(1, 1)),
    30
  );

  RandomSampler sampler(4);
  PathIntegrator integrator(camera, sampler, 1);
  parallelInit();
  integrator.render(scene);
  parallelCleanup();
  film.writeImage("./point.png");

  return 0;
}
