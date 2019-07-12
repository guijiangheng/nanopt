#include <memory>
#include <nanopt/core/triangle.h>
#include <nanopt/core/visibilitytester.h>
#include <nanopt/bsdfs/diffuse.h>
#include <nanopt/integrators/path.h>
#include <nanopt/lights/infinite.h>

namespace nanopt {

Spectrum PathIntegrator::li(const Ray& ray, const Scene& scene) const {
  Ray r(ray);
  auto specularBounce = false;
  Spectrum l(0), beta(1);

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

    auto wo = -r.d;
    Vector3f wiLocal;
    Frame frame(faceForward(isect.n, wo));
    auto f = isect.bsdf->sample(sampler.get2D(), frame.toLocal(wo), wiLocal);
    specularBounce = isect.bsdf->isDelta();
    beta *= f;
    r = isect.spawnRay(frame.toWorld(wiLocal));

    if (bounce > 3) {
      auto q = std::max(0.05f, 1 - beta.maxComponent());
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
  auto n = faceForward(isect.n, isect.wo);
  auto frame = Frame(n);
  auto woLocal = frame.toLocal(isect.wo);
  auto wiLocal = frame.toLocal(wi);
  auto f = isect.bsdf->f(woLocal, wiLocal);

  if (!f.isBlack() && tester.unoccluded(scene)) {
    if (light.isDelta())
      return f * absdot(isect.n, wi) * li / lightPdf;
    auto scatteringPdf = isect.bsdf->pdf(woLocal, wiLocal);
    ld += f * absdot(isect.n, wi) * li * powerHeuristic(lightPdf, scatteringPdf) / lightPdf;
  }

  if (light.isDelta() || isect.bsdf->isDelta())
    return Spectrum(0);

  f = isect.bsdf->sample(sampler.get2D(), woLocal, wiLocal);
  wi = frame.toWorld(wiLocal);
  lightPdf = light.pdf(isect, wi);
  if (lightPdf == 0) return ld;

  Interaction lightIsect;
  auto ray = isect.spawnRay(wi);
  auto foundIntersection = scene.intersect(ray, lightIsect);

  li = Spectrum(0);
  if (foundIntersection) {
    if ((Light*)lightIsect.triangle->light == &light)
      li = lightIsect.le(-wi);
  } else {
    li = ((InfiniteAreaLight*)&light)->le(ray);
  }

  if (!li.isBlack()) {
    auto scatteringPdf = isect.bsdf->pdf(woLocal, wiLocal);
    ld += f * li * powerHeuristic(scatteringPdf, lightPdf);
  }

  return ld;
}

}
