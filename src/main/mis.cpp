#include <nanopt/nanopt.h>

using namespace nanopt;

int main() {
  std::vector<Light*> lights;
  std::vector<Triangle> triangles;
  auto sphere = loadMeshOBJ("../scenes/veach_mi/sphere.obj");

  auto sphere1 = Mesh(
    Matrix4::translate(-1.25f, 0, 0) * Matrix4::scale(0.1f, 0.1f, 0.1f),
    sphere
  );
  auto sphere1Triangles = createTriangleMesh(sphere1);
  for (auto& triangle : sphere1Triangles)
    lights.push_back(new DiffuseAreaLight(&triangle, Spectrum(100)));
  triangles.insert(triangles.begin(), sphere1Triangles.begin(), sphere1Triangles.end());

  auto sphere2 = Mesh(
    Matrix4::translate(-3.75f, 0, 0) * Matrix4::scale(0.0333f, 0.0333f, 0.0333f),
    sphere
  );
  auto sphere2Triangles = createTriangleMesh(sphere2);
  for (auto& triangle : sphere2Triangles)
    lights.push_back(new DiffuseAreaLight(&triangle, Spectrum(901.803f)));
  triangles.insert(triangles.begin(), sphere2Triangles.begin(), sphere2Triangles.end());

  auto sphere3 = Mesh(
    Matrix4::translate(1.25f, 0, 0) * Matrix4::scale(0.3f, 0.3f, 0.3f),
    sphere
  );
  auto sphere3Triangles = createTriangleMesh(sphere3);
  for (auto& triangle : sphere3Triangles)
    lights.push_back(new DiffuseAreaLight(&triangle, Spectrum(11.1111f)));
  triangles.insert(triangles.begin(), sphere3Triangles.begin(), sphere3Triangles.end());

  auto sphere4 = Mesh(
    Matrix4::translate(3.75f, 0, 0) * Matrix4::scale(0.9f, 0.9f, 0.9f),
    sphere
  );
  auto sphere4Triangles = createTriangleMesh(sphere4);
  for (auto& triangle : sphere4Triangles)
    lights.push_back(new DiffuseAreaLight(&triangle, Spectrum(1.23457f)));
  triangles.insert(triangles.begin(), sphere4Triangles.begin(), sphere4Triangles.end());

  auto sphere5 = Mesh(
    Matrix4::translate(0, 4, -3),
    sphere
  );
  auto sphere5Triangles = createTriangleMesh(sphere5);
  for (auto& triangle : sphere5Triangles)
    lights.push_back(new DiffuseAreaLight(&triangle, Spectrum(100.0f)));
  triangles.insert(triangles.begin(), sphere5Triangles.begin(), sphere5Triangles.end());

  auto plate1Material = std::make_unique<PlasticMaterial>(
    Spectrum(0.0175f, 0.0225f, 0.0325f),
    Spectrum(0.9675f),
    0.005, false
  );
  auto plate1 = loadMeshOBJ("../scenes/veach_mi/plate1.obj");
  auto plate1Triangles = createTriangleMesh(plate1, plate1Material.get());
  triangles.insert(triangles.begin(), plate1Triangles.begin(), plate1Triangles.end());

  auto plate2Material = std::make_unique<PlasticMaterial>(
    Spectrum(0.0175f, 0.0225f, 0.0325f),
    Spectrum(0.9675f),
    0.02, false
  );
  auto plate2 = loadMeshOBJ("../scenes/veach_mi/plate2.obj");
  auto plate2Triangles = createTriangleMesh(plate2, plate2Material.get());
  triangles.insert(triangles.begin(), plate2Triangles.begin(), plate2Triangles.end());

  auto plate3Material = std::make_unique<PlasticMaterial>(
    Spectrum(0.0175f, 0.0225f, 0.0325f),
    Spectrum(0.9675f),
    0.05, false
  );
  auto plate3 = loadMeshOBJ("../scenes/veach_mi/plate3.obj");
  auto plate3Triangles = createTriangleMesh(plate3, plate3Material.get());
  triangles.insert(triangles.begin(), plate3Triangles.begin(), plate3Triangles.end());

  auto plate4Material = std::make_unique<PlasticMaterial>(
    Spectrum(0.0175f, 0.0225f, 0.0325f),
    Spectrum(0.9675f),
    0.1, false
  );
  auto plate4 = loadMeshOBJ("../scenes/veach_mi/plate4.obj");
  auto plate4Triangles = createTriangleMesh(plate4, plate4Material.get());
  triangles.insert(triangles.begin(), plate4Triangles.begin(), plate4Triangles.end());

  auto floorMaterial = std::make_unique<MatteMaterial>(Spectrum(0.1f));
  auto floor = loadMeshOBJ("../scenes/veach_mi/floor.obj");
  auto floorTriangles = createTriangleMesh(floor, floorMaterial.get());
  triangles.insert(triangles.begin(), floorTriangles.begin(), floorTriangles.end());

  BVHAccel accel(std::move(triangles));
  Film film(Vector2i(768, 512));
  Scene scene(accel, std::move(lights));

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
