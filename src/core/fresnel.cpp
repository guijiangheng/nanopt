#include <nanopt/core/fresnel.h>

namespace nanopt {

float frDielectric(float cosThetaI, float eta) {
  if (cosThetaI < 0) {
    eta = 1 / eta;
    cosThetaI = -cosThetaI;
  }

  auto sinThetaI = std::sqrt(std::max(0.0f, 1 - cosThetaI * cosThetaI));
  auto sinThetaT = sinThetaI / eta;

  if (sinThetaT >= 1) return 1;

  auto cosThetaT = std::sqrt(std::max(0.0f, 1 - sinThetaT * sinThetaT));
  auto rPral = (eta * cosThetaI - cosThetaT) / (eta * cosThetaI + cosThetaT);
  auto rPerp = (cosThetaI - eta * cosThetaT) / (cosThetaI + eta * cosThetaT);

  return (rPral * rPral + rPerp * rPerp) / 2;
}

Spectrum frConductor(
  float cosThetaI,
  const Spectrum& etaI,
  const Spectrum& etaT,
  const Spectrum& k) {

  auto eta = etaT / etaI;
  auto etaK = k / etaI;

  auto cosThetaI2 = cosThetaI * cosThetaI;
  auto sinThetaI2 = 1 - cosThetaI2;
  auto eta2 = eta * eta;
  auto etaK2 = etaK * etaK;

  auto t0 = eta2 - etaK2 - Spectrum(sinThetaI2);
  auto a2plusb2 = sqrt(t0 * t0 + eta2 * etaK2 * 4);
  auto t1 = a2plusb2 + Spectrum(cosThetaI2);
  auto a = sqrt((a2plusb2 + t0) * 0.5f);
  auto t2 = a * cosThetaI * 2;
  auto rs = (t1 - t2) / (t1 + t2);

  auto t3 = a2plusb2 * cosThetaI2 + Spectrum(sinThetaI2 * sinThetaI2);
  auto t4 = t2 * sinThetaI2;
  auto rp = rs * (t3 - t4) / (t3 + t4);

  return (rp + rs) / 2;
}

}
