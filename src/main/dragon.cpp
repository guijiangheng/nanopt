#include <nanopt/nanopt.h>

using namespace nanopt;

int main() {
  Scene scene;

  auto dragon = Mesh(
    Matrix4::rotate(Vector3f(0, 1, 0), -53),
    loadMeshPLY("../scenes/dragon.ply")
  );
  scene.addMesh(dragon);

  int indices[] = { 0, 1, 2, 2, 3, 0 };
  Vector3f vertices[] = {
    { -1000, -1000, -40 },
    {  1000, -1000, -40 },
    {  1000,  1000, -40 },
    { -1000,  1000, -40 }
  };

  auto pIndices = new int[6];
  memcpy(pIndices, indices, sizeof(int) * 6);
  auto pVertices = new Vector3f[4];
  memcpy(pVertices, vertices, sizeof(Vector3f) * 4);

  auto plane = Mesh(
    ShadingMode::Flat, 4, 2,
    pIndices, pVertices, nullptr, nullptr
  );
  scene.addMesh(plane);
  scene.activate();

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
