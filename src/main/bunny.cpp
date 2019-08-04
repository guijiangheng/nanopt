#include <nanopt/nanopt.h>

using namespace nanopt;

int main() {
  Scene scene;
  auto mesh = loadMeshOBJ("../scenes/bunny.obj");
  scene.addMesh(mesh);
  scene.activate(BuildMethod::HLBVH);
  Film film(Vector2i(512, 512));
  PerspectiveCamera camera(
    Matrix4::lookAt(
      Vector3f(-0.0315182, 0.284011, -0.7331),
      Vector3f(-0.0123771, 0.0540913, 0.239922),
      Vector3f(0.00717446, 0.973206, 0.229822)
    ),
    film,
    defaultScreenBounds(1),
    16
  );
  RandomSampler sampler(1);
  NormalIntegrator integrator(camera, sampler);
  parallelInit();
  integrator.render(scene);
  parallelCleanup();
  film.writeImage("bunny.png");
  return 0;
}
