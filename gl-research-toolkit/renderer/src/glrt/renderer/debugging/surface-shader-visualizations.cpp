#include <glrt/renderer/debugging/surface-shader-visualizations.h>
#include <glrt/renderer/toolkit/reloadable-shader.h>

namespace glrt {
namespace renderer {

typedef glrt::renderer::ReloadableShader ReloadableShader;

SurfaceShaderVisualization currentSurfaceShaderVisualization = SurfaceShaderVisualization::NONE;

QMap<QString, SurfaceShaderVisualization> allSurfaceShaderVisualizations()
{
  QMap<QString, SurfaceShaderVisualization> map;

#define VALUE(MACRO) map[#MACRO] = SurfaceShaderVisualization::MACRO

  map[" - "] = SurfaceShaderVisualization::NONE;

  VALUE(LIGHTING_FLAT);
  VALUE(LIGHTING_ONLY_DIFFUSE);
  VALUE(LIGHTING_ONLY_SPECULAR);
  VALUE(LIGHTING_SILVER);
  VALUE(LIGHTING_PERFECT_MIRROR);
  VALUE(LIGHTING_ROUGH_MIRROR);
  VALUE(LIGHTING_SMOOTH_MIRROR);

  return map;
}

void setCurrentSurfaceShaderVisualization(SurfaceShaderVisualization visualization)
{
  if(currentSurfaceShaderVisualization == visualization)
    return;

  QSet<QString>& globalPreprocessorBlock = ReloadableShader::globalPreprocessorBlock;

  QMap<QString, SurfaceShaderVisualization> map = allSurfaceShaderVisualizations();

  currentSurfaceShaderVisualization = visualization;

  for(auto i=map.begin(); i!=map.end(); ++i)
  {
    const QString& key = i.key();
    SurfaceShaderVisualization value = i.value();

    QString macro = QString("#define %0").arg(key);

    if(visualization == value && visualization!=SurfaceShaderVisualization::NONE)
    {
      globalPreprocessorBlock.insert(macro);
    }else
    {
      globalPreprocessorBlock.remove(macro);
    }
  }

  ReloadableShader::reloadAll();
}


} // namespace renderer
} // namespace glrt
