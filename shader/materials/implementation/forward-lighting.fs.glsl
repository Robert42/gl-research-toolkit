#ifdef FORWARD_RENDERER

#include <lighting/rendering-equation.glsl>

#include "common-uniform-blocks.glsl"

out vec4 color;

void apply_material(in BaseMaterial material, in SurfaceData surface, float alpha)
{
  color = vec4(light_material(material, surface.position, scene.camera_position), alpha);
}

#endif // FORWARD_RENDERER
