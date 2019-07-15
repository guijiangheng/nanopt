#include <nanopt/nanopt.h>

using namespace nanopt;

int main() {
  std::vector<Light*> lights;
  std::vector<Triangle*> triangles;

  auto mesh = loadMeshOBJ("../scenes/table/mesh_1.obj");

  auto lightMesh1 = Mesh(
    Matrix4::translate(10, 0, -25) * Matrix4::scale(0.06, 0.06, -1),
    mesh
  );
  auto lightMesh1Triangles = createTriangleMesh(lightMesh1);
  for (auto triangle : lightMesh1Triangles)
    lights.push_back(new DiffuseAreaLight(triangle, Spectrum(3, 3, 2.5), true));
  triangles.insert(triangles.begin(), lightMesh1Triangles.begin(), lightMesh1Triangles.end());

  auto lightMesh2 = Mesh(
    Matrix4::translate(0, 0, -60) * Matrix4::scale(0.3, 0.3, -1),
    mesh
  );
  auto lightMesh2Triangles = createTriangleMesh(lightMesh2);
  for (auto triangle : lightMesh2Triangles)
    lights.push_back(new DiffuseAreaLight(triangle, Spectrum(1, 1, 1.6), true));
  triangles.insert(triangles.begin(), lightMesh2Triangles.begin(), lightMesh2Triangles.end());

  auto plateMaterial = std::make_unique<MatteMaterial>(Spectrum(0.2));
  auto plateMesh = Mesh(
    Matrix4::translate(3, 0, 0),
    loadMeshOBJ("../scenes/table/mesh_0.obj")
  );
  auto plateMeshTriangles = createTriangleMesh(plateMesh, plateMaterial.get());
  triangles.insert(triangles.begin(), plateMeshTriangles.begin(), plateMeshTriangles.end());

  auto floorMaterial = std::make_unique<MatteMaterial>(Spectrum(0.5));
  auto floorMesh = Mesh(
    Matrix4::translate(-35, 25, 0) * Matrix4::scale(0.2, 0.35, 0.5),
    mesh
  );
  auto floorTriangles = createTriangleMesh(floorMesh, floorMaterial.get());
  triangles.insert(triangles.begin(), floorTriangles.begin(), floorTriangles.end());

  auto glass1Material = std::make_unique<GlassMaterial>(Spectrum(1), Spectrum(1), 1.33);
  auto glass1Mesh = Mesh(
    Matrix4::translate(-1, 0, 0),
    loadMeshOBJ("../scenes/table/mesh_2.obj")
  );
  glass1Mesh.shadingMode = ShadingMode::Smooth;
  auto glass1Triangles = createTriangleMesh(glass1Mesh, glass1Material.get());
  triangles.insert(triangles.begin(), glass1Triangles.begin(), glass1Triangles.end());

  auto glass2Material = std::make_unique<GlassMaterial>(Spectrum(1), Spectrum(1), 1.5);
  auto glass2Mesh = Mesh(
    Matrix4::translate(-1, 0, 0),
    loadMeshOBJ("../scenes/table/mesh_3.obj")
  );
  glass2Mesh.shadingMode = ShadingMode::Smooth;
  auto glass2Triangles = createTriangleMesh(glass2Mesh, glass2Material.get());
  triangles.insert(triangles.begin(), glass2Triangles.begin(), glass2Triangles.end());

  auto glass3Material = std::make_unique<GlassMaterial>(Spectrum(1), Spectrum(1), 0.8866667);
  auto glass3Mesh = Mesh(
    Matrix4::translate(-1, 0, 0),
    loadMeshOBJ("../scenes/table/mesh_4.obj")
  );
  glass3Mesh.shadingMode = ShadingMode::Smooth;
  auto glass3Triangles = createTriangleMesh(glass3Mesh, glass3Material.get());
  triangles.insert(triangles.begin(), glass3Triangles.begin(), glass3Triangles.end());

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

  BVHAccel accel(std::move(triangles));
  Scene scene(accel, std::move(lights));
  RandomSampler sampler(512);
  PathIntegrator integrator(camera, sampler, 20);
  parallelInit();
  integrator.render(scene);
  parallelCleanup();
  film.writeImage("./table.png");

  return 0;
}
