#ifndef BVHUSAGE_H
#define BVHUSAGE_H

#include <glrt/dependencies.h>

namespace glrt {
namespace renderer {

enum class BvhUsage
{
  NO_BVH,
  BVH_RECURSIVE,
};

extern BvhUsage currentBvhUsage;
QMap<QString, BvhUsage> allcurrentBvhUsages();
void setCurrentBVHUsage(BvhUsage bvhUsage);

void init_bvh_shader_macros();

// FIXME set to a lower value!
const quint16 BVH_MAX_DEPTH = 256;

} // namespace renderer
} // namespace glrt


#endif // BVHUSAGE_H
