#pragma once

#include <nanopt/core/ray.h>

namespace nanopt {

struct  Interaction {
  Vector3f p;
  Vector3f n;
  Vector2f uv;
  Vector3f wo;
};

}
