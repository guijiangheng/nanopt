#pragma once

#include <nanopt/core/interaction.h>

namespace nanopt {

class Scene;

class VisibilityTester {
public:
  VisibilityTester() = default;

  VisibilityTester(const Interaction& ref, const Vector3f& target) noexcept
    : ref(&ref), target(target)
  { }

  bool unoccluded(const Scene& scene) const;

private:
  const Interaction* ref;
  const Vector3f target;
};

}
