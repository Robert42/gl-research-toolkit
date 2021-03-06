#include <glrt/renderer/debugging/surface-shader-visualizations.h>
#include <glrt/renderer/toolkit/reloadable-shader.h>
#include <glrt/renderer/bvh-usage.h>

namespace glrt {
namespace renderer {

typedef glrt::renderer::ReloadableShader ReloadableShader;

bool show_forward_only = true;

SurfaceShaderVisualization currentSurfaceShaderVisualization = SurfaceShaderVisualization::NONE;

#define pretty_screenshot 0

QMap<QString, SurfaceShaderVisualization> allSurfaceShaderVisualizations()
{
  QMap<QString, SurfaceShaderVisualization> map;

#define VALUE(MACRO) map[#MACRO] = SurfaceShaderVisualization::MACRO

  map[" - "] = SurfaceShaderVisualization::NONE;

#if !pretty_screenshot
  VALUE(AMBIENT_OCCLUSION);
  VALUE(BVH_NEAREST_LEAF_INDEX_0);
  VALUE(BVH_NEAREST_LEAF_INDEX_1);
  VALUE(BVH_NEAREST_LEAF_INDEX_2);
  VALUE(BVH_OCCLUSION_GRID);
  VALUE(BVH_OCCLUSION_GRID_ONLY_0);
  VALUE(BVH_OCCLUSION_GRID_ONLY_1);
  VALUE(BVH_OCCLUSION_GRID_ONLY_2);
  VALUE(CANDIDATE_GRID_NUM_STATIC_CANDIDATES);
  VALUE(CANDIDATE_GRID_NUM_DYNAMIC_CANDIDATES);
  VALUE(CASCADED_GRID_WEIGHTS);
  VALUE(CASCADED_GRID_WEIGHTS_TINTED);
#endif
  VALUE(DISTANCEFIELD_AO);
#if !pretty_screenshot
  VALUE(DISTANCEFIELD_AO_COST_TEX);
  VALUE(DISTANCEFIELD_AO_COST_CONE_SPHERE_INTERSECTION_TEST);
  VALUE(DISTANCEFIELD_AO_COST_NUM_CONETRACED_SDF);
  VALUE(DISTANCEFIELD_AO_COST_SDF_ARRAY_ACCESS);
  VALUE(DISTANCEFIELD_AO_NUM_NEGATIVE_SAMPLES);
#endif
  VALUE(LIGHTING_FLAT);
#if !pretty_screenshot
  VALUE(LIGHTING_ONLY);
  VALUE(LIGHTING_ONLY_DIFFUSE);
  VALUE(LIGHTING_ONLY_SPECULAR);
  VALUE(LIGHTING_SILVER);
  VALUE(LIGHTING_MIRROR_PERFECT);
  VALUE(LIGHTING_MIRROR_ROUGH);
  VALUE(LIGHTING_MIRROR_SMOOTH);
  VALUE(MATERIAL_NORMAL_WS);
  VALUE(MATERIAL_ALPHA);
  VALUE(MATERIAL_ROUGHNESS_ADJUSTED);
  VALUE(MATERIAL_ROUGHNESS_UNADJUSTED);
  VALUE(MATERIAL_SMOOTHNESS);
  VALUE(MATERIAL_BASE_COLOR);
  VALUE(MATERIAL_METAL_MASK);
  VALUE(MATERIAL_EMISSION);
  VALUE(MATERIAL_REFLECTANCE);
  VALUE(MATERIAL_OCCLUSION);
  if(show_forward_only)
  {
    VALUE(MESH_NORMALS_WS);
    VALUE(MESH_UVS);
    VALUE(MESH_TANGENTS_WS);
    VALUE(MESH_BITANGENTS_WS);
  }
  VALUE(SHOW_NAN);
  VALUE(SHOW_INF);
  if(show_forward_only)
  {
    VALUE(TEXTURE_BASECOLOR);
    VALUE(TEXTURE_BASECOLOR_ALPHA);
    VALUE(TEXTURE_NORMAL_LS);
    VALUE(TEXTURE_BUMP);
    VALUE(TEXTURE_SMOOTHENESS);
    VALUE(TEXTURE_REFLECTIVITY);
    VALUE(TEXTURE_METALLIC);
    VALUE(TEXTURE_AO);
    VALUE(TEXTURE_EMISSION);
  }
#endif

  return map;
}

void setCurrentSurfaceShaderVisualization(SurfaceShaderVisualization visualization)
{
  if(currentSurfaceShaderVisualization == visualization)
    return;

  QMap<QString, SurfaceShaderVisualization> map = allSurfaceShaderVisualizations();

  currentSurfaceShaderVisualization = visualization;

  for(auto i=map.begin(); i!=map.end(); ++i)
  {
    const QString& key = i.key();
    SurfaceShaderVisualization value = i.value();

    ReloadableShader::defineMacro(key, visualization == value && visualization!=SurfaceShaderVisualization::NONE, false);
  }

  ReloadableShader::reloadAll();
}


} // namespace renderer
} // namespace glrt
