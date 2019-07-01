#pragma once

#include <stack>
#include <memory>
#include <numeric>

namespace nanopt {

class Distribution1D {
public:
  Distribution1D() = default;

  Distribution1D(const float* func, int n);

  float sampleContinuous(float u, float& p, int& index) const;

  int sampleDiscrete(float u, float& p) const;

  float discretePdf(int index) const {
    return p[index];
  }

public:
  int n;
  float sum;
  std::unique_ptr<float[]> p;

private:
  std::unique_ptr<float[]> aliasP;
  std::unique_ptr<int[]> aliasIndex;
};

}
