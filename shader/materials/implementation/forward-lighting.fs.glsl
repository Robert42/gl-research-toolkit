#ifdef FORWARD_RENDERER

#include <lighting/rendering-equation.glsl>

#include <glrt/glsl/layout-constants.h>

layout(binding=UNIFORM_BINDING_SCENE_FRAGMENT_BLOCK, std140) uniform SceneFragmentBlock
{
  vec3 camera_position;
}scene;

out vec4 color;

void apply_material(in BaseMaterial material, in SurfaceData surface, float alpha)
{
  vec3 incoming_luminance = light_material(material, surface.position, scene.camera_position);
  
  float exposure = 1.f; // Only dummy value, to be corrected
  
  incoming_luminance *= exposure;
  
#ifdef DEPTH_PREPASS
  color = vec4(vec3(0), alpha);
#endif

#ifdef FORWARD_PASS
  color = vec4(accurateLinearToSRGB(incoming_luminance), alpha);
#endif 
  
  
}

#endif // FORWARD_RENDERER
