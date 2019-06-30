#pragma once

#include <functional>
#include <nanopt/math/vector2.h>

namespace nanopt {

void parallelInit();
void parallelCleanup();
void parallelFor1D(std::function<void(int64_t)> func, std::int64_t count, int chunkSize);
void parallelFor2D(std::function<void(const Vector2i&)> func, const Vector2i& count);

}
