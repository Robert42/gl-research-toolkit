#include <glrt/renderer/bvh-usage.h>
#include <glrt/renderer/toolkit/reloadable-shader.h>

namespace glrt {
namespace renderer {

extern int currentNumBvhGrids;

BvhUsage currentBvhUsage = BvhUsage::BVH_WITH_STACK;
int currentNumBvhGrids = 0;

int num_grid_cascades()
{
  return currentNumBvhGrids;
}

void set_num_grid_cascades(int n)
{
  Q_ASSERT(n>=1 && n<=3);
  if(currentNumBvhGrids == n)
    return;

  ReloadableShader::globalPreprocessorBlock.remove(QString("#define NUM_GRID_CASCADES %0").arg(currentNumBvhGrids));
  currentNumBvhGrids = n;
  ReloadableShader::globalPreprocessorBlock.insert(QString("#define NUM_GRID_CASCADES %0").arg(currentNumBvhGrids));

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

  ReloadableShader::globalPreprocessorBlock.remove(QString("#define BVH_GRID_NUM_COMPONENTS %0").arg(bvh_is_grid_with_four_components(bvhUsage) ? 1 : 4));
  ReloadableShader::globalPreprocessorBlock.insert(QString("#define BVH_GRID_NUM_COMPONENTS %0").arg(bvh_is_grid_with_four_components(bvhUsage) ? 4 : 1));

  ReloadableShader::reloadAll();
}

void init_bvh_shader_macros()
{
  set_num_grid_cascades(3);
  ReloadableShader::globalPreprocessorBlock.insert(QString("#define AO_RADIUS 3.5"));
  ReloadableShader::globalPreprocessorBlock.insert(QString("#define MAX_NUM_STATIC_MESHES %0").arg(MAX_NUM_STATIC_MESHES));
  ReloadableShader::globalPreprocessorBlock.insert(QString("#define BVH_MAX_STACK_DEPTH %0").arg(BVH_MAX_STACK_DEPTH));
  ReloadableShader::globalPreprocessorBlock.insert(QString("#define BVH_MAX_VISITED_LEAVES %0").arg(BVH_MAX_VISITED_LEAVES));
  ReloadableShader::globalPreprocessorBlock.insert(QString("#define BVH_USE_GRID_OCCLUSION %0").arg(BVH_USE_GRID_OCCLUSION));
  ReloadableShader::globalPreprocessorBlock.insert(QString("#define MAX_NUM_GRID_CASCADES %0").arg(MAX_NUM_GRID_CASCADES));
  setCurrentBVHUsage(renderer::BvhUsage::NO_BVH);
}

} // namespace renderer
} // namespace glrt
