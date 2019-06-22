#include <nanopt/utils/imageio.h>

using namespace nanopt;

void testReadWritePNG() {
  int width, height;
  auto data = nanopt::readImage("../assets/dragon.png", width, height);
  auto offset = 0;
  for (auto y = 0; y < height; ++y)
    for (auto x = 0; x < width; ++x) {
      data[offset] = inverseGammaCorrect(data[offset]);
      data[offset] = Spectrum(data[offset].y());
      ++offset;
    }
  nanopt::writeImage("./result.png", width, height, data.get());
}

void testReadWriteEXR() {
  auto offset = 0;
  int width, height;
  auto data = nanopt::readImage("../assets/dragon.exr", width, height);
  for (auto y = 0; y < height; ++y)
    for (auto x = 0; x < width; ++x) {
      data[offset] = Spectrum(data[offset].y());
      ++offset;
    }
  nanopt::writeImage("./result.exr", width, height, data.get());
}

void testReadExrWritePng() {
  int width, height;
  auto data = nanopt::readImage("../assets/dragon.exr", width, height);
  nanopt::writeImage("./exr-to-png.png", width, height, data.get());
}

int main() {
  testReadWritePNG();
  testReadWriteEXR();
  testReadExrWritePng();
  return 0;
}
