#include <memory>
#include <nanopt/core/triangle.h>
#include <nanopt/core/visibilitytester.h>
#include <nanopt/bsdfs/diffuse.h>
#include <nanopt/integrators/path.h>
#include <nanopt/lights/infinite.h>

namespace nanopt {

Spectrum PathIntegrator::li(const Ray& ray, const Scene& scene) const {
  Interaction isect;
  if (scene.intersect(ray, isect)) {
    isect.computeScatteringFunctions();
    auto l = isect.le(-ray.d);
    if (isect.bsdf)
      l += sampleOneLight(isect, scene);
    return l;
  }
  return Spectrum(0);
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
    return ld;

  f = isect.bsdf->sample(sampler.get2D(), woLocal, wiLocal);
  if (f.isBlack()) return ld;

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
