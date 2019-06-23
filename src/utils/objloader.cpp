#include <cstring>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <nanopt/utils/objloader.h>

namespace nanopt {

inline void next(char*& str, char c) {
  while (*str != c) ++str;
  ++str;
}

inline void safe_next(char*& str, char c) {
  while (*str && *str != c) ++str;
  if (*str) ++str;
}

inline bool eq(const char* a, const char* b, int n) {
  return !std::strncmp(a, b, n) && a[n] == ' ';
}

inline Vector2f vec2(char*& str) {
  auto x = (float)std::atof(str); next(str, ' ');
  auto y = (float)std::atof(str);
  return { x, y };
}

inline Vector3f vec3(char*& str) {
  auto x = (float)std::atof(str); next(str, ' ');
  auto y = (float)std::atof(str); next(str, ' ');
  auto z = (float)std::atof(str);
  return { x, y, z };
}

struct Vertex {
  Vertex() = default;

  Vertex(char*& str) noexcept {
    p  = std::atoi(str); next(str, '/');
    uv = std::atoi(str); next(str, '/');
    n  = std::atoi(str);
  }

  bool operator==(const Vertex& v) const {
    return p == v.p && n == v.n && uv == v.uv;
  }

  bool operator!=(const Vertex& v) const {
    return p != v.p && n != v.n && uv != v.uv;
  }

  int p, n, uv;
};

Mesh loadMeshOBJ(const std::string& filename) {
  std::ifstream file(filename);
  if (file.fail())
    throw std::runtime_error("Unable to load obj file: " + filename);

  std::vector<Vector3f> positions;
  std::vector<Vector3f> normals;
  std::vector<Vector2f> uvs;
  std::vector<int> indices;
  std::vector<Vertex> vertices;

  auto hash = [](const Vertex& v) {
    auto h = std::hash<int>()(v.p);
    h = h * 37 + std::hash<int>()(v.uv);
    h = h * 37 + std::hash<int>()(v.n);
    return h;
  };
  std::unordered_map<Vertex, int, decltype(hash)> map(200, hash);

  char line[1024];
  while (file.getline(line, 1024)) {
    auto str = line;
    if (eq(str, "v", 1)) {
      positions.emplace_back(vec3(str += 2));
    } else if (eq(str, "n", 1)) {
      normals.emplace_back(vec3(str += 2));
    } else if (eq(str, "vt", 2)) {
      uvs.emplace_back(vec2(str += 3));
    } else if (eq(str, "f", 1)) {
      str += 2;
      Vertex verts[6];
      auto nVerts = 3;

      verts[0] = Vertex(str); next(str, ' ');
      verts[1] = Vertex(str); next(str, ' ');
      verts[2] = Vertex(str); safe_next(str, ' ');

      if (*str) {
        nVerts = 6;
        verts[3] = Vertex(str);
        verts[4] = verts[0];
        verts[5] = verts[1];
      }

      for (auto i = 0; i < nVerts; ++i) {
        auto& v = verts[i];
        auto it = map.find(v);
        if (it == map.end()) {
          map[v] = vertices.size();
          indices.push_back(vertices.size());
          vertices.push_back(v);
        } else {
          indices.push_back(it->second);
        }
      }
    }
  }

  auto nVerts = (int)vertices.size();

  auto p = new Vector3f[nVerts];
  for (auto i = 0; i < nVerts; ++i) {
    p[i] = positions[vertices[i].p - 1];
  }

  Vector3f* n = nullptr;
  if (normals.size()) {
    n = new Vector3f[nVerts];
    for (auto i = 0; i < nVerts; ++i)
      n[i] = normals[vertices[i].n - 1];
  }

  Vector2f* uv = nullptr;
  if (uvs.size()) {
    uv = new Vector2f[nVerts];
    for (auto i = 0; i < nVerts; ++i)
      uv[i] = uvs[vertices[i].uv - 1];
  }

  int* index = new int[indices.size()];
  memcpy(index, indices.data(), sizeof(int) * indices.size());

  return Mesh(
    ShadingMode::Flat,
    positions.size(),
    indices.size() / 3,
    index, p, n, uv
  );
}

}
