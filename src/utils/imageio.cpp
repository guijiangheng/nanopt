#include <ghc/filesystem.hpp>
#include <lodepng/lodepng.h>
#include <nanopt/utils/imageio.h>
#include <ImfRgba.h>
#include <ImfRgbaFile.h>

namespace nanopt {

namespace fs = ghc::filesystem;

static Spectrum* readImagePNG(const std::string& filename, int& width, int& height) {
  unsigned xres, yres;
  std::vector<std::uint8_t> bytes;
  auto error = lodepng::decode(bytes, xres, yres, filename.c_str(), LCT_RGB);
  if (error) throw std::runtime_error("Failed to load png file: " + std::string(lodepng_error_text(error)));
  width = (int)xres;
  height = (int)yres;
  auto offset = 0;
  auto data = new Spectrum[width * height];
  for (auto y = 0; y < height; ++y)
    for (auto x = 0; x < width; ++x) {
      data[offset][0] = bytes[offset * 3 + 0] / 255.0f;
      data[offset][1] = bytes[offset * 3 + 1] / 255.0f;
      data[offset][2] = bytes[offset * 3 + 2] / 255.0f;
      ++offset;
    }
  return data;
}

static Spectrum* readImageEXR(const std::string& filename, int& width, int& height) {
  using namespace Imf;
  using namespace Imath;
  RgbaInputFile file(filename.c_str());
  auto window = file.dataWindow();
  width = window.max.x - window.min.x + 1;
  height = window.max.y - window.min.y + 1;
  std::vector<Rgba> pixels(width * height);
  file.setFrameBuffer(&pixels[0] - window.min.x - window.min.y * width, 1, width);
  file.readPixels(window.min.y, window.max.y);
  auto data = new Spectrum[width * height];
  for (auto i = 0; i < width * height; ++i)
    data[i] = { pixels[i].r, pixels[i].g, pixels[i].b };
  return data;
}

std::unique_ptr<Spectrum[]>
readImage(const std::string& filename, int& width, int& height) {
  auto extension = fs::path(filename).extension();
  if (extension == ".exr")
    return std::unique_ptr<Spectrum[]>(readImageEXR(filename, width, height));
  else if (extension == ".png")
    return std::unique_ptr<Spectrum[]>(readImagePNG(filename, width, height));
  throw std::runtime_error("Input image file format \"" + extension.string() + "\" not supported!");
}

static void writeImagePNG(const std::string& filename, int width, int height, Spectrum* data) {
  int offset = 0;
  std::unique_ptr<std::uint8_t[]> bytes(new std::uint8_t[width * height * 3]);
  for (auto y = 0; y < height; ++y)
    for (auto x = 0; x < width; ++x) {
      auto color = gammaCorrect(data[offset]);
      bytes[offset * 3 + 0] = (std::uint8_t)std::min(255 * color[0], 255.0f);
      bytes[offset * 3 + 1] = (std::uint8_t)std::min(255 * color[1], 255.0f);
      bytes[offset * 3 + 2] = (std::uint8_t)std::min(255 * color[2], 255.0f);
      ++offset;
    }
  lodepng::encode(filename, bytes.get(), width, height, LCT_RGB);
}

static void writeImageEXR(const std::string& filename, int width, int height, Spectrum* data) {
  using namespace Imf;
  using namespace Imath;
  std::unique_ptr<Rgba[]> rgbas(new Rgba[width * height]);
  for (auto i = 0; i < width * height; ++i)
    rgbas[i] = Rgba(data[i][0], data[i][1], data[i][2]);
  Box2i displayWindow(V2i(0, 0), V2i(width - 1, height - 1));
  Box2i dataWindow(V2i(0, 0), V2i(width - 1, height - 1));
  RgbaOutputFile file(filename.c_str(), displayWindow, dataWindow, WRITE_RGB);
  file.setFrameBuffer(rgbas.get(), 1, width);
  file.writePixels(height);
}

void writeImage(const std::string& filename, int width, int height, Spectrum* data) {
  auto extension = fs::path(filename).extension();
  if (extension == ".exr")
    return writeImageEXR(filename, width, height, data);
  else if (extension == ".png")
    return writeImagePNG(filename, width, height, data);
  throw std::runtime_error("Output image file format \"" + extension.string() + "\" not supported!");
}

}
