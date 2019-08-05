#include <nanopt/nanopt.h>

using namespace nanopt;

int main() {
  Scene scene;

  auto wallsMat = std::make_unique<MatteMaterial>(Spectrum(0.725, 0.71, 0.68));
  auto wallsMesh = loadMeshOBJ("../scenes/cbox/walls.obj");
  wallsMesh.material = wallsMat.get();
  scene.addMesh(wallsMesh);

  auto rightWallMat = std::make_unique<MatteMaterial>(Spectrum(0.161, 0.133, 0.427));
  auto rightWallMesh = loadMeshOBJ("../scenes/cbox/rightwall.obj");
  rightWallMesh.material = rightWallMat.get();
  scene.addMesh(rightWallMesh);

  auto leftWallMat = std::make_unique<MatteMaterial>(Spectrum(0.630, 0.065, 0.05));
  auto leftWallMesh = loadMeshOBJ("../scenes/cbox/leftwall.obj");
  leftWallMesh.material = leftWallMat.get();
  scene.addMesh(leftWallMesh);

  auto sphere1Mat = std::make_unique<MirrorMaterial>(Spectrum(1.0f));
  auto sphere1Mesh = loadMeshOBJ("../scenes/cbox/sphere1.obj");
  sphere1Mesh.material = sphere1Mat.get();
  sphere1Mesh.shadingMode = ShadingMode::Smooth;
  scene.addMesh(sphere1Mesh);

  auto sphere2Mat = std::make_unique<GlassMaterial>(Spectrum(1), Spectrum(1), 1.4f);
  auto sphere2Mesh = loadMeshOBJ("../scenes/cbox/sphere2.obj");
  sphere2Mesh.material = sphere2Mat.get();
  sphere2Mesh.shadingMode = ShadingMode::Smooth;
  scene.addMesh(sphere2Mesh);

  auto lightMesh = loadMeshOBJ("../scenes/cbox/light.obj");
  lightMesh.light = new DiffuseAreaLight(&lightMesh, Spectrum(40));
  scene.addMesh(lightMesh);

  scene.activate();

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

  RandomSampler sampler(256);
  PathIntegrator integrator(camera, sampler, 10);
  parallelInit();
  integrator.render(scene);
  parallelCleanup();
  film.writeImage("./glass.png");

  return 0;
}
