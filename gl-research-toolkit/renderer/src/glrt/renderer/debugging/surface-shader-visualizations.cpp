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
  VALUE(LIGHTING_ONLY);
  VALUE(LIGHTING_ONLY_DIFFUSE);
  VALUE(LIGHTING_ONLY_SPECULAR);
  VALUE(LIGHTING_SILVER);
  VALUE(LIGHTING_MIRROR_PERFECT);
  VALUE(LIGHTING_MIRROR_ROUGH);
  VALUE(LIGHTING_MIRROR_SMOOTH);
  VALUE(MATERIAL_NORMAL_WS);
  VALUE(MATERIAL_ALPHA);
  VALUE(MATERIAL_SMOOTHNESS);
  VALUE(MATERIAL_BASE_COLOR);
  VALUE(MATERIAL_METAL_MASK);
  VALUE(MATERIAL_EMISSION);
  VALUE(MATERIAL_REFLECTANCE);
  VALUE(MATERIAL_OCCLUSION);
  VALUE(MESH_NORMALS_WS);
  VALUE(MESH_UVS);
  VALUE(MESH_TANGENTS_WS);
  VALUE(MESH_BITANGENTS_WS);
  VALUE(SHOW_NAN);
  VALUE(SHOW_INF);
  VALUE(TEXTURE_BASECOLOR);
  VALUE(TEXTURE_BASECOLOR_ALPHA);
  VALUE(TEXTURE_NORMAL_LS);
  VALUE(TEXTURE_BUMP);
  VALUE(TEXTURE_SMOOTHENESS);
  VALUE(TEXTURE_REFLECTIVITY);
  VALUE(TEXTURE_METALLIC);
  VALUE(TEXTURE_AO);
  VALUE(TEXTURE_EMISSION);

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
