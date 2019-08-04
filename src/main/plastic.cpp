#include <nanopt/nanopt.h>

using namespace nanopt;

int main() {
  Scene scene;

  auto material = std::make_unique<PlasticMaterial>(Spectrum(0.2f, 0.2f, 0.4f), Spectrum(0.6f), 0.28f, false);
  auto ajaxMesh = loadMeshOBJ("../scenes/ajax.obj");
  ajaxMesh.shadingMode = ShadingMode::Smooth;
  ajaxMesh.material = material.get();
  scene.addMesh(ajaxMesh);

  auto lightMesh = loadMeshOBJ("../scenes/light.obj");
  auto light = new DiffuseAreaLight(&lightMesh, Spectrum(20), true);
  scene.addLight(light);
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

  RandomSampler sampler(64);
  PathIntegrator integrator(camera, sampler);
  parallelInit();
  integrator.render(scene);
  parallelCleanup();
  film.writeImage("plastic.png");

  return 0;
}
