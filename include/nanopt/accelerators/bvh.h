#pragma once

#include <cstdint>
#include <vector>
#include <nanopt/core/accel.h>
#include <nanopt/core/triangle.h>

namespace nanopt {

struct BVHNode;
struct PrimInfo;

struct LinearBVHNode {
  Bounds3f bounds;
  std::uint16_t nPrims;
  std::uint16_t splitAxis;
  union {
    int primsOffset;
    int rightChild;
  };

  LinearBVHNode(const Bounds3f& bounds, int primsOffset, std::uint16_t nPrims)
    : bounds(bounds), nPrims(nPrims), primsOffset(primsOffset)
  { }

  LinearBVHNode(const Bounds3f& bounds, std::uint16_t splitAxis)
    : bounds(bounds), nPrims(0), splitAxis(splitAxis)
  { }
};

class BVHAccel : public Accelerator {
public:
  BVHAccel(std::vector<Triangle>&& triangles) noexcept;

  Bounds3f getBounds() const override {
    return nodes[0].bounds;
  }

  bool intersect(const Ray& ray) const override;

  bool intersect(const Ray& ray, Interaction& isect) const override;

private:
  BVHNode* createLeafNode(
    std::vector<PrimInfo>& primInfos,
    int start, int end, int& totalNodes,
    std::vector<Triangle>& orderedPrims) const;

  BVHNode* exhaustBuild(
    std::vector<PrimInfo>& primInfos,
    int start, int end, int& totalNodes,
    std::vector<Triangle>& orderedPrims) const;

  BVHNode* sahBuild(
    std::vector<PrimInfo>& primInfos,
    int start, int end, int& totalNodes,
    std::vector<Triangle>& orderedPrims) const;

  void destroyBVHTree(const BVHNode* node) const;

  void flattenBVHTree(const BVHNode* node);

private:
  std::vector<Triangle> triangles;
  std::vector<LinearBVHNode> nodes;
  static constexpr int BUCKETS = 16;
  static constexpr int SAH_APPLY_COUNT = 32;
  static constexpr float AABB_SHAPE_INTERSECT_COST_RATIO = 1.0f / 4;
};

}
