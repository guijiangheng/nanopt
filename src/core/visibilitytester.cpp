#include <nanopt/core/scene.h>
#include <nanopt/core/visibilitytester.h>

namespace nanopt {

bool VisibilityTester::unoccluded(const Scene& scene) const {
  return !scene.intersect(ref->spawnRayTo(target));
}

}
