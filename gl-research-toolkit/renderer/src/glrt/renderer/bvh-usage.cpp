#include <glrt/renderer/bvh-usage.h>
#include <glrt/renderer/toolkit/reloadable-shader.h>

namespace glrt {
namespace renderer {

BvhUsage currentBvhUsage = BvhUsage::BVH_RECURSIVE;

// FIXME set to a lower value!
const quint16 BVH_MAX_DEPTH = 256;

QMap<QString, BvhUsage> allcurrentBvhUsages()
{
  QMap<QString, BvhUsage> map;

#define VALUE(MACRO) map[#MACRO] = BvhUsage::MACRO

  VALUE(NO_BVH);
  VALUE(BVH_RECURSIVE);

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

  ReloadableShader::reloadAll();
}

void init_bvh_shader_macros()
{
  ReloadableShader::globalPreprocessorBlock.insert(QString("#define BVH_MAX_DEPTH %0").arg(BVH_MAX_DEPTH));
  glrt::renderer::setCurrentBVHUsage(renderer::BvhUsage::NO_BVH);
}

} // namespace renderer
} // namespace glrt
