#include <nanopt/nanopt.h>

using namespace nanopt;

int main() {
  auto mesh = loadMeshOBJ("../scenes/ajax.obj");
  auto triangles = createTriangleMesh(mesh);
  BVHAccel accel(std::move(triangles));
  Scene scene(accel);
  Film film(Vector2i(900, 900));

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
  NormalIntegrator integrator(camera, sampler);
  parallelInit();
  integrator.render(scene);
  parallelCleanup();
  film.writeImage("./point.png");

  return 0;
}
