#include <nanopt/nanopt.h>

using namespace nanopt;

int main() {
  std::vector<Light*> lights;
  std::vector<Triangle*> triangles;

  auto material = std::make_unique<PlasticMaterial>(Spectrum(0.2f, 0.2f, 0.4f), Spectrum(0.6f), 0.28f, false);
  auto ajaxMesh = loadMeshOBJ("../scenes/ajax.obj");
  ajaxMesh.shadingMode = ShadingMode::Smooth;
  auto ajaxTriangles = createTriangleMesh(ajaxMesh, material.get());
  triangles.insert(triangles.begin(), ajaxTriangles.begin(), ajaxTriangles.end());

  auto lightMesh = loadMeshOBJ("../scenes/light.obj");
  auto ligthTriangles = createTriangleMesh(lightMesh);
  for (auto triangle : ligthTriangles)
    lights.push_back(new DiffuseAreaLight(triangle, Spectrum(20), true));
  triangles.insert(triangles.begin(), ligthTriangles.begin(), ligthTriangles.end());

  BVHAccel accel(std::move(triangles));
  Film film(Vector2i(768, 768));
  Scene scene(accel, std::move(lights));

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
