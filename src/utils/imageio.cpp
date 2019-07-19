#define TINYEXR_IMPLEMENTATION

#include <cstring>
#include <tinyexr.h>
#include <lodepng/lodepng.h>
#include <nanopt/utils/imageio.h>

namespace nanopt {

static std::string extension(const std::string& filename) {
  auto pos = filename.find_last_of(".");
  if (pos == std::string::npos) return "";
  return filename.substr(pos + 1);
}

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
  float* rgba;
  const char* err;
  if (LoadEXR(&rgba, &width, &height, filename.c_str(), &err) != 0)
    throw std::runtime_error("load exr from" + filename + " failed.");
  auto nPixels = width * height;
  auto data = new Spectrum[nPixels];
  for (auto i = 0; i < nPixels; ++i)
    data[i] = { rgba[i * 4 + 0], rgba[i * 4 + 1], rgba[i * 4 + 2] };
  delete rgba;
  return data;
}

std::unique_ptr<Spectrum[]>
readImage(const std::string& filename, int& width, int& height) {
  auto ext = extension(filename);
  if (ext == "exr")
    return std::unique_ptr<Spectrum[]>(readImageEXR(filename, width, height));
  else if (ext == "png")
    return std::unique_ptr<Spectrum[]>(readImagePNG(filename, width, height));
  throw std::runtime_error("Input image file format \"" + ext + "\" not supported!");
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
  EXRImage image;
  InitEXRImage(&image);

  image.num_channels = 3;

  auto nPixels = width * height;
  std::vector<float> images[3];
  images[0].resize(nPixels);
  images[1].resize(nPixels);
  images[2].resize(nPixels);

  for (auto i = 0; i < nPixels; ++i) {
    images[0][i] = data[i].e[0];
    images[1][i] = data[i].e[1];
    images[2][i] = data[i].e[2];
  }

  float* imagePtr[3];
  imagePtr[0] = &(images[2].at(0)); // B
  imagePtr[1] = &(images[1].at(0)); // G
  imagePtr[2] = &(images[0].at(0)); // R

  image.images = (unsigned char**)imagePtr;
  image.width = width;
  image.height = height;

  EXRHeader header;
  InitEXRHeader(&header);
  header.num_channels = 3;
  header.channels = (EXRChannelInfo*)malloc(sizeof(EXRChannelInfo) * header.num_channels);
  strcpy(header.channels[0].name, "B");
  strcpy(header.channels[1].name, "G");
  strcpy(header.channels[2].name, "R");
  header.pixel_types = (int*)malloc(sizeof(int) * header.num_channels);
  header.requested_pixel_types = (int*)malloc(sizeof(int) * header.num_channels);
  for (auto i = 0; i < header.num_channels; ++i) {
    header.pixel_types[i] = TINYEXR_PIXELTYPE_FLOAT;
    header.requested_pixel_types[i] = TINYEXR_PIXELTYPE_HALF;
  }

  const char* err;
  auto ret = SaveEXRImageToFile(&image, &header, filename.c_str(), &err);
  if (ret != TINYEXR_SUCCESS)
    throw std::runtime_error("save exr to" + filename + " failed.");

  free(header.channels);
  free(header.pixel_types);
  free(header.requested_pixel_types);
}

void writeImage(const std::string& filename, int width, int height, Spectrum* data) {
  auto ext = extension(filename);
  if (ext == "exr")
    return writeImageEXR(filename, width, height, data);
  else if (ext == "png")
    return writeImagePNG(filename, width, height, data);
  throw std::runtime_error("Output image file format \"" + ext + "\" not supported!");
}

}
