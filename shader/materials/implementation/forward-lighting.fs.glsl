#ifdef FORWARD_RENDERER

#include <lighting/rendering-equation.glsl>

#include "common-uniform-blocks.glsl"

out vec4 color;

void apply_material(in MaterialOutput material_output)
{
  color = vec4(light_material(material_output, direction_to_camera(material_output.position)), material_output.color.a);
}

#endif // FORWARD_RENDERER
