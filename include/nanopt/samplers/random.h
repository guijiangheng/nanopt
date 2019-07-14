#pragma once

#include <random>
#include <nanopt/core/sampler.h>

namespace nanopt {

class RandomSampler : public Sampler {
public:
  explicit RandomSampler(std::int64_t samplesPerPixel) noexcept
    : Sampler(samplesPerPixel)
    , device()
    , generator(device())
    , distribution(0, 1)
  { }

  std::unique_ptr<Sampler> clone(int seed) const override {
    auto sampler = new RandomSampler(samplesPerPixel);
    sampler->generator.seed(seed);
    return std::unique_ptr<Sampler>(sampler);
  }

  float get1D() override {
    return distribution(generator);
  }

  Vector2f get2D() override {
    return Vector2f(
      distribution(generator),
      distribution(generator)
    );
  }

private:
  std::random_device device;
  std::mt19937_64 generator;
  std::uniform_real_distribution<float> distribution;
};

}
