#include <nanopt/core/integrator.h>

namespace nanopt {

void Integrator::render(const Scene& scene) {
  constexpr auto TileSize = 16;
  auto& pixelBounds = camera.film.pixelBounds;
  auto diag = pixelBounds.diag();
  Vector2i nTiles(
    (diag.x + TileSize - 1) / TileSize,
    (diag.y + TileSize - 1) / TileSize);
  parallelFor2D([&](const Vector2i& tile) {
    auto x0 = pixelBounds.pMin.x + TileSize * tile.x;
    auto x1 = std::min(pixelBounds.pMax.x, x0 + TileSize);
    auto y0 = pixelBounds.pMin.y + TileSize * tile.y;
    auto y1 = std::min(pixelBounds.pMax.y, y0 + TileSize);
    Bounds2i tileBounds(Vector2i(x0, y0), Vector2i(x1, y1));
    auto seed = tile.y * nTiles.x + tile.x;
    auto tileSampler = sampler.clone(seed);
    for (auto p : tileBounds) {
      Spectrum l(0);
      tileSampler->startPixel();
      do {
        auto cameraSample = sampler.getCameraSample(p);
        auto ray = camera.generateRay(cameraSample);
        l += li(ray, scene);
      } while (sampler.startNextSample());
      auto offsetX = p.x - pixelBounds.pMin.x;
      auto offsetY = p.y - pixelBounds.pMin.y;
      camera.film.pixels[offsetY * diag.x + offsetX] = l / tileSampler->samplesPerPixel;
    }
  }, nTiles);
}

}
