#ifndef GLRT_RENDERER_SURFACESHADERVISUALIZATIONS_H
#define GLRT_RENDERER_SURFACESHADERVISUALIZATIONS_H

#include <glrt/renderer/dependencies.h>

namespace glrt {
namespace renderer {


enum class SurfaceShaderVisualization
{
  NONE,
  LIGHTING_FLAT,
  LIGHTING_ONLY_DIFFUSE,
  LIGHTING_ONLY_SPECULAR,
  LIGHTING_SILVER,
  LIGHTING_PERFECT_MIRROR,
  LIGHTING_ROUGH_MIRROR,
  LIGHTING_SMOOTH_MIRROR,
};

extern SurfaceShaderVisualization currentSurfaceShaderVisualization;
QMap<QString, SurfaceShaderVisualization> allSurfaceShaderVisualizations();
void setCurrentSurfaceShaderVisualization(SurfaceShaderVisualization visualization);


} // namespace renderer
} // namespace glrt

#endif // GLRT_RENDERER_SURFACESHADERVISUALIZATIONS_H
