#include <nanopt/core/distribution1d.h>

namespace nanopt {

Distribution1D::Distribution1D(const float* func, int n) {
  this->n = n;
  sum = std::accumulate(func, func + n, 0.0f);
  auto inv = 1 / sum;

  std::stack<float> low, high;
  p.reset(new float[n]);
  aliasP.reset(new float[n]);
  aliasIndex.reset(new int[n]);

  for (auto i = 0; i < n; ++i) {
    p[i] = func[i] * inv;
    aliasP[i] = func[i] * n;
    if (aliasP[i] < 1) low.push(i);
    else if (aliasP[i] > 1) high.push(i);
  }

  while (!low.empty()) {
    auto l = low.top();
    auto h = high.top();
    low.pop();
    aliasIndex[l] = h;
    aliasP[h] = aliasP[l] + aliasP[h] - 1;
    if (aliasP[h] == 1) high.pop();
    else if (aliasP[h] < 1) {
      high.pop();
      low.push(h);
    }
  }
}

float Distribution1D::sampleContinuous(float u, float& p, int& index) const {
  u *= n;
  index = (int)u;
  auto uRemapped = u - index;
  if (uRemapped < aliasP[index]) {
    uRemapped /= aliasP[index];
  } else {
    uRemapped = (uRemapped - aliasP[index]) / (1 - aliasP[index]);
    index = aliasIndex[index];
  }
  p = this->p[index] * n;
  return (index + uRemapped) / n;
}

int Distribution1D::sampleDiscrete(float u, float& p) const {
  u *= n;
  auto index = (int)u;
  auto uRemapped = u - index;
  if (uRemapped > aliasP[index])
    index = aliasIndex[index];
  p = this->p[index];
  return index;
}

}
