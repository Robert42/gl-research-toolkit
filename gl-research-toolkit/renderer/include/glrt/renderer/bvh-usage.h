#ifndef BVHUSAGE_H
#define BVHUSAGE_H

#include <glrt/dependencies.h>

namespace glrt {
namespace renderer {

enum class BvhUsage
{
  NO_BVH,         // Just go linear over all BVH leaves
  BVH_WITH_STACK, // All SDFs within a single BVH-Tree
};

extern BvhUsage currentBvhUsage;
QMap<QString, BvhUsage> allcurrentBvhUsages();
void setCurrentBVHUsage(BvhUsage bvhUsage);

void init_bvh_shader_macros();

// FIXME set to a lower value!
const quint16 MAX_NUM_STATIC_MESHES = 256;
const quint16 BVH_MAX_STACK_DEPTH = MAX_NUM_STATIC_MESHES;
const quint16 BVH_MAX_VISITED_LEAVES = MAX_NUM_STATIC_MESHES;

} // namespace renderer
} // namespace glrt


#endif // BVHUSAGE_H
