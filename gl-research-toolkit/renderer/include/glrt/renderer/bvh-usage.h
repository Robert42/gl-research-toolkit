#ifndef BVHUSAGE_H
#define BVHUSAGE_H

#include <glrt/renderer/toolkit/glsl-macro-wrapper.h>

namespace glrt {
namespace renderer {

enum class BvhUsage : quint32
{
  NO_BVH,         // Just go linear over all BVH leaves
  BVH_WITH_STACK, // All SDFs within a single BVH-Tree

  // bvh grid usages with r16ui must be >=1024 and <1536
  // bvh grid usages with rgba16ui must be >=1536 and <2048
  BVH_GRID_NEAREST_LEAF = 1024,
  BVH_GRID_NEAREST_LEAF_UNCLAMPED_OCCLUSION,
  BVH_GRID_NEAREST_FOUR_LEAVES = 1536,
  BVH_GRID_NEAREST_FOUR_LEAVES_UNCLAMPED_OCCLUSION,
};

extern BvhUsage currentBvhUsage;

QMap<QString, BvhUsage> allcurrentBvhUsages();
void setCurrentBVHUsage(BvhUsage bvhUsage);

void init_bvh_shader_macros();

constexpr const quint16 MAX_NUM_STATIC_MESHES = 255;

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

inline bool bvh_is_occlusion_grid_unclamped(BvhUsage usage)
{
  const quint32 _usage = quint32(usage);
  return bvh_is_grid(currentBvhUsage) && (_usage&1)!=0;
}

inline bool isUsingBvhLeafGrid()
{
  return bvh_is_grid(currentBvhUsage);
}

uint16_t num_grid_cascades();
void set_num_grid_cascades(uint16_t n);

uint16_t bvh_traversal_stack_depth();
void set_bvh_traversal_stack_depth(uint16_t n);

uint16_t bvh_traversal_leaf_result_array_length();
void set_bvh_traversal_leaf_result_array_length(uint16_t n);

extern GLSLMacroWrapper<float> SDFSAMPLING_SPHERETRACING_START;
extern GLSLMacroWrapper<float> SDFSAMPLING_SELF_SHADOW_AVOIDANCE;
extern GLSLMacroWrapper<int> SDFSAMPLING_EXPONENTIAL_NUM;
extern GLSLMacroWrapper<float> SDFSAMPLING_EXPONENTIAL_START;
extern GLSLMacroWrapper<float> SDFSAMPLING_EXPONENTIAL_FACTOR;
extern GLSLMacroWrapper<float> SDFSAMPLING_EXPONENTIAL_OFFSET;

#define MAX_NUM_GRID_CASCADES 3
#define BVH_USE_GRID_OCCLUSION 1

} // namespace renderer
} // namespace glrt


#endif // BVHUSAGE_H
