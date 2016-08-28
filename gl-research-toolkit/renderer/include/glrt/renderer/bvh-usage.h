#ifndef BVHUSAGE_H
#define BVHUSAGE_H

#include <glrt/dependencies.h>

namespace glrt {
namespace renderer {

enum class BvhUsage : quint32
{
  NO_BVH,         // Just go linear over all BVH leaves
  BVH_WITH_STACK, // All SDFs within a single BVH-Tree

  // bvh grid usages with r16ui must be >=1024 and <1536
  // bvh grid usages with rgba16ui must be >=1536 and <2048
  BVH_GRID_NEAREST_LEAF = 1024,
  BVH_GRID_NEAREST_LEAF_WITH_HEURISTIC_FOR_REST,
  BVH_GRID_NEAREST_FOUR_LEAVES = 1536,
  BVH_GRID_NEAREST_FOUR_LEAVES_WITH_HEURISTIC_FOR_REST,
};

extern BvhUsage currentBvhUsage;

QMap<QString, BvhUsage> allcurrentBvhUsages();
void setCurrentBVHUsage(BvhUsage bvhUsage);

void init_bvh_shader_macros();

// FIXME set to a lower value!
const quint16 MAX_NUM_STATIC_MESHES = 255;
const quint16 BVH_MAX_STACK_DEPTH = MAX_NUM_STATIC_MESHES;
const quint16 BVH_MAX_VISITED_LEAVES = MAX_NUM_STATIC_MESHES;

inline bool bvh_is_grid(BvhUsage usage)
{
  const quint32 _usage = quint32(usage);

  return _usage>=1024 && _usage<2048;
}

inline bool bvh_is_grid_with_four_components(BvhUsage usage)
{
  const quint32 _usage = quint32(usage);

  return _usage>=1536 && _usage<2048;
}

inline bool isUsingBvhLeafGrid()
{
  return bvh_is_grid(currentBvhUsage);
}

#define NUM_GRID_CASCADES 3

} // namespace renderer
} // namespace glrt


#endif // BVHUSAGE_H
