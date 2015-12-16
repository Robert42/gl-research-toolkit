#ifdef FORWARD_RENDERER

#include <lighting/rendering-equation.glsl>

#include "common-uniform-blocks.glsl"

out vec4 color;

void apply_material(in BaseMaterial material, in SurfaceData surface, float alpha)
{
  ViewerData viewer;
  viewer.direction_to_camera = direction_to_camera(surface.position);
  
  color = vec4(light_material(material, surface, viewer), alpha);
}

#endif // FORWARD_RENDERER
