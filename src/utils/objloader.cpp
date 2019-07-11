#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <nanopt/utils/objloader.h>

namespace nanopt {

std::vector<std::string> tokenize(const std::string& string, const std::string& delim) {
  std::vector<std::string> tokens;
  std::string::size_type lastPos = 0;
  auto pos = string.find_first_of(delim, lastPos);
  while (lastPos != std::string::npos) {
    tokens.push_back(string.substr(lastPos, pos - lastPos));
    lastPos = pos;
    if (lastPos != std::string::npos) {
      lastPos += 1;
      pos = string.find_first_of(delim, lastPos);
    }
  }
  return tokens;
}

struct Vertex {
  Vertex() noexcept : p(0), n(0), uv(0)
  { }

  Vertex(const std::string& string) {
    auto tokens = tokenize(string, "/");
    p = std::atoi(tokens[0].c_str());
    if (tokens.size() >= 2 && !tokens[1].empty())
      uv = std::atoi(tokens[1].c_str());
    if (tokens.size() == 3 && !tokens[2].empty())
      n = std::atoi(tokens[2].c_str());
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

  std::string lineStr;
  while (std::getline(file, lineStr)) {
    std::istringstream line(lineStr);
    std::string prefix;
    line >> prefix;

    if (prefix == "v") {
      Vector3f p;
      line >> p.x >> p.y >> p.z;
      p.z = -p.z;
      positions.push_back(p);
    } else if (prefix == "vt") {
      Vector2f uv;
      line >> uv.x >> uv.y;
      uvs.push_back(uv);
    } else if (prefix == "vn") {
      Vector3f n;
      line >> n.x >> n.y >> n.z;
      n.z = -n.z;
      normals.push_back(normalize(n));
    } else if (prefix == "f") {
      Vertex verts[6];
      auto nVerts = 3;
      std::string v1, v2, v3, v4;
      line >> v1 >> v2 >> v3 >> v4;
      verts[0] = Vertex(v1);
      verts[1] = Vertex(v2);
      verts[2] = Vertex(v3);

      if (!v4.empty()) {
        verts[3] = Vertex(v4);
        verts[4] = verts[0];
        verts[5] = verts[2];
        nVerts = 6;
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

  auto index = new int[indices.size()];
  memcpy(index, indices.data(), sizeof(int) * indices.size());

  return Mesh(
    ShadingMode::Flat,
    positions.size(),
    indices.size() / 3,
    index, p, n, uv
  );
}

}
