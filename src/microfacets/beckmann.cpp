#include <nanopt/math/math.h>
#include <nanopt/microfacets/beckmann.h>

namespace nanopt {

float BeckmannDistribution::d(const Vector3f& wh) const {
  auto tan2Theta = nanopt::tan2Theta(wh);
  if (std::isinf(tan2Theta)) return 0;
  auto cos4Theta = cos2Theta(wh) * cos2Theta(wh);
  return std::exp(-tan2Theta * (cos2Theta(wh) / (alphax * alphax) +
                                sin2Theta(wh) / (alphay * alphay))) /
         (Pi * alphax * alphay * cos4Theta);
}

float BeckmannDistribution::lambda(const Vector3f& w) const {
  auto absTanTheta = std::abs(tanTheta(w));
  if (std::isinf(absTanTheta)) return 0;
  auto alpha = std::sqrt(cos2Phi(w) * alphax * alphax + sin2Phi(w) * alphay * alphay);
  auto a = 1 / (alpha * absTanTheta);
  if (a >= 1.6f) return 0;
  return (1 - 1.259f * a + 0.396f * a * a) / (3.535f * a + 2.181f * a * a);
}

Vector3f BeckmannDistribution::sample(const Vector2f& u) const {
  float phi, tan2Theta;
  auto logSample = std::log(u[0]);
  if (alphax == alphay) {
    phi = u[1] * 2 * Pi;
    tan2Theta = -alphax * alphax * logSample;
  } else {
    phi = std::atan(alphay / alphax * std::tan(2 * Pi * u[1] + 0.5f * Pi));
    if (u[1] > 0.5f) phi += Pi;
    auto sinPhi = std::sin(phi);
    auto sin2Phi = sinPhi * sinPhi;
    auto cos2Phi = 1 - sinPhi;
    tan2Theta = -logSample / (cos2Phi / (alphax * alphax) + sin2Phi / (alphay * alphay));
  }
  auto cosTheta = 1 / std::sqrt(1 + tan2Theta);
  auto sinTheta = std::sqrt(std::max(0.0f, 1 - cosTheta * cosTheta));
  return sphericalDirection(sinTheta, cosTheta, phi);
}

}
