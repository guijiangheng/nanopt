#include <nanopt/nanopt.h>

using namespace nanopt;

int main() {
  auto mesh = Mesh(
    Matrix4::rotate(Vector3f(0, 1, 0), -53),
    loadMeshPLY("../scenes/dragon.ply")
  );
  auto triangles = createTriangleMesh(mesh);
  BVHAccel accel(std::move(triangles));
  Scene scene(accel);
  Film film(Vector2i(800, 800));
  PerspectiveCamera camera(
    Matrix4::lookAt(
      Vector3f(277, -240, 250),
      Vector3f(0, 60, -30),
      Vector3f(0, 0, 1)
    ),
    film,
    Bounds2f(Vector2f(-1, -1), Vector2f(1, 1)),
    30
  );
  RandomSampler sampler(1);
  NormalIntegrator integrator(camera, sampler);
  parallelInit();
  integrator.render(scene);
  parallelCleanup();
  film.writeImage("./dragon.png");
  return 0;
}
