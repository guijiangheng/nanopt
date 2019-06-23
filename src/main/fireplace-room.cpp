#include <experimental/filesystem>
#include <nanopt/nanopt.h>

using namespace std;
using namespace nanopt;

int main() {
  auto mesh = loadMeshOBJ("../scenes/fireplace-room/fireplace_room.obj");
  mesh.shadingMode = ShadingMode::Smooth;
  auto triangles = createTriangleMesh(mesh);
  BVHAccel accel(std::move(triangles));
  Scene scene(accel);
  Film film(Vector2i(1920, 1080));

  PerspectiveCamera camera(
    Matrix4::lookAt(
      Vector3f(5.101118f, 1.083746f, 2.756308f),
      Vector3f(4.167568f, 1.078925f, 2.397892f),
      Vector3f(0, 1, 0)
    ),
    film,
    defaultScreenBounds(1920.0f/ 1080),
    43.0f
  );

  RandomSampler sampler(4);
  NormalIntegrator integrator(camera, sampler);
  integrator.render(scene);
  film.writeImage("./fireplace-room.png");

  return 0;
}
