#include <nanopt/nanopt.h>

using namespace nanopt;

int main() {
  Scene scene;

  auto material = std::make_unique<MatteMaterial>(Spectrum(1));
  auto mesh = loadMeshOBJ("../scenes/ajax.obj");
  mesh.shadingMode = ShadingMode::Smooth;
  mesh.material = material.get();
  scene.addMesh(mesh);
  scene.addLight(new PointLight(Vector3f(-20, 40, -20), Spectrum(2992)));
  scene.activate();

  Film film(Vector2i(768, 768));
  PerspectiveCamera camera(
    Matrix4::lookAt(
      Vector3f(-65.6055, 47.5762, -24.3583),
      Vector3f(-64.8161, 47.2211, -23.8576),
      Vector3f(0.299858, 0.934836, 0.190177)
    ),
    film,
    defaultScreenBounds(1),
    30
  );

  RandomSampler sampler(32);
  PathIntegrator integrator(camera, sampler, 1);
  parallelInit();
  integrator.render(scene);
  parallelCleanup();
  film.writeImage("./point.exr");

  return 0;
}
