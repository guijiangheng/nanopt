#include <nanopt/nanopt.h>

using namespace nanopt;

int main() {
  Scene scene;

  auto sphere = loadMeshOBJ("../scenes/veach_mi/sphere.obj");

  auto sphere1 = Mesh(
    Matrix4::translate(-1.25f, 0, 0) * Matrix4::scale(0.1f, 0.1f, 0.1f),
    sphere
  );
  sphere1.light = new DiffuseAreaLight(&sphere1, Spectrum(100));
  scene.addMesh(sphere1);

  auto sphere2 = Mesh(
    Matrix4::translate(-3.75f, 0, 0) * Matrix4::scale(0.0333f, 0.0333f, 0.0333f),
    sphere
  );
  sphere2.light = new DiffuseAreaLight(&sphere2, Spectrum(901.803f));
  scene.addMesh(sphere2);

  auto sphere3 = Mesh(
    Matrix4::translate(1.25f, 0, 0) * Matrix4::scale(0.3f, 0.3f, 0.3f),
    sphere
  );
  sphere3.light = new DiffuseAreaLight(&sphere3, Spectrum(11.1111f));
  scene.addMesh(sphere3);

  auto sphere4 = Mesh(
    Matrix4::translate(3.75f, 0, 0) * Matrix4::scale(0.9f, 0.9f, 0.9f),
    sphere
  );
  sphere4.light = new DiffuseAreaLight(&sphere4, Spectrum(1.23457f));
  scene.addMesh(sphere4);

  auto sphere5 = Mesh(
    Matrix4::translate(0, 4, -3),
    sphere
  );
  sphere5.light = new DiffuseAreaLight(&sphere5, Spectrum(100.0f));
  scene.addMesh(sphere5);

  // auto plate1Material = std::make_unique<PlasticMaterial>(
  //   Spectrum(0.0175f, 0.0225f, 0.0325f),
  //   Spectrum(0.9675f),
  //   0.005, false
  // );
  // auto plate1 = loadMeshOBJ("../scenes/veach_mi/plate1.obj");
  // plate1.material = plate1Material.get();
  // scene.addMesh(plate1);

  // auto plate2Material = std::make_unique<PlasticMaterial>(
  //   Spectrum(0.0175f, 0.0225f, 0.0325f),
  //   Spectrum(0.9675f),
  //   0.02, false
  // );
  // auto plate2 = loadMeshOBJ("../scenes/veach_mi/plate2.obj");
  // plate2.material = plate2Material.get();
  // scene.addMesh(plate2);

  // auto plate3Material = std::make_unique<PlasticMaterial>(
  //   Spectrum(0.0175f, 0.0225f, 0.0325f),
  //   Spectrum(0.9675f),
  //   0.05, false
  // );
  // auto plate3 = loadMeshOBJ("../scenes/veach_mi/plate3.obj");
  // plate3.material = plate3Material.get();
  // scene.addMesh(plate3);

  // auto plate4Material = std::make_unique<PlasticMaterial>(
  //   Spectrum(0.0175f, 0.0225f, 0.0325f),
  //   Spectrum(0.9675f),
  //   0.1, false
  // );
  // auto plate4 = loadMeshOBJ("../scenes/veach_mi/plate4.obj");
  // plate4.material = plate4Material.get();
  // scene.addMesh(plate4);

  auto floorMaterial = std::make_unique<MatteMaterial>(Spectrum(0.1f));
  auto floor = loadMeshOBJ("../scenes/veach_mi/floor.obj");
  floor.material = floorMaterial.get();
  scene.addMesh(floor);

  scene.activate();

  Film film(Vector2i(768, 512));

  auto ratio = 512 / 768.f;
  PerspectiveCamera camera(
    Matrix4::lookAt(
      Vector3f(0, 6, -27.5),
      Vector3f(0, -1.5, -2.5),
      Vector3f(0, 1, 0)
    ),
    film,
    Bounds2f(Vector2f(-1, -ratio), Vector2f(1, ratio)),
    25
  );

  RandomSampler sampler(256);
  PathIntegrator integrator(camera, sampler);
  parallelInit();
  integrator.render(scene);
  parallelCleanup();
  film.writeImage("mis.png");

  return 0;
}
