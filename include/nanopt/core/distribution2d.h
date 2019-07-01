#pragma once

#include <vector>
#include <nanopt/math/vector2.h>
#include <nanopt/core/distribution1d.h>

namespace nanopt {

class Distribution2D {
public:
  Distribution2D(const float* p, int width, int height) noexcept {
    pConditional.reserve(height);
    std::unique_ptr<float[]> marginal(new float[height]);
    for (auto i = 0; i < height; ++i) {
      pConditional.emplace_back(&p[width * i], width);
      marginal[i] = pConditional[i].sum;
    }
    pMarginal = Distribution1D(marginal.get(), height);
  }

  Vector2f sampleContinuous(const Vector2f& sample, float& p) const {
    int w, h;
    float ps[2];
    auto v = pMarginal.sampleContinuous(sample[0], ps[0], h);
    auto u = pConditional[h].sampleContinuous(sample[1], ps[1], w);
    p = ps[0] * ps[1];
    return Vector2f(u, v);
  }

  float pdf(const Vector2f& p) const {
    auto w = pConditional[0].n;
    auto h = pMarginal.n;
    auto u = (int)(w * p[0]);
    auto v = (int)(h * p[1]);
    return pConditional[v].p[u] * pConditional[v].sum / pMarginal.sum * w * h;
  }

private:
  Distribution1D pMarginal;
  std::vector<Distribution1D> pConditional;
};

}
