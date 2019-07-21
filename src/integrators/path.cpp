#include <memory>
#include <nanopt/core/bsdf.h>
#include <nanopt/core/triangle.h>
#include <nanopt/core/visibilitytester.h>
#include <nanopt/lights/infinite.h>
#include <nanopt/integrators/path.h>

namespace nanopt {

Spectrum PathIntegrator::li(const Ray& ray, const Scene& scene) const {
  Ray r(ray);
  auto etaScaleFix = 1.0f;
  auto specularBounce = false;
  Spectrum l(0), beta(1), rrBeta(1);

  for (auto bounce = 0; bounce < maxDepth; ++bounce) {
    Interaction isect;
    auto foundIntersection = scene.intersect(r, isect);

    if (bounce == 0 || specularBounce) {
      if (foundIntersection)
        l += beta * isect.le(-r.d);
      else if (scene.infiniteLight)
        l += beta * scene.infiniteLight->le(r);
    }

    if (!foundIntersection) break;
    isect.computeScatteringFunctions();
    if (!isect.bsdf) break;
    l += beta * sampleOneLight(isect, scene);

    float etaScale;
    float scatteringPdf;
    Vector3f wi, wo = -r.d;
    auto f = isect.bsdf->sample(sampler.get2D(), wo, wi, scatteringPdf, etaScale);

    if (f.isBlack()) break;

    beta *= f * absdot(isect.ns, wi) / scatteringPdf;
    etaScaleFix *= etaScale;
    rrBeta = beta * etaScaleFix;
    specularBounce = isect.bsdf->isDelta();
    r = isect.spawnRay(wi);

    if (rrBeta.maxComponent() < 1.0f && bounce > 3) {
      auto q = std::max(0.05f, 1 - rrBeta.maxComponent());
      if (sampler.get1D() < q) break;
      beta /= 1 - q;
    }
  }

  return l;
}

Spectrum PathIntegrator::estimateDirect(
  const Interaction& isect,
  const Light& light,
  const Scene& scene) const {

  Vector3f wi;
  float lightPdf;
  VisibilityTester tester;
  auto li = light.sample(isect, sampler.get2D(), wi, lightPdf, tester);
  if (li.isBlack()) return Spectrum(0);

  auto ld = Spectrum(0);
  auto f = isect.bsdf->f(isect.wo, wi) * absdot(isect.ns, wi);
  if (!f.isBlack() && tester.unoccluded(scene)) {
    if (light.isDelta())
      return f * li / lightPdf;
    auto scatteringPdf = isect.bsdf->pdf(isect.wo, wi);
    ld += f * li * powerHeuristic(lightPdf, scatteringPdf) / lightPdf;
  }

  if (!light.isDelta() && !isect.bsdf->isDelta()) {
    float etaScale;
    float scatteringPdf;
    f = isect.bsdf->sample(sampler.get2D(), isect.wo, wi, scatteringPdf, etaScale);
    f *= absdot(isect.ns, wi);

    if (!f.isBlack()) {
      lightPdf = light.pdf(isect, wi);
      if (lightPdf == 0) return ld;

      Interaction lightIsect;
      auto ray = isect.spawnRay(wi);
      auto foundIntersection = scene.intersect(ray, lightIsect);
      auto li = Spectrum(0);
      if (foundIntersection) {
        if ((Light*)lightIsect.triangle->light == &light)
          li = lightIsect.le(-wi);
      } else {
        li = ((InfiniteAreaLight*)&light)->le(ray);
      }

      if (!li.isBlack()) {
        ld += f * li * powerHeuristic(scatteringPdf, lightPdf) / scatteringPdf;
      }
    }
  }

  return ld;
}

}
