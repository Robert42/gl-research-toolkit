#include <glrt/renderer/bvh-usage.h>
#include <glrt/renderer/toolkit/reloadable-shader.h>
#include <glrt/scene/scene-data.h>

namespace glrt {
namespace renderer {

extern uint16_t _bvh_traversal_leaf_result_array_length;

GLSLMacroWrapper<float> SDFSAMPLING_SPHERETRACING_START("#define SDFSAMPLING_SPHERETRACING_START %0", 0.0f);
GLSLMacroWrapper<float> SDFSAMPLING_SELF_SHADOW_AVOIDANCE("#define SDFSAMPLING_SELF_SHADOW_AVOIDANCE %0", 1.0f);
GLSLMacroWrapper<int> SDFSAMPLING_EXPONENTIAL_NUM("#define SDFSAMPLING_EXPONENTIAL_NUM %0", 4);
GLSLMacroWrapper<float> SDFSAMPLING_EXPONENTIAL_FIRST_SAMPLE("#define SDFSAMPLING_EXPONENTIAL_FIRST_SAMPLE %0", 1.f / 16.f);
GLSLMacroWrapper<float> SDFSAMPLING_EXPONENTIAL_START("#define SDFSAMPLING_EXPONENTIAL_START %0", 0.25f);
GLSLMacroWrapper<float> SDFSAMPLING_EXPONENTIAL_FACTOR("#define SDFSAMPLING_EXPONENTIAL_FACTOR %0", 2.f);
GLSLMacroWrapper<float> SDFSAMPLING_EXPONENTIAL_OFFSET("#define SDFSAMPLING_EXPONENTIAL_OFFSET %0", 0.f);
GLSLMacroWrapper<float> AO_RADIUS("#define AO_RADIUS %0", 3.5f);
GLSLMacroWrapper<uint16_t> N_GI_CONES("#define N_GI_CONES %0", 9);

GLSLMacroWrapper<uint16_t> NUM_GRID_CASCADES("#define NUM_GRID_CASCADES %0", 3);
GLSLMacroWrapper<uint16_t> BVH_MAX_STACK_DEPTH("#define BVH_MAX_STACK_DEPTH %0", MAX_NUM_STATIC_MESHES);
GLSLMacroWrapper<uint16_t> HOLD_BACK_HUGE_LEAf_FROM_BVH_TREE("#define HOLD_BACK_HUGE_LEAf_FROM_BVH_TREE %0", 0);

GLSLMacroWrapper<bool> AO_FALLBACK_NONE("#define AO_FALLBACK_NONE %0", false);
GLSLMacroWrapper<bool> AO_FALLBACK_CLAMPED("#define AO_FALLBACK_CLAMPED %0", false);

// TODO:: replace the following with GLSLMacroWrappers
BvhUsage currentBvhUsage = BvhUsage::BVH_WITH_STACK;
uint16_t _bvh_traversal_leaf_result_array_length = 0;

uint16_t bvh_traversal_leaf_result_array_length()
{
  return _bvh_traversal_leaf_result_array_length;
}

void set_bvh_traversal_leaf_result_array_length(uint16_t n)
{
  Q_ASSERT(n>=1);
  if(_bvh_traversal_leaf_result_array_length == n)
    return;

  ReloadableShader::globalPreprocessorBlock.remove(QString("#define BVH_MAX_VISITED_LEAVES %0").arg(_bvh_traversal_leaf_result_array_length));
  _bvh_traversal_leaf_result_array_length = n;
  ReloadableShader::globalPreprocessorBlock.insert(QString("#define BVH_MAX_VISITED_LEAVES %0").arg(_bvh_traversal_leaf_result_array_length));

  ReloadableShader::reloadAll();
}

QMap<QString, BvhUsage> allcurrentBvhUsages()
{
  QMap<QString, BvhUsage> map;

#define VALUE(MACRO) map[#MACRO] = BvhUsage::MACRO

  VALUE(NO_BVH);
  VALUE(BVH_WITH_STACK);
  VALUE(BVH_GRID_NEAREST_LEAF);
  VALUE(BVH_GRID_NEAREST_FOUR_LEAVES);
  VALUE(BVH_GRID_NEAREST_LEAF_UNCLAMPED_OCCLUSION);
  VALUE(BVH_GRID_NEAREST_FOUR_LEAVES_UNCLAMPED_OCCLUSION);

  return map;
}

void setCurrentBVHUsage(BvhUsage bvhUsage)
{
  if(currentBvhUsage == bvhUsage)
    return;

  QMap<QString, BvhUsage> map = allcurrentBvhUsages();

  currentBvhUsage = bvhUsage;

  for(auto i=map.begin(); i!=map.end(); ++i)
  {
    const QString& key = i.key();
    BvhUsage value = i.value();

    ReloadableShader::defineMacro(key, bvhUsage == value, false);
  }

  ReloadableShader::defineMacro("BVH_USE_GRID", bvh_is_grid(bvhUsage), false);
  ReloadableShader::defineMacro("BVH_GRID_UNCLAMPED_OCCLUSION", bvh_is_occlusion_grid_unclamped(bvhUsage), false);
  ReloadableShader::defineMacro("BVH_GRID_HAS_FOUR_COMPONENTS", bvh_is_grid_with_four_components(bvhUsage), false);

  // TODO:: replace the following with GLSLMacroWrappers
  ReloadableShader::globalPreprocessorBlock.remove(QString("#define BVH_GRID_NUM_COMPONENTS %0").arg(bvh_is_grid_with_four_components(bvhUsage) ? 1 : 4));
  ReloadableShader::globalPreprocessorBlock.insert(QString("#define BVH_GRID_NUM_COMPONENTS %0").arg(bvh_is_grid_with_four_components(bvhUsage) ? 4 : 1));

  ReloadableShader::reloadAll();
}

void init_bvh_shader_macros()
{
  HOLD_BACK_HUGE_LEAf_FROM_BVH_TREE.connectWith(&scene::HOLD_BACK_HUGE_LEAf_FROM_BVH_TREE);

  set_bvh_traversal_leaf_result_array_length(MAX_NUM_STATIC_MESHES);
  ReloadableShader::globalPreprocessorBlock.insert(QString("#define MAX_NUM_STATIC_MESHES %0").arg(MAX_NUM_STATIC_MESHES));
  ReloadableShader::globalPreprocessorBlock.insert(QString("#define BVH_USE_GRID_OCCLUSION %0").arg(BVH_USE_GRID_OCCLUSION));
  ReloadableShader::globalPreprocessorBlock.insert(QString("#define MAX_NUM_GRID_CASCADES %0").arg(MAX_NUM_GRID_CASCADES));
  setCurrentBVHUsage(renderer::BvhUsage::NO_BVH);

  GLSLMacroWrapperInterface::initAll();
}

} // namespace renderer
} // namespace glrt
