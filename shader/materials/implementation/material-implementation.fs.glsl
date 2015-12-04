#include <preprocessor-block.h>

struct MaterialOutput
{
  vec4 color;
  vec3 emission;
  float roughness;
  vec3 position;
  float metallic;
  vec3 normal;
  float occlusion;
};

out vec4 color;

#ifdef FORWARD_RENDERER
#include "forward-lighting.fs.glsl"
#endif

void apply_material(in MaterialOutput material_output)
{
  float alpha = material_output.color.a;
  
  vec3 emission = material_output.emission;
  vec3 diffuse_color = material_output.color.rgb;
  
  vec3 diffuse_lighting = direct_diffuse_light(material_output.position, material_output.normal);
  
  color = vec4(emission + diffuse_color * diffuse_lighting, alpha);
}
