#include <nanopt/nanopt.h>

using namespace nanopt;

int main() {
  Scene scene;

  auto mesh = loadMeshOBJ("../scenes/table/mesh_1.obj");

  auto lightMesh1 = Mesh(
    Matrix4::translate(10, 0, -25) * Matrix4::scale(0.06, 0.06, -1),
    mesh
  );
  scene.addLight(new DiffuseAreaLight(&lightMesh1, Spectrum(3, 3, 2.5), true));

  auto lightMesh2 = Mesh(
    Matrix4::translate(0, 0, -60) * Matrix4::scale(0.3, 0.3, -1),
    mesh
  );
  scene.addLight(new DiffuseAreaLight(&lightMesh2, Spectrum(3, 3, 2.5), true));

  auto plateMaterial = std::make_unique<MatteMaterial>(Spectrum(0.2));
  auto plateMesh = Mesh(
    Matrix4::translate(3, 0, 0),
    loadMeshOBJ("../scenes/table/mesh_0.obj")
  );
  plateMesh.material = plateMaterial.get();
  scene.addMesh(plateMesh);

  auto floorMaterial = std::make_unique<MatteMaterial>(Spectrum(0.5));
  auto floorMesh = Mesh(
    Matrix4::translate(-35, 25, 0) * Matrix4::scale(0.2, 0.35, 0.5),
    mesh
  );
  floorMesh.material = floorMaterial.get();
  scene.addMesh(floorMesh);

  auto glass1Material = std::make_unique<GlassMaterial>(Spectrum(1), Spectrum(1), 1.33);
  auto glass1Mesh = Mesh(
    Matrix4::translate(-1, 0, 0),
    loadMeshOBJ("../scenes/table/mesh_2.obj")
  );
  glass1Mesh.material = glass1Material.get();
  glass1Mesh.shadingMode = ShadingMode::Smooth;
  scene.addMesh(glass1Mesh);

  auto glass2Material = std::make_unique<GlassMaterial>(Spectrum(1), Spectrum(1), 1.5);
  auto glass2Mesh = Mesh(
    Matrix4::translate(-1, 0, 0),
    loadMeshOBJ("../scenes/table/mesh_3.obj")
  );
  glass2Mesh.material = glass2Material.get();
  glass2Mesh.shadingMode = ShadingMode::Smooth;
  scene.addMesh(glass2Mesh);

  auto glass3Material = std::make_unique<GlassMaterial>(Spectrum(1), Spectrum(1), 0.8866667);
  auto glass3Mesh = Mesh(
    Matrix4::translate(-1, 0, 0),
    loadMeshOBJ("../scenes/table/mesh_4.obj")
  );
  glass3Mesh.shadingMode = ShadingMode::Smooth;
  glass3Mesh.material = glass3Material.get();
  scene.addMesh(glass3Mesh);

  scene.activate();

  Film film(Vector2i(800, 600));
  PerspectiveCamera camera(
    Matrix4::lookAt(
      Vector3f(32.1259, -68.0505, -36.597),
      Vector3f(31.6866, -67.2776, -36.1392),
      Vector3f(-0.22886, 0.39656, -0.889024)
    ),
    film,
    Bounds2f(Vector2f(-1, -0.75), Vector2f(1, 0.75)),
    35
  );

  RandomSampler sampler(512);
  PathIntegrator integrator(camera, sampler, 20);
  parallelInit();
  integrator.render(scene);
  parallelCleanup();
  film.writeImage("./table.png");

  return 0;
}
