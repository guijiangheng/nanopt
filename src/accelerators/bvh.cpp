#include <memory>
#include <atomic>
#include <algorithm>
#include <nanopt/core/parallel.h>
#include <nanopt/accelerators/bvh.h>

namespace nanopt {

struct PrimInfo {
  PrimInfo() = default;

  PrimInfo(int primIndex, const Bounds3f& bounds) noexcept
    : primIndex(primIndex)
    , bounds(bounds)
    , centroid((bounds.pMin + bounds.pMax) * 0.5f)
  { }

  int primIndex;
  Bounds3f bounds;
  Vector3f centroid;
};

struct BVHNode {
  BVHNode() = default;

  BVHNode(const Bounds3f& b, int primsOffset, int nPrims) noexcept
    : bounds(b)
    , left(nullptr), right(nullptr)
    , primsOffset(primsOffset)
    , nPrims(nPrims)
  { }

  BVHNode(int axis, BVHNode* left, BVHNode* right) noexcept
    : bounds(merge(left->bounds, right->bounds))
    , left(left) , right(right)
    , splitAxis(axis), primsOffset(0), nPrims(0)
  { }

  Bounds3f bounds;
  BVHNode *left, *right;
  int splitAxis, primsOffset, nPrims;
};

struct Bucket {
  int count = 0;
  Bounds3f bounds;
};

struct MortonPrimitive {
  int primIndex;
  std::uint32_t mortonCode;
};

struct LBVHTreelet {
  int beg;
  int end;
  BVHNode* node;
};

void BVHAccel::build(BuildMethod method) {
  auto nPrims = getPrimitiveCount();
  primIndices.resize(nPrims);
  for (auto i = 0; i < nPrims; ++i)
    primIndices[i] = i;

  auto primIndex = 0;
  std::vector<PrimInfo> primInfos;
  for (auto mesh : meshs)
    for (auto i = 0; i < mesh->nTriangles; ++i, ++primIndex)
      primInfos.emplace_back(primIndex, mesh->getBounds(i));

  int totalNodes = 0;
  std::vector<int> orderedPrims;
  orderedPrims.reserve(nPrims);
  auto root = method == BuildMethod::SAH ?
    sahBuild(primInfos, 0, nPrims, totalNodes, orderedPrims) :
    hierarchicalLinearBuild(primInfos, totalNodes, orderedPrims);
  primIndices = std::move(orderedPrims);

  nodes.reserve(totalNodes);
  flattenBVHTree(root);
  destroyBVHTree(root);
}

BVHNode* BVHAccel::createLeafNode(
    std::vector<PrimInfo>& primInfos,
    int beg,
    int end,
    int& totalNodes,
    std::vector<int>& orderedPrims) const {

  ++totalNodes;
  Bounds3f bounds;
  auto firstPrimOffset = (int)orderedPrims.size();
  for (auto i = beg; i < end; ++i) {
    bounds.merge(primInfos[i].bounds);
    orderedPrims.emplace_back(primInfos[i].primIndex);
  }

  return new BVHNode(bounds, firstPrimOffset, end - beg);
}

BVHNode* BVHAccel::exhaustBuild(
  std::vector<PrimInfo>& primInfos,
  int beg,
  int end,
  int& totalNodes,
  std::vector<int>& orderedPrims) const {

  auto nPrims = end - beg;
  if (nPrims == 1)
    return createLeafNode(primInfos, beg, end, totalNodes, orderedPrims);

  float totalAreaInv;
  Bounds3f totalBounds;
  auto totalBoundsInitialized = false;

  int splitPrim;
  int splitAxis = -1;
  float minCost = nPrims;

  for (auto axis = 0; axis < 3; ++axis) {
    std::sort(&primInfos[beg], &primInfos[end - 1] + 1, [=](auto& a, auto& b) {
      return a.centroid[axis] < b.centroid[axis];
    });

    auto rightBounds = std::make_unique<Bounds3f[]>(nPrims);
    for (auto i = nPrims - 1; i > 0; --i)
      rightBounds[i - 1] = merge(rightBounds[i], primInfos[beg + i].bounds);

    if (!totalBoundsInitialized) {
      totalBoundsInitialized = true;
      totalBounds = merge(primInfos[beg].bounds, rightBounds[0]);
      totalAreaInv = 1 / totalBounds.area();
    }

    Bounds3f leftBound;
    for (auto i = 0; i < nPrims - 1; ++i) {
      auto counts = i + 1;
      leftBound.merge(primInfos[beg + i].bounds);
      auto cost = AABB_SHAPE_INTERSECT_COST_RATIO +
        (counts * leftBound.area() + (nPrims - counts) * rightBounds[i].area()) * totalAreaInv;
      if (cost < minCost) {
        splitAxis = axis;
        splitPrim = i;
        minCost = cost;
      }
    }
  }

  if (splitAxis == -1)
    return createLeafNode(primInfos, beg, end, totalNodes, orderedPrims);

  std::sort(&primInfos[beg], &primInfos[end - 1] + 1, [=](auto& a, auto& b) {
    return a.centroid[splitAxis] < b.centroid[splitAxis];
  });

  ++totalNodes;

  return new BVHNode(
    splitAxis,
    exhaustBuild(primInfos, beg, beg + splitPrim + 1, totalNodes, orderedPrims),
    exhaustBuild(primInfos, beg + splitPrim + 1, end, totalNodes, orderedPrims)
  );
}

BVHNode* BVHAccel::sahBuild(
  std::vector<PrimInfo>& primInfos,
  int beg,
  int end,
  int& totalNodes,
  std::vector<int>& orderedPrims) const {

  auto nPrims = end - beg;
  if (nPrims == 1)
    return createLeafNode(primInfos, beg, end, totalNodes, orderedPrims);

  if (nPrims < SAH_APPLY_COUNT)
    return exhaustBuild(primInfos, beg, end, totalNodes, orderedPrims);

  Bounds3f centroidBounds;
  for (auto i = beg; i < end; ++i)
    centroidBounds.merge(primInfos[i].centroid);
  int dim = centroidBounds.maxExtent();

  if (centroidBounds.pMax[dim] - centroidBounds.pMin[dim] < 0.00001f)
    return createLeafNode(primInfos, beg, end, totalNodes, orderedPrims);

  Bucket buckets[BUCKETS];
  auto inv = 1 / (centroidBounds.pMax[dim] - centroidBounds.pMin[dim]);
  for (auto i = beg; i < end; ++i) {
    auto offset = primInfos[i].centroid[dim] - centroidBounds.pMin[dim];
    auto b = (int)(BUCKETS * offset * inv);
    if (b == BUCKETS) --b;
    ++buckets[b].count;
    buckets[b].bounds.merge(primInfos[i].bounds);
  }

  Bounds3f rightBounds[BUCKETS];
  for (auto i = BUCKETS - 2; i >= 0; --i)
    rightBounds[i] = merge(rightBounds[i + 1], buckets[i + 1].bounds);
  Bounds3f totalBounds = merge(rightBounds[0], buckets[0].bounds);
  auto totalAreaInv = 1 / totalBounds.area();

  int splitBucket = -1;
  int counts = 0;
  float minCost = nPrims;
  Bounds3f leftBound;

  for (auto i = 0; i < BUCKETS - 1; ++i) {
    counts += buckets[i].count;
    leftBound.merge(buckets[i].bounds);
    float cost = AABB_SHAPE_INTERSECT_COST_RATIO +
      (counts * leftBound.area() + (nPrims - counts) * rightBounds[i].area()) * totalAreaInv;
    if (cost < minCost) {
      splitBucket = i;
      minCost = cost;
    }
  }

  if (splitBucket == -1)
    return exhaustBuild(primInfos, beg, end, totalNodes, orderedPrims);

  auto pmid = std::partition(&primInfos[beg], &primInfos[end - 1] + 1, [=](auto& p) {
    auto offset = p.centroid[dim] - centroidBounds.pMin[dim];
    auto b = (int)(BUCKETS * offset * inv);
    if (b == BUCKETS)  b = BUCKETS - 1;
    return b <= splitBucket;
  });
  auto mid = pmid - &primInfos[0];

  ++totalNodes;

  return new BVHNode(
    dim,
    sahBuild(primInfos, beg, mid, totalNodes, orderedPrims),
    sahBuild(primInfos, mid, end, totalNodes, orderedPrims)
  );
}

inline std::uint32_t leftShift3(std::uint32_t x) {
  if (x == (1 << 10)) --x;
  x = (x | (x << 16)) & 0b00000011000000000000000011111111;
  x = (x | (x <<  8)) & 0b00000011000000001111000000001111;
  x = (x | (x <<  4)) & 0b00000011000011000011000011000011;
  x = (x | (x <<  2)) & 0b00001001001001001001001001001001;
  return x;
}

std::uint32_t encodeMorton3(const Vector3f& p) {
  return
    leftShift3((std::uint32_t)p.z) << 2 |
    leftShift3((std::uint32_t)p.y) << 1 |
    leftShift3((std::uint32_t)p.x);
}

static void radixSort(std::vector<MortonPrimitive>& mortonPrims) {
  constexpr auto bitsPerPass = 6;
  constexpr auto nBits = 30;
  constexpr auto nPass = nBits / bitsPerPass;

  std::vector<MortonPrimitive> tmp(mortonPrims.size());
  auto in = &tmp;
  auto out = &mortonPrims;

  for (auto pass = 0; pass < nPass; ++pass) {
    std::swap(in, out);

    auto lowBit = pass * bitsPerPass;
    constexpr auto nBuckets = 1 << bitsPerPass;
    int bucketCount[nBuckets] = { 0 };
    constexpr auto mask = (1 << bitsPerPass) - 1;

    for (auto& p : *in) {
      auto bucket = (p.mortonCode >> lowBit) & mask;
      ++bucketCount[bucket];
    }

    int outIndex[nBuckets];
    outIndex[0] = 0;
    for (auto i = 1; i < nBuckets; ++i)
      outIndex[i] = outIndex[i - 1] + bucketCount[i - 1];

    for (auto& p : *in) {
      auto bucket = (p.mortonCode >> lowBit) & mask;
      (*out)[outIndex[bucket]++] = p;
    }
  }

  std::swap(mortonPrims, tmp);
}

BVHNode* BVHAccel::exhaustBuildUpper(
  std::vector<BVHNode*>& treelets,
  int beg,
  int end,
  int& totalNodes) const {

  auto nNodes = end - beg;
  if (nNodes == 1) return treelets[beg];

  float totalAreaInv;
  Bounds3f totalBounds;
  auto totalBoundsInitialized = false;
  auto splitNode = 0;
  auto splitAxis = 0;
  auto minCost = (float)nNodes;

  for (auto axis = 0; axis < 3; ++axis) {
    std::sort(&treelets[beg], &treelets[end - 1] + 1, [=](auto a, auto b) {
      return a->bounds.centroid()[axis] < b->bounds.centroid()[axis];
    });

    auto rightBounds = std::make_unique<Bounds3f[]>(nNodes);
    for (auto i = nNodes - 1; i > 0; --i)
      rightBounds[i - 1] = merge(rightBounds[i], treelets[beg + i]->bounds);

    if (!totalBoundsInitialized) {
      totalBoundsInitialized = true;
      totalBounds = merge(treelets[beg]->bounds, rightBounds[0]);
      totalAreaInv = 1 / totalBounds.area();
    }

    Bounds3f leftBounds;
    for (auto i = 0; i < nNodes - 1; ++i) {
      auto counts = i + 1;
      leftBounds.merge(treelets[beg + i]->bounds);
      auto cost = AABB_SHAPE_INTERSECT_COST_RATIO +
        (counts * leftBounds.area() + (nNodes - counts) * rightBounds[i].area()) * totalAreaInv;
      if (cost < minCost) {
        splitAxis = axis;
        splitNode = i;
        minCost = cost;
      }
    }
  }

  std::sort(&treelets[beg], &treelets[end - 1] + 1, [&](auto a, auto b) {
    return a->bounds.centroid()[splitAxis] < b->bounds.centroid()[splitAxis];
  });

  ++totalNodes;

  return new BVHNode(
    splitAxis,
    exhaustBuildUpper(treelets, beg, beg + splitNode + 1, totalNodes),
    exhaustBuildUpper(treelets, beg + splitNode + 1, end, totalNodes)
  );
}

BVHNode* BVHAccel::buildTreelet(
  std::vector<PrimInfo>& primInfos,
  std::vector<MortonPrimitive>& mortonPrims,
  int beg,
  int end,
  int& totalNodes,
  std::atomic<int>& orderedPrimsOffset,
  std::vector<int>& orderedPrims,
  int bitIndex) const {

  auto nPrims = end - beg;
  if (bitIndex == -1 || nPrims <= 4) {
    ++totalNodes;
    Bounds3f bounds;
    auto firstPrimOffset = orderedPrimsOffset.fetch_add(nPrims);
    auto orderedPrimsIndex = firstPrimOffset;
    for (auto i = beg; i < end; ++i) {
      auto primIndex = mortonPrims[i].primIndex;
      bounds.merge(primInfos[primIndex].bounds);
      orderedPrims[orderedPrimsIndex++] = primIndex;
    }
    return new BVHNode(bounds, firstPrimOffset, nPrims);
  }

  auto mask = 1 << bitIndex;
  if ((mortonPrims[beg].mortonCode & mask) == (mortonPrims[end - 1].mortonCode & mask))
    return buildTreelet(
      primInfos, mortonPrims,
      beg, end, totalNodes,
      orderedPrimsOffset, orderedPrims, bitIndex - 1);

  auto searchBeg = beg + 1;
  auto searchEnd = end - 1;
  while (searchBeg != searchEnd) {
    auto mid = (searchBeg + searchEnd) / 2;
    if ((mortonPrims[beg].mortonCode & mask) == (mortonPrims[mid].mortonCode & mask))
      searchBeg = mid + 1;
    else
      searchEnd = mid;
  }

  ++totalNodes;
  auto splitOffset = searchBeg;
  auto splitAxis = bitIndex % 3;

  return new BVHNode(
    splitAxis,
    buildTreelet(
      primInfos, mortonPrims,
      beg, splitOffset, totalNodes,
      orderedPrimsOffset, orderedPrims, bitIndex - 1),
    buildTreelet(
      primInfos, mortonPrims,
      splitOffset, end, totalNodes,
      orderedPrimsOffset, orderedPrims, bitIndex - 1)
  );
}

BVHNode* BVHAccel::buildUpperSAH(
  std::vector<BVHNode*>& treelets,
  int beg,
  int end,
  int& totalNodes) const {

  auto nNodes = end - beg;
  if (nNodes == 1) return treelets[beg];

  if (nNodes < SAH_APPLY_COUNT)
    return exhaustBuildUpper(treelets, beg, end, totalNodes);

  Bounds3f centroidBounds;
  for (auto i = beg; i < end; ++i)
    centroidBounds.merge(treelets[i]->bounds.centroid());
  auto dim = centroidBounds.maxExtent();

  Bucket buckets[BUCKETS];
  auto inv = 1 / (centroidBounds.pMax[dim] - centroidBounds.pMin[dim]);
  for (auto i = beg; i < end; ++i) {
    auto centroid = treelets[i]->bounds.centroid();
    auto offset = centroid[dim] - centroidBounds.pMin[dim];
    auto b = (int)(BUCKETS * offset * inv);
    if (b == BUCKETS) --b;
    ++buckets[b].count;
    buckets[b].bounds.merge(treelets[i]->bounds);
  }

  Bounds3f rightBounds[BUCKETS];
  for (auto i = BUCKETS - 2; i >= 0; --i)
    rightBounds[i] = merge(rightBounds[i + 1], buckets[i + 1].bounds);
  auto totalBounds = merge(rightBounds[0], buckets[0].bounds);
  auto totalAreaInv = 1 / totalBounds.area();

  auto splitBucket = 0;
  auto counts = 0;
  auto minCost = (float)nNodes;
  Bounds3f leftBounds;

  for (auto i = 0; i < BUCKETS - 1; ++i) {
    counts += buckets[i].count;
    leftBounds.merge(buckets[i].bounds);
    auto cost = AABB_SHAPE_INTERSECT_COST_RATIO +
      (counts * leftBounds.area() + (nNodes - counts) * rightBounds[i].area()) * totalAreaInv;
    if (cost < minCost) {
      splitBucket = i;
      minCost = cost;
    }
  }

  auto pmid = std::partition(&treelets[beg], &treelets[end - 1] + 1, [=](auto treelet) {
    auto centroid = treelet->bounds.centroid();
    auto offset = centroid[dim] - centroidBounds.pMin[dim];
    auto b = (int)(BUCKETS * offset * inv);
    if (b == BUCKETS) --b;
    return b <= splitBucket;
  });
  auto mid = pmid - &treelets[0];

  ++totalNodes;

  return new BVHNode(
    dim,
    buildUpperSAH(treelets, beg, mid, totalNodes),
    buildUpperSAH(treelets, mid, end, totalNodes)
  );
}

BVHNode* BVHAccel::hierarchicalLinearBuild(
  std::vector<PrimInfo>& primInfos,
  int& totalNodes,
  std::vector<int>& orderedPrims) const {

  Bounds3f bounds;
  for (auto& p : primInfos)
    bounds.merge(p.centroid);

  auto nPrims = (int)primInfos.size();
  std::vector<MortonPrimitive> mortonPrims(nPrims);
  parallelFor([&](int i) {
    constexpr auto mortonScale = (float)(1 << 10);
    auto centroidOffset = bounds.offset(primInfos[i].centroid);
    mortonPrims[i] = { i, encodeMorton3(centroidOffset * mortonScale) };
  }, nPrims, 512);

  radixSort(mortonPrims);

  std::vector<LBVHTreelet> treeletsToBuild;
  auto beg = 0;
  auto end = 1;
  constexpr auto mask = 0b00111111111111000000000000000000;
  for (; end < nPrims; ++end) {
    if ((mortonPrims[beg].mortonCode & mask) !=
        (mortonPrims[end].mortonCode & mask)) {
      treeletsToBuild.push_back({ beg, end, nullptr });
      beg = end;
    }
  }
  treeletsToBuild.push_back({ beg, end, nullptr });

  auto nTreelets = treeletsToBuild.size();
  orderedPrims.reserve(nPrims);
  std::atomic<int> atomicTotalNodes(0);
  std::atomic<int> orderedPrimsOffset(0);
  parallelFor([&](int i) {
    auto nodesCreated = 0;
    constexpr auto firstBitIndex = 29 - 12;
    auto& treelet = treeletsToBuild[i];
    treelet.node = buildTreelet(
      primInfos, mortonPrims,
      treelet.beg, treelet.end, nodesCreated,
      orderedPrimsOffset, orderedPrims, firstBitIndex);
    atomicTotalNodes += nodesCreated;
  }, nTreelets);
  totalNodes = atomicTotalNodes;

  std::vector<BVHNode*> treelets;
  treelets.reserve(nTreelets);
  for (auto& treelet : treeletsToBuild)
    treelets.push_back(treelet.node);

  return buildUpperSAH(treelets, 0, nTreelets, totalNodes);
}

void BVHAccel::destroyBVHTree(const BVHNode* node) const {
  auto nPrims = node->nPrims;
  auto left = node->left;
  auto right = node->right;
  delete node;
  if (nPrims == 0) {
    destroyBVHTree(left);
    destroyBVHTree(right);
  }
}

void BVHAccel::flattenBVHTree(const BVHNode* node) {
  if (node->nPrims) {
    nodes.emplace_back(node->bounds, node->primsOffset, (uint16_t)node->nPrims);
  } else {
    nodes.emplace_back(node->bounds, node->splitAxis);
    auto& linearNode = nodes.back();
    flattenBVHTree(node->left);
    linearNode.rightChild = (int)nodes.size();
    flattenBVHTree(node->right);
  }
}

bool BVHAccel::intersect(const Ray& ray, Interaction& isect) const {
  Vector3f invDir(1 / ray.d.x, 1 / ray.d.y, 1 / ray.d.z);
  const int dirIsNeg[3] = { invDir.x < 0, invDir.y < 0, invDir.z < 0 };

  int index;
  auto hit = false;
  int nodesToVisit[64];
  nodesToVisit[0] = 0;
  int currentIndex, toVisitOffset = 0;

  while (toVisitOffset != -1) {
    currentIndex = nodesToVisit[toVisitOffset--];
    auto& node = nodes[currentIndex];
    if (node.bounds.intersect(ray, invDir, dirIsNeg)) {
      if (node.nPrims) {
        for (auto i = 0; i < node.nPrims; ++i) {
          auto triIndex = primIndices[node.primsOffset + i];
          auto mesh = findMesh(triIndex);
          if (mesh->intersect(triIndex, ray, isect)) {
            index = triIndex;
            hit = true;
            isect.mesh = mesh;
          }
        }
      } else {
        if (dirIsNeg[node.splitAxis]) {
          nodesToVisit[++toVisitOffset] = currentIndex + 1;
          nodesToVisit[++toVisitOffset] = node.rightChild;
        } else {
          nodesToVisit[++toVisitOffset] = node.rightChild;
          nodesToVisit[++toVisitOffset] = currentIndex + 1;
        }
      }
    }
  }

  if (hit) {
    isect.mesh->computeIntersection(index, isect);
    isect.wo = -ray.d;
  }

  return hit;
}

bool BVHAccel::intersect(const Ray& ray) const {
  Vector3f invDir(1 / ray.d.x, 1 / ray.d.y, 1 / ray.d.z);
  const int dirIsNeg[3] = { invDir.x < 0, invDir.y < 0, invDir.z < 0 };

  int nodesToVisit[64];
  nodesToVisit[0] = 0;
  int currentIndex, toVisitOffset = 0;

  while (toVisitOffset != -1) {
    currentIndex = nodesToVisit[toVisitOffset--];
    auto& node = nodes[currentIndex];
    if (node.bounds.intersect(ray, invDir, dirIsNeg)) {
      if (node.nPrims) {
        for (auto i = 0; i < node.nPrims; ++i) {
          auto triIndex = primIndices[node.primsOffset + i];
          auto mesh = findMesh(triIndex);
          if (mesh->intersect(triIndex, ray))
            return true;
        }
      } else {
        if (dirIsNeg[node.splitAxis]) {
          nodesToVisit[++toVisitOffset] = currentIndex + 1;
          nodesToVisit[++toVisitOffset] = node.rightChild;
        } else {
          nodesToVisit[++toVisitOffset] = node.rightChild;
          nodesToVisit[++toVisitOffset] = currentIndex + 1;
        }
      }
    }
  }

  return false;
}

}
