#include <memory>
#include <nanopt/bsdfs/diffuse.h>
#include <nanopt/integrators/path.h>
#include <nanopt/core/visibilitytester.h>

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

  float pdf;
  Vector3f wi;
  VisibilityTester tester;
  auto li = light.sample(isect, sampler.get2D(), wi, pdf, tester);

  if (li.isBlack()) return Spectrum(0);

  Frame frame(faceForward(isect.n, isect.wo));
  auto f = isect.bsdf->f(frame.toLocal(isect.wo), frame.toLocal(wi));

  if (!f.isBlack() && tester.unoccluded(scene))
    return f * absdot(isect.n, wi) * li / pdf;

  return Spectrum(0);
}

}
