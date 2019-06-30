#pragma once

#include <memory>

namespace nanopt {

class Distribution1D {
public:
  Distribution1D() = default;

  Distribution1D(const float* func, int n) noexcept
      : p(new float[n])
      , aliasP(new float[n])
      , aliasIndex(new int[n]) {
  }

private:
  std::unique_ptr<float[]> p;
  std::unique_ptr<float> aliasP;
  std::unique_ptr<int> aliasIndex;
};

}
