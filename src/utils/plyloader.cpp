#include <string>
#include <fstream>
#include <sstream>
#include <memory>
#include <vector>
#include <nanopt/math/vector3.h>
#include <nanopt/utils/plyloader.h>

namespace nanopt {

class Property {
public:
  explicit Property(const std::string& name) noexcept : name(name) { }
  virtual ~Property() = default;
  virtual void parse(std::istringstream& line) = 0;
  virtual void read(std::ifstream& stream) = 0;

public:
  std::string name;
};

template <typename T>
class TypedProperty : public Property {
public:
  explicit TypedProperty(const std::string& name) noexcept : Property(name)
  { }

  void parse(std::istringstream& line) override {
    T val;
    line >> val;
    data.push_back(val);
  }

  void read(std::ifstream& stream) override {
    T val;
    stream.read((char*)&val, sizeof(T));
    data.push_back(val);
  }

public:
  std::vector<T> data;
};

template <typename T>
class TypedListProperty : public Property {
public:
  TypedListProperty(const std::string& name) noexcept : Property(name)
  { }

  void parse(std::istringstream& line) override {
    int n;
    line >> n;
    std::vector<T> vec;
    vec.reserve(n);
    for (auto i = 0; i < n; ++i) {
      T val;
      line >> val;
      vec.push_back(val);
    }
    data.push_back(std::move(vec));
  }

  void read(std::ifstream& stream) override {
    std::uint8_t n = 0;
    stream.read((char*)&n, 1);
    auto count = (int)n;
    std::vector<T> vec;
    vec.reserve(count);
    for (auto i = 0; i < count; ++i) {
      T val;
      stream.read((char*)&val, sizeof(T));
      vec.push_back(val);
    }
    data.push_back(std::move(vec));
  }

public:
  std::vector<std::vector<T>> data;
};

Property* createProperty(bool isList, const std::string& name, const std::string& type) {
  if (isList) {
    if (type == "int" || type == "int32") return new TypedListProperty<int>(name);
  } else {
    if (type == "int" || type == "int32") return new TypedProperty<int>(name);
    if (type == "float" || type == "float32") return new TypedProperty<float>(name);
  }
  throw std::runtime_error("PLY loader: unrecognized data type: " + type + ".");
}

class Element {
public:
  Element(const std::string& name, int count) noexcept : name(name), count(count)
  { }

  bool hasProperty(const std::string& name) const {
    for (auto& p : properties)
      if (p->name == name) return true;
    return false;
  }

  void addProperty(Property* property) {
    if (hasProperty(property->name))
      throw std::runtime_error("PLY loader: property with name: " + name + " already exist.");
    properties.emplace_back(property);
  }

  const Property* getProperty(const std::string& name) const {
    for (auto& p : properties)
      if (p->name == name) return p.get();
    return nullptr;
  }

public:
  std::string name;
  int count;
  std::vector<std::unique_ptr<Property>> properties;
};

enum class DataFormat { Text, Binary };

class PLYData {
public:
  explicit PLYData(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (file.fail())
      throw std::runtime_error("Unable to load ply file: " + filename);
    parseHeader(file);
    if (inputFormat == DataFormat::Text) parseTextBody(file);
    else parseBinaryBody(file);
  }

  void parseHeader(std::ifstream& stream) {
    std::string line;
    while (std::getline(stream, line)) {
      std::string token;
      std::istringstream lineStream(line);
      lineStream >> token;
      if (token == "format") parseFormatHeader(lineStream);
      else if (token == "element") parseElementHeader(lineStream);
      else if (token == "property") parsePropertyHeader(lineStream);
      else if (token == "end_header") break;
    }
  }

  void parseFormatHeader(std::istringstream& line) {
    std::string type, version;
    line >> type >> version;
    if (version != "1.0") throw std::runtime_error("PLY loader: only version 1.0 is supported.");
    if (type == "ascii") inputFormat = DataFormat::Text;
    else if (type == "binary_little_endian") inputFormat = DataFormat::Binary;
    else if (type == "binary_big_endian") throw std::runtime_error("PLY loader: big endian binary format not supported.");
    else throw std::runtime_error("PLY loader: unknown file format.");
  }

  void parseElementHeader(std::istringstream& line) {
    int count;
    std::string name;
    line >> name >> count;
    if (hasElement(name))
      throw std::runtime_error("PLY loader: element " + name + " already exist.");
    elements.emplace_back(name, count);
  }

  void parsePropertyHeader(std::istringstream& line) {
    std::string token;
    line >> token;
    if (token == "list") {
      std::string countType, type, name;
      line >> countType >> type >> name;
      elements.back().addProperty(createProperty(true, name, type));
    } else {
      std::string name;
      auto& type = token;
      line >> name;
      elements.back().addProperty(createProperty(false, name, type));
    }
  }

  void parseBinaryBody(std::ifstream& stream) {
    for (auto& e : elements)
      for (auto i = 0; i < e.count; ++i)
        for (auto& p : e.properties)
          p->read(stream);
  }

  void parseTextBody(std::ifstream& stream) {
    for (auto& e : elements)
      for (auto i = 0; i < e.count; ++i) {
        std::string line;
        std::getline(stream, line);
        std::istringstream lineStream(line);
        for (auto& p : e.properties)
          p->parse(lineStream);
      }
  }

  bool hasElement(const std::string& name) const {
    for (auto& e : elements)
      if (e.name == name) return true;
    return false;
  }

  const Element* getElement(const std::string& name) const {
    for (auto& e : elements)
      if (e.name == name) return &e;
    return nullptr;
  }

public:
  DataFormat inputFormat = DataFormat::Text;
  std::vector<Element> elements;
};

Mesh loadMeshPLY(const std::string& filename) {
  PLYData ply(filename);

  auto vertex = ply.getElement("vertex");
  auto n = vertex->count;

  auto px = (TypedProperty<float>*)vertex->getProperty("x");
  auto py = (TypedProperty<float>*)vertex->getProperty("y");
  auto pz = (TypedProperty<float>*)vertex->getProperty("z");

  auto p = new Vector3f[n];
  for (auto i = 0; i < n; ++i) {
    p[i] = Vector3f(px->data[i], py->data[i], pz->data[i]);
  }

  auto faces = (TypedListProperty<int>*)ply.getElement("face")->getProperty("vertex_indices");
  std::vector<int> indices;
  indices.reserve(faces->data.size() * 6);
  for (auto& face : faces->data) {
    if (face.size() != 3 && face.size() != 4) continue;
    indices.emplace_back(face[0]);
    indices.emplace_back(face[1]);
    indices.emplace_back(face[2]);
    if (face.size() == 4) {
      indices.emplace_back(face[3]);
      indices.emplace_back(face[0]);
      indices.emplace_back(face[2]);
    }
  }

  indices.shrink_to_fit();
  auto index = new int[indices.size()];
  memcpy(index, indices.data(), sizeof(int) * indices.size());

  return Mesh(
    ShadingMode::Flat,
    n,
    indices.size() / 3,
    index, p, nullptr, nullptr
  );
}

}
