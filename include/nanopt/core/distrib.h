#pragma once

#include <vector>
#include <algorithm>

namespace nanopt {

class Distribution1D {
public:
  explicit Distribution1D(int n) noexcept {
    cdf.reserve(n + 1);
    cdf.push_back(0);
  }

  void append(float pdf) {
    cdf.push_back(cdf.back() + pdf);
  }

  float normalize() {
    auto sum = cdf.back();
    auto sumInv = 1 / sum;
    for (auto i = 1, n = (int)cdf.size(); i < n; ++i)
      cdf[i] *= sumInv;
    cdf.back() = 1.0f;
    return sum;
  }

  int sample(float u, float& pdf) const {
    auto itr = std::lower_bound(cdf.begin(), cdf.end(), u);
    auto index = itr - cdf.begin();
    pdf = cdf[index] - cdf[index - 1];
    return (int)(index - 1);
  }

private:
  std::vector<float> cdf;
};

}
