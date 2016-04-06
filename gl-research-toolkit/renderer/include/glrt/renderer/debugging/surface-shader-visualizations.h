#ifndef GLRT_RENDERER_SURFACESHADERVISUALIZATIONS_H
#define GLRT_RENDERER_SURFACESHADERVISUALIZATIONS_H

#include <glrt/renderer/dependencies.h>

namespace glrt {
namespace renderer {


enum class SurfaceShaderVisualization
{
  NONE,
  LIGHTING_FLAT,
  LIGHTING_ONLY,
  LIGHTING_ONLY_DIFFUSE,
  LIGHTING_ONLY_SPECULAR,
  LIGHTING_MIRROR_PERFECT,
  LIGHTING_MIRROR_ROUGH,
  LIGHTING_MIRROR_SMOOTH,
  LIGHTING_SILVER,
  MESH_NORMALS_WS,
  MESH_UVS,
  MESH_TANGENTS_WS,
  SHOW_NAN,
  SHOW_INF,
  TEXTURE_BASECOLOR,
  TEXTURE_BASECOLOR_ALPHA,
  TEXTURE_NORMAL_LS,
  TEXTURE_HEIGHT,
  TEXTURE_SMOOTHENESS,
  TEXTURE_REFLECTIVITY,
  TEXTURE_METALLIC,
  TEXTURE_AO,
  TEXTURE_EMISSION,
};

extern SurfaceShaderVisualization currentSurfaceShaderVisualization;
QMap<QString, SurfaceShaderVisualization> allSurfaceShaderVisualizations();
void setCurrentSurfaceShaderVisualization(SurfaceShaderVisualization visualization);


} // namespace renderer
} // namespace glrt

#endif // GLRT_RENDERER_SURFACESHADERVISUALIZATIONS_H
