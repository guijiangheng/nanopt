#include <iostream>
#include <nanopt/nanopt.h>

using namespace nanopt;

int main() {
  auto mesh = loadMeshOBJ("../scenes/bunny.obj");
  auto triangles = createTriangleMesh(mesh);
  BVHAccel accel(std::move(triangles));
  Scene scene(accel);
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
  integrator.render(scene);
  film.writeImage("bunny.png");
  return 0;
}
