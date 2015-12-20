#ifdef FORWARD_RENDERER

#include <lighting/rendering-equation.glsl>

#include "common-uniform-blocks.glsl"

out vec4 color;

void apply_material(in BaseMaterial material, in SurfaceData surface, float alpha)
{
  vec3 incoming_luminance = light_material(material, surface.position, scene.camera_position);
  
  float exposure = 1.f; // Only dummy value, to be corrected
  
  incoming_luminance *= exposure;
  
  color = vec4(accurateLinearToSRGB(incoming_luminance), alpha);
  
  
}

#endif // FORWARD_RENDERER
