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

uint16_t bvh_traversal_leaf_result_array_length();
void set_bvh_traversal_leaf_result_array_length(uint16_t n);

extern GLSLMacroWrapper<float> SDFSAMPLING_SPHERETRACING_START;
extern GLSLMacroWrapper<float> SDFSAMPLING_SELF_SHADOW_AVOIDANCE;
extern GLSLMacroWrapper<int> SDFSAMPLING_EXPONENTIAL_NUM;
extern GLSLMacroWrapper<float> SDFSAMPLING_EXPONENTIAL_FIRST_SAMPLE;
extern GLSLMacroWrapper<float> SDFSAMPLING_EXPONENTIAL_START;
extern GLSLMacroWrapper<float> SDFSAMPLING_EXPONENTIAL_FACTOR;
extern GLSLMacroWrapper<float> SDFSAMPLING_EXPONENTIAL_OFFSET;
extern GLSLMacroWrapper<float> AO_RADIUS;
extern GLSLMacroWrapper<float> AO_STATIC_FALLBACK_FADING_START;
extern GLSLMacroWrapper<float> AO_STATIC_FALLBACK_FADING_END;
extern GLSLMacroWrapper<bool> AO_USE_CANDIDATE_GRID;
extern GLSLMacroWrapper<bool> AO_IGNORE_FALLBACK_SDF;
extern GLSLMacroWrapper<bool> AO_FALLBACK_SDF_ONLY;


extern GLSLMacroWrapper<uint16_t> N_GI_CONES;

extern GLSLMacroWrapper<uint16_t> NUM_GRID_CASCADES;
extern GLSLMacroWrapper<uint16_t> BVH_MAX_STACK_DEPTH;
extern GLSLMacroWrapper<uint16_t> HOLD_BACK_HUGE_LEAf_FROM_BVH_TREE;

// LIMIT_255
#define MAX_SDF_CANDIDATE_GRID_SIZE 32
extern GLSLMacroWrapper<uint32_t> SDF_CANDIDATE_GRID_SIZE;

#define MAX_SDF_MERGED_STATIC 512
extern VariableWithCallback<uint32_t> MERGED_STATIC_SDF_SIZE;

extern GLSLMacroWrapper<bool> AO_FALLBACK_NONE;
extern GLSLMacroWrapper<bool> AO_FALLBACK_CLAMPED;

inline uint16_t num_grid_cascades(){return NUM_GRID_CASCADES.get_value();}
inline uint16_t bvh_traversal_stack_depth(){return BVH_MAX_STACK_DEPTH.get_value();}

#define MAX_NUM_GRID_CASCADES 3
#define BVH_USE_GRID_OCCLUSION 1

} // namespace renderer
} // namespace glrt


#endif // BVHUSAGE_H
