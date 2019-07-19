#include <nanopt/nanopt.h>

using namespace nanopt;

int main() {
  auto wallsMat = std::make_unique<MatteMaterial>(Spectrum(0.725, 0.71, 0.68));
  auto wallsMesh = loadMeshOBJ("../scenes/cbox/walls.obj");
  auto triangles = createTriangleMesh(wallsMesh, wallsMat.get());

  auto rightWallMat = std::make_unique<MatteMaterial>(Spectrum(0.161, 0.133, 0.427));
  auto rightWallMesh = loadMeshOBJ("../scenes/cbox/rightwall.obj");
  auto rightWallTriangles = createTriangleMesh(rightWallMesh, rightWallMat.get());
  triangles.insert(triangles.begin(), rightWallTriangles.begin(), rightWallTriangles.end());

  auto leftWallMat = std::make_unique<MatteMaterial>(Spectrum(0.630, 0.065, 0.05));
  auto leftWallMesh = loadMeshOBJ("../scenes/cbox/leftwall.obj");
  auto leftWallTriangles = createTriangleMesh(leftWallMesh, leftWallMat.get());
  triangles.insert(triangles.begin(), leftWallTriangles.begin(), leftWallTriangles.end());

  auto sphere1Mat = std::make_unique<MirrorMaterial>(Spectrum(1.0f));
  auto sphere1Mesh = loadMeshOBJ("../scenes/cbox/sphere1.obj");
  auto sphere1Triangles = createTriangleMesh(sphere1Mesh, sphere1Mat.get());
  triangles.insert(triangles.begin(), sphere1Triangles.begin(), sphere1Triangles.end());
  sphere1Mesh.shadingMode = ShadingMode::Smooth;

  auto sphere2Mat = std::make_unique<GlassMaterial>(Spectrum(1), Spectrum(1), 1.4f);
  auto sphere2Mesh = loadMeshOBJ("../scenes/cbox/sphere2.obj");
  auto sphere2Triangles = createTriangleMesh(sphere2Mesh, sphere2Mat.get());
  triangles.insert(triangles.begin(), sphere2Triangles.begin(), sphere2Triangles.end());
  sphere2Mesh.shadingMode = ShadingMode::Smooth;

  std::vector<Light*> lights;
  auto lightMesh = loadMeshOBJ("../scenes/cbox/light.obj");
  auto lightTriangles = createTriangleMesh(lightMesh);
  for (auto triangle : lightTriangles)
    lights.push_back(new DiffuseAreaLight(triangle, Spectrum(40)));
  triangles.insert(triangles.begin(), lightTriangles.begin(), lightTriangles.end());


  Film film(Vector2i(800, 600));
  PerspectiveCamera camera(
    Matrix4::scale(-1, 1, 1) *
    Matrix4::lookAt(
      Vector3f(0, 0.919769, -5.41159),
      Vector3f(0, 0.893051, -4.41198),
      Vector3f(0, 1, 0)
    ),
    film,
    Bounds2f(Vector2f(-1, -0.75), Vector2f(1, 0.75)),
    27.7856
  );

  BVHAccel accel(std::move(triangles));
  Scene scene(accel, std::move(lights));
  RandomSampler sampler(32);
  PathIntegrator integrator(camera, sampler, 10);
  parallelInit();
  integrator.render(scene);
  parallelCleanup();
  film.writeImage("./glass.png");

  return 0;
}
